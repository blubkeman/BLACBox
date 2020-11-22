/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_SBL2360_RS232.cpp - Library for the Roboteq SBL2360 drive motor controller using command priority RS232.
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Peripheral_SBL2360_RS232.h"

#if defined(SBL2360_SERIAL)

/* ===========================================================================
 *                       SBL2360 subclass functions
 * =========================================================================== */

// =====================
//      Constructor
// =====================
SBL2360_Serial::SBL2360_Serial(Buffer * pBuffer) : DriveMotor(pBuffer)
{
  _buffer = pBuffer;
  _previousSteering = -1;
  _previousThrottle = -1;
  _previousTime     = 0;

  // Debugging

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  _className = F("SBL2360_Serial::");
  #endif
}

// ====================
//      Destructor
// ====================
// This destructor is explicitly defined only to avoid a linker error.
SBL2360_Serial::~SBL2360_Serial(void) {}

// =================
//      begin()
// =================
void SBL2360_Serial::begin(void)
{
  // -------------------------------------------
  // Call the parent motor controller's begin().
  // -------------------------------------------
  
  DriveMotor::begin();

  // --------------------------------------------------------
  // Start the Serial communication with the Roboteq SBL2360.
  // --------------------------------------------------------

  DriveSerial.begin(SBL2360_BAUD_RATE);

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
void SBL2360_Serial::interpretController(void)
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
static void SBL2360_Serial::driveISR(void)
{
  if (anchor != NULL)
    anchor->_stopDrive();
}

// =====================
//      _runDrive()
// =====================
void SBL2360_Serial::_runDrive(void)
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

    // ---------------------------------------------------------------
    // Determine which speed we're using: normal or overdrive.
    // Scale it in accordance with the SBL2360's configured max speed.
    // ---------------------------------------------------------------

    byte maxDriveSpeed = DRIVESPEED1;
    if ( DriveMotor::_inOverdrive )
      maxDriveSpeed = DRIVESPEED2;
    maxDriveSpeed = map(maxDriveSpeed, 0, 127, 0, SBL2360_MAX_SPEED);

    // --------------------------------------------------------
    // Convert the x,y coordinates to a range of -1000 to 1000.
    // These are the values the SBL2360 expects to receive.
    // --------------------------------------------------------

    int steering;   // -1000 to -1 = left, 1 to 1000 = right, 0 = stop
    int throttle;   // -1000 to -1 = reverse, 1 to 1000 = forward, 0 = stop

    if ( stickX = JOYSTICK_CENTER )
      steering = 0;
    else if ( stickX < JOYSTICK_CENTER )
      steering = map(stickX, 0, (JOYSTICK_CENTER - JOYSTICK_DEBUG_DRIVE_ZONE), -1000, -1);
    else
      steering = map(stickX, (JOYSTICK_CENTER + JOYSTICK_DEBUG_DRIVE_ZONE), 255, 1, 1000);

    if ( stickY = JOYSTICK_CENTER )
      throttle = 0;
    else if ( stickY < JOYSTICK_CENTER )
      throttle = map(stickY, 0, (JOYSTICK_CENTER - JOYSTICK_DEBUG_DRIVE_ZONE), -1000, -1);
    else
      throttle = map(stickY, (JOYSTICK_CENTER + JOYSTICK_DEBUG_DRIVE_ZONE), 255, 1, 1000);

    // ------------------------------------
    // Create and send the command but only
    // when one of the inputs changes.
    // ------------------------------------

    String cmd = "";
    if ( maxDriveSpeed != _previousSpeed || steering != _previousSteering || throttle != _previousThrottle ) {
      cmd  = sprintf("!S 1 %s_", maxDriveSpeed);
      cmd += sprintf("!S 2 %s_", maxDriveSpeed);
      cmd += sprintf("!G 1 %s_", throttle);
      cmd += sprintf("!G 2 %s\\r", steering);
    }

    if ( cmd != "") {

      #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
      output = _className+F("_runDrive()");
      output += F(" - Sending drive command:");
      output += F(" steering: "); output += steering;
      output += F(" throttle: "); output += throttle;
      output += F(" speed:"); output += maxDriveSpeed;
      printOutput();
      #endif

      _buffer->setDriveStopped(false);
      _serialWrite(cmd);
    }

    _previousSpeed = maxDriveSpeed;
    _previousSteering = steering;
    _previousThrottle = throttle;
  }
}

// ======================
//      _stopDrive()
// ======================
void SBL2360_Serial::_stopDrive(void)
{
  // Stop the motors.

  _serialWrite("!MS 1_!MS 2\\r");

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

// ======================
//      _serialWrite()
// ======================
void SBL2360_Serial::_serialWrite(String inStr)
{
  uint8_t something;
  if (DriveSerial.available()) {
    for (int i=0; i<inStr.length(); i++) {
      DriveSerial.write(inStr[i]);
    }
  }
}

// ======================
//      _serialRead()
// ======================
void SBL2360_Serial::_serialRead(void)
{
  uint8_t something;
  if (DriveSerial.available()) {
    something = DriveSerial.read();
  }
}
#endif
