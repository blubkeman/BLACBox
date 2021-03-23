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


#if defined(SBL2360) || defined(SBL1360)
/* ================================================================================
 *                         Roboteq Motor Controller Class
 * ================================================================================ */

#if defined(RS232)
const int ROBOTEQ_BAUD_RATE = 115200;     // I strongly recommend not changing this.
#endif

// =====================
//      Constructor
// =====================
#if defined(PS4_CONTROLLER)
Roboteq_DriveMotor::Roboteq_DriveMotor(Controller_PS4 * pController) : DriveMotor()
#else
Roboteq_DriveMotor::Roboteq_DriveMotor(Controller_PS3 * pController) : DriveMotor()
#endif
{
  m_controller = pController;

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
  m_className = "Roboteq_DriveMotor::";
  #endif
}

// ====================
//      Destructor
// ====================
Roboteq_DriveMotor::~Roboteq_DriveMotor(void) {}

// =================
//      begin()
// =================
void Roboteq_DriveMotor::begin(void)
{
  // -----------------------------------------------------------------------------
  // Call the parent class's begin(). It establishes our dead man switch, if used.
  // -----------------------------------------------------------------------------

  DriveMotor::begin();

  // -------------------------------------
  // Start communication with the Roboteq.
  // -------------------------------------

  #if defined(PULSE)

  m_pulse1Signal.attach(PULSE1_PIN);
  m_pulse1Signal.write(SERVO_CENTER);

  m_pulse2Signal.attach(PULSE2_PIN);
  m_pulse2Signal.write(SERVO_CENTER);

  m_scriptSignal.attach(SCRIPT_PIN);

  #elif defined(RS232)

  DriveSerial.begin(ROBOTEQ_BAUD_RATE);

  #endif

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
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
void Roboteq_DriveMotor::stop(void)
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

  #ifdef DEBUG
  output = m_className+F("stop()");
  output += F(" - ");
  output += F("Stop drive motors");
  printOutput();
  #endif

  // ----------------------
  // Send the stop command.
  // ----------------------

  #if defined(PULSE)

  m_writePulse(SERVO_CENTER);

  #elif defined(SERIAL)

  m_serialWrite(F("!MS 1_!MS 2\\r"));

  #endif

  // ------------------------
  // Update the drive status.
  // ------------------------

  driveStopped = true;
}

// ===================
//      m_drive()
// ===================
void Roboteq_DriveMotor::m_drive(void)
{
  // ---------------------------------------------------------------------------
  // Get the inputs based on the throttle and steering values from the joystick.
  // ---------------------------------------------------------------------------

  #if defined(SBL2360)

  m_mapInputs(m_steering, m_throttle);

  #elif defined(SBL1360)

  m_mixBHD(m_steering, m_throttle);

  #endif

  // -----------------------------------
  // Do nothing when there is no change.
  // -----------------------------------

  if ( m_previousInput1 == m_input1 && m_previousInput2 == m_input2 ) {
    return;
  }

  // -------------------------------
  // Send the values to the Roboteq.
  // -------------------------------

  #if defined(PULSE)

  m_writePulse(m_input1, m_input2);

  #elif defined(RS232)

  String cmd = F("!G 1 ");
  cmd += m_input1;
  cmd += F("_!G 2 ");
  cmd += m_input2;
  cmd += F("\\r");
  m_serialWrite(cmd);

  #endif

  // ------------------------
  // Update the drive status.
  // ------------------------

  driveStopped = false;

  #ifdef DEBUG
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

#if defined(SBL2360)
// =======================
//      m_mapInputs()
// =======================
void Roboteq_DriveMotor::m_mapInputs(int steering, int throttle)
{
  // ----------------------------------------------------------------------
  // Map the stick into the servo value range of 0 to 180 degrees.
  // Invert the throttle signal so that forward is high and reverse is low.
  // ----------------------------------------------------------------------

  m_input1 = map(throttle, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MAX, SERVO_MIN);
  m_input2 = map(steering, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MIN, SERVO_MAX);

  // ----------------------------------------------------------------------
  // The check the dead zone once more. This time checking the servo range.
  // ----------------------------------------------------------------------

  if ( abs(m_input1 - SERVO_CENTER) < SERVO_DEAD_ZONE ) {
    m_input1 = SERVO_CENTER;
  }
  if ( abs(m_input2 - SERVO_CENTER) < SERVO_DEAD_ZONE ) {
    m_input2 = SERVO_CENTER;
  }
}
#endif

#if defined(PULSE)
// ========================
//      m_writePulse()
// ========================
void Roboteq_DriveMotor::m_writePulse(int input1, int input2)
{
  m_pulse1Signal.write(input1);
  m_pulse2Signal.write(input2);
}
void Roboteq_DriveMotor::m_writePulse(int input)
{
  m_writePulse(input, input);
}
#endif

// ===============================
//      m_writeScript()
// ===============================
void Roboteq_DriveMotor::m_writeScript(void)
{
  m_scriptSignal.write(speedProfile);
}
#endif
