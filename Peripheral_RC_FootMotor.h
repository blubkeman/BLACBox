/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_RC_FootMotor.h - Library for R/C foot motor controllers
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _PERIPHERAL_RC_FOOTMOTOR_H_
#define _PERIPHERAL_RC_FOOTMOTOR_H_

#include "Globals.h"
#include <Servo.h>
#include "Support_Foot_Motors.h"

extern String output;
extern HardwareSerial &MotorSerial;


/* ===============================================
 *  Drive Controls
 * ===============================================
 *  PS3 Navigation (single or dual)
 *  
 *    PS+Cross      = Disable drive/steering
 *    PS+Circle     = Enable drive/steering
 *
 *    Without Deadman Switch
 *    ======================
 *    Nav1 Stick    = Drive, steer, 'normal' speed
 *    L2+Nav1 Stick = Drive, steer, 'turbo' speed
 *    
 *    With Deadman Switch
 *    ======================
 *    L2+Nav1 Stick    = Drive, steer, 'normal' speed
 *    L2+L1+Nav1 Stick = Drive, steer, 'turbo' speed
 *    
 * =============================================== */

/* ===========================================================
 * This is a child class for remote control (RC) foot motors.
 * =========================================================== */
class RC_FootMotor : public FootMotor
{
  public:
    RC_FootMotor(Buffer *);
    virtual void begin(void);
    virtual void calculateDrive(void);

  // For use by attachInterrupt
    static RC_FootMotor * RC_FootMotor::anchor;
    static void footISR(void);

  protected:
    Servo _leftFootSignal;
    Servo _rightFootSignal;
    int _leftFoot;      // Left motor value sent to motor controller
    int _rightFoot;     // Right motor value sent to motor controller
    byte _drivespeed1;  // 'Normal' speed
    byte _drivespeed2;  // 'Turbo' speed

    void _runMotors(void);
    virtual void _stopFeet(void);
};
#endif
