/*
 * Peripheral_Foot_Motor.h - Library for foot motor control for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 18 April 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
*/

#ifndef _PERIPHERAL_FOOT_MOTORS_H_
#define _PERIPHERAL_FOOT_MOTORS_H_

#include "Globals.h"
#include <Servo.h>

extern String output;

/* ===============================================
 *  Drive Controls
 * ===============================================
 *  PS3 Navigation (single or dual)
 *    PS+Cross   = Disable drive/steering
 *    PS+Circle  = Enable drive/steering
 *    Nav1 Stick = Drive, steer
 * =============================================== */
class FootMotor {

  protected:
    Buffer* _buffer;
    long _currentMillis;
    long _previousMillis;

  public:
    // =====================
    //      Constructor
    // =====================
    FootMotor() {
      _buffer->setStatus(FootMotorEnabled, true);
      _buffer->setStatus(FootMotorStopped, true);
    };

    // ===================================
    //        interpretController()
    // ===================================
    virtual void interpretController() {
      // Check for enabling/disabling the stick.
      if (_buffer->getButton(PS)) {
        if (_buffer->getButton(SELECT)) { _buffer->setStatus(FootMotorEnabled, true); }
        if (_buffer->getButton(START))  { _buffer->setStatus(FootMotorEnabled, false); }
      }

      // Stop the foot motors any time any of these conditions are met:
      //  1. A primary controller is not connected.
      //  2. The drive/steer joystick is disabled using PS+CROSS, PS+SELECT, or PS+SHARE
      //  3. The L1 button is pressed (this switches the left joystick to running the dome rotation)
      if ( _buffer->isPrimaryConnected() == 0 || \
           _buffer->isFootEnabled() == 0 || \
           _buffer->getButton(L1)) {
        stopFeet();
        return;
      }

      calculateDrive();
    };

    // =========================================================
    //        Other functions to be defined by subclasses
    // =========================================================
    virtual void calculateDrive();
    virtual void stopFeet();
};

class Sabertooth_FootMotor : public FootMotor {

  private:
    Sabertooth _sabertooth;

  public:
    // =====================
    //      Constructor
    // =====================
    Sabertooth_FootMotor(Buffer* pBuffer) : _sabertooth(SABERTOOTH_ADDR, Serial2) {
      _buffer = pBuffer;
    };
    // =====================
    //        begin()
    // =====================
    void begin() {
      _sabertooth.autobaud();       // Send the autobaud command to the Sabertooth controller(s).
      _sabertooth.setTimeout(300);  // DMB:  How low can we go for safety reasons?  multiples of 100ms
      _sabertooth.setDeadband(DRIVE_DEAD_BAND_RANGE);
      stopFeet();

      #ifdef BLACBOX_DEBUG
      Serial.println(F("Sabertooth foot motors started."));
      #endif
    };

    // ===========================
    //        stopFeet()
    // ===========================
    virtual void stopFeet() {
      _sabertooth.stop();
      _buffer->setStatus(FootMotorStopped, true);
    };

