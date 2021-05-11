/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DomeMotor.cpp - Library for supported dome motor controllers
 * Created by Brian Lubkeman, 10 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "DomeMotor.h"


/* ================================================================================
 *                         Parent Controller Functions
 * ================================================================================ */
// =====================
//      Constructor
// =====================
DomeMotor::DomeMotor(Controller* pController, const byte settings[], const unsigned long timings[])
{
  m_controller = pController;
  m_settings = settings;
  m_timings  = timings;

  m_domeStick = &m_controller->domeStick;
  m_button = &m_controller->button;

  m_rotationStatus = STOPPED;

  m_targetPosition = 0;  // (0 - 359) - degrees in a circle, 0 = home
  m_stopTurnTime   = 0;
  m_startTurnTime  = 0;
  m_previousTime   = 0;

  m_automationRunning = false;
  m_automationSettingsInvalid = false;
}

// ====================
//      Destructor
// ====================
DomeMotor::~DomeMotor() {}

// =================
//      begin()
// =================
void DomeMotor::begin(void)
{
  // ---------------------------------------------------
  // Prepare the random number seed for dome automation.
  // ---------------------------------------------------

  randomSeed(analogRead(0));

  // ----------------------------------
  // Validate dome automation settings.
  // ----------------------------------

  if ( m_timings[iTurn360] < m_timings[iTurn360Min] ||
       m_timings[iTurn360] > m_timings[iTurn360Max] ||
       m_settings[iAutoSpeed] < m_settings[iAutoSpeedMin] ||
       m_settings[iAutoSpeed] > m_settings[iAutoSpeedMax] ) {

    m_automationSettingsInvalid = true;

    #if defined(DEBUG)
    Debug.print(DBG_ERROR, F("DomeMotor"), F("begin()"), F("Invalid settings"));
    Debug.print(DBG_VERBOSE, F("  Turn time: "),  (String)m_timings[iTurn360]);
    Debug.print(DBG_VERBOSE, F("\t Min: "),       (String)m_timings[iTurn360Min]);
    Debug.print(DBG_VERBOSE, F("\t Max: "),       (String)m_timings[iTurn360Max]);
    Debug.print(DBG_VERBOSE, F("  Dome speed: "), (String)m_settings[iAutoSpeed]);
    Debug.print(DBG_VERBOSE, F("\t Min: "),       (String)m_settings[iAutoSpeedMin]);
    Debug.print(DBG_VERBOSE, F("\t Max: "),       (String)m_settings[iAutoSpeedMax]);
    #endif
  }
}

// ===============================
//      interpretController()
// ===============================
void DomeMotor::interpretController(void)
{
/* ===============================================
 *
 *                            PS3 Navigation    PS3|PS4|PS5 Controller
 *                            ==============    ===========================
 *  Enable dome automation    L2|R2 + Circle    L2|R2 + Select|Share|Create
 *  Disable dome automation   L2|R2 + Cross     L2|R2 + Start|Options
 *
 *                            Dual PS3 Navs     Single PS3 Nav
 *                            ===============   ==================
 *  Manual dome rotation      Secondary stick   L2 + primary stick
 *
 * =============================================== */  

  // -------------------------------------------
  // When no controller is found stop the motor.
  // -------------------------------------------

  if ( m_controller->connectionStatus() == NONE ) {
    #if defined(DEBUG)
    Debug.print(DBG_WARNING, F("DomeMotor"), F("interpretController()"), F("No controller"));
    #endif
    return;
  }


  // ----------------------------------------
  // Look for dome automation enable/disable.
  // ----------------------------------------

  if ( m_button->pressed(L2) || m_button->pressed(R2) ) {
    if ( m_button->clicked(L4) && isAutomationRunning() ) {
      m_automationOff();
    } else if ( m_button->clicked(R4) && ! isAutomationRunning() ) {
      m_automationOn();
    }
  }

  // ------------------------------------------------------------------------
  // Flood control. Don't overload the motor controller with excessive input.
  // ------------------------------------------------------------------------

  unsigned long currentTime = millis();
  if ( (currentTime - m_previousTime) < (m_settings[iDomeLatency] * 2) ) {
    return;
  }
  m_previousTime = currentTime;

  // ================================
  // Look for dome rotation commands.
  // ================================

  // When L1|R1 is pressed, anticipate L3|R3 being pressed.
  // When this happens, do not read the stick position.

  if ( (m_domeStick->side == left && m_button->pressed(L1)) ||
       (m_domeStick->side == right && m_button->pressed(R1)) ) {
    return;
  }

  // --------------------------
  // Get the joystick position.
  // --------------------------

  byte stickPosition = m_domeStick->center;

  if ( m_controller->getType() != 0 ) {
    // The controller is a PS3, PS4, or PS5 controller.
    stickPosition = m_domeStick->rotation();
  } else if ( m_controller->connectionStatus() == FULL ) {
    // The controller is a pair of PS3 Move Navigations.
    stickPosition = m_domeStick->rotation();
  } else if ( m_button->pressed(L2) ) {
    // The controller is a single PS3 Move Navigation.
    stickPosition = m_domeStick->rotation();
  } else {
    return;
  }

  // -----------------------------------------------------
  // A stick within its dead zone is the same as centered.
  // Stop dome rotation when the stick is centered.
  // -----------------------------------------------------
  
  if ( abs(stickPosition - m_domeStick->center) < m_domeStick->deadZone ) {
    stop();
    return;
  }

  // --------------------------------------------------
  // Convert the joystick position to a rotation speed.
  // --------------------------------------------------

  int rotationSpeed = map(stickPosition, m_domeStick->minValue, m_domeStick->maxValue, -m_settings[iDomeSpeed], m_settings[iDomeSpeed]);

  // -------------------------------------------
  // Turn off dome automation if manually moved.
  // -------------------------------------------

  if ( rotationSpeed != 0 && isAutomationRunning() ) {
    m_automationOff();
  }

  // --------------------------
  // Send the rotation command.
  // --------------------------

  m_rotateDome(rotationSpeed);
}


