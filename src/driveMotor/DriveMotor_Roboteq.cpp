/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DriveMotor_Roboteq.cpp - Library for the Roboteq SBL2360 or SBL1360 drive motor controller
 * Created by Brian Lubkeman, 16 June 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "DriveMotor.h"

const int ROBOTEQ_BAUD_RATE = 115200;     // I strongly recommend not changing this.

enum comm_mode_e {
  Pulse,
  RS232
};

enum mixing_e {
  byDriver,
  bySketch
};

/* ================================================================================
 *                         Roboteq Motor Controller Class
 * ================================================================================ */

// =====================
//      Constructor
// =====================
DriveMotor_Roboteq::DriveMotor_Roboteq
  ( Controller* pController,
    const int settings[],
    const byte pins[],
    const byte roboteqSettings[] )
  : DriveMotor(pController, settings, pins)
{
  m_roboteqSettings = roboteqSettings;
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

  if ( m_roboteqSettings[iCommMode] == Pulse ) {

    // Pulse mode

    m_pulse1Signal.attach(m_pins[iDrivePin1]);
    m_pulse1Signal.write(m_servoCenter);

    m_pulse2Signal.attach(m_pins[iDrivePin2]);
    m_pulse2Signal.write(m_servoCenter);

  } else if ( m_settings[iCommMode] == RS232 ) {

    // RS232 (Serial) mode

    DriveMotor_Serial.begin(ROBOTEQ_BAUD_RATE);

  } else {

    // Unknown communication type setting.

    return;
  }

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("DriveMotor_Roboteq"), F("begin()"), F("Roboteq motor controller started"));
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

  #if defined(DEBUG)
  Debug.print(DBG_WARNING, F("DriveMotor_Roboteq"), F("stop()"), F("Stop drive motors"));
  #endif

  // ----------------------
  // Send the stop command.
  // ----------------------

  if ( m_settings[iCommMode] == Pulse ) {

    // Pulse mode

    m_writePulse(m_servoCenter);

  } else if ( m_settings[iCommMode] == RS232 ) {

    // RS232 (Serial) mode

    m_writeSerial(F("!MS 1_!MS 2\\r"));

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

  if ( m_roboteqSettings[iMixing] == byDriver ) {

    // Mixing is done by the motor driver.

    m_analogToServo(m_throttle, m_steering);

  	#if defined(DEBUG)
  	String msg = (String)m_input1;
  	msg += F("/");
  	msg += (String)m_input2;
  	msg += F(" (");
  	msg += (String)(544 + round(m_input1 * 10.311));
  	msg += F("/");
  	msg += (String)(544 + round(m_input2 * 10.311));
  	msg += F(")");
  	char buff[22];
  	msg.toCharArray(buff, 22);
  	Debug.print(DBG_VERBOSE, F("DriveMotor_Roboteq"), F("m_drive()"), F("Throttle/Steering: "), (String)buff);
  	#endif
	
  } else if ( m_roboteqSettings[iMixing] == bySketch ) {

    // Mixing is handled by this sketch.

    m_mixBHD(m_throttle, m_steering);

  } else {

    // Unknown mixing setting.

    return;
  }

  // ------------------------
  // Update the drive status.
  // ------------------------

  driveStopped = false;

  // -------------------------------
  // Send the values to the Roboteq.
  // -------------------------------

  switch (m_settings[iCommMode]) {

    case Pulse:
      // Pulse mode
      m_writePulse(m_input1, m_input2);
      break;

    case RS232:
      // RS232 (Serial) mode
      char cmd[22];
      sprintf(cmd, "!G 1 %i_!G 2 %i\\r", m_input1, m_input2);
      m_writeSerial(cmd);
      break;

    default:
      // Unknown communication type setting.
      return;
  }

  // ----------------------------
  // Remember the stick position.
  // ----------------------------

  m_previousInput1 = m_input1;
  m_previousInput2 = m_input2;
}

