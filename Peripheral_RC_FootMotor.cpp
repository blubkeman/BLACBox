/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_RC_FootMotor.cpp - Library for R/C foot motor controllers
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Peripheral_RC_FootMotor.h"

/* ===========================================================================
 *                       RC_FootMotor subclass functions
 * =========================================================================== */

// =====================
//      Constructor
// =====================
RC_FootMotor::RC_FootMotor(Buffer* pBuffer) : FootMotor(pBuffer)
{
  _buffer = pBuffer;
}

// =====================
//        begin()
// =====================
void RC_FootMotor::begin()
{
  #ifdef DEADMAN
  _initDeadMan();
  #endif

  _leftFootSignal.attach(LEFT_FOOT_PIN);
  _rightFootSignal.attach(RIGHT_FOOT_PIN);

  //convert drivespeed values to something that will work for the Q85's
  _drivespeed1 = map(DRIVESPEED1, 0, 127, 90, 180);
  _drivespeed2 = map(DRIVESPEED2, 0, 127, 90, 180);

  // This is used by the dome's interrupt.

  anchor = this;

  #ifdef BLACBOX_DEBUG
  output = _className+F("begin()");
  output += F(" - RC foot motors started ");
  Serial.println(output);
  #endif
}

// ==============================
//        calculateDrive()
// ==============================
void RC_FootMotor::calculateDrive()
{
  // Get input from the controller.

  _leftFoot = _buffer->getStick(LeftHatX);
  _rightFoot = _buffer->getStick(LeftHatY);

  // Send input to the motor controller.

  _runMotors();

  // Prepare for the next iteration.

  _previousMillis = _currentMillis;
  return true;
}

// ===================
//      footISR()
// ===================
static void RC_FootMotor::footISR()
{
  if (anchor != NULL)
    anchor->_stopFeet();
}


// ===========================
//        _stopFeet()
// ===========================
void RC_FootMotor::_stopFeet()
{
  // Stop the motors.

  _leftFootSignal.write(90);
  _rightFootSignal.write(90);

  // Update the buffer's status.

  _buffer->setFootStopped(true);

  // Reset the foot motor interrupt.

  digitalWrite(footInterruptPin, HIGH);

  #if defined(DEBUG_FOOT) || defined (DEBUG_ALL)
  output = _className+F("_stopFeet()");
  output += F(" - Foot motors stopped.");
  Serial.println(output);
  #endif
}

// ===========================
//       _runMotors()
// ===========================
void RC_FootMotor::_runMotors()
{
  // Send data to the motor controller to drive the motors.

  _leftFootSignal.write(_leftFoot);
  _rightFootSignal.write(_rightFoot);

  // Update the foot motor status to reflect that the motors are running.

  _buffer->setFootStopped(false);
}
