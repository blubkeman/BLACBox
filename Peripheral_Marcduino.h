/*
 * Peripheral_Marcduino.h - Library for Marcduino integration for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 18 April 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
*/

//  WORK IN PROGRESS

#ifndef _PERIPHERAL_MARCDUINO_H_
#define _PERIPHERAL_MARCDUINO_H_

#include <avr/pgmspace.h>
#include "Globals.h"
#include "Settings_MD_Functions.h"

typedef struct
{
  byte Status;
  unsigned long Start;
  byte Start_Delay;
  byte Open_Time;
} PanelState_Struct;

extern String output;

/* =========================================================
 *           Marcduino class definition
 * ========================================================= */
class Marcduino {

  private:
    Buffer* _buffer;
    String _namespace;
    String _functionName;
    CustomFunctions_Struct _myFunction;
    PanelSettings_Struct   _myPanels[NUMBER_OF_DOME_PANELS];
    PanelState_Struct      _panelState[NUMBER_OF_DOME_PANELS];
    void _runStandardFunction(uint8_t);
    void _runCustomFunction(uint8_t);
    void _sendToBody(String);
    void _sendToDome(String);
    int8_t _getStandardFunctionIndex();
    int8_t _getCustomFunctionIndex(uint8_t);
    int8_t _getPanelRoutineIndex(uint8_t);
    String _get_pgm_string(const char *tableValue);
    void _setMyFunction(uint8_t, uint8_t);
    uint8_t _setPanelState(uint8_t);
    
  // For automation
    unsigned long _randomSeconds[3];
    unsigned long _lastRandomTime[3];

  public:
    // =====================
    //      Constructor
    // =====================
    Marcduino(Buffer* pBuffer) {
      _buffer = pBuffer;
      _buffer->setStatus(Status::CustomDomePanelRunning, false);
      _namespace = F("Marcduino::");
    };

    // ================
    //     begin()
    // ================
    void begin() {

      String _functionName = _namespace+F("begin()");

      #ifdef BLACBOX_DEBUG
      Serial.println(_functionName);
      #ifndef MD_BODY_MASTER
      Serial.println(F("  Dome master defined."));
      #else
      Serial.print(F("  Dome and body master defined. Sound to "));
      #ifndef MD_BODY_SOUND
      Serial.print(F("dome"));
      #else
      Serial.print(F("body"));
      #endif
      Serial.println(F(" master."));
      #endif
      #endif
    };

    // ==============================
    //     interpretController()
    // ==============================
    void interpretController() {

      String _functionName = _namespace+F("interpretController()");

      // Enter quiet mode if we lose the controllers.
      if (_buffer->getCycle() == 2) {
        #ifdef BLACBOX_DEBUG
        Serial.println(_functionName);
        Serial.println(F("  No controller. Quiet mode."));
        #endif
        _runStandardFunction(standardFunctions[3]);
        return;
      }

      int8_t sfIdx = _getStandardFunctionIndex();

      // Stop processing when there is no valid buttons pressed.
      if (sfIdx < 0 || sfIdx >= sizeof(functionTypes)) { return; }

      #ifdef BLACBOX_VERBOSE
      Serial.println(_functionName);
      output = F("  Function type known");
      Serial.println(output);
      #endif

      if (functionTypes[sfIdx] == 0)
        _runStandardFunction(standardFunctions[sfIdx]);
      else
        _runCustomFunction(sfIdx);

    };

    // ======================
    //      automation()
    // ======================
    void automation() {

      String _functionName = _namespace+F("automation()");

      // Holoprojector automation

      #ifdef HOLOPROJECTORS
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
            Serial1.print(table_stdCmd[22]);
          }
        }
      }
      #endif
    };

    // ==============================
    //     runCustomPanelSequence()
    // ==============================
    void runCustomPanelSequence() {

      String _functionName = _namespace+F("runCustomPanelSequence()");

      #ifdef BLACBOX_VERBOSE
      Serial.println(_functionName);
      Serial.println(F("  Running custom routine"));
      #endif

      String cmd;
      uint8_t idx;
      uint8_t statusTotal = 0;
      for (uint8_t i = 0; i < NUMBER_OF_DOME_PANELS; i++) {
        if (_panelState[i].Status == 1) {
          idx = 34+(i*2);
          if ((_panelState[i].Start + (_panelState[i].Start_Delay * 1000)) < millis()) {
            // Send open dome panel #i+1
            Serial1.print(table_stdCmd[idx]);
            _panelState[i].Status = 2;

            #ifdef BLACBOX_DEBUG
            output = F("  Opening panel #"); output += i+1;
            Serial.println(output);
            #endif
          }
        }
        if (_panelState[i].Status == 2) {
          idx = 35+(i*2);
          if ((_panelState[i].Start + ((_panelState[i].Start_Delay + _panelState[i].Open_Time) * 1000)) < millis()) {
            // Send close dome panel #i+1
            Serial1.print(table_stdCmd[idx]);
            _panelState[i].Status = 0;

            #ifdef BLACBOX_DEBUG
            output = F("  Closing panel #"); output += i+1;
            Serial.println(output);
            #endif
          }
        }
        statusTotal += _panelState[i].Status;
      }
      // If all the panels have now closed - close out the custom routine
      if (statusTotal == 0) {
         _buffer->setStatus(Status::CustomDomePanelRunning, false);

      }
    };
};

