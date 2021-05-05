/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Marcduino.h - Library for the Marcduino system
 * Created by Brian Lubkeman, 5 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef __BLACBOX_MARCDUINO_H__
#define __BLACBOX_MARCDUINO_H__

//#include "../Controller/Controller.h"
#include "Controller.h"

#define DEBUG
#if defined(DEBUG)
//#define VERBOSE
#endif

#if defined(DEBUG)
extern String output;
extern void printOutput(void);
#endif
extern String getPgmString(const char *);
extern HardwareSerial &MD_Dome_Serial;
extern HardwareSerial &MD_Body_Serial;

const int MARCDUINO_BAUD_RATE = 9600;  // Do not change this!

typedef struct {
  byte panelNbr;
  int startDelay;
  int openDuration;
  unsigned long startedTime;
  bool completed;
} Panel_Routine_Struct;

enum marcduino_setting_index_e {
  iFxCntl,      // 0  - FX control system.
  iBodyMaster,  // 1  - Marcduino body master is used.
  iSoundMaster, // 2  - Marcduino master controling sound.
  iCmdSet,      // 3  - Marcduino command set.
  iSoundBoard,  // 4  - Sound board.
  iMagicPanel,  // 5  - Magic panel is used.
  iBodyPanels,  // 6  - Number of body panels on servos.
  iDomePanels,  // 7  - Number of dome panels on servos.
  iHp,          // 8  - Number of holoprojectors on servos.
  iHpDelayMin,  // 9  - HP automation delay min.
  iHpDelayMax,  // 10 - HP automation delay max.
  iRadio        // 11 - Feather radio in use 
};

enum panel_action_e {
  OPEN,
  CLOSE
};

/* =========================================================
 *           Marcduino class definition
 * ========================================================= */
class Marcduino {

  private:
    Controller* m_controller;
    byte* m_settings;

    Button* m_button;

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

    int  m_getButtonsPressed(void);
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
    void m_soundWave(void);
    void m_soundFastWave(void);
    void m_soundWave2(void);
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

    #if defined(DEBUG)
    String m_className;
    #endif

  public:
    Marcduino(Controller* pController, const byte settings[]);
    ~Marcduino(void);
    void begin(void);
    void interpretController(void);
    void runHoloAutomation(void);
    void runCustomPanelRoutine();
    bool isCustomPanelRunning(void);

    // Preprogrammed modes
    void quietMode(void);
};
#endif
