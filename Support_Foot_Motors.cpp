/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Support_Foot_Motor.cpp - Library for parent class of foot motor controllers
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Support_Foot_Motors.h"


/* ===========================================================================
 *                          FootMotor class functions
 * =========================================================================== */

// =====================
//      Constructor
// =====================
FootMotor::FootMotor(Buffer* pBuffer)
{
  _buffer = pBuffer;
  _buffer->setFootEnabled(true);
  _buffer->setFootStopped(true);
}

// ===================================
//        interpretController()
// ===================================
void FootMotor::interpretController()
{
  // First, look for any reason to stop the motors.

  // Stop the motors when we lose the primary controller.

  if ( ! _buffer->isPrimaryConnected() && !_buffer->isFootStopped()) {
    _buffer->stopFootMotor();
    return;
  }

  // Check for enabling/disabling the stick.

  if (_buffer->getButton(PS)) {
    if (_buffer->getButton(SELECT)) {
      _buffer->setFootEnabled(true);
    }
    if (_buffer->getButton(START)) {
      _buffer->setFootEnabled(false);
    }
  }

  // Stop the foot motors any time any of these conditions are met:
  //  1. A primary controller is not connected.
  //  2. The drive/steer joystick is disabled using PS+CROSS, PS+SELECT, or PS+SHARE

  if ( ! _buffer->isPrimaryConnected() || ! _buffer->isFootEnabled() ) {
    if ( ! _buffer->isFootStopped() ) { _buffer->stopFootMotor(); }
    return;
  }

  // Check the deadman switch. When L2 is not actively
  // pressed, do not process motor control input.

  #ifdef DEADMAN
  if (_isDeadMan()) { return; }
  #endif

  // Interpret the controller's input and
  // calculate how to drive the motors.

  calculateDrive();
}

// ===================================
//          _isDeadMan()
// ===================================
bool FootMotor::_isDeadMan()
{
  // Check the deadman switch.

  if ((_buffer->getButton(L2))) {

    // L2 is pressed. Allow drive control.

    digitalWrite(DEADMAN_PIN, HIGH);
    return false;

  } else {

    // L2 is not pressed. Prevent drive control.

    digitalWrite(DEADMAN_PIN, LOW);
    if (! _buffer->isFootStopped() ) { _buffer->stopFootMotor(); }
    return true;

  }
}

// ===================================
//          _initDeadMan()
// ===================================
void FootMotor::_initDeadMan()
{
  pinMode(DEADMAN_PIN,OUTPUT);
  digitalWrite(DEADMAN_PIN,LOW);
}
