/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Drive_Motor_Sabertooth.cpp - Library for the Sabertooth motor controller
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Drive_Motors.h"

/* ================================================================================
 *                        Sabertooth Motor Controller Class
 * ================================================================================ */

#if defined(SABERTOOTH)

// =====================
//      Constructor
// =====================
Sabertooth_DriveMotors::Sabertooth_DriveMotors(Buffer * pBuffer) : DriveMotor(pBuffer) {}


// ====================
//      Destructor
// ====================
// This destructor is explicitly defined to avoid a linker error.
Sabertooth_DriveMotors::~Sabertooth_DriveMotors(void) {}


// =================
//      begin()
// =================
void Sabertooth_DriveMotors::begin(void)
{
  // -----------------------------------------------------------------------------
  // Call the parent class's begin(). It establishes our dead man switch, if used.
  // -----------------------------------------------------------------------------
  
  DriveMotor::begin();

  // -------------------------------------
  // Start communication with the Sabertooth.
  // -------------------------------------

  ST->autobaud();          // Send the autobaud command to the Sabertooth controller(s).
  ST->setTimeout(10);      //DMB:  How low can we go for safety reasons?  multiples of 100ms
  ST->setDeadband(JOYSTICK_DEAD_ZONE);
  ST->stop();

  pinMode(SCRIPT_PIN, OUTPUT);

  // ----------
  // Debugging.
  // ----------

  #if defined (DEBUG_DRIVE) || defined(DEBUG_ALL)
  output = F("Sabertooth::");
  output += F("begin()");
  output += F(" - Sabertooth motor controller started.");
  printOutput();
  #endif
}


// ===============================
//      interpretController()
// ===============================
void Sabertooth_DriveMotors::interpretController(void)
{
  // --------------------------------
  // Stop when we have no controller.
  // --------------------------------

  if ( ! _buffer->isControllerConnected() ) {
    stop();
    return;
  }

  // ---------------------------------------
  // Look for a change in the speed profile.
  // ---------------------------------------

  DriveMotor::_updateSpeedProfile();

  // --------------------------------------
  // Stop when the drive stick is disabled.
  // --------------------------------------

  DriveMotor::_toggleDriveEnabled();
  if ( ! _buffer->isDriveEnabled() ) {
    stop();
    return;
  }

  #if defined(DEADMAN)
  // -----------------------------------------------------
  // Stop when a deadman switch is in use but not pressed.
  // -----------------------------------------------------

  if ( ! DriveMotor::_isDeadmanPressed() ) {
    stop();
    return;
  }
  #endif

  // ------------------------------------------------------------------------
  // Flood control. Don't overload the motor controller with excessive input.
  // ------------------------------------------------------------------------

  long currentTime = millis();
  if ( (currentTime - _previousTime) <= SERIAL_LATENCY ) {
    return;
  }
  _previousTime = currentTime;

  // --------------------------------------------------------------------------------
  // Skip reading the stick when L1 is pressed unless L1 is the dead man switch. That
  // happens only when using a single PS3 Navigation controller. This is being done
  // in anticipation of changing the speed profile with L1+L3.
  // --------------------------------------------------------------------------------

  if ( ! _buffer->isHalfControllerConnected() ) {
    if ( _buffer->getButton(L1) ) {
      return;
    }
  }

  // -----------------------------------------------------
  // Ready to drive. Start by getting the joystick inputs.
  // -----------------------------------------------------

  _steering = _buffer->getStick(LeftHatX);  // Returns the x-axis position of the joystick (range 0-255)
  _throttle = _buffer->getStick(LeftHatY);  // Returns the y-axis position of the joystick (range 0-255)

  // -------------------------------------------------------
  // When within the dead zone, treat it the same as center.
  // -------------------------------------------------------
  
  if ( abs(_steering - JOYSTICK_CENTER) < JOYSTICK_DEAD_ZONE ) { _steering = JOYSTICK_CENTER; }
  if ( abs(_throttle - JOYSTICK_CENTER) < JOYSTICK_DEAD_ZONE ) { _throttle = JOYSTICK_CENTER; }

  // --------------------------------
  // Stop when the stick is centered.
  // --------------------------------

  if ( _steering == JOYSTICK_CENTER && _throttle == JOYSTICK_CENTER ) {

    stop();

  } else {

    // -----------------------------
    // Map to the servo value range.
    // -----------------------------

    _steering = map(_steering, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MIN, SERVO_MAX);
    _throttle = map(_throttle, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MAX, SERVO_MIN);

    // ----------------------------------------------
    // Send only the changes in the joystick position.
    // The motor will keep running a steady position.
    // ----------------------------------------------

    if ( _previousSteering != _steering || _previousThrottle != _throttle ) {
      _drive();
    }

  }

  // ---------------------------
  // Prepare for the next cycle.
  // ---------------------------

  _previousSteering = _steering;
  _previousThrottle = _throttle;
}


// ==================
//      _drive()
// ==================
void Sabertooth_DriveMotors::_drive(void)
{
  // To be determined

  _buffer->setDriveStopped(false);

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
  output = F("Sabertooth::interpretController()");
  output += F(" - Deg/mS:");

  output += F(" steering=");
  output += _steering;
  output += F("/");
  output += (544 + round(_steering * 10.311));
  if ( _steering < SERVO_CENTER ) {
    output += F(" (left) ");
  } else if ( _steering > SERVO_CENTER ) {
    output += F(" (right) ");
  }

  output += F(" throttle=");
  output += _throttle;
  output += F("/");
  output += (544 + round(_throttle * 10.311));
  if ( _throttle < SERVO_CENTER ) {
    output += F(" (reverse) ");
  } else if ( _throttle > SERVO_CENTER ) {
    output += F(" (forward) ");
  }
  printOutput();
  #endif
}


// ================
//      stop()
// ================
void Sabertooth_DriveMotors::stop(void)
{
  if ( _buffer->isDriveStopped() ) { return; }

  // To be determined

  _buffer->setDriveStopped(true);
}
#endif
