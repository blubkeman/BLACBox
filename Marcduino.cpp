/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Marcduino.cpp - Library for the Marcduino system
 * Created by Brian Lubkeman, 20 February 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Marcduino.h"
#include "Settings_Marcduino.h"

/* ================================================================================
 *                               Marcduino Functions
 * ================================================================================ */
// =====================
//      Constructor
// =====================
#if defined(PS4_CONTROLLER)
Marcduino::Marcduino(Controller_PS4 * pController)
#else
Marcduino::Marcduino(Controller_PS3 * pController)
#endif
{
  m_controller = pController;
  m_customPanelRunning = false;
  m_holoAutomationRunning = false;

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
  m_className = "Marcduino::";
  #endif
}

// =================
//      begin()
// =================
void Marcduino::begin()
{
  #ifdef DEBUG
  output = m_className+F("begin()");
  output += F(" - ");
  output += F("Marcduino started.");
  printOutput();
  #endif
}

// ===============================
//      interpretController()
// ===============================
void Marcduino::interpretController(void)
{
  // ---------------------------------------
  // Do nothing when there is no controller.
  // ---------------------------------------

  if ( m_controller->connectionStatus() == NONE ) {
    return;
  }

  m_mapIndex = -1;

  // ------------------------------
  // Look for pressed base buttons.
  // ------------------------------

  byte baseButton = -1;  
  if ( m_controller->getButtonClick(UP) )            { baseButton = 0; }
  else if ( m_controller->getButtonClick(RIGHT) )    { baseButton = 1; }
  else if ( m_controller->getButtonClick(DOWN) )     { baseButton = 2; }
  else if ( m_controller->getButtonClick(LEFT) )     { baseButton = 3; }
  else if ( m_controller->getButtonClick(TRIANGLE) ) { baseButton = 4; }
  else if ( m_controller->getButtonClick(CIRCLE) )   { baseButton = 5; }
  else if ( m_controller->getButtonClick(CROSS) )    { baseButton = 6; }
  else if ( m_controller->getButtonClick(SQUARE) )   { baseButton = 7; }

  // ------------------------------------------
  // Do nothing when no base button is pressed.
  // ------------------------------------------

  if ( baseButton < 0 ) {
    return;
  }

  // ----------------------------------
  // Look for pressed modifier buttons.
  // ----------------------------------

  byte modifierButton = -1;

  if ( m_controller->getButtonPress(L1) || m_controller->getButtonPress(R1) ) {
    modifierButton = 8;
  #if defined(PS4_CONTROLLER)
  } else if ( m_controller->getButtonPress(SHARE) ) {
  #else
  } else if ( m_controller->getButtonPress(SELECT) ) {
  #endif
    modifierButton = 16;
  #if defined(PS4_CONTROLLER)
  } else if ( m_controller->getButtonPress(OPTIONS) ) {
  #else
  } else if ( m_controller->getButtonPress(START) ) {
  #endif
    modifierButton = 24;
  #if defined(PS3_NAVIGATION)
  } else if ( m_controller->getButtonPress(PS) || m_controller->getButtonPress(PS2) ) {
  #else
  } else if ( m_controller->getButtonPress(PS) ) {
  #endif
    modifierButton = 32;
  }

  // ----------------------------------------------------------------
  // Determine the index to use on buttonMap[] to get the command(s).
  // ----------------------------------------------------------------

  m_mapIndex = baseButton + modifierButton;


  // -------------------------------------------
  // Send the command(s) to the master board(s).
  // -------------------------------------------

  m_sendCommand(m_mapIndex);
}

// =====================
//      quietMode()
// =====================
void Marcduino::quietMode(void)
{
  
}

// ======================
//      automation()
// ======================
void Marcduino::automation(void)
{
  
}

// ==================================
//      runCustomPanelSequence()
// ==================================
void Marcduino::runCustomPanelSequence(void)
{
  
}

// ================================
//      isCustomPanelRunning()
// ================================
bool Marcduino::isCustomPanelRunning(void)
{
  return m_customPanelRunning;
}

// =========================
//      m_sendCommand()
// =========================
void Marcduino::m_sendCommand(byte mapIndex)
{
  // -----------------------------------------------
  // Lookup the dome command. When found, send it
  // to the dome master. When not found, do nothing.
  // -----------------------------------------------

  String pgmString = getPgmString(buttonMap[mapIndex].domeCommand);
  if (pgmString != "") {
    m_sendCommand(pgmString, &MD_Dome_Serial);

    #ifdef DEBUG
    output = m_className+F("m_sendCommand()");
    output += F(" - ");
    output += F("Send ");
    output += pgmString;
    output += F(" to dome master.");
    printOutput();
    #endif
  }

  // -----------------------------------------------
  // Lookup the body command. When found, send it
  // to the body master. When not found, do nothing.
  // -----------------------------------------------

  #ifdef MD_BODY_MASTER
  pgmString = getPgmString(buttonMap[mapIndex].bodyCommand);
  if (pgmString != "") {
    m_sendCommand(pgmString, &MD_Body_Serial);

    #ifdef DEBUG
    output = m_className+F("m_sendCommand()");
    output += F(" - ");
    output += F("Send ");
    output += pgmString;
    output += F(" to body master.");
    printOutput();
    #endif
  }
  #endif
}
void Marcduino::m_sendCommand(String inStr, HardwareSerial * targetSerial)
{
  // --------------------------------------------------------------
  // This function allows for the use of either a slip ring (wired)
  //  or a Feather Radio (wireless) connection to the dome.
  // --------------------------------------------------------------

  #if defined(FEATHER_RADIO)

  if (targetSerial == MD_Body_Serial) {
    targetSerial.print(inStr);
  } else {
    // Send one character at a time to the Feather Radio.
    for (int i=0; i<inStr.length(); i++) {
      targetSerial.write(inStr[i]);
    }
  }

  #else

  targetSerial->print(inStr);

  #endif
}

// ===========================
//      m_setPanelState()
// ===========================
byte Marcduino::m_setPanelState(byte)
{
  
}

// ======================================
//      m_setHoloAutomationRunning()
// ======================================
void Marcduino::m_setHoloAutomationRunning(bool)
{
  
}

// =====================================
//      m_isHoloAutomationRunning()
// =====================================
bool Marcduino::m_isHoloAutomationRunning(void)
{
  
}
