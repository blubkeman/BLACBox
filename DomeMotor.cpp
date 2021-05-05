/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DomeMotor.cpp - Library for supported dome motor controllers
 * Created by Brian Lubkeman, 5 May 2021
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

  m_joystick = &m_controller->domeStick;
  m_button = &m_controller->button;

  m_rotationStatus = STOPPED;
  m_turnDirection  = m_settings[iInvertTurn];

  m_targetPosition = 0;  // (0 - 359) - degrees in a circle, 0 = home
  m_stopTurnTime   = 0;
  m_startTurnTime  = 0;
  m_previousTime   = 0;

  m_automationRunning = false;
  m_automationSettingsInvalid = false;

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  m_className = F("DomeMotor::");
  #endif
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

    #if defined(VERBOSE)
    output = m_className+F("begin()");
    output += F(" - ");
    output += F("Invalid settings.");
    output += F("\n");
    output += F("  Turn time: ");  output += m_timings[iTurn360];
    output += F("\t Min: ");       output += m_timings[iTurn360Min];
    output += F("\t Max: ");       output += m_timings[iTurn360Max];
    output += F("\n");
    output += F("  Dome speed: "); output += m_settings[iAutoSpeed];
    output += F("\t Min: ");       output += m_settings[iAutoSpeedMin];
    output += F("\t Max: ");       output += m_settings[iAutoSpeedMax];
    printOutput();
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
    output = m_className+F("interpretController()");
    output += F(" - ");
    output += F("No controller");
    printOutput();
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

  if ( (m_joystick->side == left && m_button->pressed(L1)) ||
       (m_joystick->side == right && m_button->pressed(R1)) ) {
    return;
  }

  // --------------------------
  // Get the joystick position.
  // --------------------------

  byte stickPosition = m_joystick->center;

  if ( m_controller->getType() != 0 ) {
    // The controller is a PS3, PS4, or PS5 controller.
    stickPosition = m_joystick->rotation();
  } else if ( m_controller->connectionStatus() == FULL ) {
    // The controller is a pair of PS3 Move Navigations.
    stickPosition = m_joystick->rotation();
  } else if ( m_button->pressed(L2) ) {
    // The controller is a single PS3 Move Navigation.
    stickPosition = m_joystick->rotation();
  } else {
    return;
  }

  // -----------------------------------------------------
  // A stick within its dead zone is the same as centered.
  // Stop dome rotation when the stick is centered.
  // -----------------------------------------------------
  
  if ( abs(stickPosition - m_joystick->center) < m_joystick->deadZone ) {
    stop();
    return;
  }

  // --------------------------------------------------
  // Convert the joystick position to a rotation speed.
  // --------------------------------------------------

  int rotationSpeed = map(stickPosition, m_joystick->minValue, m_joystick->maxValue, -m_settings[iDomeSpeed], m_settings[iDomeSpeed]);

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

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  output = m_className+F("automationOn()");
  output += F(" - ");
  output += F("Dome automation");
  output += F(" enabled.");
  printOutput();
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

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  output = m_className+F("automationOff()");
  output += F(" - ");
  output += F("Dome automation");
  output += F(" disabled.");
  printOutput();
  #endif
}

// =========================
//      runHoloAutomation()
// =========================
void DomeMotor::runHoloAutomation(void)
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
   
  // ----------
  // Debugging.
  // ----------

  #if defined(VERBOSE)
  output = m_className+F("m_automationInit()");
  output += F(" - ");
  output += F("Turn set");
  output += F("\n");
  output += F("  Current time: ");    output += currentTime;        output += F("\n");
  output += F("  Target position: "); output += m_targetPosition;   output += F("\n");
  output += F("  Next start time: "); output += m_startTurnTime;    output += F("\n");
  output += F("  Next stop time:  "); output += m_stopTurnTime;
  printOutput();
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

    // ----------
    // Debugging.
    // ----------

    #if defined(VERBOSE)
    output = m_className+F("m_automationReady()");
    output += F(" - ");
    output += F("Ready to turn");
    printOutput();
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

    #if defined(VERBOSE)
    output = m_className+F("m_automationTurn()");
    output += F(" - ");
    output += F("Turning.");
    printOutput();
    #endif

    int rotationSpeed = m_settings[iAutoSpeed] * m_turnDirection;
    m_rotateDome(rotationSpeed);
  } 
  else {

    // -------------------------------
    // Turn completed. Stop the motor.
    // -------------------------------

    #if defined(VERBOSE)
    output = m_className+F("m_automationTurn()");
    output += F(" - ");
    output += F("Stop turning.");
    printOutput();
    #endif

    stop();

    // ---------------------------
    // Advance the rotation cycle.
    // ---------------------------

    m_rotationStatus = STOPPED;
  }
}
