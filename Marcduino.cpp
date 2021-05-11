/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Marcduino.cpp - Library for the Marcduino system
 * Created by Brian Lubkeman, 10 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Marcduino.h"
#include "Marcduino_Panel_Routines.h"

/* ================================================================================
 *                               Marcduino Functions
 * ================================================================================ */
// =====================
//      Constructor
// =====================
Marcduino::Marcduino(Controller* pController, const byte settings[])
{
  m_controller = pController;
  m_settings = settings;
  m_button = &m_controller->button;

  m_cprRunning = false;
  m_holoAutomationRunning = false;
  m_aurabesh = false;
}

// ====================
//      Destructor
// ====================
Marcduino::~Marcduino() {}

// =================
//      begin()
// =================
void Marcduino::begin()
{
  // Start the Serial communication.

  MD_Dome_Serial.begin(MARCDUINO_BAUD_RATE);
  if ( m_settings[iBodyMaster] ) {
    MD_Body_Serial.begin(MARCDUINO_BAUD_RATE);
  }
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
    #if defined(DEBUG)
    Debug.print(DBG_VERBOSE, F("Marcduino"), F("interpretController()"), F("No controller"));
    #endif
    return;
  }

  // ---------------------------------------------------
  // Determine what button combination is being pressed.
  // ---------------------------------------------------

  m_buttonIndex = m_getButtonsPressed();

  #if defined(DEBUG)
  if ( m_buttonIndex > -1 ) {
    Debug.print(DBG_VERBOSE, F("Marcduino"), F("interpretController()"), F("Button combo:"), (String)m_buttonIndex);
  }
  #endif

  if ( m_settings[iCmdSet] == 0 ) {

    // These mappings should mimic the SHADOW+MD controls.

    #if defined(DEBUG)
    if ( m_buttonIndex > -1 ) {
      Debug.print(DBG_VERBOSE, F("Marcduino"), F("interpretController()"), F("SHADOW+MD command set"));
    }
    #endif

    switch (m_buttonIndex) {
      case 0 :  { quietMode();         break; } // UP
      case 1 :  { m_midAwakeMode();    break; } // RIGHT
      case 2 :  { m_fullAwakeMode();   break; } // DOWN
      case 3 :  { m_awakePlusMode();   break; } // LEFT
      case 4 :  { m_marchingAnts();    break; } // TRIANGLE
      case 5 :  { m_bodyPanelClose(1); break; } // CIRCLE
      case 6 :  { m_cantinaBeep();     break; } // CROSS
      case 7 :  { m_bodyPanelOpen(1);  break; } // SQUARE
      case 8 :  { m_leiaMessage();     break; } // L1/R1 + UP
      case 9 :  { m_wave();            break; } // L1/R1 + RIGHT
      case 10 : { m_cantinaDance();    break; } // L1/R1 + DOWN
      case 11 : { m_wave2();           break; } // L1/R1 + LEFT
      case 12 : { m_domePanelClose(1); break; } // L1/R1 + TRIANGLE
      case 13 : { m_domePanelOpen(2);  break; } // L1/R1 + CIRCLE
      case 14 : { m_domePanelOpen(1);  break; } // L1/R1 + CROSS
      case 15 : { m_domePanelClose(2); break; } // L1/R1 + SQUARE
      case 16 : { m_volumeUp();        break; } // SHARE/SELECT + UP
      case 17 : { m_hpLightOn(0);      break; } // SHARE/SELECT + RIGHT
      case 18 : { m_volumeDown();      break; } // SHARE/SELECT + DOWN
      case 19 : { m_hpLightOff(0);     break; } // SHARE/SELECT + LEFT
      case 20 : { m_volumeMid();       break; } // SHARE/SELECT + TRIANGLE
      case 21 : { m_domePanelClose(0); break; } // SHARE/SELECT + CIRCLE
      case 22 : { m_volumeMax();       break; } // SHARE/SELECT + CROSS
      case 23 : { m_domePanelOpen(0);  break; } // SHARE/SELECT + SQUARE
      case 24 : { m_disco();           break; } // OPTIONS/START + UP
      case 25 : { m_fastWave();        break; } // OPTIONS/START + RIGHT
      case 26 : { m_scream();          break; } // OPTIONS/START + DOWN
      case 27 : { m_faint();           break; } // OPTIONS/START + LEFT
      case 28 : { m_hpReset(0);        break; } // OPTIONS/START + TRIANGLE
      case 29 : { m_hpLightOn(0);      break; } // OPTIONS/START + CIRCLE
      case 30 : { m_hpRandomMove(0);   break; } // OPTIONS/START + CROSS
      case 31 : { m_hpLightOff(0);     break; } // OPTIONS/START + SQUARE
      case 32 : {                               // PS/PS2 + UP
                  m_soundPlayTrack(8, 8);
                  m_logicsStarWars();
                  break; }
      case 33 : {                               // PS/PS2 + RIGHT
                  m_soundPlayTrack(8, 9);
                  m_logicsReset(0);
                  break; }
      case 34 : {                               // PS/PS2 + DOWN
                  m_soundPlayTrack(8, 10);
                  m_logicsReset(0);
                  break; }
      case 35 : {                               // PS/PS2 + LEFT
                  m_soundPlayTrack(8, 11);
                  m_logicsReset(0);
                  break; }
      case 36 : { m_domePanelClose(3); break; } // PS/PS2 + TRIANGLE
      case 37 : { m_domePanelOpen(4);  break; } // PS/PS2 + CIRCLE
      case 38 : { m_domePanelOpen(3);  break; } // PS/PS2 + CROSS
      case 39 : { m_domePanelClose(4); break; } // PS/PS2 + SQUARE
      default : { break; }
    }

  } else if ( m_settings[iCmdSet] == 1 ) {

    // This is where you may define your own button combo mappings.

    #if defined(DEBUG)
    if ( m_buttonIndex > -1 ) {
      Debug.print(DBG_VERBOSE, F("Marcduino"), F("interpretController()"), F("Custom command set"));
    }
    #endif
    switch (m_buttonIndex) {
      case 0 :  { quietMode();             break; } // UP
      case 1 :  { m_midAwakeMode();        break; } // RIGHT
      case 2 :  { m_fullAwakeMode();       break; } // DOWN
      case 3 :  { m_awakePlusMode();       break; } // LEFT
      case 4 :  { break; } // TRIANGLE
      case 5 :  { break; } // CIRCLE
      case 6 :  { break; } // CROSS
      case 7 :  { break; } // SQUARE
      case 8 :  { m_volumeUp();            break; } // L1/R1 + UP
      case 9 :  { m_hpReset(0);            break; } // L1/R1 + RIGHT
      case 10 : { m_volumeDown();          break; } // L1/R1 + DOWN
      case 11 : { m_hpRandomMove(0);       break; } // L1/R1 + LEFT
      case 12 : { m_volumeMid();           break; } // L1/R1 + TRIANGLE
      case 13 : { m_hpLightOn(0);          break; } // L1/R1 + CIRCLE
      case 14 : { m_volumeMax();           break; } // L1/R1 + CROSS
      case 15 : { m_hpLightOff(0);         break; } // L1/R1 + SQUARE
      case 16 : { m_soundPlayTrack(1, 22); break; } // SHARE/SELECT + UP        : 022_Chatter-22.mp3
      case 17 : { m_soundPlayTrack(2, 9);  break; } // SHARE/SELECT + RIGHT     : 034_Idle-09.mp3
      case 18 : { m_soundPlayTrack(3, 6);  break; } // SHARE/SELECT + DOWN      : 056_Acknowledge-06.mp3
      case 19 : { m_soundPlayTrack(4, 1);  break; } // SHARE/SELECT + LEFT      : 076_Engage-01.mp3
      case 20 : { m_soundPlayTrack(1, 25); break; } // SHARE/SELECT + TRIANGLE  : 025_Chatter-25.mp3
      case 21 : { m_soundPlayTrack(2, 17); break; } // SHARE/SELECT + CIRCLE    : 042_Idle-17.mp3
      case 22 : { m_soundPlayTrack(3, 20); break; } // SHARE/SELECT + CROSS     : 070_ActionComplete-09.mp3
      case 23 : { m_soundPlayTrack(7, 15); break; } // SHARE/SELECT + SQUARE    : 165_ThreatWarning-04.mp3
      case 24 : { m_soundPlayTrack(4, 17); break; } // OPTIONS/START + UP       : 092_Alert-03.mp3
      case 25 : { m_soundPlayTrack(8, 1);  break; } // OPTIONS/START + RIGHT    : 176_ImperialMarch.mp3
      case 26 : { m_soundPlayTrack(6, 4);  break; } // OPTIONS/START + DOWN     : 129_Hurt-04.mp3
      case 27 : { m_soundPlayTrack(8, 2);  break; } // OPTIONS/START + LEFT     : 177_DuelOfTheFates.mp3
      case 28 : { m_soundPlayTrack(4, 22); break; } // OPTIONS/START + TRIANGLE : 097_Atack-01.mp3
      case 29 : { m_soundPlayTrack(8, 3);  break; } // OPTIONS/START + CIRCLE   : 178_ImperialSuite.mp3
      case 30 : { m_soundPlayTrack(7, 3);  break; } // OPTIONS/START + CROSS    : 153_Pain-03.mp3
      case 31 : { m_soundPlayTrack(8, 4);  break; } // OPTIONS/START + SQUARE   : 179_GnRScream.mp3
      case 32 : { m_bodyPanelOpen(1);      break; } // PS/PS2 + UP
      case 33 : { m_bodyPanelClose(2);     break; } // PS/PS2 + RIGHT
      case 34 : { m_bodyPanelClose(1);     break; } // PS/PS2 + DOWN
      case 35 : { m_bodyPanelOpen(2);      break; } // PS/PS2 + LEFT
      case 36 : { break; } // PS/PS2 + TRIANGLE
      case 37 : { break; } // PS/PS2 + CIRCLE
      case 38 : { break; } // PS/PS2 + CROSS
      case 39 : { break; } // PS/PS2 + SQUARE
      default : { break; }
    }

  } else {

    // Unknown command set setting.

    #if defined(DEBUG)
    Debug.print(DBG_ERROR, F("Marcduino"), F("interpretController()"), F("Unknown command set"));
    #endif

    return;
  }
}