// ================================
//      _runStandardFunction()
// ================================
void Marcduino::_runStandardFunction(uint8_t f) {

  String _functionName = _namespace+F("_runStandardFunction()");

  // Do nothing when given an invalid function number.

  if (f < 1 || f > 78) { return; }

  uint8_t idx = f-1;

  #ifdef BLACBOX_DEBUG
  Serial.println(_functionName);
  output = F("  Function: "); output += f;
  Serial.println(output);
  #endif

  #ifndef MD_BODY_MASTER

  // Only a dome master exists. Send it everything except body panel commands.

  if (f < 54 || f > 75)
    _sendToDome(_get_pgm_string(table_stdCmd[f]));

  #else
  #ifndef MD_BODY_SOUND

  // Both dome and body master exist, but sound is still sent to the dome master.
  // Send everything to the dome master except body panel commands.

  if (f < 54 || f > 75)
    _sendToDome(_get_pgm_string(table_stdCmd[f]));
  else
    _sendToBody(_get_pgm_string(table_stdCmd[f]));

  #else

  // Both dome and body master exist, but sound is sent to the body master.

  if (f >= 2 && f <= 14) {
    
    _sendToDome(_get_pgm_string(table_domeCmd[f-2]));
    _sendToBody(_get_pgm_string(table_bodyCmd[f-2]));
    
  } else if (f >= 54 && f <= 75)

    // Send body panel commands to the body master.

    _sendToBody(_get_pgm_string(table_stdCmd[f]));

  else

    // Send everything else to the dome master.

    _sendToDome(_get_pgm_string(table_stdCmd[f]));

  #endif

  #ifdef BLACBOX_VERBOSE
  Serial.print(tbl_stdDescr[f]);
  #endif

  #endif
  
};

// =======================
//      _sendToBody()
// =======================
void Marcduino::_sendToBody(String in) {
  Serial3.print(in);

  #ifdef BLACBOX_DEBUG
  Serial.print(F("  Body Command: "));
  Serial.print(in);
  Serial.println(F("\\r"));
  #endif
};

// =======================
//      _sendToDome()
// =======================
void Marcduino::_sendToDome(String in) {
  Serial1.print(in);

  #ifdef BLACBOX_DEBUG
  Serial.print(F("  Dome Command: "));
  Serial.print(in);
  Serial.println(F("\\r"));
  #endif
};

