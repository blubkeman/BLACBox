/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DomeMotor.h - Library for supported dome motor controllers
 * Created by Brian Lubkeman, 16 June 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef __BLACBOX_DOME_MOTOR_H__
#define __BLACBOX_DOME_MOTOR_H__

#include <Sabertooth.h>
#include "../toolbox/DebugUtils.h"
#include "../controller/Controller.h"

#define DEBUG

extern HardwareSerial &DomeMotor_Serial;

enum automation_stage_e {
  STOPPED,
  READY,
  TURNING
};

enum domeMotor_setting_index_e {
  iDomeMotorDriver,   // 0 - Dome motor driver.
  iDomeSpeed,         // 1 - Dome speed.
  iAutoSpeedMin,      // 2 - Automated dome speed minimum.
  iAutoSpeedMax,      // 3 - Automated dome speed maximum.
  iAutoSpeed,         // 4 - Automated dome speed.
  iDomeLatency        // 5 - Serial latency.
};

enum domeMotor_timing_index_e {
  iTurn360Min,  // 0 - Minimum time allowed to complete a full 360 turn.
  iTurn360Max,  // 1 - Maximum time allowed to complete a full 360 turn.
  iTurn360,     // 2 - Time to turn a full 360 turn at automated dome speed.
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

    Joystick_Dome* m_domeStick;
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

  public:
    DomeMotor(Controller* pController, const byte settings[], const unsigned long timings[]);
    ~DomeMotor(void);
    void begin(void);
    void interpretController(void);
    void runAutomation(void);
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
