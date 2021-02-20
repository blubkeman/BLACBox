/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DriveMotor.cpp - Library for supported drive motor controllers
 * Created by Brian Lubkeman, 20 February 2021
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
DriveMotor::DriveMotor(void)
{
  driveEnabled = true;
  driveStopped = true;
  speedProfile = WALK;

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
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

  pinMode(DEADMAN_PIN,OUTPUT);

  #if defined(DEADMAN)

    #ifdef DEBUG
    output = m_className+F("begin()");
    output += F(" - ");
    output += F("Dead man switch");
    output += F(" enabled.");
    printOutput();
    #endif
  
    digitalWrite(DEADMAN_PIN,LOW);

  #else

    #ifdef DEBUG
    output = m_className+F("begin()");
    output += F(" - ");
    output += F("Dead man switch");
    output += F(" disabled.");
    printOutput();
    #endif
  
    digitalWrite(DEADMAN_PIN,HIGH);

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
  
  if ( m_controller->connectionStatus() == NONE ) {
    #ifdef DEBUG
    output = m_className+F("interpretController()");
    output += F(" - ");
    output += F("No controller");
    printOutput();
    #endif
    return;
  }

  // ---------------------------------------
  // Look for a change in the speed profile.
  // ---------------------------------------

  if ( DRIVE_STICK == 0 ) {
    if ( m_controller->getButtonPress(L1) && m_controller->getButtonClick(L3) ) {
      m_updateSpeedProfile();
    }
  } else if ( DRIVE_STICK == 1 ) {
    if ( m_controller->getButtonPress(R1) && m_controller->getButtonClick(R3) ) {
      m_updateSpeedProfile();
    }
  }

  // -----------------------------------
  // Look for disabling the drive stick.
  // -----------------------------------

  if ( m_controller->getButtonPress(PS) || m_controller->getButtonPress(PS2) ) {

    if ( driveEnabled && m_controller->getButtonPress(4) ) {

      #ifdef DEBUG
      output = m_className+F("interpretController()");
      output += F(" - ");
      output += F("Drive motor");
      output += F(" disabled");
      printOutput();
      #endif

      driveEnabled = false;
      m_controller->setLed();

    } else if ( ! driveEnabled && m_controller->getButtonPress(5) ) {
      
      #ifdef DEBUG
      output = m_className+F("interpretController()");
      output += F(" - ");
      output += F("Drive motor");
      output += F(" enabled");
      printOutput();
      #endif

      driveEnabled = true;
      m_controller->setLed();

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

    #ifdef DEBUG
    output = F("DriveMotor::interpretController()");
    output += F(" - ");
    output += F("Deadman not pressed");
    printOutput();
    #endif

    stop();
    return;
  }
  #endif

  // ------------------------------------------------------------------------
  // Flood control. Don't overload the motor controller with excessive input.
  // ------------------------------------------------------------------------

  unsigned long currentTime = millis();
  if ( (currentTime - m_previousTime) <= SERIAL_LATENCY ) {
    return;
  }
  m_previousTime = currentTime;

  // ---------------------------------------------------------------
  // Get the drive joystick steering (X) and throttle (Y) positions.
  // ---------------------------------------------------------------

  if ( DRIVE_STICK == 0 ) {

    // ------------------------------------------------------------------------------
    // Get input from the left stick (or PS3 Nav primary controller).
    // Skip this when L1 is pressed. This anticipates pressing L1+L3 to change speed.
    // ------------------------------------------------------------------------------

    if ( m_controller->getButtonPress(L1) ) {
      return;
    }
    m_steering = m_controller->getAnalogHat(LeftHatX);  // Returns the x-axis position of the joystick (range 0-255)
    m_throttle = m_controller->getAnalogHat(LeftHatY);  // Returns the y-axis position of the joystick (range 0-255)

  } else if ( DRIVE_STICK == 1 ) {

    // ------------------------------------------------------------------------------
    // Get input from the right stick (or PS3 Nav optional controller).
    // Skip this when R1 is pressed. This anticipates pressing R1+R3 to change speed.
    // ------------------------------------------------------------------------------

    if ( m_controller->getButtonPress(R1) ) {
      return;
    }
    #if defined(PS3_NAVIGATION)
    m_steering = m_secondController->getAnalogHat(LeftHatX);  // Returns the x-axis position of the joystick (range 0-255)
    m_throttle = m_secondController->getAnalogHat(LeftHatY);  // Returns the y-axis position of the joystick (range 0-255)
    #else
    m_steering = m_controller->getAnalogHat(RightHatX);  // Returns the x-axis position of the joystick (range 0-255)
    m_throttle = m_controller->getAnalogHat(RightHatY);  // Returns the y-axis position of the joystick (range 0-255)
    #endif

  } else {

    // -------------------------------------
    // Do nothing when our settings are bad.
    // -------------------------------------

    return;
  }

  // -----------------------------------------------------------
  // A stick within its dead zone is treated the same as center.
  // -----------------------------------------------------------
  
  if ( abs(m_steering - JOYSTICK_CENTER) < JOYSTICK_DEAD_ZONE ) {
    m_steering = JOYSTICK_CENTER;
  }
  if ( abs(m_throttle - JOYSTICK_CENTER) < JOYSTICK_DEAD_ZONE ) {
    m_throttle = JOYSTICK_CENTER;
  }

  // -------------------------------------------
  // Stop the motors when the stick is centered.
  // Otherwise, send the drive command.
  // -------------------------------------------

  if ( m_steering == JOYSTICK_CENTER && m_throttle == JOYSTICK_CENTER ) {
    stop();
  } else {
    m_drive();
  }
}

// ================================
//      m_updateSpeedProfile()
// ================================
void DriveMotor::m_updateSpeedProfile(void)
{
  #if ( (defined(SBL2360) || defined(SBL1360)) && (defined(PS3_CONTROLLER) || defined(PS4_CONTROLLER)) )

  // -------------------------------------------------------------------
  // For PS3 and PS4 controllers using the Roboteq motor controller,
  // the full spectrum of speed profiles is available. Cycle through it.
  // -------------------------------------------------------------------

  if ( speedProfile == SPRINT ) {
    speedProfile = WALK;
  } else {
    speedProfile++;
  }

  #else

  // -----------------------------------------------------------
  // For all other controllers, only two profiles are available.
  // -----------------------------------------------------------

  if ( speedProfile == RUN ) {
    speedProfile = WALK;
  } else {
    speedProfile = RUN;
  }

  #endif

  m_controller->setLed();
  m_writeSpeedProfile();

  #ifdef DEBUG
  output = m_className+F("m_updateSpeedProfile()");
  output += F(" - ");
  output += F("Speed profile set to: ");
  switch (speedProfile) {
    case WALK: {
      output += F("WALK");
      break;
    }
    case JOG: {
      output += F("JOG");
      break;
    }
    case RUN: {
      output += F("RUN");
      break;
    }
    case SPRINT: {
      output += F("SPRINT");
      break;
    }
  }
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

  #if defined(PS3_NAVIGATION)

    if ( m_controller->connectionStatus() == FULL ) {

      // -------------------------------------------------------------
      // We have dual Nav controllers. L2 or R2 is the deadman switch.
      // -------------------------------------------------------------

      if ( m_controller->getButtonPress(L2) || m_controller->getButtonPress(R2) ) {
        digitalWrite(DEADMAN_PIN, HIGH);
        return true;
      } else {
        digitalWrite(DEADMAN_PIN, LOW);
        return false;
      }

    } else {

      // ----------------------------------------------------------
      // We have a single Nav controller. L1 is the deadman switch.
      // ----------------------------------------------------------

      if ( m_controller->getButton(L1) ) {
        digitalWrite(DEADMAN_PIN, HIGH);
        return true;
      } else {
        digitalWrite(DEADMAN_PIN, LOW);
        return false;
      }

    }

  #elif defined(PS3_CONTROLLER) || defined(PS4_CONTROLLER)

    // -----------------------------------------------------------
    // For PS3 or PS4 controllers, L2 or R2 is the deadman switch.
    // -----------------------------------------------------------

    if ( m_controller->getButtonPress(L2) || m_controller->getButtonPress(R2) ) {
      digitalWrite(DEADMAN_PIN, HIGH);
      return true;
    } else {
      digitalWrite(DEADMAN_PIN, LOW);
      return false;
    }

  #else

    // -------------------------------------------------------
    // We do not have a controller for which code is prepared.
    // -------------------------------------------------------

    return false;

  #endif
}

#if defined(SBL1360) || defined(SABERTOOTH)
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

  if ( stickX == JOYSTICK_CENTER && stickY == JOYSTICK_CENTER ) {

    m_input1=SERVO_CENTER;
    m_input2=SERVO_CENTER;
    return;

  }

  int xInt = 0;
  int yInt = 0;

  if (stickY < JOYSTICK_CENTER) {
    yInt = map(stickY, JOYSTICK_MIN, (JOYSTICK_CENTER - JOYSTICK_DEAD_ZONE), 100, 1);
  } else {
    yInt = map(stickY, (JOYSTICK_CENTER + JOYSTICK_DEAD_ZONE), JOYSTICK_MAX, -1, -100);
  }

  if (stickX < JOYSTICK_CENTER) {
    xInt = map(stickX, JOYSTICK_MIN, (JOYSTICK_CENTER - JOYSTICK_DEAD_ZONE), -100, -1);
  } else {
    xInt = map(stickX, (JOYSTICK_CENTER + JOYSTICK_DEAD_ZONE), JOYSTICK_MAX, 1, 100);
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

  m_input1=map(leftSpeed, -100, 100, SERVO_MAX, SERVO_MIN);
  m_input2=map(rightSpeed, -100, 100, SERVO_MAX, SERVO_MIN);
}
#endif

#if defined(RS232)
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
#endif
