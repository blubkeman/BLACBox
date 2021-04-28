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
const byte OPEN = 0;
const byte CLOSE = 1;


/* ==========================================
 *           Custom data structures
 * ========================================== */

typedef struct {
  byte panelNbr;
  int startDelay;
  int openDuration;
  unsigned long startedTime;
  bool completed;
} Panel_Routine_Struct;


/* =========================================================
 *           Marcduino class definition
 * ========================================================= */
class Marcduino {

  private:
    #if defined(PS5_CONTROLLER)
    Controller_PS5 * m_controller;
    #elif defined(PS4_CONTROLLER)
    Controller_PS4 * m_controller;
    #else
    Controller_PS3 * m_controller;
    #endif

    int m_buttonIndex;
    bool m_holoAutomationRunning;
    unsigned long m_randomSeconds[3];
    unsigned long m_lastRandomTime[3];

    bool m_cprRunning;
    int m_cprRowCount;
    int m_cprCompletedCount;
    unsigned long m_cprStartTime;
    String m_cprBuilCommand(String, int);
    Panel_Routine_Struct * m_cprRunningRoutine;
    void m_startCustomPanelRoutine(Panel_Routine_Struct [], int);

    bool m_aurabesh;

    void m_sendCommand(String, HardwareSerial *);

    // Preprogrammed modes
    void m_fullAwakeMode(void);
    void m_midAwakeMode(void);
    void m_awakePlusMode(void);

    void m_cantinaDance(void);
    void m_disco(void);
    void m_faint(void);
    void m_fastWave(void);
    void m_leiaMessage(void);
    void m_scream(void);
    void m_wave(void);
    void m_wave2(void);

    // Panel commands
    void m_runSequence(uint8_t sequenceNumber);
    void m_bodyPanelOpen(uint8_t panelNumber);
    void m_bodyPanelClose(uint8_t panelNumber);
    void m_bodyPanelRemoteControl(uint8_t panelNumber);
    void m_bodyPanelBuzzKill(uint8_t panelNumber);
    void m_bodyPanelHold(uint8_t panelNumber);
    void m_domePanelOpen(uint8_t panelNumber);
    void m_domePanelClose(uint8_t panelNumber);
    void m_domePanelRemoteControl(uint8_t panelNumber);
    void m_domePanelBuzzKill(uint8_t panelNumber);
    void m_domePanelHold(uint8_t panelNumber);

    // Holoprojector commands
    void m_hpRandomMove(uint8_t hpNumber);
    void m_hpLightOn(uint8_t hpNumber);
    void m_hpLightOff(uint8_t hpNumber);
    void m_hpRemoteControl(uint8_t hpNumber);
    void m_hpReset(uint8_t hpNumber);
    void m_hpHold(uint8_t hpNumber);
    void m_hpOnBoard(uint8_t hpNumber, uint8_t seconds);
    void m_hpFlicker(uint8_t hpNumber, uint8_t seconds);

    // Magic panel commands
    void m_magicPanel(uint8_t seconds);
    void m_magicPanelFlicker(uint8_t seconds);

    // Display logic commands
    void m_logicsTest(uint8_t displayNumber);
    void m_logicsReset(uint8_t displayNumber);
    void m_logicsAlarm(void);
    void m_logicsAlarmTimed(void);
    void m_logicsShortCircuit(void);
    void m_logicsScream(void);
    void m_logicsLeia(void);
    void m_logicsStarWars(void);
    void m_logicsMarch(void);
    void m_logicsOff(uint8_t displayNumber);
    void m_logicsSpectrum(uint8_t displayNumber);
    void m_logicsText(String text, uint8_t displayNumber);
    void m_logicsToggleFont(uint8_t displayNumber);
    void m_logicsRandomStyle(uint8_t style, uint8_t displayNumber);

    // PSI commands
    void m_psiOn(uint8_t psiNumber);
    void m_psiNormal(uint8_t psiNumber);
    void m_psiFirstColor(uint8_t psiNumber);
    void m_psiSecondColor(uint8_t psiNumber);
    void m_psiOff(uint8_t psiNumber);

    // Sound commands
    void m_soundNext(uint8_t bank);
    void m_soundFirst(uint8_t bank);
    void m_soundPlayTrack(uint8_t bank, uint8_t track);
    void m_soundRandom(void);
    void m_soundRandomOff(void);
    void m_soundStop(void);
    void m_soundScream(void);
    void m_soundFaint(void);
    void m_soundLeia(void);
    void m_soundShortCantina(void);
    void m_soundStarWars(void);
    void m_soundMarch(void);
    void m_soundLongCantina(void);
    void m_soundDisco(void);

    // Volume control
    void m_volumeDown(void);
    void m_volumeUp(void);
    void m_volumeMid(void);
    void m_volumeMax(void);

    // Command support functions
    String m_leftPad(uint8_t n, char c, uint8_t width);
    bool m_inList(const uint8_t valueToFind, const uint8_t list[]);

    #ifdef DEBUG
    String m_className;
    #endif

  public:
    #if defined(PS5_CONTROLLER)
    Marcduino(Controller_PS5 * pController);
    #elif defined(PS4_CONTROLLER)
    Marcduino(Controller_PS4 * pController);
    #else
    Marcduino(Controller_PS3 * pController);
    #endif
    void begin(void);
    void interpretController(void);
    void runHoloAutomation(void);
    void runCustomPanelRoutine();
    bool isCustomPanelRunning(void);

    // Preprogrammed modes
    void quietMode(void);
};
#endif