// ===============================
//      m_getButtonsPressed()
// ===============================
int Marcduino::m_getButtonsPressed(void)
{
  int baseButton = -1;

  // ------------------------------
  // Look for pressed base buttons.
  // Stop when none are pressed.
  // ------------------------------

  if ( m_button->clicked(UP) )            { baseButton = 0; }
  else if ( m_button->clicked(RIGHT) )    { baseButton = 1; }
  else if ( m_button->clicked(DOWN) )     { baseButton = 2; }
  else if ( m_button->clicked(LEFT) )     { baseButton = 3; }
  else if ( m_button->clicked(TRIANGLE) ) { baseButton = 4; }
  else if ( m_button->clicked(CIRCLE) )   { baseButton = 5; }
  else if ( m_button->clicked(CROSS) )    { baseButton = 6; }
  else if ( m_button->clicked(SQUARE) )   { baseButton = 7; }
  else {
    return baseButton;
  }

  // ----------------------------------
  // Look for pressed modifier buttons.
  // ----------------------------------

  int modifier = 0;

  if ( m_button->pressed(L1) || 
       m_button->pressed(R1) )      { modifier = 8;  }
  else if ( m_button->pressed(L4) ) { modifier = 16; }
  else if ( m_button->pressed(R4) ) { modifier = 24; }
  else if ( m_button->pressed(PS) ) { modifier = 32; }

  // ----------------------------
  // Return the calculated index.
  // ----------------------------

  return ( baseButton + modifier );
}

