/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DomeMotor.h - Library for supported dome motor controllers
 * Created by Brian Lubkeman, 23 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _DOME_MOTOR_H_
#define _DOME_MOTOR_H_

#include <Sabertooth.h>
#include "Controller.h"

extern bool domeStopped;
extern HardwareSerial &DomeMotorSerial;

#if defined(DEBUG) || defined(TEST_CONTROLLER)
extern String output;
extern void printOutput(void);
#endif

// Dome automation stages

const byte STOPPED = 0;
const byte READY = 1;
const byte TURNING = 2;

/* ================================================================================
 *                              Parent Dome Motor Class
 * ================================================================================ */
class DomeMotor
{
  protected:
    #if defined(PS4_CONTROLLER)
    Controller_PS4 * m_controller;
    #else
    Controller_PS3 * m_controller;
    #endif
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

    #ifdef DEBUG
    String m_className;
    #endif

  public:
    DomeMotor(void);
    virtual ~DomeMotor(void);
    void begin(void);
    void interpretController(void);
    void runAutomation(void);
    bool isAutomationRunning(void);

    virtual void stop(void) {};
};


#if defined(SYREN10)
/* ================================================================================
 *                              Syren10 Dome Motor Class
 * ================================================================================ */
class Syren10_DomeMotor : public DomeMotor
{
  private:
    Sabertooth m_syren;

    virtual void m_rotateDome(int rotationSpeed);

  public:
    #if defined(PS4_CONTROLLER)
    Syren10_DomeMotor(Controller_PS4 * pController);
    #else
    Syren10_DomeMotor(Controller_PS3 * pController);
    #endif
    virtual ~Syren10_DomeMotor(void);
    void begin(void);

    virtual void stop(void);
};
#endif

#endif
