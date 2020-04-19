/*
 * Peripheral_Dome_Motor.h - Library for dome motor control for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 18 April 2020
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

  public:
    // =====================
    //      Constructor
    // =====================
    Dome_Motor() : _Syren(SYREN_ADDR, Serial2) {
      _buffer->setStatus(DomeMotorStopped, true);
      
      _turnDirection      = 1;  // 1 = positive turn, -1 negative turn
      _targetPosition     = 0;  // (0 - 359) - degrees in a circle, 0 = home
      _stopTurnTime       = 0;  // millis() when next turn should stop
      _startTurnTime      = 0;  // millis() when next turn should start
      _previousMillis     = 0;
      _rotationStatus     = 0;  // 0 = stopped, 1 = prepare to turn, 2 = turning
    };

    // =====================
    //        begin()
    // =====================
    void begin() {
      // Start Syren 10 dome motor.
      _Syren.autobaud();
      _Syren.setTimeout(300);
    };

    // =====================
    //     automation()
    // =====================
    void automation() {
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
        output = F("Dome_Motor::");
        output += F("_rotateDome - ");
        output += F("Dome Automation: Initial Turn Set");
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
          output = F("Dome_Motor::");
          output += F("_rotateDome - ");
          output += F("Dome Automation: Ready To Start Turn");
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
          output = F("Dome_Motor::");
          output += F("_rotateDome - ");
          output += F("Turning Now!!");
          Serial.println(output);
          #endif
        } 
        else {
          // turn completed - stop the motor
          _rotationStatus = 0;
          _Syren.stop();
          #ifdef BLACBOX_DEBUG
          output = F("Dome_Motor::");
          output += F("_rotateDome - ");
          output += F("STOP TURN!!");
          Serial.println(output);
          #endif
        }
      }
    
    };

    // ==============================
    //     interpretController()
    // ==============================
    void interpretController() {
      // ===============================================
      //  Dome automation
      // ===============================================
      //    L2+Cross  = Disable dome automation
      //    L2+Circle = Enable dome automation
      // ===============================================
      if (_buffer->getButton(L2)) {
        if (_buffer->getButton(SELECT))     
          _buffer->setStatus(Status::DomeMotorEnabled, false);
        else if (_buffer->getButton(START))
          _buffer->setStatus(Status::DomeMotorEnabled, true);
      }
  
      // ===============================================
      //  Dome rotation
      // ===============================================
      //    Cross         = Rotate dome CW at the fixed speed
      //    Circle        = Rotate dome CCW at the fixed speed
      //    Nav2 Stick    = Rotate dome at variable speed and direction
      //    L1+Nav1 Stick = Rotate dome at variable speed and direction
      // ===============================================

      //Flood control prevention
      //This is intentionally set to double the rate of the Foot Motor Latency
      if ((millis() - _previousMillis) < (2*SERIAL_LATENCY) ) return;  
  
      int rotationSpeed = 0;

      if ( !(_buffer->getButton(L1)) && !(_buffer->getButton(L2)) && \
           !(_buffer->getButton(R1)) && !(_buffer->getButton(R2)) && \
           !(_buffer->getButton(PS)) && !(_buffer->getButton(PS2)) ) {
        if (_buffer->getButton(SELECT)) {
          // Rotate the dome counterclockwise at a fixed speed.
          rotationSpeed = -75;
        } else if (_buffer->getButton(START)) {
          // Rotate the dome clockwise at a fixed speed.
          rotationSpeed = 75;
        }
      } else {
        int stickPosition = 127;
        if ( (_buffer->stickOffCenter(RightHatX)) && \
             !(_buffer->getButton(R1)) && !(_buffer->getButton(R2)) ) {
          // Rotate the dome in the direction and speed according to the right joystick.
          stickPosition = _buffer->getStick(RightHatX);
        } else if ( (_buffer->stickOffCenter(LeftHatX)) && \
                    (_buffer->getButton(L1)) ) {
          // Rotate the dome in the direction and speed according to the left joystick.
          stickPosition = _buffer->getStick(LeftHatX);
        }
        rotationSpeed = (map(stickPosition, 0, 255, -DOME_SPEED, DOME_SPEED));
        if ( abs(stickPosition-128) < JOYSTICK_DOME_DEAD_ZONE_RANGE ) 
          rotationSpeed = 0;

        // Turn off dome automation if manually moved

        if (rotationSpeed != 0 && _buffer->isDomeAutomationRunning() == true) {
          _buffer->setStatus(Status::DomeMotorEnabled, false);
          _rotationStatus = 0;
          _targetPosition = 0;

          #ifdef BLACBOX_DEBUG
          output = F("Dome_Motor::");
          output += F("_rotateDome - ");
          output += F("Dome Automation OFF");
          Serial.println(output);
          #endif
        }
      }
      _rotateDome(rotationSpeed);
    };

};

#endif

// =======================
//      _rotateDome()
// =======================
void Dome_Motor::_rotateDome(int rotationSpeed) {

    unsigned long currentMillis = millis();
    if ( (!(_buffer->isDomeStopped()) || rotationSpeed != 0) && \
         ((currentMillis - _previousMillis) > (2*SERIAL_LATENCY)) ) {

      #ifdef BLACBOX_DEBUG
      output = F("Dome_Motor::");
      output += F("_rotateDome - ");
      Serial.println(output);
      if (rotationSpeed < 0)
        output = F("  Spinning dome left at speed: ");
      else if (rotationSpeed > 0)
        output = F("  Spinning dome right at speed: ");
      else
        output = F("  Stopping dome spin speed: ");
      output += rotationSpeed;
      Serial.println(output);
      #endif

      if (rotationSpeed != 0)
        _buffer->setStatus(DomeMotorStopped, false);
      else
        _buffer->setStatus(DomeMotorStopped, true);

      _previousMillis = currentMillis;      
      _Syren.motor(rotationSpeed);

    }
};