// ==============================
//      _runCustomFunction()
// ==============================
void Marcduino::_runCustomFunction(uint8_t inIdx) {

  String _functionName = _namespace+F("_runCustomFunction()");

  // Get the index for the custom function array
  int8_t cfIdx = _getCustomFunctionIndex(inIdx);

  // Validate the custom function index
  if (cfIdx < 0) {
    #ifdef BLACBOX_DEBUG
    Serial.println(_functionName);
    Serial.println(F("  Custom function type defined but not found"));
    #endif
    return;
  } else if (cfIdx >= sizeof(customFunctions)) {
    #ifdef BLACBOX_DEBUG
    Serial.println(_functionName);
    Serial.println(F("  Custom function index exceeds array size"));
    #endif
    return;
  }

  // Get the custom function settings.
  _setMyFunction(inIdx, cfIdx);

  #ifdef BLACBOX_DEBUG
  Serial.println(_functionName);
  #endif

  String soundCmd = "";
  String panelCmd = "";
  String logicCmd = "";
  uint8_t f;
  
  //
  // Get the custom sound command
  //

  if (_myFunction.mp3Number >= 182 && \
      _myFunction.mp3Number <= 200)  {
    uint8_t track = _myFunction.mp3Number - 175;
    soundCmd = F("$8");  // MP3Number 182 = bank 8 track 7.
    soundCmd += track;    // ...
    soundCmd += F("\r");  // MP3Number 200 = bank 8 track 25.

    // Execute the sound command.
    Serial1.print(soundCmd);

    #ifdef BLACBOX_DEBUG
    Serial.println(_functionName);
    output = F("  Sound Command: ");
    output += soundCmd;
    output += F("\\r");
    Serial.println(output);

    #ifdef BLACBOX_VERBOSE
    output = "  Sound descr: bank 8 track ";
    output += track;
    Serial.println(output);
    #endif

    #endif
  }

  //
  // Get the custom panel command
  //

  f = _myFunction.panelFunction;
  if (f > 0 && f < 9) {
    // Run a predesigned panel sequence
    panelCmd = _get_pgm_string(table_panelCmd[f]);

    // Close all the panels prior to next custom routine
    Serial1.print(table_stdCmd[33]);
    delay(50);    // give panel close command time to process before starting next panel command

    // Execute the panel command
    Serial1.print(panelCmd);

    #ifdef BLACBOX_DEBUG
    output = F("  Panel Command: ");
    output += panelCmd;
    output += "\\r";
    Serial.println(output);
    #ifdef BLACBOX_VERBOSE
    Serial.println(_functionName);
    output = "  Panel descr: ";
    output += table_panelDescr[f];
    Serial.println(output);
    #endif
    #endif

  } else if (f == 9) {
    // Prepare to run a custom panel sequence
    _buffer->setStatus(Status::CustomDomePanelRunning, true);
    if (_setPanelState(inIdx) == 0)
       _buffer->setStatus(Status::CustomDomePanelRunning, false);

    #ifdef BLACBOX_DEBUG
    if (_buffer->isCustomDomePanelRunning())
      Serial.println(F("  Panel Command: Running custom routine"));
    #endif
  }

  //
  // Get the custom logic display command
  //

  f = _myFunction.logicFunction;
  if (f > 0) {
    logicCmd = _get_pgm_string(table_logicCmd[f-1]);

    // Execute the logic display command.
    if (_myFunction.panelFunction > 0)
      delay(30);  // Give a slight delay to avoid conflict between panel and logic commands
    Serial1.print(logicCmd);

    #ifdef BLACBOX_DEBUG
    output = logicCmd; output += "\\r";
    Serial.print(output);
    #endif

    if (_myFunction.logicFunction == 8) {
      logicCmd = "@0M";
      logicCmd += _myFunction.customText;
      logicCmd += "\r";

      // Send the custom text.
      delay(50);  // Give another slight delay to avoid conflict
      Serial1.print(logicCmd);

      #ifdef BLACBOX_DEBUG
      output = logicCmd; output += "\\r";
      Serial.print(output);
      #endif
    }
    Serial.println();

    #ifdef BLACBOX_VERBOSE
    output = "  Logic descr: ";
    output += table_logicDescr[f];
    Serial.println(output);
    #endif
  }
};

// =====================================
//      _getStandardFunctionIndex()
// =====================================
int8_t Marcduino::_getStandardFunctionIndex() {

  String _functionName = _namespace+F("_getStandardFunctionIndex()");

// This function evaluates which buttons are pressed and returns the index
// value appropriate for the functionTypes and standardFunctions arrays.

  int8_t out = -1;

  // L2 is not used here. It is reserved for use with the dome and foot motors.
  if (_buffer->getButton(L2) || _buffer->getButton(R2))
    return out;

  if (_buffer->getButton(UP))          { out = 0; }
  else if (_buffer->getButton(RIGHT))  { out = 1; }
  else if (_buffer->getButton(DOWN))   { out = 2; }
  else if (_buffer->getButton(LEFT))   { out = 3; }
  if (_buffer->getButton(TRIANGLE))    { out = 20; }
  else if (_buffer->getButton(CIRCLE)) { out = 21; }
  else if (_buffer->getButton(CROSS))  { out = 22; }
  else if (_buffer->getButton(SQUARE)) { out = 23; }

  if (out > -1) {
    if (_buffer->getButton(SELECT))     { out += 4; }
    else if (_buffer->getButton(START)) { out += 8; }
    else if (_buffer->getButton(PS) || \
             _buffer->getButton(PS2))   { out += 12; }
    else if (_buffer->getButton(L1) || \
             _buffer->getButton(R1))    { out += 16; }
  }

  #ifdef BLACBOX_VERBOSE
  if (out > -1) {
    Serial.println(_functionName);
    output = F("  sfIndex: "); output += out;
    Serial.println(output);
  }
  #endif

  return out;

};

