/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DriveMotor.h - Library for supported drive motor controllers
 * Created by Brian Lubkeman, 20 February 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _DRIVE_MOTOR_H_
#define _DRIVE_MOTOR_H_

#include "Controller.h"

extern bool driveEnabled;
extern bool driveStopped;
extern byte speedProfile;

extern String output;
extern void printOutput(void);

extern HardwareSerial &DriveSerial;


/* ================================================================================
 *                             Parent Drive Motor Class
 * ================================================================================ */
class DriveMotor
{
  protected:
    #if defined(PS4_CONTROLLER)
    Controller_PS4 * m_controller;
    #else
    Controller_PS3 * m_controller;
    #endif
    unsigned long m_previousTime;
    int m_throttle;
    int m_input1;
    int m_previousInput1;
    int m_steering;
    int m_input2;
    int m_previousInput2;

    bool m_isDeadmanPressed(void);
    void m_updateSpeedProfile(void);

    virtual void m_drive(void) {};
    virtual void m_writeSpeedProfile(void) {};

    #if defined(SBL1360) || defined(SABERTOOTH)
    void m_mixBHD(byte stickX, byte stickY);
    #endif

    #if defined(RS232)
    void m_serialWrite(String inStr);
    #endif

    #ifdef DEBUG
    String m_className;
    #endif

  public:
    DriveMotor(void);
    virtual ~DriveMotor(void);
    void begin(void);
    void interpretController(void);
    virtual void stop(void) {};
};

#if defined(SBL2360) || defined(SBL1360)
/* ================================================================================
 *                           Roboteq SBL2360 or SBL1360
 * ================================================================================ */
#if defined(PULSE)
#include <Servo.h>
#endif

class Roboteq_DriveMotor : public DriveMotor
{
  private:
    #if defined(PULSE)
    Servo m_pulse1Signal;
    Servo m_pulse2Signal;
    Servo m_scriptSignal;
    #endif

    virtual void m_drive(void);
    virtual void m_writeSpeedProfile(void);

    #if defined(SBL2360)
    void m_mapInputs(int steering, int throttle);
    #endif

    #if defined(PULSE)
    void m_pulseWrite(int input1, int input2);
    void m_pulseWrite(int input1);
    #endif

  public:
    #if defined(PS4_CONTROLLER)
    Roboteq_DriveMotor(Controller_PS4 * pController);
    #else
    Roboteq_DriveMotor(Controller_PS3 * pController);
    #endif
    virtual ~Roboteq_DriveMotor(void);
    void begin(void);
    virtual void stop(void);
};
#endif

#endif
