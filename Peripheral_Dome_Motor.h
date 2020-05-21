/*
 * Peripheral_Dome_Motor.h - Library for dome motor control for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 21 May 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
*/

//  WORK IN PROGRESS

#ifndef _PERIPHERAL_DOME_MOTOR_H_
#define _PERIPHERAL_DOME_MOTOR_H_

#include "Globals.h"
#include <math.h>
#include <Sabertooth.h>

extern String output;

/* ===============================================
 *  Dome Controls
 * ===============================================
 *  PS3 Navigation (single or dual)
 *    L2+Cross      = Disable dome automation
 *    L2+Circle     = Enable dome automation
 *    Cross         = Rotate dome CW at the fixed speed
 *    Circle        = Rotate dome CCW at the fixed speed
 *    L1+Nav1 Stick = Rotate dome at variable speed and direction
 *    Nav2 Stick    = Rotate dome at variable speed and direction
 * =============================================== */


/* =========================================
 *           Dome class definition
 * ========================================= */
class Dome_Motor
{
  private:
    Buffer* _buffer;
    Sabertooth _Syren;
    int _turnDirection;
    float _targetPosition;
    unsigned long _stopTurnTime;
    unsigned long _startTurnTime;
    unsigned long _previousMillis;
    int _rotationStatus;
    void _rotateDome(int rotationSpeed);
    int _checkDeadZone(int stickPosition);

    #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
    String _className;
    #endif

  public:
    // =====================
    //      Constructor
    // =====================
    Dome_Motor(Buffer* pBuffer) : _Syren(SYREN_ADDR, Serial2) {
      _buffer = pBuffer;
      _buffer->setStatus(Status::DomeMotorEnabled, true);
      _buffer->setStatus(Status::DomeMotorStopped, true);
      _buffer->setStatus(Status::DomeAutomationRunning, false);
      _buffer->setStatus(Status::CustomDomePanelRunning, false);
      
      _turnDirection      = 1;  // 1 = positive turn, -1 negative turn
      _targetPosition     = 0;  // (0 - 359) - degrees in a circle, 0 = home
      _stopTurnTime       = 0;  // millis() when next turn should stop
      _startTurnTime      = 0;  // millis() when next turn should start
      _previousMillis     = 0;
      _rotationStatus     = 0;  // 0 = stopped, 1 = prepare to turn, 2 = turning

      #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
      _className = F("Dome_Motor::");
      #endif

    };

    // =====================
    //        begin()
    // =====================
    void begin() {

      #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
      String functionName = _className+F("begin()");
      #endif

      // Start Syren 10 dome motor.
      _Syren.autobaud();
      _Syren.setTimeout(300);

      #ifdef BLACBOX_DEBUG
      output = functionName;
      output += F(" - Dome motor started.");
      Serial.println(output);
      #endif
};

