/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_SBL2360_Pulse.cpp - Library for the Roboteq SBL2360 drive motor controller using command priority Pulse
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Peripheral_SBL2360_Pulse.h"

#if defined(SBL2360_PULSE)

/* ===========================================================================
 *                       SBL2360 subclass functions
 * =========================================================================== */

// =====================
//      Constructor
// =====================
SBL2360_PWM::SBL2360_PWM(Buffer * pBuffer) : DriveMotor(pBuffer)
{
  _buffer = pBuffer;
  _previousSteering = -1;
  _previousThrottle = -1;
  _previousTime     = 0;

  // Debugging

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  _className = F("SBL2360_PWM::");
  #endif
}

// ====================
//      Destructor
// ====================
// This destructor is explicitly defined only to avoid a linker error.
SBL2360_PWM::~SBL2360_PWM(void) {}

// =================
//      begin()
// =================
void SBL2360_PWM::begin(void)
{
  // -------------------------------------------
  // Call the parent motor controller's begin().
  // -------------------------------------------
  
  DriveMotor::begin();

  // -------------------
  // Start the PWM pins.
  // -------------------

  _throttleSignal.attach(THROTTLE_PIN);
  _steeringSignal.attach(STEERING_PIN);

  // ----------------------
  // Prepare the interrupt.
  // ----------------------

  anchor = this;

  // ----------
  // Debugging.
  // ----------

  #if defined (DEBUG_DRIVE) || defined(DEBUG_ALL)
  output = _className+F("begin()");
  output += F(" - SBL2360 started.");
  printOutput();
  #endif
}

// ===============================
//      interpretController()
// ===============================
void SBL2360_PWM::interpretController(void)
{
  // -------------------------------------------------
  // Check the DeadMan switch. When L1 is not actively
  // pressed, do not process motor control input.
  // -------------------------------------------------

  #ifdef DEBUG_DRIVE
  if ( DriveMotor::_isDeadManActive() ) { return; }
  #endif

  // ------------------------------------------------------
  // Look for a change in the drive enable/disabled status.
  // ------------------------------------------------------

  DriveMotor::_toggleDriveEnabled();

  // -------------------------------------
  // Look for a change in the drive speed.
  // -------------------------------------

  DriveMotor::_toggleDriveSpeed();

  // -----------------------------------
  // Check the drive stick for movement.
  // -----------------------------------

  if ( DriveMotor::_isUserDriving() )
    _runDrive();  // We have drive input. Run the motor.
  else
    DriveMotor::stop(); // No drive input. Stop the motor if it's running.
}

// ===================
//      driveISR()
// ===================
static void SBL2360_PWM::driveISR(void)
{
  if (anchor != NULL)
    anchor->_stopDrive();
}

// =====================
//      _runDrive()
// =====================
void SBL2360_PWM::_runDrive(void)
{
  // -----------------------
  // Calculate motor inputs.
  // -----------------------

  long currentTime = millis();

  if ( (currentTime - _previousTime) > SERIAL_LATENCY ) {

    _buffer->setDriveStopped(false);

    // ---------------------------------------
    // Account for the joystick's "Dead zone".
    // ---------------------------------------

    int stickX = _buffer->getStick(LeftHatX);   // 0-127 = left,    128-255 = right
    int stickY = _buffer->getStick(LeftHatY);   // 0-127 = forward, 128-255 = reverse

    if ( abs(stickX - JOYSTICK_CENTER) < JOYSTICK_DEBUG_DRIVE_ZONE )
      stickX = JOYSTICK_CENTER;

    if ( abs(stickY - JOYSTICK_CENTER) < JOYSTICK_DEBUG_DRIVE_ZONE )
      stickY = JOYSTICK_CENTER;

    // -------------------------------------------------------
    // Determine which speed we're using: normal or overdrive.
    // -------------------------------------------------------

    byte maxDriveSpeed = DRIVESPEED1;
    if ( DriveMotor::_inOverdrive )
      maxDriveSpeed = DRIVESPEED2;

    // -----------------------------------------------------------------------------------------------
    // Constrain our max speed to within the servo value range.
    //   Servo range is 0-180 with center 90. Therefore, the min (0) is center-90 and max (180) is
    //   center+90. This means the max speed we can specify is 90, not 127 as suggested in Settings.h.
    // -----------------------------------------------------------------------------------------------

    if (maxDriveSpeed > SERVO_CENTER)
      maxDriveSpeed = SERVO_CENTER;

    // --------------------------
    // Setup the servo endpoints.
    // --------------------------

    int servoMin = SERVO_CENTER - maxDriveSpeed;
    int servoMax = SERVO_CENTER + maxDriveSpeed;

    // --------------------------------------------------------
    // Finally, map the stick coordinates into the servo values
    // range which is now constrained by maxDriveSpeed.
    // --------------------------------------------------------

    int steering = map(stickY, 0, 255, servoMin, servoMax);
    int throttle = map(stickX, 0, 255, servoMin, servoMax);

    if ( _previousThrottle != throttle || _previousSteering != steering ) {

      // ---------------------
      // Send only new inputs.
      // ---------------------

      #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
      output = _className+F("_runDrive()");
      output += F(" - Sending drive command:");
      output += F(" steering: "); output += steering;
      output += F(" throttle: "); output += throttle;
      printOutput();
      #endif

      _buffer->setDriveStopped(false);
      _steeringSignal.write(steering);
      _throttleSignal.write(throttle);
    }
  
    _previousSteering = steering;
    _previousThrottle = throttle;
  }
}

// ======================
//      _stopDrive()
// ======================
void SBL2360_PWM::_stopDrive(void)
{
  // Stop the motors.

  _throttleSignal.write(SERVO_CENTER);
  _steeringSignal.write(SERVO_CENTER);

  // Update the buffer's status.

  _buffer->setDriveStopped(true);

  // Reset the drive motor interrupt.

  digitalWrite(DRIVE_INTERRUPT_PIN, HIGH);

  #if defined(DEBUG_DRIVE) || defined (DEBUG_ALL)
  output = _className+F("_stopDrive()");
  output += F(" - Drive motors stopped.");
  printOutput();
  #endif
}
#endif
