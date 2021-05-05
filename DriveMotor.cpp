/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DriveMotor.cpp - Library for supported drive motor controllers
 * Created by Brian Lubkeman, 5 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "DriveMotor.h"


/* ================================================================================
 *                         Parent Controller Functions
 * ================================================================================ */
// =====================
//      Constructor
// =====================
DriveMotor::DriveMotor(Controller* pController, const int settings[], const byte pins[])
{
  m_controller = pController;
  m_settings   = settings;
  m_pins       = pins;

  m_joystick = &m_controller->driveStick;
  m_button = &m_controller->button;

  driveEnabled   = true;
  driveStopped   = true;
  speedProfile   = WALK;
  prevConnStatus = NONE;

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  m_className = F("DriveMotor::");
  #endif
}

// ====================
//      Destructor
// ====================
DriveMotor::~DriveMotor() {}

// =================
//      begin()
// =================
void DriveMotor::begin()
{
  // --------------------------------
  // Set up a deadman switch, or not.
  // --------------------------------

  pinMode(m_pins[iDeadManPin],OUTPUT);

  if ( m_settings[iDeadMan] ) {

    #if defined(DEBUG)
    output = m_className+F("begin()");
    output += F(" - ");
    output += F("Dead man switch");
    output += F(" enabled.");
    printOutput();
    #endif
  
    digitalWrite(m_pins[iDeadManPin],LOW);

  } else {

    #if defined(DEBUG)
    output = m_className+F("begin()");
    output += F(" - ");
    output += F("Dead man switch");
    output += F(" disabled.");
    printOutput();
    #endif
  
    digitalWrite(m_pins[iDeadManPin],HIGH);

  }
}

// =======================
//      displayInit()
// =======================
void DriveMotor::displayInit(void)
{
  #if defined(DEBUG)
  output += F("\n  Drive enable:  ");
  if ( driveEnabled ) {
    output += F("true");
  } else {
    output += F("false");
  }
  output += F("\n  Drive stopped: ");
  if ( driveStopped ) {
    output += F("true");
  } else {
    output += F("false");
  }
  output += F("\n  Speed profile: ");
  switch (speedProfile) {
    case WALK:   { output += F("Walk");   break; }
    case JOG:    { output += F("Jog");    break; }
    case RUN:    { output += F("Run");    break; }
    case SPRINT: { output += F("Sprint"); break; }
    default:     { output += F("Unknown"); }
  };
  #endif
}

// ===============================
//      interpretController()
// ===============================
void DriveMotor::interpretController(void)
{
  // -------------------------------------------------
  // Do nothing when there is no controller connected.
  // -------------------------------------------------

  byte connStatus = m_controller->connectionStatus();
  if ( connStatus == NONE ) {

    if ( prevConnStatus != NONE ) {
      prevConnStatus = NONE;
    }
    
    #if defined(DEBUG)
    output = m_className+F("interpretController()");
    output += F(" - ");
    output += F("No controller");
    printOutput();
    #endif
    return;
  }

  // ----------------------------------------------------------
  // When there is a new controller connection, update the LED.
  // ----------------------------------------------------------

  if ( prevConnStatus == NONE ) {
    prevConnStatus = connStatus;
    m_controller->setLed(driveEnabled, speedProfile);
  }

  // ---------------------------------------
  // Look for a change in the speed profile.
  // ---------------------------------------

  if ( (m_joystick->side == left && m_button->pressed(L1) && m_button->clicked(L3)) ||
       (m_joystick->side == right && m_button->pressed(R1) && m_button->clicked(R3)) ) {
    m_setSpeedProfile();
  }

  // -----------------------------------
  // Look for disabling the drive stick.
  // -----------------------------------

  if ( m_button->pressed(PS) || m_button->pressed(PS2) ) {

    if ( driveEnabled && m_button->clicked(L4) ) {

      #if defined(DEBUG)
      output = m_className+F("interpretController()");
      output += F(" - ");
      output += F("Drive motor");
      output += F(" disabled");
      printOutput();
      #endif

      driveEnabled = false;
      m_controller->setLed(driveEnabled, speedProfile);

    } else if ( ! driveEnabled && m_button->clicked(R4) ) {
      
      #if defined(DEBUG)
      output = m_className+F("interpretController()");
      output += F(" - ");
      output += F("Drive motor");
      output += F(" enabled");
      printOutput();
      #endif

      driveEnabled = true;
      m_controller->setLed(driveEnabled, speedProfile);

    }
  }

  // --------------------------------------------------
  // Stop the mootors when the drive stick is disabled.
  // --------------------------------------------------

  if ( ! driveEnabled ) {
    stop();
    return;
  }

  #if defined(DEADMAN)
  // ----------------------------------------------------------------
  // Stop the motors when a deadman switch is in use but not pressed.
  // ----------------------------------------------------------------

  if ( ! m_isDeadmanPressed() ) {
    stop();
    return;
  }
  #endif

  // ------------------------------------------------------------------------
  // Flood control. Don't overload the motor controller with excessive input.
  // ------------------------------------------------------------------------

  unsigned long currentTime = millis();
  if ( (currentTime - m_previousTime) <= m_settings[iDriveLatency] ) {
    return;
  }
  m_previousTime = currentTime;

  // -------------------------------------------------------
  // When pressing L1|R1, we anticipate L3|R3 to be pressed.
  // Skip reading the joystick in this case.
  // -------------------------------------------------------

  if ( (m_joystick->side == left && m_button->pressed(L1)) ||
       (m_joystick->side == right && m_button->pressed(R1)) ) {
    return;
  }

  // ---------------------------------------------------------------
  // Get the drive joystick steering (X) and throttle (Y) positions.
  // ---------------------------------------------------------------

  m_steering = m_joystick->steering();
  m_throttle = m_joystick->throttle();

  // -----------------------------------------------------------
  // A stick within its dead zone is treated the same as center.
  // -----------------------------------------------------------
  
  if ( abs(m_steering - m_joystick->center) < m_joystick->deadZone ) {
    m_steering = m_joystick->center;
  }
  if ( abs(m_throttle - m_joystick->center) < m_joystick->deadZone ) {
    m_throttle = m_joystick->center;
  }

  // -------------------------------------------
  // Stop the motors when the stick is centered.
  // Otherwise, send the drive command.
  // -------------------------------------------

  if ( m_steering == m_joystick->center && m_throttle == m_joystick->center ) {
    stop();
  } else {
    m_drive();
  }
}

