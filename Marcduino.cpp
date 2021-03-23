/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Marcduino.cpp - Library for the Marcduino system
 * Created by Brian Lubkeman, 23 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Marcduino.h"

#if defined(MD_CUSTOM_CMDS)
#include "Marcduino_Custom_Mappings.h"
#else
#include "Marcduino_Standard_Mappings.h"
#endif

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
  // Start the Serial communication.

  MD_Dome_Serial.begin(MARCDUINO_BAUD_RATE);
  #if defined(MD_BODY_MASTER)
  MD_Body_Serial.begin(MARCDUINO_BAUD_RATE);
  #endif

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
    #ifdef DEBUG
    output = m_className+F("interpretController()");
    output += F(" - ");
    output += F("No controller");
    printOutput();
    #endif
    return;
  }

  // ---------------------------------------------------
  // Determine what button combination is being pressed.
  // ---------------------------------------------------

  m_buttonIndex = m_controller->getButtonsPressed();
  if ( m_buttonIndex < 0 || m_buttonIndex > sizeof(buttonMap)-1 ) {
    return;
  }

  #if defined(DEBUG)
  output = m_className+F("interpretController()");
  output += F(" - ");
  output += F("mapIndex = ");
  output += m_buttonIndex;
  printOutput();
  #endif

  // -------------------------------------------
  // Send the command(s) to the master board(s).
  // -------------------------------------------

  m_sendCommand(m_buttonIndex);
}

// =====================
//      quietMode()
// =====================
void Marcduino::quietMode(void)
{
  #if defined(MD_BODY_MASTER)
  m_sendCommand(getPgmString(cmd_DomeQuiet), &MD_Dome_Serial);
    #if defined(MD_BODY_SOUND)
    m_sendCommand(getPgmString(cmd_BodyQuiet), &MD_Body_Serial);
    #else
    m_sendCommand(getPgmString(cmd_ClosePanelAll), &MD_Body_Serial);
    #endif
  #else
  m_sendCommand(getPgmString(cmd_QuietMode), &MD_Dome_Serial);
  #endif
}

// =========================
//      runAutomation()
// =========================
void Marcduino::runAutomation(void)
{
  // ------------------------
  // Holoprojector automation
  // ------------------------

  if ( m_holoAutomationRunning ) {
    unsigned long currentTime;
    for (int hp = 0; hp < NUMBER_OF_HOLOPROJECTORS; hp++) {

      // -----------------------------------------------------
      // Set the random delay interval for this holoprojector.
      // -----------------------------------------------------

      if (m_randomSeconds[hp] == 0) {
        m_randomSeconds[hp] = random(AUTO_HP_DELAY_MIN, AUTO_HP_DELAY_MAX + 1) * 1000;
      }

      // --------------------------------------------------------------
      // Move both horizontal and vertical servos to a random position.
      // --------------------------------------------------------------

      currentTime = millis();
      if (currentTime > (m_lastRandomTime[hp] + m_randomSeconds[hp])) {
        m_lastRandomTime[hp] = currentTime;
        m_randomSeconds[hp] = 0;
        m_sendCommand(cmd_HPRandomOn, &MD_Dome_Serial);
      }
    }
  }
}

// ==================================
//      runCustomPanelSequence()
// ==================================
void Marcduino::runCustomPanelSequence(void)
{
  /* FUTURE: This function limits each panel to a single, timed 
   *  opening/closing. I'm not satisfied with that, but have not 
   *  yet thought through what is needed to run a more complex 
   *  sequence. */

  #if defined(DEBUG_MARCDUINO) || defined (DEBUG_ALL)
  output = m_className+F("runCustomPanelSequence()");
  output += F(" - ");
  output += F("Running custom routine");
  printOutput();
  #endif

  // Initialize local variables.

  String cmd;
  String xx = "";
  byte statusTotal = 0;

  // Repeat for each dome panel.

  for (byte i = 0; i < NUMBER_OF_DOME_PANELS; i++) {

    // Prepare the dome panel number for use in the command string.
    // Left pad with 0 when the number is a single digit.

    if (i < 10) { xx = "0"; }
    xx += (String)(i+1);

    // Send the 'open dome panel' command.

    if (m_panelState[i].Status == 1) {
      if ((m_panelState[i].Start + (m_panelState[i].Start_Delay * 1000)) < millis()) {
        cmd = ":OP"+xx+"\r";
        m_sendCommand(cmd, &MD_Dome_Serial);
        m_panelState[i].Status = 2;

        #if defined (DEBUG)
        output = F("  Opening");
        output += F(" panel #");
        output += i+1;
        printOutput();
        #endif
      }
    }

    // Send the 'close dome panel' command.

    if (m_panelState[i].Status == 2) {
      if ((m_panelState[i].Start + ((m_panelState[i].Start_Delay + m_panelState[i].Open_Time) * 1000)) < millis()) {
        cmd = ":CL"+xx+"\r";
        m_sendCommand(cmd, &MD_Dome_Serial);
        m_panelState[i].Status = 0;

        #if defined (DEBUG)
        output = F("  Closing");
        output += F(" panel #");
        output += i+1;
        printOutput();
        #endif
      }
    }
    statusTotal += m_panelState[i].Status;
  }

  // If all the panels have now closed - close out the custom routine

  if (statusTotal == 0) {
    m_customPanelRunning = false;
  }
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
  // ------------------------------------
  // Lookup the dome command and send it.
  // ------------------------------------

  m_sendCommand(getPgmString(buttonMap[mapIndex].domeCommand), &MD_Dome_Serial);

  // ------------------------------------
  // Lookup the body command and send it.
  // ------------------------------------

  #ifdef MD_BODY_MASTER
  m_sendCommand(getPgmString(buttonMap[mapIndex].bodyCommand), &MD_Body_Serial);
  #endif
}
void Marcduino::m_sendCommand(String inStr, HardwareSerial * targetSerial)
{
  if ( inStr == "" ) {
    return;
  }
  
  // --------------------------------------------------------------
  // This function allows for the use of either a slip ring (wired)
  //  or a Feather Radio (wireless) connection to the dome.
  // --------------------------------------------------------------

  #if defined(FEATHER_RADIO)
  
  if (targetSerial == MD_Dome_Serial) {

    // Send one character at a time to the Feather Radio.
    for (int i=0; i<inStr.length(); i++) {
      targetSerial.write(inStr[i]);
    }

    #ifdef DEBUG
    output = m_className+F("m_sendCommand()");
    output += F(" - ");
    output += F("Sent ");
    output += inStr;
    output += F(" to dome via Feather Radio.");
    printOutput();
    #endif

    return;
  }

  #endif

  targetSerial->print(inStr);

  #ifdef DEBUG
  output = m_className+F("m_sendCommand()");
  output += F(" - ");
  output += F("Sent ");
  output += inStr;
  if ( targetSerial == &MD_Dome_Serial ) {
    output += F(" to dome");
  } else if ( targetSerial == &MD_Body_Serial ) {
    output += F(" to body");
  }
  output += F(" via Serial.");
  printOutput();
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