    // ==============================
    //        calculateDrive()
    // ==============================
    virtual void calculateDrive() {

      _buffer->setStatus(FootMotorEnabled, false);
      int joystickPosition = _buffer->getStick(LeftHatY);
      int footDriveSpeed = 0;
      int stickSpeed = 0;
      int turnnum = 0;

      #if FOOT_CONTROLLER == 0
      if (_buffer->getButton(L2)) {
        int throttle = 0;
        if (joystickPosition < 127)
          throttle = joystickPosition - _buffer->getButton(L2);
        else
          throttle = joystickPosition + _buffer->getButton(L2);
        stickSpeed = (map(throttle, -255, 510, -DRIVESPEED2, DRIVESPEED2));                
      } else
        stickSpeed = (map(joystickPosition, 0, 255, -DRIVESPEED1, DRIVESPEED1));

      if ( abs(joystickPosition-128) < JOYSTICK_FOOT_DEAD_ZONE_RANGE)
          footDriveSpeed = 0;
      else if (footDriveSpeed < stickSpeed) {
        if (stickSpeed-footDriveSpeed<(RAMPING+1))
          footDriveSpeed+=RAMPING;
        else
          footDriveSpeed = stickSpeed;
      }
      else if (footDriveSpeed > stickSpeed) {
        if (footDriveSpeed-stickSpeed<(RAMPING+1))
          footDriveSpeed-=RAMPING;
        else
          footDriveSpeed = stickSpeed;  
      }
      
      turnnum = (_buffer->getStick(LeftHatX));

      //TODO:  Is there a better algorithm here?  
      if ( abs(footDriveSpeed) > 50)
          turnnum = (map(_buffer->getStick(LeftHatX), 54, 200, -(TURN_SPEED/4), (TURN_SPEED/4)));
      else if (turnnum <= 200 && turnnum >= 54)
          turnnum = (map(_buffer->getStick(LeftHatX), 54, 200, -(TURN_SPEED/3), (TURN_SPEED/3)));
      else if (turnnum > 200)
          turnnum = (map(_buffer->getStick(LeftHatX), 201, 255, TURN_SPEED/3, TURN_SPEED));
      else if (turnnum < 54)
          turnnum = (map(_buffer->getStick(LeftHatX), 0, 53, -TURN_SPEED, -(TURN_SPEED/3)));
      #endif

      _currentMillis = millis();
      if ( (_currentMillis - _previousMillis) > SERIAL_LATENCY  ) {
        #ifdef BLACBOX_VERBOSE
        if ( footDriveSpeed < -driveDeadBandRange || footDriveSpeed > driveDeadBandRange) {
          output += "Driving Droid at footSpeed: ";
          output += footDriveSpeed;
          output += "!  DriveStick is Enabled\r\n";
          output += "Joystick: ";              
          output += _buffer->getStick(LeftHatX);
          output += "/";              
          output += _buffer->getStick(LeftHatY);
          output += " turnnum: ";              
          output += turnnum;
          output += "/";              
          output += footDriveSpeed;
          output += " Time of command: ";              
          output += millis();
        }
        #endif
      }

      _sabertooth.turn(turnnum * INVERT_TURN_DIRECTION);
      _sabertooth.drive(footDriveSpeed);
      // The Sabertooth won't act on mixed mode packet serial commands until
      // it has received power levels for BOTH throttle and turning, since it
      // mixes the two together to get diff-drive power levels for both motors.

      _previousMillis = +_currentMillis;
      return true; //we sent a foot command
    };
};

class RC_FootMotor : public FootMotor {

  private:
    Servo _leftFootSignal;
    Servo _rightFootSignal;
    int _leftFoot;    // Foot speed value (-100 to 100)
    int _rightFoot;   // Foot speed value (-100 to 100)
    byte _drivespeed1;
    byte _drivespeed2;

  public:
    // =====================
    //      Constructor
    // =====================
    RC_FootMotor(Buffer* pBuffer) {
      _buffer = pBuffer;
    };

    // =====================
    //        begin()
    // =====================
    void begin() {
      _leftFootSignal.attach(LEFT_FOOT_PIN);
      _rightFootSignal.attach(RIGHT_FOOT_PIN);
      
      //convert drivespeed values to something that will work for the Q85's
      _drivespeed1 = map(DRIVESPEED1, 0, 127, 90, 180);
      _drivespeed2 = map(DRIVESPEED2, 0, 127, 90, 180);

      stopFeet();

      #ifdef BLACBOX_DEBUG
      Serial.println(F("RC foot motors started."));
      #endif
    };

    // ===========================
    //        stopFeet()
    // ===========================
    virtual void stopFeet() {
      _leftFootSignal.write(90);
      _rightFootSignal.write(90);
      _buffer->setStatus(FootMotorStopped, true);
    };

    // ==============================
    //        calculateDrive()
    // ==============================
    virtual void calculateDrive() {

      // Experimental Q85. Untested Madness!!! Use at your own risk and 
      // expect your droid to run away in flames.
      // use BigHappyDude's mixing algorithm to get values for each foot...
      if (_buffer->getButton(L2) > 0)
        mixBHD(_buffer->getStick(LeftHatX),_buffer->getStick(LeftHatY),_drivespeed2);
      else
        mixBHD(_buffer->getStick(LeftHatX),_buffer->getStick(LeftHatY),_drivespeed1);

      // Now we've got values for leftFoot and rightFoot, output those somehow...
      _leftFootSignal.write(_leftFoot);
      _rightFootSignal.write(_rightFoot);

      _previousMillis = _currentMillis;
      return true; //we sent a foot command   
    };