// ===================================
//      _getCustomFunctionIndex()
// ===================================
int8_t Marcduino::_getCustomFunctionIndex(uint8_t in) {

  String _functionName = _namespace+F("_getCustomFunctionIndex()");

  int8_t out;
  switch (in) {
      // Index 0 is reserved for initializations
    case 12: { out = 1; break; }  // PS + UP
    case 13: { out = 2; break; }  // PS + RIGHT
    case 14: { out = 3; break; }  // PS + DOWN
    case 15: { out = 4; break; }  // PS + LEFT
    default: { out = -1; break; } // Return an invalid index.
  };

  #ifdef BLACBOX_VERBOSE
  Serial.println(_functionName);
  output = F("  Index in: ");  output += in;
  Serial.println(output);
  output = F("  Index out: "); output += out;
  Serial.println(output);
  #endif

  return out;

};

// =================================
//      _getPanelRoutineIndex()
// =================================
int8_t Marcduino::_getPanelRoutineIndex(uint8_t in) {

  String _functionName = _namespace+F("_getPanelRoutineIndex()");

  int8_t out;
  switch (in) {
      // Index 0 is reserved for initializations
    case 12: { out = 1; break; }  // PS + UP
    default: { out = -1; break; } // Return an invalid index.
  };

  #ifdef BLACBOX_VERBOSE
  Serial.println(_functionName);
  output = F("  Index in: ");  output += in;
  Serial.println(output);
  output = F("  Index out: "); output += out;
  Serial.println(output);
  #endif

  return out;

};

// ==============================
//      _setMyFunction()
// ==============================
void Marcduino::_setMyFunction(uint8_t sfIdx, uint8_t cfIdx) {

  String _functionName = _namespace+F("_setMyFunction()");

  #ifdef BLACBOX_VERBOSE
  Serial.println(_functionName);
  output = F("  sfIndex in: "); output += sfIdx;
  Serial.println(output);
  output = F("  cfIndex in: "); output += cfIdx;
  Serial.println(output);
  #endif

  // Initialize it first.
  _myFunction = customFunctions[0];
  for (uint8_t i = 0; i < NUMBER_OF_DOME_PANELS; i++)
    _myPanels[i] = panelRoutine_NONE[i];

  #ifdef BLACBOX_VERBOSE
  Serial.println(F("  Initialized"));
  #endif

  // Now go get the custom function settings.
  _myFunction = customFunctions[cfIdx];
  if (_myFunction.panelFunction == 9) {
    int8_t prIdx = _getPanelRoutineIndex(sfIdx);
    for (uint8_t i = 0; i < NUMBER_OF_DOME_PANELS; i++)
      _myPanels[i] = (table_panelRoutines[prIdx])[i];
    #ifdef BLACBOX_VERBOSE
    Serial.println(_functionName);
    output = F("  prIndex out: "); output += prIdx;
    Serial.println(output);
    #endif
  }

  #ifdef BLACBOX_VERBOSE
  output = F("  Function settings set");
  Serial.println(output);
  #endif

};

// ==========================
//      _setPanelState()
// ==========================
uint8_t Marcduino::_setPanelState(uint8_t idx) {

  String _functionName = _namespace+F("_setPanelState()");

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

  #ifdef BLACBOX_VERBOSE
  if (statusTotal > 0) {
    Serial.println(_functionName);
    for (uint8_t i = 0; i < NUMBER_OF_DOME_PANELS; i++) {
      output = F("  Panel #"); output += i+1;
      output += F("  Status: "); output += _panelState[i].Status;
      output += F(" Start: "); output += _panelState[i].Start;
      output += F(" Delay: "); output += _panelState[i].Start_Delay;
      output += F(" Open: "); output += _panelState[i].Open_Time;
      Serial.println(output);
    }
  }
  #endif

  return statusTotal;
}

// ===========================
//      _get_pgm_string()
// ===========================
String Marcduino::_get_pgm_string(const char *tableValue) {

  String _functionName = _namespace+F("_get_pgm_string");

  // This function takes in an array element (row) that has been
  // stored in program memory and returns its string content.

  String pgmOutput = "";
  for (byte k = 0; k < strlen_P(tableValue); k++) {
    pgmOutput += (char)pgm_read_byte_near(tableValue + k);
  }
  return pgmOutput;
}

#endif