// ================================
//      m_setSpeedProfile()
// ================================
void DriveMotor::m_setSpeedProfile(void)
{
  if ( m_settings[iMotorDriver] == 0 && m_controller->getType() > 0 ) {
    
    // ---------------------------------------------------------------------
    // For PS3, PS4, and PS5 controllers using the Roboteq motor controller,
    // the full spectrum of speed profiles is available. Cycle through it.
    // ---------------------------------------------------------------------

    if ( speedProfile == SPRINT ) {
      speedProfile = WALK;
    } else {
      speedProfile++;
    }

  } else {

    // -----------------------------------------------------------
    // For all other controllers, only two profiles are available.
    // -----------------------------------------------------------

    if ( speedProfile == RUN ) {
      speedProfile = WALK;
    } else {
      speedProfile = RUN;
    }

  }

  m_controller->setLed(driveEnabled, speedProfile);
  m_writeScript();

  #if defined(DEBUG)
  output = m_className+F("m_setSpeedProfile()");
  output += F(" - ");
  output += F("Speed profile set to: ");
  switch (speedProfile) {
    case WALK:   { output += F("Walk");   break; }
    case JOG:    { output += F("Jog");    break; }
    case RUN:    { output += F("Run");    break; }
    case SPRINT: { output += F("Sprint"); break; }
    default:     { output += F("Unknown"); }
  };
  printOutput();
  #endif
}

// =============================
//      m_isDeadmanPressed()
// =============================
bool DriveMotor::m_isDeadmanPressed(void)
{
  // ---------------------------------------------
  // Check the deadman switch.  The deadman switch
  // varies a little for different controllers.
  // ---------------------------------------------

  if ( m_controller->getType() == 0) {

    if ( m_controller->connectionStatus() == FULL ) {

      // -------------------------------------------------------------
      // We have dual Nav controllers. L2 or R2 is the deadman switch.
      // -------------------------------------------------------------

      if ( m_button->pressed(L2) || m_button->pressed(R2) ) {
        digitalWrite(m_pins[iDeadMan], HIGH);
        return true;
      } else {
        digitalWrite(m_pins[iDeadMan], LOW);
        return false;
      }

    } else {

      // ----------------------------------------------------------
      // We have a single Nav controller. L1 is the deadman switch.
      // ----------------------------------------------------------

      if ( m_button->pressed(L1) ) {
        digitalWrite(m_pins[iDeadMan], HIGH);
        return true;
      } else {
        digitalWrite(m_pins[iDeadMan], LOW);
        return false;
      }

    }

  } else if ( m_controller->getType() > 0 ) {

    // -----------------------------------------------------------------
    // For PS3, PS4, or PS5 controllers, L2 or R2 is the deadman switch.
    // -----------------------------------------------------------------

    if ( m_button->pressed(L2) || m_button->pressed(R2) ) {
      digitalWrite(m_pins[iDeadMan], HIGH);
      return true;
    } else {
      digitalWrite(m_pins[iDeadMan], LOW);
      return false;
    }

  } else {

    // -------------------------------------------------------
    // We do not have a controller for which code is prepared.
    // -------------------------------------------------------

    return false;

  }
}

// ====================
//      m_mixBHD()
// ====================
void DriveMotor::m_mixBHD(byte stickX, byte stickY) {
/* =============================================================
 *  This is my interpretation of BigHappyDude's mixing function for differential (tank) style driving using two motors.  
 *  We will take the joystick's X and Y positions, mix these into a diamond matrix, and convert to a servo value range 
 *   (0-180) for the left and right foot motors.
 *  The maximum drive speed BHD used is excluded in this version as that is handled through a script in the Roboteq.
 *  If you wish to understand how this works, please see the comments in the mixBHD function implemented into the
 *   SHADOW_MD_Q85 code.
 * ============================================================= */

  if ( stickX == m_joystick->center && stickY == m_joystick->center ) {

    m_input1=m_servoCenter;
    m_input2=m_servoCenter;
    return;

  }

  int xInt = 0;
  int yInt = 0;

  if (stickY < m_joystick->center) {
    yInt = map(stickY, m_joystick->minValue, (m_joystick->center - m_joystick->deadZone), 100, 1);
  } else {
    yInt = map(stickY, (m_joystick->center + m_joystick->deadZone), m_joystick->maxValue, -1, -100);
  }

  if (stickX < m_joystick->center) {
    xInt = map(stickX, m_joystick->minValue, (m_joystick->center - m_joystick->deadZone), -100, -1);
  } else {
    xInt = map(stickX, (m_joystick->center + m_joystick->deadZone), m_joystick->maxValue, 1, 100);
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
//      m_serialWrite()
// =======================
void DriveMotor::m_serialWrite(String inStr)
{
  if (DriveSerial.available()) {
    for (int i=0; i<inStr.length(); i++) {
      DriveSerial.write(inStr[i]);
    }
  }
}
