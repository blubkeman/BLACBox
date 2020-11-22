/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_SBL2360_Pulse.h - Library for the Roboteq SBL2360 drive motor controller using command priority Pulse
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _PERIPHERAL_SBL2360_PULSE_H_
#define _PERIPHERAL_SBL2360_PULSE_H_

#include "Buffer.h"
#include "Peripheral_DriveMotor_All.h"
#include <Servo.h>

extern String output;
extern void printOutput(void);


/* ===============================================================
 * This is a child class for the Roboteq SBL2360 motor controller.
 * This is the version using Pulse command priority.
 * =============================================================== */
class SBL2360_PWM : public DriveMotor
{
  public:
    SBL2360_PWM(Buffer *);
    virtual ~SBL2360_PWM(void);
    void begin(void);
    virtual void interpretController(void);

  // For use by attachInterrupt
    static SBL2360_PWM * SBL2360_PWM::anchor;
    static void driveISR(void);

  protected:
    Servo _throttleSignal;
    Servo _steeringSignal;
    int _previousThrottle;
    int _previousSteering;
    long _previousTime;

    virtual void _runDrive(void);
    virtual void _stopDrive(void);
};
#endif
