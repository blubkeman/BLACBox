/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DomeMotor.cpp - Library for supported dome motor controllers
 * Created by Brian Lubkeman, 23 March 2021
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
DomeMotor::DomeMotor(void)
{
  m_rotationStatus = STOPPED;
  m_previousTime   = 0;

  m_turnDirection  = 1;  // 1 = positive turn, -1 negative turn
  m_targetPosition = 0;  // (0 - 359) - degrees in a circle, 0 = home
  m_stopTurnTime   = 0;
  m_startTurnTime  = 0;

  m_automationRunning = false;
  m_automationSettingsInvalid = false;

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
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
void DomeMotor::begin()
{
  // ---------------------------------------------------
  // Prepare the random number seed for dome automation.
  // ---------------------------------------------------

  randomSeed(analogRead(0));

  // ----------------------------------
  // Validate dome automation settings.
  // ----------------------------------

  if ( TIME_360_DOME_TURN < TIME_360_DOME_TURN_MIN ||
       TIME_360_DOME_TURN > TIME_360_DOME_TURN_MAX ||
       DOME_AUTO_SPEED < DOME_AUTO_SPEED_MIN ||
       DOME_AUTO_SPEED > DOME_AUTO_SPEED_MAX ) {

    m_automationSettingsInvalid = true;

    #ifdef DEBUG
    output = m_className+F("begin()");
    output += F(" - ");
    output += F("Invalid settings.");
    output += F("\n");
    output += F("  Turn time: ");  output += TIME_360_DOME_TURN;
    output += F("\t Min: ");       output += TIME_360_DOME_TURN_MIN;
    output += F("\t Max: ");       output += TIME_360_DOME_TURN_MAX;
    output += F("\n");
    output += F("  Dome speed: "); output += DOME_AUTO_SPEED;
    output += F("\t Min: ");       output += DOME_AUTO_SPEED_MIN;
    output += F("\t Max: ");       output += DOME_AUTO_SPEED_MAX;
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
 *                            ==============    ======================
 *  Enable dome automation    L2+Circle         L2+Select|Share|Create
 *  Disable dome automation   L2+Cross          L2+Start|Options
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
    #ifdef DEBUG
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

  if ( m_controller->getButtonPress(L2) ) {

    if ( isAutomationRunning() ) {

      #if defined(PS5_CONTROLLER)
      if ( m_controller->getButtonClick(CREATE) ) {
      #elif defined(PS4_CONTROLLER)
      if ( m_controller->getButtonClick(SHARE) ) {
      #else
      if ( m_controller->getButtonClick(SELECT) ) {
      #endif
        m_automationOff();
      }

    } else {

      #if defined(PS4_CONTROLLER) || defined(PS5_CONTROLLER)
      if ( m_controller->getButtonClick(OPTIONS) ) { 
      #else
      if ( m_controller->getButtonClick(START) ) { 
      #endif
        m_automationOn();
      }
    }
  }

  // ------------------------------------------------------------------------
  // Flood control. Don't overload the motor controller with excessive input.
  // ------------------------------------------------------------------------

  unsigned long currentTime = millis();
  if ( (currentTime - m_previousTime) < (SERIAL_LATENCY * 2) ) {
    return;
  }
  m_previousTime = currentTime;

  // ================================
  // Look for dome rotation commands.
  // ================================

  // --------------------------
  // Get the joystick position.
  // --------------------------

  byte stickPosition = JOYSTICK_CENTER;

  #if defined(PS3_NAVIGATION)

  if ( m_controller->connectionStatus() == HALF ) {
    if ( ! m_controller->getButtonPress(L2) ) {
      return;
    }
    stickPosition = m_controller->getAnalogHat(LeftHatX);
  } else if ( m_controller->connectionStatus() == FULL ) {
    stickPosition = m_controller->getAnalogHat(RightHatX);
  } else {
    return;
  }
  
  #else

  /* ===============================================
   * 
   *          PS3,PS4,PS5      PS3,PS4,PS5
   *          -----------  or  ------------------
   * Drive    Left stick       Right stick
   * Dome     Right stick      Left stick
   *
   * This is configurable in the Settings.h tab.
   * 
   * =============================================== */

  if ( DRIVE_STICK == 0 ) {
    stickPosition = m_controller->getAnalogHat(RightHatX);
  } else if ( DRIVE_STICK == 1 ) {
    stickPosition = m_controller->getAnalogHat(LeftHatX);  
  } else {
    return;
  }
  #endif

  // -----------------------------------------------------
  // A stick within its dead zone is the same as centered.
  // Stop dome rotation when the stick is centered.
  // -----------------------------------------------------
  
  if ( abs(stickPosition - JOYSTICK_CENTER) < JOYSTICK_DEAD_ZONE ) {
    stop();
    return;
  }

  // --------------------------------------------------
  // Convert the joystick position to a rotation speed.
  // --------------------------------------------------

  int rotationSpeed = map(stickPosition, JOYSTICK_MIN, JOYSTICK_MAX, -DOME_SPEED, DOME_SPEED);

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

  #ifdef DEBUG
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

  #ifdef DEBUG
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
      m_stopTurnTime = m_startTurnTime + ((int)m_targetPosition / (float)360.0 * (unsigned long)TIME_360_DOME_TURN);
      m_turnDirection = 1;
    } else {
      m_stopTurnTime = m_startTurnTime + (((float)360.0 - (int)m_targetPosition) / (float)360.0 * (unsigned long)TIME_360_DOME_TURN);
      m_turnDirection = -1;
    }

  } else {

    // ----------------------------------------------
    // Previous positon was not home. Return to home.
    // ----------------------------------------------

    m_startTurnTime = currentTime + (random(1,6) * 1000); // Wait 1-5 seconds before returning home.
    if ( m_targetPosition < 180 ) {
      m_stopTurnTime = m_startTurnTime + ((int)m_targetPosition / (float)360.0 * (unsigned long)TIME_360_DOME_TURN);
    } else {
      m_stopTurnTime = m_startTurnTime + (((float)360 - (int)m_targetPosition) / (float)360.0 * (unsigned long)TIME_360_DOME_TURN);
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

  #ifdef DEBUG
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

    #ifdef DEBUG
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

    #ifdef DEBUG
    output = m_className+F("m_automationTurn()");
    output += F(" - ");
    output += F("Turning.");
    printOutput();
    #endif

    int rotationSpeed = DOME_AUTO_SPEED * m_turnDirection;
    m_rotateDome(rotationSpeed);
  } 
  else {

    // -------------------------------
    // Turn completed. Stop the motor.
    // -------------------------------

    #ifdef DEBUG
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
