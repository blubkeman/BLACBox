/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DriveMotor.cpp - Library for supported drive motor controllers
 * Created by Brian Lubkeman, 10 May 2021
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

  m_driveStick = &m_controller->driveStick;
  m_button = &m_controller->button;

  driveEnabled   = true;
  driveStopped   = true;
  speedProfile   = WALK;
  prevConnStatus = NONE;
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
    Debug.print(DBG_INFO, F("DriveMotor"), F("begin()"), F("Dead man switch enabled."));
    #endif
  
    digitalWrite(m_pins[iDeadManPin],LOW);

  } else {

    #if defined(DEBUG)
    Debug.print(DBG_INFO, F("DriveMotor"), F("begin()"), F("Dead man switch disabled."));
    #endif
  
    digitalWrite(m_pins[iDeadManPin],HIGH);

  }
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
    Debug.print(DBG_INFO, F("DriveMotor"), F("interpretController()"), F("No controller"));
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

  if ( (m_driveStick->side == left && m_button->pressed(L1) && m_button->clicked(L3)) ||
       (m_driveStick->side == right && m_button->pressed(R1) && m_button->clicked(R3)) ) {
    m_setSpeedProfile();
  }

  // -----------------------------------
  // Look for disabling the drive stick.
  // -----------------------------------

  if ( m_button->pressed(PS) || m_button->pressed(PS2) ) {

    if ( driveEnabled && m_button->clicked(L4) ) {

      #if defined(DEBUG)
      Debug.print(DBG_INFO, F("DriveMotor"), F("interpretController()"), F("Drive motor disabled"));
      #endif

      driveEnabled = false;
      m_controller->setLed(driveEnabled, speedProfile);

    } else if ( ! driveEnabled && m_button->clicked(R4) ) {
      
      #if defined(DEBUG)
      Debug.print(DBG_INFO, F("DriveMotor"), F("interpretController()"), F("Drive motor enabled"));
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

  if ( (m_driveStick->side == left && m_button->pressed(L1)) ||
       (m_driveStick->side == right && m_button->pressed(R1)) ) {
    return;
  }

  // ---------------------------------------------------------------
  // Get the drive joystick steering (X) and throttle (Y) positions.
  // ---------------------------------------------------------------

  m_steering = m_driveStick->steering();
  m_throttle = m_driveStick->throttle();

  // -----------------------------------------------------------
  // A stick within its dead zone is treated the same as center.
  // -----------------------------------------------------------
  
  if ( abs(m_steering - m_driveStick->center) < m_driveStick->deadZone ) {
    m_steering = m_driveStick->center;
  }
  if ( abs(m_throttle - m_driveStick->center) < m_driveStick->deadZone ) {
    m_throttle = m_driveStick->center;
  }

  // -------------------------------------------
  // Stop the motors when the stick is centered.
  // Otherwise, send the drive command.
  // -------------------------------------------

  if ( m_steering == m_driveStick->center && m_throttle == m_driveStick->center ) {
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
  switch (speedProfile) {
    case WALK: {
      Debug.print(DBG_VERBOSE, F("DriveMotor"), F("m_setSpeedProfile()"), F("Speed profile set to: Walk"));
      break; }
    case JOG: {
      Debug.print(DBG_VERBOSE, F("DriveMotor"), F("m_setSpeedProfile()"), F("Speed profile set to: Jog"));
      break; }
    case RUN: {
      Debug.print(DBG_VERBOSE, F("DriveMotor"), F("m_setSpeedProfile()"), F("Speed profile set to: Run"));
      break; }
    case SPRINT: {
      Debug.print(DBG_VERBOSE, F("DriveMotor"), F("m_setSpeedProfile()"), F("Speed profile set to: Sprint"));
      break; }
    default: {
      Debug.print(DBG_VERBOSE, F("DriveMotor"), F("m_setSpeedProfile()"), F("Speed profile set to: Unknown"));
      break; }
  };
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
