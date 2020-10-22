/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Marcduino.h - Library for the Marcduino system
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _PERIPHERAL_MARCDUINO_H_
#define _PERIPHERAL_MARCDUINO_H_

#include "Globals.h"
#include <avr/pgmspace.h>

extern String output;
extern HardwareSerial &MotorSerial;
extern HardwareSerial &DomeSerial;
extern HardwareSerial &BodySerial;


/* ==========================================
 *           Custom data structures
 * ========================================== */
typedef struct {
  byte Status;
  unsigned long Start;
  byte Start_Delay;
  byte Open_Time;
} PanelState_Struct;

typedef struct {
  bool Use;
  int Start_Delay;
  int Open_Time;
} CustomPanelRoutine_Struct;


/* =========================================================
 *           Marcduino class definition
 * ========================================================= */
class Marcduino {

  public:
    Marcduino(Buffer * pBuffer);
    void begin();
    void interpretController();
    void automation();
    void runCustomPanelSequence();

  private:
    Buffer * _buffer;

    int8_t _getButtonIndex();
    String _getPgmString(const char *); // Used with command arrays.
    String _getPgmString(const char);   // Used with cmd_xx constants.
    void _sendCommand(uint8_t);         // Used with command arrays
    void _sendCommand(String, HardwareSerial); // Send a string to a given Serial
    void _quietMode();

  // For custom panel routines.
    CustomPanelRoutine_Struct  _panelRoutine_NONE[NUMBER_OF_DOME_PANELS];
    CustomPanelRoutine_Struct  _panelRoutine_PSUP[NUMBER_OF_DOME_PANELS];
    CustomPanelRoutine_Struct  _myPanels[NUMBER_OF_DOME_PANELS];
    PanelState_Struct     _panelState[NUMBER_OF_DOME_PANELS]; 
    uint8_t _setPanelState(uint8_t);

  // For automation.
    unsigned long _randomSeconds[3];
    unsigned long _lastRandomTime[3];

    #if defined(DEBUG_MARCDUINO) || defined (DEBUG_ALL)
    String _className;
    String output;
    #endif
};
#endif
