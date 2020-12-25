/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Drive_Motor_Roboteq.cpp - Library for the Roboteq SBL2360 or SBL1360 drive motor controller
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Drive_Motors.h"

/* ================================================================================
 *                         Roboteq Motor Controller Class
 * ================================================================================ */

#if defined(SBL2360_PULSE) || defined(SBL1360) || defined(SBL2360_SERIAL)

// =====================
//      Constructor
// =====================
Roboteq_DriveMotors::Roboteq_DriveMotors(Buffer * pBuffer) : DriveMotor(pBuffer) {}


// ====================
//      Destructor
// ====================
// This destructor is explicitly defined to avoid a linker error.
Roboteq_DriveMotors::~Roboteq_DriveMotors(void) {}


// =================
//      begin()
// =================
void Roboteq_DriveMotors::begin(void)
{
  // -----------------------------------------------------------------------------
  // Call the parent class's begin(). It establishes our dead man switch, if used.
  // -----------------------------------------------------------------------------
  
  DriveMotor::begin();

  // -------------------------------------
  // Start communication with the Roboteq.
  // -------------------------------------

  #if defined(SLB2360_PULSE)

  #if defined(USE_SERVO)
  _steeringSignal.attach(STEERING_PIN);
  _throttleSignal.attach(THROTTLE_PIN);
  #else
  pinMode(STEERING_PIN, OUTPUT);
  pinMode(THROTTLE_PIN, OUTPUT);
  #endif

  #elif defined(SBL1360)

  #if defined(USE_SERVO)
  _leftFootSignal.attach(LEFT_FOOT_PIN);
  _rightFootSignal.attach(RIGHT_FOOT_PIN);
  #else
  pinMode(LEFT_FOOT_PIN, OUTPUT);
  pinMode(RIGHT_FOOT_PIN, OUTPUT);
  #endif

  #elif defined(SBL2360_SERIAL)

  DriveSerial.begin(ROBOTEQ_BAUD_RATE);

  #endif

  // ---------------------
  // Start the script pin.
  // ---------------------

  pinMode(SCRIPT_PIN, OUTPUT);

  // ----------
  // Debugging.
  // ----------

  #if defined (DEBUG_DRIVE) || defined(DEBUG_ALL)
  output = F("Roboteq::");
  output += F("begin()");
  output += F(" - Roboteq motor controller started.");
  printOutput();
  #endif
}


// ===============================
//      interpretController()
// ===============================
void Roboteq_DriveMotors::interpretController(void)
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

  unsigned long currentTime = millis();
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

  #if defined(SBL1360)
  // -----------------------------------------------------
  // Mix steering and throttle to produce left foot and
  // right foot motor values within the servo value range.
  // -----------------------------------------------------

  DriveMotor::_mixBHD(_steering, _throttle);
  #endif

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

    #if defined(USE_SERVO)
    _steering = map(_steering, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MIN, SERVO_MAX);
    _throttle = map(_throttle, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MAX, SERVO_MIN);
    #else
    // Do not map steering or throttle. We'll send their joystick positions.
    #endif

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
void Roboteq_DriveMotors::_drive(void)
{
  #if defined(SBL2360_PULSE)

  #if defined(USE_SERVO)
  _steeringSignal.write(_steering);
  _throttleSignal.write(_throttle);    
  #else
  analogWrite(STEERING_PIN, _steering);
  analogWrite(THROTTLE_PIN, _throttle);
  #endif

  #elif defined(SBL1360)

  #if defined(USE_SERVO)
  _leftFootSignal.write(_leftFoot);
  _rightFootSignal.write(_rightFoot);    
  #else
  analogWrite(LEFT_FOOT_PIN, _leftFoot);
  analogWrite(RIGHT_FOOT_PIN, _rightFoot);
  #endif

  #elif defined(SBL2360_SERIAL)

  String cmd = F("!G 1 "); cmd += _throttle;
  cmd += F("_!G 2 ");      cmd += _steering;
  cmd += F("\\r");
  _serialWrite(cmd);

  #endif

  _buffer->setDriveStopped(false);

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
  output = F("Roboteq::interpretController()");
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
void Roboteq_DriveMotors::stop(void)
{
  if ( _buffer->isDriveStopped() ) { return; }

  #if defined(SBL2360_PULSE)

  #if defined(USE_SERVO)
  _steeringSignal.write(SERVO_CENTER);
  _throttleSignal.write(SERVO_CENTER);
  #else
  analogWrite(STEERING_PIN, JOYSTICK_CENTER);
  analogWrite(THROTTLE_PIN, JOYSTICK_CENTER);
  #endif

  #elif defined(SBL1360)

  #if defined(USE_SERVO)
  _leftFootSignal.write(SERVO_CENTER);
  _rightFootSignal.write(SERVO_CENTER);
  #else
  analogWrite(LEFT_FOOT_PIN, JOYSTICK_CENTER);
  analogWrite(RIGHT_FOOT_PIN, JOYSTICK_CENTER);
  #endif

  #elif defined(SBL2360_SERIAL)

  _serialWrite("!MS 1_!MS 2\\r");

  #endif

  _buffer->setDriveStopped(true);
}
#endif