    // ======================
    //        mixBHD()
    // ======================
    void mixBHD(byte stickX, byte stickY, byte maxDriveSpeed) { 
      
    //maxDriveSpeed should be between 90 and 180

    // This is BigHappyDude's mixing function, for differential (tank) style 
    // drive using two motor controllers.
    // Takes a joysticks X and Y values, mixes using the diamind mix, and 
    // output a value 0-180 for left and right motors.     
    // 180,180 = both feet full speed forward.
    // 000,000 = both feet full speed reverse.
    // 180,000 = left foot full forward, right foot full reverse (spin droid clockwise)
    // 000,180 = left foot full reverse, right foot full forward (spin droid counter-clockwise)
    // 090,090 = no movement
    // for simplicity, we think of this diamond matrix as a range from -100 to +100,
    // then map the final values to servo range (0-180) at the end 
    // Ramping and Speed mode applied on the droid.
    
    // If movement outside deadzone
    if(((stickX <= 113) || (stickX >= 141)) || ((stickY <= 113) || (stickY >= 141))) {

      //  Map to easy grid -100 to 100 in both axis, including deadzones.

      int YDist = 0;  // set to 0 as a default value if no if used.
      int XDist = 0;

      if(stickY <= 113)
        YDist = (map(stickY, 0, 113, 100, 1));     //  Map the up direction stick value to Drive speed
      else if(stickY >= 141)
        YDist = (map(stickY, 141, 255, -1, -100)); //  Map the down direction stick value to Drive speed

      if(stickX <= 113)
        XDist = (map(stickX, 0, 113, -100, -1));   //  Map the left direction stick value to Turn speed
      else if(stickX >= 141)
        XDist = (map(stickX, 141, 255, 1, 100));   //  Map the right direction stick value to Turn speed

      //  Constrain to Diamond values.  using 2 line equations and find the intersect, boiled down to the minimum
      //  This was the inspiration; https://github.com/declanshanaghy/JabberBot/raw/master/Docs/Using%20Diamond%20Coordinates%20to%20Power%20a%20Differential%20Drive.pdf 

      float TempYDist = YDist;
      float TempXDist = XDist;

      // If outside top left. Equation of line is y=x+Max, so if y > x+Max then it is above line
      if ( YDist > (XDist + 100) ) {

        // OK, the first fun bit.
        // For the 2 lines, this is always true: y = m1*x + b1 and y = m2*x - b2
        // y - y = m1*x + b1  - m2*x - b2  or 0 = (m1 - m2)*x + b1 - b2
        // We have y = x+100 and y = ((change in y)/Change in x))x
        // So:   x = -100/(1-(change in y)/Change in x)) and using y = x+100 we can find y with the new x
        // Not too bad when simplified. :P
        TempXDist = -100 / (1 - (TempYDist / TempXDist));
        TempYDist = TempXDist + 100;

      } else if (YDist > (100 - XDist)) {  //  if outside top right

        // repeat intesection for y = 100 - x
        TempXDist = -100 / (-1 - (TempYDist / TempXDist));
        TempYDist = -TempXDist + 100;

      } else if (YDist<(-XDist-100)) {  //  if outside bottom left

        // repeat intesection for y = -x - 100
        TempXDist = 100/(-1-(TempYDist/TempXDist));
        TempYDist = -TempXDist-100;

      } else if (YDist<(XDist-100)) {  //  if outside bottom right

        // repeat intesection for y = x - 100
        TempXDist = 100/(1-(TempYDist/TempXDist));
        TempYDist = TempXDist-100;

      }

      //  all coordinates now in diamond. next translate to the diamond coordinates.
      //  for the left.  send ray to y = x + Max from coordinates along y = -x + b
      //  find for b, solve for coordinates and resut in y then scale using y = (y - max/2)*2
      float LeftSpeed = ((TempXDist+TempYDist-100)/2)+100;
      LeftSpeed = (LeftSpeed-50)*2;

      //  for right send ray to y = -x + Max from coordinates along y = x + b find intersction coordinates and then use the Y vaule and scale.
      float RightSpeed = ((TempYDist-TempXDist-100)/2)+100;
      RightSpeed = (RightSpeed-50)*2;

      // this all results in a -100 to 100 range of speeds, so shift to servo range...
      //  eg. for a maxDriveSpeed of 140, we'd need the value to map to between 40 and 140
      //  eg. for a maxDriveSpeed of 180, we'd need the value to map to between 0 and 180
      //_leftFoot=map(LeftSpeed, -100, 100, (180-maxDriveSpeed), maxDriveSpeed);
      //_rightFoot=map(RightSpeed, -100, 100, (180-maxDriveSpeed), maxDriveSpeed);
      _leftFoot=map(LeftSpeed, -100, 100, maxDriveSpeed, (180-maxDriveSpeed) );
      _rightFoot=map(RightSpeed, -100, 100, maxDriveSpeed, (180-maxDriveSpeed) );

      } else {
        _leftFoot=90;
        _rightFoot=90;
      }
    }

};

#endif