// ============================================
//          Dome automation functions
// ============================================


// ===============================
//      isAutomationRunning()
// ===============================
bool DomeMotor::isAutomationRunning(void)
{
  return m_automationRunning;
}

// ==========================
//      m_automationOn()
// ==========================
void DomeMotor::m_automationOn(void)
{
  m_automationRunning = true;

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("DomeMotor"), F("automationOn()"), F("Dome automation"), F("enabled."));
  #endif
}

// ===========================
//      m_automationOff()
// ===========================
void DomeMotor::m_automationOff(void)
{
  m_automationRunning = false;
  m_rotationStatus = STOPPED;
  m_targetPosition = 0;

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("DomeMotor"), F("automationOff()"), F("Dome automation"), F("disabled."));
  #endif
}

// =========================
//      runAutomation()
// =========================
void DomeMotor::runAutomation(void)
{
  // ----------------------------------------------
  // Do not run automation if settings are invalid.
  // ----------------------------------------------

  if ( m_automationSettingsInvalid ) {
    return;
  }

  // ------------------------------------------------------------
  // Call the function appropriate to the current rotation cycle.
  // ------------------------------------------------------------

  switch(m_rotationStatus) {
    case STOPPED:
      m_automationInit();
      break;
    case READY:
      m_automationReady();
      break;
    case TURNING:
      m_automationTurn();
      break;
  }
}

// =========================
//     m_automationInit()
// =========================
void DomeMotor::m_automationInit(void)
{
  unsigned long currentTime = millis();

  if ( m_targetPosition == 0 ) {

    // -----------------------------------------------
    // Previous position was home. Set a new position.
    // -----------------------------------------------

    m_startTurnTime = currentTime + (random(3, 11) * 1000); // Wait 3-10 seconds before turning.
    m_targetPosition = random(5,354);
    if ( m_targetPosition < 180 ) {
      m_stopTurnTime = m_startTurnTime + ((int)m_targetPosition / (float)360.0 * (unsigned long)m_timings[iTurn360]);
      m_turnDirection = 1;
    } else {
      m_stopTurnTime = m_startTurnTime + (((float)360.0 - (int)m_targetPosition) / (float)360.0 * (unsigned long)m_timings[iTurn360]);
      m_turnDirection = -1;
    }

  } else {

    // ----------------------------------------------
    // Previous positon was not home. Return to home.
    // ----------------------------------------------

    m_startTurnTime = currentTime + (random(1,6) * 1000); // Wait 1-5 seconds before returning home.
    if ( m_targetPosition < 180 ) {
      m_stopTurnTime = m_startTurnTime + ((int)m_targetPosition / (float)360.0 * (unsigned long)m_timings[iTurn360]);
    } else {
      m_stopTurnTime = m_startTurnTime + (((float)360 - (int)m_targetPosition) / (float)360.0 * (unsigned long)m_timings[iTurn360]);
    }
    m_turnDirection *= -1;
    m_targetPosition = 0;
  }

  // ---------------------------
  // Advance the rotation cycle.
  // ---------------------------
  
  m_rotationStatus = READY;

  #if defined(DEBUG)
  Debug.print(DBG_VERBOSE, F("DomeMotor"), F("m_automationInit()"), F("Turn set"));
  Debug.print(DBG_VERBOSE, F("  Current time: "),    (String)currentTime);
  Debug.print(DBG_VERBOSE, F("  Target position: "), (String)m_targetPosition);
  Debug.print(DBG_VERBOSE, F("  Next start time: "), (String)m_startTurnTime);
  Debug.print(DBG_VERBOSE, F("  Next stop time:  "), (String)m_stopTurnTime);
  #endif
}

// ==========================
//     m_automationReady()
// ==========================
void DomeMotor::m_automationReady(void)
{
  if (m_startTurnTime < millis()) {

    // ---------------------------
    // Advance the rotation cycle.
    // ---------------------------

    m_rotationStatus = TURNING;

    #if defined(DEBUG)
    Debug.print(DBG_INFO, F("DomeMotor"), F("m_automationReady()"), F("Ready to turn"));
    #endif
  }
}

// ===========================
//     m_automationTurn()
// ===========================
void DomeMotor::m_automationTurn(void)
{
  if ( millis() < m_stopTurnTime ) {

    // ------------------------------------------------------
    // Actively turn the dome until it reaches its stop time.
    // ------------------------------------------------------

    #if defined(DEBUG)
    Debug.print(DBG_INFO, F("DomeMotor"), F("m_automationTurn()"), F("Turning"));
    #endif

    int rotationSpeed = m_settings[iAutoSpeed] * m_turnDirection;
    m_rotateDome(rotationSpeed);
  } 
  else {

    // -------------------------------
    // Turn completed. Stop the motor.
    // -------------------------------

    #if defined(DEBUG)
    Debug.print(DBG_INFO, F("DomeMotor"), F("m_automationTurn()"), F("Stop turning"));
    #endif

    stop();

    // ---------------------------
    // Advance the rotation cycle.
    // ---------------------------

    m_rotationStatus = STOPPED;
  }
}