// =========================
//      m_sendCommand()
// =========================
void Marcduino::m_sendCommand(String inStr, HardwareSerial* targetSerial)
{
  // ----------------------------------------------------------
  // Handle the special case of running a custom panel routine.
  // ----------------------------------------------------------

  int pos = inStr.indexOf(":CPR");
  if ( pos > -1 ) {
    // Determine which custom panel routine to run.
    switch (inStr.substring(pos+4, pos+5).toInt()) {
      case 1:
        m_startCustomPanelRoutine(sampleRoutine, sizeof(sampleRoutine));
        break;
      default:
        return;
    }
    // Remove the custom panel routine from the command.
    inStr.remove(pos, 8);
  }

  // --------------------------------------------------------------------
  // Stop when no command remains. Otherwise, continue with what remains.
  // --------------------------------------------------------------------

  if ( inStr == "" ) {
    return;
  }

  // --------------------------------------------------------------
  // This function allows for the use of either a slip ring (wired)
  //  or a Feather Radio (wireless) connection to the dome.
  // --------------------------------------------------------------

  char buff[50];
  if ( m_settings[iRadio] ) {
  
    if ( targetSerial == &MD_Dome_Serial ) {

      // Send one character at a time to the Feather Radio.
      for (int i=0; i<inStr.length(); i++) {
        targetSerial->write(inStr[i]);
      }

      #if defined(DEBUG)
      sprintf(buff, "%s to dome via Feather Radio", inStr);
      Debug.print(DBG_INFO, F("Marcduino"), F("m_sendCommand()"), buff);
      #endif
  
      return;
    }
  }

  targetSerial->print(inStr);

  #if defined(DEBUG)
  if ( targetSerial == &MD_Dome_Serial ) {
    sprintf(buff, "%s to dome via Serial", inStr);
  } else if ( targetSerial == &MD_Body_Serial ) {
    sprintf(buff, "%s to body via Serial", inStr);
  }
  Debug.print(DBG_INFO, F("Marcduino"), F("m_sendCommand()"), buff);
  #endif
}

