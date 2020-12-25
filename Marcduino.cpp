/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Marcduino.cpp - Library for the Marcduino system
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Marcduino.h"
#include "Settings_Marcduino.h"

#if defined(MARCDUINO)

// =====================
//      Constructor
// =====================
Marcduino::Marcduino(Buffer* pBuffer)
{
  _buffer = pBuffer;

  #if defined(DEBUG_MARCDUINO) || defined (DEBUG_ALL)
  _className = F("Marcduino::");
  #endif
}

// ================
//     begin()
// ================
void Marcduino::begin()
{
  #if defined(DEBUG_MARCDUINO) || defined(DEBUG_ALL)
  output = _className+F("begin()");
  output += F(" - Marcduino system started.");
    output += F("\r\n Dome");
      #ifdef MD_BODY_MASTER
      output += F(" and body");
      #endif
    output += F(" master defined.");
    output += F("\r\n  Sound to ");
      #if defined(MD_BODY_MASTER) && defined(BODY_SOUND)
      output += F("body master.");
      #else
      output += F("dome master.");
      #endif
  if (Serial) { Serial.println(output); }
  #endif
}

// ==============================
//     interpretController()
// ==============================
void Marcduino::interpretController()
{
  // Enter quiet mode if we lose the controllers.

  if ( ! _buffer->isControllerConnected() ) {

    #if defined(DEBUG_MARCDUINO) || defined (DEBUG_ALL)
    output = _className+F("interpretController()");
    output += F(" - No controller. Quiet mode.");
    if (Serial) { Serial.println(output); }
    #endif

    _quietMode();
    return;
  }

  // Skip interpreting when there are no valid buttons pressed.

  int8_t btnIdx = _getButtonIndex();
  if (btnIdx < 0 || btnIdx >= sizeof(domeCommands)) {
    return;
  }

  // Send the command(s) to the master board(s).

  _sendCommand(btnIdx);
}

// ======================
//      automation()
// ======================
void Marcduino::automation()
{
  // Holoprojector automation

  if (_buffer->isHoloAutomationRunning()) {
    unsigned long currentTime;
    for (int holoNumber = 0; holoNumber < NUMBER_OF_HOLOPROJECTORS; holoNumber++) {

      // Set the random delay interval for this holoprojector.

      if (_randomSeconds[holoNumber] == 0) {
        _randomSeconds[holoNumber] = random(AUTO_HP_DELAY_MIN, AUTO_HP_DELAY_MAX + 1) * 1000;
      }

      // Move both horizontal and vertical servos to a random position.

      currentTime = millis();
      if (currentTime > (_lastRandomTime[holoNumber] + _randomSeconds[holoNumber])) {
        _lastRandomTime[holoNumber] = currentTime;
        _randomSeconds[holoNumber] = 0;
        _sendCommand(getPgmString(cmd_HPRandomOn), MD_DomeSerial);
      }
    }
  }
}

// ==============================
//     runCustomPanelSequence()
// ==============================
void Marcduino::runCustomPanelSequence()
{

  /* FUTURE: This function limits each panel to a single, timed 
   *  opening/closing. I'm not satisfied with that, but have not 
   *  yet thought through what is needed to run a more complex 
   *  sequence.
   */


  #if defined(DEBUG_MARCDUINO) || defined (DEBUG_ALL)
  output = _className+F("runCustomPanelSequence()");
  output += F(" - Running custom routine");
  if (Serial) { Serial.println(output); }
  #endif

  // Initialize local variables.

  String cmd;
  String xx = "";
  uint8_t statusTotal = 0;

  // Repeat for each dome panel.

  for (uint8_t i = 0; i < NUMBER_OF_DOME_PANELS; i++) {

    // Prepare the dome panel number for use in the command string.
    // Left pad with 0 when the number is a single digit.

    if (i < 10) { xx = "0"; }
    xx += (String)(i+1);

    // Send the 'open dome panel' command.

    if (_panelState[i].Status == 1) {
      if ((_panelState[i].Start + (_panelState[i].Start_Delay * 1000)) < millis()) {
        cmd = ":OP"+xx+"\r";
        _sendCommand(cmd, MD_DomeSerial);
        _panelState[i].Status = 2;

        #if defined(DEBUG_MARCDUINO) || defined (DEBUG_ALL)
        output = F("  Opening panel #");
        output += i+1;
        if (Serial) { Serial.println(output); }
        #endif
      }
    }

    // Send the 'close dome panel' command.

    if (_panelState[i].Status == 2) {
      if ((_panelState[i].Start + ((_panelState[i].Start_Delay + _panelState[i].Open_Time) * 1000)) < millis()) {
        cmd = ":CL"+xx+"\r";
        _sendCommand(cmd, MD_DomeSerial);
        _panelState[i].Status = 0;

        #if defined(DEBUG_MARCDUINO) || defined (DEBUG_ALL)
        output = F("  Closing panel #");
        output += i+1;
        if (Serial) { Serial.println(output); }
        #endif
      }
    }
    statusTotal += _panelState[i].Status;
  }

  // If all the panels have now closed - close out the custom routine

  if (statusTotal == 0) {
//     _buffer->setStatus(Status::CustomDomePanelRunning, false);
     _buffer->setDomeCustomPanelRunning(false);
  }
}

