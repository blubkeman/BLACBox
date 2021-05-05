/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DriveMotor_Roboteq.cpp - Library for the Roboteq SBL2360 or SBL1360 drive motor controller
 * Created by Brian Lubkeman, 23 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "DriveMotor.h"

const int ROBOTEQ_BAUD_RATE = 115200;     // I strongly recommend not changing this.


/* ================================================================================
 *                         Roboteq Motor Controller Class
 * ================================================================================ */

// =====================
//      Constructor
// =====================
DriveMotor_Roboteq::DriveMotor_Roboteq(
    Controller* pController,
    const int settings[],
    const byte pins[])
  : DriveMotor(pController, settings, pins)
{
  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  m_className = "DriveMotor_Roboteq::";
  #endif
}

// ====================
//      Destructor
// ====================
DriveMotor_Roboteq::~DriveMotor_Roboteq(void) {}

// =================
//      begin()
// =================
void DriveMotor_Roboteq::begin(void)
{
  // -----------------------------------------------------------------------------
  // Call the parent class's begin(). It establishes our dead man switch, if used.
  // -----------------------------------------------------------------------------

  DriveMotor::begin();

  // -------------------------------------
  // Start communication with the Roboteq.
  // -------------------------------------

  m_scriptSignal.attach(m_pins[iScriptPin]);

  if ( m_settings[iCommMode] == 0 ) {

    // Pulse mode

    m_pulse1Signal.attach(m_pins[iDrivePin1]);
    m_pulse1Signal.write(m_servoCenter);

    m_pulse2Signal.attach(m_pins[iDrivePin2]);
    m_pulse2Signal.write(m_servoCenter);

  } else if ( m_settings[iCommMode] == 1 ) {

    // RS232 (Serial) mode

    DriveSerial.begin(ROBOTEQ_BAUD_RATE);

  } else {

    // Unknown communication type setting.

    return;
  }

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  output = m_className+F("begin()");
  output += F(" - ");
  output += F("Roboteq");
  output += F(" motor controller started.");
  printOutput();
  #endif
}

// ================
//      stop()
// ================
void DriveMotor_Roboteq::stop(void)
{
  // --------------------------------------------------
  // When the motor is already not running, do nothing.
  // --------------------------------------------------

  if ( driveStopped ) {
    return;
  }

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  output = m_className+F("stop()");
  output += F(" - ");
  output += F("Stop drive motors");
  printOutput();
  #endif

  // ----------------------
  // Send the stop command.
  // ----------------------

  if ( m_settings[iCommMode] == 0 ) {

    // Pulse mode

    m_writePulse(m_servoCenter);

  } else if ( m_settings[iCommMode] == 1 ) {

    // RS232 (Serial) mode

    m_serialWrite(F("!MS 1_!MS 2\\r"));

  } else {

    // Unknown communication type setting.

    return;
  }

  // ------------------------
  // Update the drive status.
  // ------------------------

  driveStopped = true;
}

// ===================
//      m_drive()
// ===================
void DriveMotor_Roboteq::m_drive(void)
{
  // ---------------------------------------------------------------------------
  // Get the inputs based on the throttle and steering values from the joystick.
  // ---------------------------------------------------------------------------

  if ( m_settings[iMixing] == 0 ) {

    // Mixing is done by the motor driver.

    m_mapInputs(m_steering, m_throttle);

  } else if ( m_settings[iMixing] == 1 ) {

    // Mixing is handled by this sketch.

    m_mixBHD(m_steering, m_throttle);

  } else {

    // Unknown mixing setting.

    return;
  }

  // -----------------------------------
  // Do nothing when there is no change.
  // -----------------------------------

  if ( m_previousInput1 == m_input1 && m_previousInput2 == m_input2 ) {
    return;
  }

  // -------------------------------
  // Send the values to the Roboteq.
  // -------------------------------

  if ( m_settings[iCommMode] == 0 ) {

    // Pulse mode

    m_writePulse(m_input1, m_input2);
  } else if ( m_settings[iCommMode] == 1 ) {

    // RS232 (Serial) mode

    String cmd = F("!G 1 ");
    cmd += m_input1;
    cmd += F("_!G 2 ");
    cmd += m_input2;
    cmd += F("\\r");
    m_serialWrite(cmd);

  } else {

    // Unknown communication type setting.

    return;
  }

  // ------------------------
  // Update the drive status.
  // ------------------------

  driveStopped = false;

  #if defined(DEBUG)
  output = m_className+F("m_drive()");
  output += F(" - ");
  output += F("Throttle/Steering: ");
  output += m_input1;
  output += F("/");
  output += m_input2;
  output += F("\tmS:");
  output += (544 + round(m_input1 * 10.311));
  output += F("/");
  output += (544 + round(m_input2 * 10.311));
  printOutput();
  #endif

  // ----------------------------
  // Remember the stick position.
  // ----------------------------

  m_previousInput1 = m_input1;
  m_previousInput2 = m_input2;
}

// =======================
//      m_mapInputs()
// =======================
void DriveMotor_Roboteq::m_mapInputs(int steering, int throttle)
{
  // ----------------------------------------------------------------------
  // Map the stick into the servo value range of 0 to 180 degrees.
  // Invert the throttle signal so that forward is high and reverse is low.
  // ----------------------------------------------------------------------

  m_input1 = map(throttle, m_joystick->minValue, m_joystick->maxValue, m_servoMax, m_servoMin);
  m_input2 = map(steering, m_joystick->minValue, m_joystick->maxValue, m_servoMin, m_servoMax);

  // ----------------------------------------------------------------------
  // The check the dead zone once more. This time checking the servo range.
  // ----------------------------------------------------------------------

  if ( abs(m_input1 - m_servoCenter) < m_settings[iServoDeadZone] ) {
    m_input1 = m_servoCenter;
  }
  if ( abs(m_input2 - m_servoCenter) < m_settings[iServoDeadZone] ) {
    m_input2 = m_servoCenter;
  }
}

// ========================
//      m_writePulse()
// ========================
void DriveMotor_Roboteq::m_writePulse(int input1, int input2)
{
  m_pulse1Signal.write(input1);
  m_pulse2Signal.write(input2);
}
void DriveMotor_Roboteq::m_writePulse(int input)
{
  m_writePulse(input, input);
}

// ===============================
//      m_writeScript()
// ===============================
void DriveMotor_Roboteq::m_writeScript(void)
{
  m_scriptSignal.write(speedProfile);
}