// ==================================
//      Holoprojector Automation
// ==================================
void Marcduino::runAutomation(void)
{
  if ( ! m_holoAutomationRunning ) {
    return;
  }

  unsigned long currentTime;
  for (int hp = 0; hp < m_settings[iHp]; hp++) {

    // -----------------------------------------------------
    // Set the random delay interval for this holoprojector.
    // -----------------------------------------------------

    if (m_randomSeconds[hp] == 0) {
      m_randomSeconds[hp] = random(m_settings[iHpDelayMin], m_settings[iHpDelayMax] + 1) * 1000;
    }

    // --------------------------------------------------------------
    // Move both horizontal and vertical servos to a random position.
    // --------------------------------------------------------------

    currentTime = millis();
    if (currentTime > (m_lastRandomTime[hp] + m_randomSeconds[hp])) {
      m_lastRandomTime[hp] = currentTime;
      m_randomSeconds[hp] = 0;
      m_hpRandomMove(hp);
    }
  }
}

// ===============================
//      Custom Panel Routines
// ===============================
bool Marcduino::isCustomPanelRunning(void)
{
  return m_cprRunning;
}
void Marcduino::runCustomPanelRoutine()
{
  for (int i = 0; i < m_cprRowCount; i++) {

    if ( m_cprRunningRoutine[i].completed ) {
      continue;
    }

    unsigned long currentTime = millis();
    if ( currentTime <= (m_cprStartTime + m_cprRunningRoutine[i].startDelay) ) {
      m_sendCommand(m_cprBuilCommand(":OP", m_cprRunningRoutine[i].panelNbr), &MD_Dome_Serial);
      m_cprRunningRoutine[i].startedTime = currentTime;
    } else if ( currentTime <= (m_cprRunningRoutine[i].startedTime + m_cprRunningRoutine[i].openDuration) ) {
      m_sendCommand(m_cprBuilCommand(":CL", m_cprRunningRoutine[i].panelNbr), &MD_Dome_Serial);
      m_cprRunningRoutine[i].completed = true;
      m_cprCompletedCount++;
    }

  }

  if ( m_cprCompletedCount == m_cprRowCount ) {
    m_cprRunning = false;
  }
}
void Marcduino::m_startCustomPanelRoutine(Panel_Routine_Struct routine[], int arraySize)
{
  if ( ! m_cprRunning ) {
    m_cprRunning = true;
    m_cprStartTime = millis();
    m_cprCompletedCount = 0;
    for (int i = 0; i < m_cprRowCount; i++) {
      routine[i].startedTime = 0;
      routine[i].completed = false;
    }
    m_cprRunningRoutine = routine;
    m_cprRowCount = arraySize;
  }
}
String Marcduino::m_cprBuilCommand(String prefix, int panelNbr)
{
  if ( prefix != ":OP" && prefix != ":CL") {
    return "";
  }

  String xx = "0";
  xx += panelNbr;

  String cmd = prefix;
  cmd += xx.substring(xx.length()-2, xx.length()-1);
  cmd += "\r";

  return cmd;
}

// ===================================================================================================
// ===================================================================================================

String Marcduino::m_leftPad(uint8_t n, char c = '0', uint8_t width = 2)
{
  String out;
  for (int i = 0; i < width; i++) {
    out += c;
  }
  out += n;
  out.remove(0, out.length()-width);
  return out;
}