// ========================
//      _sendCommand()
// ========================
void Marcduino::_sendCommand(uint8_t btnIdx)
{
  String pgmString;

  // Lookup the dome command.
  // When found, send it to the dome master.
  // When not found, do nothing.

  pgmString = getPgmString(domeCommands[btnIdx]);
  if (pgmString != "") {
    _sendCommand(pgmString, MD_DomeSerial);

    #if defined(DEBUG_MARCDUINO) || defined(DEBUG_ALL)
    output = _className+F("_sendCommand()");
    output += F(" - Send ");
    output += pgmString;
    output += F(" to dome master.");
    if (Serial) { Serial.println(output); }
    #endif
  }

  // Lookup the body command.
  // When found, send it to the body master.
  // When not found, do nothing.

  #ifdef MD_BODY_MASTER
  pgmString = getPgmString(bodyCommands[btnIdx]);
  if (pgmString != "") {
    _sendCommand(pgmString, MD_BodySerial);

    #if defined(DEBUG_MARCDUINO) || defined(DEBUG_ALL)
    output = _className+F("_sendCommand()");
    output += F(" - Send ");
    output += pgmString;
    output += F(" to body master.");
    if (Serial) { Serial.println(output); }
    #endif
  }
  #endif
};

void Marcduino::_sendCommand(String inStr, HardwareSerial targetSerial)
{
  // This function allows for the use of either a slip ring (wired)
  //  or a Feather Radio (wireless) connection to the dome.

  #ifndef FEATHER_RADIO

  targetSerial.print(inStr);

  #else

  if (targetSerial == MD_BodySerial) {
    targetSerial.print(inStr);
  } else {
    // Send one character at a time to the Feather Radio.
    for (int i=0; i<inStr.length(); i++) {
      targetSerial.write(inStr[i]);
    }
  }

  #endif
};

// =====================================
//      _getButtonIndex()
// =====================================
int8_t Marcduino::_getButtonIndex()
{
// This function evaluates which buttons are pressed and returns the index
// value appropriate for the _functionTypes and _standardFunctions arrays.

  int8_t out = -1;

  // First, check the D-pad buttons.

  if (_buffer->getButton(UP))          { out = 0; }
  else if (_buffer->getButton(RIGHT))  { out = 1; }
  else if (_buffer->getButton(DOWN))   { out = 2; }
  else if (_buffer->getButton(LEFT))   { out = 3; }
  if (_buffer->getButton(TRIANGLE))    { out = 20; }
  else if (_buffer->getButton(CIRCLE)) { out = 21; }
  else if (_buffer->getButton(CROSS))  { out = 22; }
  else if (_buffer->getButton(SQUARE)) { out = 23; }

  // Next, check for modifier buttons.
  // L2 and R2 are not evaluated here. They are
  //  evaluated in the dome and drive motor code.

  if (out > -1) {
    if (_buffer->getButton(SELECT))     { out += 4; }
    else if (_buffer->getButton(START)) { out += 8; }
    else if (_buffer->getButton(PS) || \
             _buffer->getButton(PS2))   { out += 12; }
    else if (_buffer->getButton(L1) || \
             _buffer->getButton(R1))    { out += 16; }
  }

  #if defined(DEBUG_MARCDUINO) || defined (DEBUG_ALL)
  if (out > -1) {
    output = _className+F("_getStandardFunctionIndex()");
    output += F(" - sfIndex: ");
    output += out;
    if (Serial) { Serial.println(output); }
  }
  #endif

  // Return the result.

  return out;
};

// ======================
//      _quietMode()
// ======================
void Marcduino::_quietMode()
{
  #if defined(MD_BODY_MASTER) && defined(BODY_SOUND)
  _sendCommand(getPgmString(cmd_DomeQuiet), MD_DomeSerial);
  _sendCommand(getPgmString(cmd_BodyQuiet), MD_BodySerial);
  #else
  _sendCommand(getPgmString(cmd_QuietMode), MD_DomeSerial);
  #endif
}

// ==========================
//      _setPanelState()
// ==========================
uint8_t Marcduino::_setPanelState(uint8_t idx)
{
  // This may be an unused function.

  uint8_t statusTotal = 0;
  for (uint8_t i = 0; i < NUMBER_OF_DOME_PANELS; i++) {
    if (_myPanels[i].Use) {
      _panelState[i].Status = 1;
      _panelState[i].Start = millis();
      _panelState[i].Start_Delay = _myPanels[i].Start_Delay;
      _panelState[i].Open_Time   = _myPanels[i].Open_Time;
      if ( _myPanels[i].Start_Delay > 30 || \
           _myPanels[i].Start_Delay < 0  || \
           _myPanels[i].Open_Time > 30   || \
           _myPanels[i].Open_Time < 0 )
        _panelState[i].Status = 0;
    }
    statusTotal += _panelState[i].Status;
  }

  #if defined(DEBUG_MARCDUINO) || defined (DEBUG_ALL)
  if (statusTotal > 0) {
    output = _className+F("_setPanelState()");
    if (Serial) { Serial.println(output); }
    for (uint8_t i = 0; i < NUMBER_OF_DOME_PANELS; i++) {
      output = F(" - Panel #"); output += i+1;
      output += F(" Status: "); output += _panelState[i].Status;
      output += F(" Start: "); output += _panelState[i].Start;
      output += F(" Delay: "); output += _panelState[i].Start_Delay;
      output += F(" Open: "); output += _panelState[i].Open_Time;
      if (Serial) { Serial.println(output); }
    }
  }
  #endif

  return statusTotal;
}
#endif
