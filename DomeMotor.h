/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DomeMotor.h - Library for supported dome motor controllers
 * Created by Brian Lubkeman, 5 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef __BLACBOX_DOME_MOTOR_H__
#define __BLACBOX_DOME_MOTOR_H__

#include <Sabertooth.h>
//#include "../Controller/Controller.h"
#include "Controller.h"

#define DEBUG
#if defined(DEBUG)
#define VERBOSE
#endif

extern HardwareSerial &DomeMotorSerial;

#if defined(DEBUG)
extern String output;
extern void printOutput(void);
#endif

enum automation_stage_e {
  STOPPED,
  READY,
  TURNING
};

enum domeMotor_setting_index_e {
  iDomeMotorDriver,   // 0 - Dome motor driver.
  iDomeSpeed,         // 1 - Dome speed.
  iAutoSpeed,         // 2 - Automated dome speed.
  iAutoSpeedMin,      // 3 - Automated dome speed minimum.
  iAutoSpeedMax,      // 4 - Automated dome speed maximum.
  iInvertTurn,        // 5 - Invert turn direction.
  iDomeLatency        // 6 - Serial latency.
}; 

enum domeMotor_timing_index_e {
  iTurn360,     // 0 - Time to turn a full 360 turn at automated dome speed.
  iTurn360Min,  // 1 - Minimum time allowed to complete a full 360 turn.
  iTurn360Max   // 2 - Maximum time allowed to complete a full 360 turn.
};

/* ================================================================================
 *                              Parent Dome Motor Class
 * ================================================================================ */
class DomeMotor
{
  protected:
    Controller* m_controller;
    byte* m_settings;
    unsigned long* m_timings;

    Joystick_Dome* m_joystick;
    Button* m_button;

    bool m_domeStopped;
    byte m_rotationStatus;
    byte m_turnDirection;
    unsigned int m_targetPosition;
    unsigned long m_stopTurnTime;
    unsigned long m_startTurnTime;
    unsigned long m_previousTime;
    bool m_automationRunning;
    bool m_automationSettingsInvalid;

    void m_automationOn(void);
    void m_automationOff(void);
    void m_automationInit(void);
    void m_automationReady(void);
    void m_automationTurn(void);

    virtual void m_rotateDome(int rotationSpeed) {};

    #if defined(DEBUG)
    String m_className;
    #endif

  public:
    DomeMotor(Controller* pController, const byte settings[], const unsigned long timings[]);
    ~DomeMotor(void);
    void begin(void);
    void interpretController(void);
    void runHoloAutomation(void);
    bool isAutomationRunning(void);

    virtual void stop(void) {};
};
/* ================================================================================
 *                              Syren10 Dome Motor Class
 * ================================================================================ */
class DomeMotor_Syren10 : public DomeMotor
{
  private:
    Sabertooth m_syren;

    int* m_syrenSettings;

    virtual void m_rotateDome(int rotationSpeed);

  public:
    DomeMotor_Syren10(Controller* pController, const byte settings[], const unsigned long timings[], const int syrenSettings[]);
    virtual ~DomeMotor_Syren10(void);
    void begin(void);

    virtual void stop(void);
};
#endif
