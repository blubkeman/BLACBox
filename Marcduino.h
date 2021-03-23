/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Marcduino.h - Library for the Marcduino system
 * Created by Brian Lubkeman, 23 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _MARCDUINO_H_
#define _MARCDUINO_H_

#include "Controller.h"

#if defined(DEBUG) || defined(TEST_CONTROLLER)
extern String output;
extern void printOutput(void);
#endif

extern String getPgmString(const char *);

#ifdef MARCDUINO
extern HardwareSerial &MD_Dome_Serial;
#ifdef MD_BODY_MASTER
extern HardwareSerial &MD_Body_Serial;
#endif
#endif

const int MARCDUINO_BAUD_RATE = 9600;  // Do not change this!


/* ==========================================
 *           Custom data structures
 * ========================================== */
typedef struct {
  byte Status;
  unsigned long Start;
  byte Start_Delay;
  byte Open_Time;
} PanelState_Struct;

/*
typedef struct {
  bool Use;
  int Start_Delay;
  int Open_Time;
} CustomPanelRoutine_Struct;
*/


/* =========================================================
 *           Marcduino class definition
 * ========================================================= */
class Marcduino {

  private:
    #if defined(PS4_CONTROLLER)
    Controller_PS4 * m_controller;
    #else
    Controller_PS3 * m_controller;
    #endif
    int m_buttonIndex;
    bool m_customPanelRunning;
    bool m_holoAutomationRunning;
    PanelState_Struct m_panelState[NUMBER_OF_DOME_PANELS];
    unsigned long m_randomSeconds[3];
    unsigned long m_lastRandomTime[3];

    void m_sendCommand(byte);
    void m_sendCommand(String, HardwareSerial *);
    byte m_setPanelState(byte);
    void m_setHoloAutomationRunning(bool);
    bool m_isHoloAutomationRunning(void);

    #ifdef DEBUG
    String m_className;
    #endif

  public:
    #if defined(PS4_CONTROLLER)
    Marcduino(Controller_PS4 * pController);
    #else
    Marcduino(Controller_PS3 * pController);
    #endif
    void begin(void);
    void interpretController(void);
    void quietMode(void);
    void runAutomation(void);
    void runCustomPanelSequence(void);
    bool isCustomPanelRunning(void);
};
#endif