// ===========================
//      m_analogToServo()
// ===========================
void DriveMotor_Roboteq::m_analogToServo(int throttle, int steering)
{
  // ----------------------------------------------------------------------
  // Map the joystick into the servo value range of 0 to 180 degrees.
  // ----------------------------------------------------------------------

  m_input1 = map(throttle, m_driveStick->minValue, m_driveStick->maxValue, m_servoMin, m_servoMax);
  m_input2 = map(steering, m_driveStick->minValue, m_driveStick->maxValue, m_servoMin, m_servoMax);

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
  // Our input is in degrees (0-180), however the m_pulseXSignal variables
  // are Servo objects. That class converts degrees to milliseconds when
  // writing its data out. Thus the output value to the Roboteq is in the
  // range 544-2400 with 1472 as center.

  m_pulse1Signal.write(input1);		// throttle (2360) or left foot (1360)
  m_pulse2Signal.write(input2);		// steering (2360) or right foot (1360)

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

  int output = 45 + (speedProfile * 45);	// yields: WALK = 45, JOG = 90, RUN = 135, SPRINT = 180;

  #if defined(DEBUG)
  Debug.print(DBG_VERBOSE, F("DriveMotor_Roboteq"), F("m_writeScript()"), F("Speed profile: "), (String)output);
  #endif

  m_scriptSignal.write(output);
}


// ====================
//      m_mixBHD()
// ====================
void DriveMotor_Roboteq::m_mixBHD(byte throttle, byte steering) {
/* =============================================================
 *  This is my interpretation of BigHappyDude's mixing function for differential (tank) style driving using two motors.  
 *  We will take the joystick's X (steering) and Y (throttle) positions, mix these into a diamond matrix, and convert
 *   to a servo value range (0-180) for the left and right foot motors.
 *  The maximum drive speed BHD used is excluded in this version as that is handled through a script in the Roboteq.
 *  If you wish to understand how this works, please see the comments in the mixBHD function implemented into the
 *   SHADOW_MD_Q85 code.
 * ============================================================= */

  if ( steering == m_driveStick->center && throttle == m_driveStick->center ) {

    m_input1=m_servoCenter;
    m_input2=m_servoCenter;
    return;

  }

  int xInt = 0;
  int yInt = 0;

  if (throttle < m_driveStick->center) {
    yInt = map(throttle, m_driveStick->minValue, (m_driveStick->center - m_driveStick->deadZone), 100, 1);
  } else {
    yInt = map(throttle, (m_driveStick->center + m_driveStick->deadZone), m_driveStick->maxValue, -1, -100);
  }

  if (steering < m_driveStick->center) {
    xInt = map(steering, m_driveStick->minValue, (m_driveStick->center - m_driveStick->deadZone), -100, -1);
  } else {
    xInt = map(steering, (m_driveStick->center + m_driveStick->deadZone), m_driveStick->maxValue, 1, 100);
  }

  float xFloat = xInt;
  float yFloat = yInt;

  if ( yInt > (xInt + 100) ) {
    xFloat = -100 / (1 - (yFloat / xFloat));
    yFloat = xFloat + 100;
  } else if ( yInt > (100 - xInt) ) {
    xFloat = -100 / (-1 - (yFloat / xFloat));
    yFloat = -xFloat + 100;
  } else if (yInt < (-xInt - 100)) {
    xFloat = 100 / (-1 - (yFloat / xFloat));
    yFloat = -xFloat - 100;
  } else if (yInt < (xInt - 100)) {
    xFloat = 100 / (1 - (yFloat / xFloat));
    yFloat = xFloat - 100;
  }

  float leftSpeed = ((xFloat + yFloat - 100) / 2) + 100;
  leftSpeed = (leftSpeed - 50) * 2;

  float rightSpeed = ((yFloat - xFloat - 100) / 2) + 100;
  rightSpeed = (rightSpeed - 50) * 2;

  m_input1=map(leftSpeed, -100, 100, m_servoMax, m_servoMin);
  m_input2=map(rightSpeed, -100, 100, m_servoMax, m_servoMin);
}

// =======================
//      m_writeSerial()
// =======================
void DriveMotor_Roboteq::m_writeSerial(String inStr)
{
  if (DriveMotor_Serial.available()) {
    for (int i=0; i<inStr.length(); i++) {
      DriveMotor_Serial.write(inStr[i]);
    }
  }
}