    // =====================
    //     automation()
    // =====================
    void automation() {

      #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
      String functionName = _className+F("automation()");
      #endif

      long rndNum;
      int domeSpeed;
    
      if (_rotationStatus == 0) {
        // Dome is currently stopped - prepare for a future turn
        if (_targetPosition == 0) {
          // Dome is currently in the home position - prepare to turn away
          _startTurnTime = millis() + (random(3, 10) * 1000);
          rndNum = random(5,354);
          // set the target position to a random degree of a 360 circle - shaving off the first and last 5 degrees
          _targetPosition = rndNum;
          if (_targetPosition < 180) {
            // Turn the dome in the positive direction
            _turnDirection = 1;
            _stopTurnTime = _startTurnTime + ((_targetPosition / 360) * TIME_360_DOME_TURN_RIGHT);
          }
          else {
            // Turn the dome in the negative direction
            _turnDirection = -1;
            _stopTurnTime = _startTurnTime + (((360 - _targetPosition) / 360) * TIME_360_DOME_TURN_LEFT);
          }
        }
        else {
          // Dome is not in the home position - send it back to home
          _startTurnTime = millis() + (random(3, 10) * 1000);
          if (_targetPosition < 180) {
            _turnDirection = -1;
            _stopTurnTime = _startTurnTime + ((_targetPosition / 360) * TIME_360_DOME_TURN_LEFT);
          }
          else {
            _turnDirection = 1;
            _stopTurnTime = _startTurnTime + (((360 - _targetPosition) / 360) * TIME_360_DOME_TURN_RIGHT);
    
          }
          _targetPosition = 0;
        }
        
        _rotationStatus = 1;  // Set dome status to preparing for a future turn
         
        #ifdef BLACBOX_DEBUG
        output = functionName;
        output += F(" - Dome Automation: Initial Turn Set");
        Serial.println(output);
        output = F("\tCurrent Time: ");
        output += millis();
        Serial.println(output);
        output = F("\tNext Start Time: ");
        output += _startTurnTime;
        Serial.println(output);
        output = F("\tNext Stop Time: ");
        output += _stopTurnTime;
        Serial.println(output);
        output = F("\tDome Target Position: ");
        output += _targetPosition;
        Serial.println(output);
        #endif
        }
      
      if (_rotationStatus == 1) {
        // Dome is prepared for a future move - start the turn when ready
        if (_startTurnTime < millis()) {
          _rotationStatus = 2;
          #ifdef BLACBOX_DEBUG
          output = _className+functionName;
          output += F(" - Dome Automation: Ready To Start Turn");
          Serial.println(output);
          #endif
        }
      }
    
      if (_rotationStatus == 2) {
    
        // Dome is now actively turning until it reaches its stop time
        if (_stopTurnTime > millis()) {
          domeSpeed = DOME_AUTO_SPEED * _turnDirection;
          _Syren.motor(domeSpeed);
          #ifdef BLACBOX_DEBUG
          output = functionName;
          output += F(" - Turning Now!!");
          Serial.println(output);
          #endif
        } 
        else {
          // turn completed - stop the motor
          _rotationStatus = 0;
          _Syren.stop();
          #ifdef BLACBOX_DEBUG
          output = functionName;
          output = F(" - STOP TURN!!");
          Serial.println(output);
          #endif
        }
      }
    
    };