bool Marcduino::m_inList(const uint8_t valueToFind, const uint8_t list[])
{
  bool out = false;
  for (size_t i = 0; i < sizeof(list); i++ ) {
    if ( list[i] == valueToFind ) {
      out = true;
      break;
    }
  }
  return out;
}

// ===================================================================================================
// ===================================================================================================
//
// Panel commands
//    panelNumber = A value btwn 0 and 10.  0=all, 1-10=individual panel
//
// ===================================================================================================
// ===================================================================================================

void Marcduino::m_bodyPanelOpen(uint8_t panelNumber) 
{
  if ( panelNumber > m_settings[iBodyPanels] ) { return; }
  m_sendCommand(String(":OP" + m_leftPad(panelNumber) + '\r'), &MD_Body_Serial);
}
void Marcduino::m_bodyPanelClose(uint8_t panelNumber)
{
  if ( panelNumber > m_settings[iBodyPanels] ) { return; }
  m_sendCommand(String(":CL" + m_leftPad(panelNumber) + '\r'), &MD_Body_Serial);
}
void Marcduino::m_bodyPanelRemoteControl(uint8_t panelNumber)
{
  if ( panelNumber > m_settings[iBodyPanels] ) { return; }
  m_sendCommand(String(":RC" + m_leftPad(panelNumber) + '\r'), &MD_Body_Serial);
}
void Marcduino::m_bodyPanelBuzzKill(uint8_t panelNumber)
{
  if ( panelNumber > m_settings[iBodyPanels] ) { return; }
  m_sendCommand(String(":ST" + m_leftPad(panelNumber) + '\r'), &MD_Body_Serial);
}
void Marcduino::m_bodyPanelHold(uint8_t panelNumber)
{
  if ( panelNumber > m_settings[iBodyPanels] ) { return; }
  m_sendCommand(String(":HD" + m_leftPad(panelNumber) + '\r'), &MD_Body_Serial);
}
void Marcduino::m_domePanelOpen(uint8_t panelNumber) 
{
  if ( panelNumber > m_settings[iDomePanels] ) { return; }
  m_sendCommand(String(":OP" + m_leftPad(panelNumber) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_domePanelClose(uint8_t panelNumber)
{
  if ( panelNumber > m_settings[iDomePanels] ) { return; }
  m_sendCommand(String(":CL" + m_leftPad(panelNumber) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_domePanelRemoteControl(uint8_t panelNumber)
{
  if ( panelNumber > m_settings[iDomePanels] ) { return; }
  m_sendCommand(String(":RC" + m_leftPad(panelNumber) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_domePanelBuzzKill(uint8_t panelNumber)
{
  if ( panelNumber > m_settings[iDomePanels] ) { return; }
  m_sendCommand(String(":ST" + m_leftPad(panelNumber) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_domePanelHold(uint8_t panelNumber)
{
  if ( panelNumber > m_settings[iDomePanels] ) { return; }
  m_sendCommand(String(":HD" + m_leftPad(panelNumber) + '\r'), &MD_Dome_Serial);
}

// ===================================================================================================
// ===================================================================================================
//
// Holoprojector commands
//    hpNumber = A value btwn 0 and 3.   0=all, 1-3=individual holoprojector
//    seconds  = A value btwn 0 and 99.  0=off, 1-98=duration in seconds, 99=indefinitely
//
// ===================================================================================================
// ===================================================================================================

void Marcduino::m_hpRandomMove(uint8_t hpNumber)
{
  if ( hpNumber > m_settings[iHp] ) { return; }
  m_sendCommand(String("*RD" + m_leftPad(hpNumber) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_hpLightOn(uint8_t hpNumber)
{
  if ( hpNumber > m_settings[iHp] ) { return; }
  m_sendCommand(String("*ON" + m_leftPad(hpNumber) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_hpLightOff(uint8_t hpNumber)
{
  if ( hpNumber > m_settings[iHp] ) { return; }
  m_sendCommand(String("*OF" + m_leftPad(hpNumber) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_hpRemoteControl(uint8_t hpNumber)
{
  if ( hpNumber > m_settings[iHp] ) { return; }
  m_sendCommand(String("*RC" + m_leftPad(hpNumber) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_hpReset(uint8_t hpNumber)
{
  if ( hpNumber > m_settings[iHp] ) { return; }
  m_sendCommand(String("*ST" + m_leftPad(hpNumber) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_hpHold(uint8_t hpNumber)
{
  if ( hpNumber > m_settings[iHp] ) { return; }
  m_sendCommand(String("*HD" + m_leftPad(hpNumber) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_hpOnBoard(uint8_t hpNumber, uint8_t seconds = 0)
{
  if ( hpNumber > m_settings[iHp] ) { return; }
  m_sendCommand(String("*H" + hpNumber + m_leftPad(min(seconds,99)) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_hpFlicker(uint8_t hpNumber, uint8_t seconds = 0)
{
  if ( hpNumber > m_settings[iHp] ) { return; }
  m_sendCommand(String("*F" + hpNumber + m_leftPad(min(seconds,99)) + '\r'), &MD_Dome_Serial);
}

// ===================================================================================================
// ===================================================================================================
//
// Magic panel commands
//    seconds  = A value btwn 0 and 99.  0=off, 1-98=duration in seconds, 99=indefinitely
//
// ===================================================================================================
// ===================================================================================================

void Marcduino::m_magicPanel(uint8_t seconds = 0)
{
  m_sendCommand(String("*MO" + m_leftPad(min(seconds,99)) + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_magicPanelFlicker(uint8_t seconds = 0)
{
  m_sendCommand(String("*MF" + m_leftPad(min(seconds,99)) + '\r'), &MD_Dome_Serial);
}

// ===================================================================================================
// ===================================================================================================
//
// Display logic commands
//    displayNumber = A value btwn 0 and 5.  0=all, 1=FTLD, 2=FBLD, 3=RLD, 4=FPSI, 5=RPSI
//    text = A string.
//    style = A value btwn 0 and 6.
//
// ===================================================================================================
// ===================================================================================================

void Marcduino::m_logicsTest(uint8_t displayNumber)
{
  if ( displayNumber > 5 ) { return; }
  m_sendCommand(String((String)"@" + displayNumber + "T0\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsReset(uint8_t displayNumber)
{
  if ( displayNumber > 5 ) { return; }
  m_sendCommand(String((String)"@" + displayNumber + "T1\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsAlarm(void)
{
  m_sendCommand(String((String)"@0T2\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsAlarmTimed(void)
{
  m_sendCommand(String((String)"@0T3\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsShortCircuit(void)
{
  m_sendCommand(String((String)"@0T4\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsScream(void)
{
  m_sendCommand(String((String)"@0T5\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsLeia(void)
{
  m_sendCommand(String((String)"@0T6\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsStarWars(void)
{
  m_sendCommand(String((String)"@0T10\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsMarch(void)
{
  m_sendCommand(String((String)"@0T11\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsOff(uint8_t displayNumber)
{
  if ( displayNumber > 5 ) { return; }
  m_sendCommand(String((String)"@" + displayNumber + "T20\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsSpectrum(uint8_t displayNumber)
{
  if ( displayNumber > 3 ) { return; }
  m_sendCommand(String((String)"@" + displayNumber + "T92\r"), &MD_Dome_Serial);
}
void Marcduino::m_logicsText(String text, uint8_t displayNumber)
{
  if ( displayNumber > 3 ) { return; }
  m_sendCommand(String((String)"@" + displayNumber + "T100\r" + (String)"@M" + text + '\r'), &MD_Dome_Serial);
}
void Marcduino::m_logicsToggleFont(uint8_t displayNumber)
{
  if ( displayNumber > 3 ) { return; }
  if ( m_aurabesh ) {
    m_sendCommand(String((String)"@" + displayNumber + "P60\r"), &MD_Dome_Serial);
    m_aurabesh = false;
  } else {
    m_sendCommand(String((String)"@" + displayNumber + "P61\r"), &MD_Dome_Serial);
    m_aurabesh = true;
  }
}
void Marcduino::m_logicsRandomStyle(uint8_t style, uint8_t displayNumber)
{
  if ( displayNumber > 3 ) { return; }
  if ( style > 6 ) { style = 0; }
  m_sendCommand(String((String)"@" + displayNumber + "R" + style + '\r'), &MD_Dome_Serial);
}

// ===================================================================================================
// ===================================================================================================
//
// PSI commands
//    psiNumber = A value 0, 4, or 5.  0=all, 4=FPSI, 5=RPSI
//
// ===================================================================================================
// ===================================================================================================

void Marcduino::m_psiOn(uint8_t psiNumber)
{
  uint8_t validationList[] = {0, 4, 5};
  if ( ! m_inList(psiNumber, validationList) ) { return; }
  m_sendCommand(String((String)"@" + psiNumber + "S0\r"), &MD_Dome_Serial);
}
void Marcduino::m_psiNormal(uint8_t psiNumber)
{
  uint8_t validationList[] = {0, 4, 5};
  if ( ! m_inList(psiNumber, validationList) ) { return; }
  m_sendCommand(String((String)"@" + psiNumber + "S1\r"), &MD_Dome_Serial);
}
void Marcduino::m_psiFirstColor(uint8_t psiNumber)
{
  uint8_t validationList[] = {0, 4, 5};
  if ( ! m_inList(psiNumber, validationList) ) { return; }
  m_sendCommand(String((String)"@" + psiNumber + "S2\r"), &MD_Dome_Serial);
}
void Marcduino::m_psiSecondColor(uint8_t psiNumber)
{
  uint8_t validationList[] = {0, 4, 5};
  if ( ! m_inList(psiNumber, validationList) ) { return; }
  m_sendCommand(String((String)"@" + psiNumber + "S3\r"), &MD_Dome_Serial);
}
void Marcduino::m_psiOff(uint8_t psiNumber)
{
  uint8_t validationList[] = {0, 4, 5};
  if ( ! m_inList(psiNumber, validationList) ) { return; }
  m_sendCommand(String((String)"@" + psiNumber + "S4\r"), &MD_Dome_Serial);
}

// ===================================================================================================
// ===================================================================================================
//
// Sound commands
//    bank = A value btwn 1 and 9.
//    track = A value btwn 1 and 99 for CF III or 1 and 25 for MP3 Trigger
//
// ===================================================================================================
// ===================================================================================================

void Marcduino::m_soundNext(uint8_t bank)
{
  if ( bank < 1 || bank > 4 ) { return; }

  if ( m_settings[iSoundMaster] == 1 ) {
    m_sendCommand(String((String)"$" + bank + '\r'), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$" + bank + '\r'), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundFirst(uint8_t bank)
{
  if ( m_settings[iSoundBoard] ) {
    if ( bank < 5 || bank > 8 ) { return; }
  } else {
    if ( bank < 5 || bank > 9 ) { return; }
  }

  if ( m_settings[iSoundMaster] == 1 ) {
    m_sendCommand(String((String)"$" + bank + '\r'), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$" + bank + '\r'), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundPlayTrack(uint8_t bank, uint8_t track)
{
  if ( m_settings[iSoundBoard] ) {
    if ( bank < 5 || bank > 8 ) { return; }
  } else {
    if ( bank < 5 || bank > 9 ) { return; }
  }

  if ( m_settings[iSoundBoard] ) {
    if ( track < 1 || bank > 25 ) { return; }
  } else {
    if ( track < 1 || bank > 99 ) { return; }
  }

  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$" + bank + track + '\r'), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$" + bank + track + '\r'), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundRandom(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$R\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$R\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundRandomOff(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$O\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$O\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundStop(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$s\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$s\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundScream(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$S\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$S\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundWave(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$213\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$213\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundFastWave(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$34\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$34\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundWave2(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$36\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$36\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundFaint(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$F\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$F\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundLeia(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$L\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$L\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundBeepCantina(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$c\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$c\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundStarWars(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$W\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$W\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundMarch(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$M\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$M\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundCantinaDance(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$C\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$C\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_soundDisco(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$D\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$D\r"), &MD_Dome_Serial);
  }
}

// ===================================================================================================
// ===================================================================================================
//
// Volume control
//
// ===================================================================================================
// ===================================================================================================
void Marcduino::m_volumeDown(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$-\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$-\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_volumeUp(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$+\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$+\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_volumeMid(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$m\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$m\r"), &MD_Dome_Serial);
  }
}
void Marcduino::m_volumeMax(void)
{
  if ( m_settings[iSoundMaster] == 1 ) { 
    m_sendCommand(String((String)"$f\r"), &MD_Body_Serial);
  } else {
    m_sendCommand(String((String)"$f\r"), &MD_Dome_Serial);
  }
}

// ===================================================================================================
// ===================================================================================================
//
// Sequence commands
//
// ===================================================================================================
// ===================================================================================================

void Marcduino::m_runSequence(uint8_t sequenceNumber)
{
  #if defined(DEBUG)
  if ( m_buttonIndex > -1 ) {
    Debug.print(DBG_VERBOSE, F("Marcduino"), F("m_runSequence()"), F("Sequence: "),(String)sequenceNumber);
  }
  #endif

  // -----------------------------------------------
  // Prevent sending an unsupported sequence number.
  // -----------------------------------------------

  if ( sequenceNumber < 0 ) {
    return;
  } else if ( sequenceNumber > 15 && sequenceNumber < 51 ) {
    return;
  } else if ( sequenceNumber > 57 ) {
    return;
  }

  // -------------------------------------
  // Always send the sequence to the dome.
  // -------------------------------------

  m_sendCommand(String(":SE" + m_leftPad(sequenceNumber) + '\r'), &MD_Dome_Serial);

  // --------------------------------------------------
  // Conditionally send panel and/or sound to the body.
  // --------------------------------------------------

  if ( m_settings[iBodyMaster] == 1 ) {
    switch (sequenceNumber) {
      case 10 : { m_bodyPanelClose(0);  break; }  // Quiet mode (close all body panels)
      case 11 : { m_bodyPanelClose(0);  break; }  // Full awake mode (close all body panels)
      case 13 : { m_bodyPanelClose(0);  break; }  // Mid awake mode (close all body panels)
      case 14 : { m_bodyPanelClose(0);  break; }  // Awake+ mode (close all body panels)
      default : { break; }
    }

    if ( m_settings[iSoundMaster] == 1 ) {
      switch (sequenceNumber) {
        case 1 : { m_soundScream();       break; }  // Scream         - scream sound, scream display, mp flicker 4 sec, hp flicker 4 sec
        case 2 : { m_soundWave();         break; }  // Wave           - happy sound (b2, #13), flash hp 4 seconds
        case 3 : { m_soundFastWave();     break; }  // Fast Wave      - moody sound (b3, #4), flash display 4 sec, hp flicker 4 sec
        case 4 : { m_soundWave2();        break; }  // Wave 2         - long happy sound (b3, #6), flash hp 3 sec
        case 5 : { m_soundBeepCantina();  break; }  // Beep Cantina   - beep cantina sound, flash hp 17 sec, spectrum display
        case 6 : { m_soundFaint();        break; }  // Faint/Short Circuit - faint sound, short circuit display, mp flicker 10 sec, hp flicker 10 sec
        case 7 : { m_soundCantinaDance(); break; }  // Cantina dance  - cantina sound, spectrum display, hp flicker 46 sec
        case 8 : { m_soundLeia();         break; }  // Leia           - Leia msg sound, hp 01 in RC mode, hp 01 flicker 34 sec, Leia display
        case 9 : { m_soundDisco();        break; }  // Disco          - disco sound, spectrum display, RLD displays "Star Wars", hp flicker indefinitely,
        case 10 : { m_soundStop();        break; }  // Quiet mode (stop sound)
        case 11 : { m_soundRandom();      break; }  // Full awake mode (random sound)
        case 13 : { m_soundRandom();      break; }  // Mid awake mode (random sound)
        case 14 : { m_soundRandom();      break; }  // Awake+ mode (random sound)
        case 15 : { m_soundScream();      break; }  // Scream, no panels
        default : { break; }
      }
    }
  }
}

void Marcduino::quietMode(void)       { m_runSequence(10); }
void Marcduino::m_fullAwakeMode(void) { m_runSequence(11); }
void Marcduino::m_midAwakeMode(void)  { m_runSequence(13); }
void Marcduino::m_awakePlusMode(void) { m_runSequence(14); }
void Marcduino::m_cantinaBeep(void)   { m_runSequence(5); }
void Marcduino::m_cantinaDance(void)  { m_runSequence(7); }
void Marcduino::m_disco(void)         { m_runSequence(9); }
void Marcduino::m_faint(void)         { m_runSequence(6); }
void Marcduino::m_fastWave(void)      { m_runSequence(3); }
void Marcduino::m_marchingAnts(void)  { m_runSequence(55); }
void Marcduino::m_leiaMessage(void)   { m_runSequence(8); }
void Marcduino::m_scream(void)        { m_runSequence(1); }
void Marcduino::m_wave(void)          { m_runSequence(2); }
void Marcduino::m_wave2(void)         { m_runSequence(4); }