    // ==============================
    //     interpretController()
    // ==============================
    void interpretController() {

      #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
      String functionName = _className+F("interpretController()");
      #endif

      // ===============================================
      //  Dome automation enable/disable
      // ===============================================
      //    L2+Circle = Enable dome automation
      //    L2+Cross  = Disable dome automation
      // ===============================================

      if ( (_buffer->isDomeEnabled()) ) {
        if ( _buffer->getButton(L2) && _buffer->getButton(SELECT) ) {
          
          // Disable the dome when commanded.

          _buffer->setStatus(Status::DomeMotorEnabled, false);
          #ifdef BLACBOX_DEBUG
          output = functionName;
          output += F(" - Disabling dome motor.");
          Serial.println(output);
          #endif
          return; // Returning keeps the processing time quick.
        }
      } else {
        if ( _buffer->getButton(L2) && _buffer->getButton(START) ) {

          // Enable the dome when commanded.

          _buffer->setStatus(Status::DomeMotorEnabled, true);
          #ifdef BLACBOX_DEBUG
          output = functionName;
          output += F(" - Enabling dome motor.");
          Serial.println(output);
          #endif
        }
        return; // Returning keeps the processing time quick.
      }
  

      //Flood control prevention
      //This is intentionally set to double the rate of the Foot Motor Latency
      if ((millis() - _previousMillis) < (2*SERIAL_LATENCY) ) return;  

      // If we get this far, the dome is still enabled.
      // Now, look for rotation commands.

      // ===============================================
      //  Dome rotation
      // ===============================================
      //    Nav2 Stick    = Rotate dome at variable speed and direction
      //    L1+Nav1 Stick = Rotate dome at variable speed and direction
      //    Cross         = Rotate dome CW at the fixed speed while driving
      //    Circle        = Rotate dome CCW at the fixed speed while driving
      // ===============================================

      int rotationSpeed = 0;

      if ( _buffer->isSecondaryConnected() ) {
        if ( _buffer->isStickOffCenter(RightHatX) ) {

          // Rotate the dome in the direction and speed according to the secondary joystick.
          rotationSpeed = _checkDeadZone(_buffer->getStick(RightHatX));

          #ifdef BLACBOX_VERBOSE
          output = functionName;
          output += F(" - Right stick moved.");
          Serial.println(output);
          #endif
        }

      } else if ( _buffer->isPrimaryConnected() ) {

        // The primary controller handles dome rotation only when there
        // is no secondary controller. Priority is given to the stick
        // first then the button controls.

        if ( _buffer->getButton(L1) ) {

          // Stop the foot motors when L1 is pressed while driving.

          if ( !(_buffer->isFootStopped()) ) {
            controller.stopFootMotors();
            return; // Interrupt the command. Let the next loop handle dome rotation.
          }

          if ( _buffer->isStickOffCenter(LeftHatX) ) {

            // Rotate the dome in the direction and speed according to the primary joystick.
            rotationSpeed = _checkDeadZone(_buffer->getStick(LeftHatX));

            #ifdef BLACBOX_VERBOSE
            output = functionName;
            output += F(" - Left stick moved.");
            Serial.println(output);
            #endif
          }
        } else {

          if ( !(_buffer->isButtonModified()) ) {
            if ( _buffer->getButton(SELECT)) {

              // Rotate the dome counterclockwise at a fixed speed.
              rotationSpeed = -75;

              #ifdef BLACBOX_DEBUG
              output = functionName;
              output += F(" - Rotate dome CCW.");
              Serial.println(output);
              #endif
            } else if ( _buffer->getButton(START) ) {

              // Rotate the dome clockwise at a fixed speed.
              rotationSpeed = 75;

              #ifdef BLACBOX_DEBUG
              output = functionName;
              output += F(" - Rotate dome CW.");
              Serial.println(output);
              #endif
            }
          }
        }
      }

      // Turn off dome automation if manually moved

      if (rotationSpeed != 0 && _buffer->isDomeAutomationRunning() == true) {
        _buffer->setStatus(Status::DomeMotorEnabled, false);
        _rotationStatus = 0;
        _targetPosition = 0;

        #ifdef BLACBOX_DEBUG
        output = functionName;
        output += F(" - Disabling automation.");
        Serial.println(output);
        #endif
      }

      // Send the rotation command.

      if ( rotationSpeed != 0 || !(_buffer->isDomeStopped()) )
        _rotateDome(rotationSpeed);

    };
};

#endif

// =======================
//      _rotateDome()
// =======================
void Dome_Motor::_rotateDome(int rotationSpeed) {

  #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
  String functionName = _className+F("_rotateDome()");
  #endif

  //Flood control prevention
  //This is intentionally set to double the rate of the Foot Motor Latency
  unsigned long currentMillis = millis();
  if ( (currentMillis - _previousMillis) <= (2*SERIAL_LATENCY) )
    return;

  #ifdef BLACBOX_DEBUG
  output = functionName;
  if (rotationSpeed < 0)
    output += F(" - Spinning dome left at speed: ");
  else if (rotationSpeed > 0)
    output += F(" - Spinning dome right at speed: ");
  else
    output += F(" - Stopping dome spin speed: ");
  output += rotationSpeed;
  Serial.println(output);
  #endif

  // Update the dome's movement status.
  if (rotationSpeed != 0)
    _buffer->setStatus(DomeMotorStopped, false);
  else
    _buffer->setStatus(DomeMotorStopped, true);

  // Send the command to the dome motor.
  _Syren.motor(rotationSpeed);

  // Prepare for the next cycle.
  _previousMillis = currentMillis;

};

// =======================
//      _checkDeadZone()
// =======================
int Dome_Motor::_checkDeadZone(int stickPosition) {
  int ret;
  ret = (map(stickPosition, 0, 255, -DOME_SPEED, DOME_SPEED));
  if ( abs(stickPosition-128) < JOYSTICK_DOME_DEAD_ZONE_RANGE ) 
    ret = 0;
  return ret;
}
