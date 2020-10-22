/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Support_Foot_Motor.h - Library for parent class of foot motor controllers
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _SUPPORT_FOOT_MOTORS_H_
#define _SUPPORT_FOOT_MOTORS_H_

#include "Globals.h"

extern String output;
extern HardwareSerial &MotorSerial;


/* ===============================================
 *  Drive Controls
 * ===============================================
 *  PS3 Navigation (single or dual)
 *    PS+Cross      = Disable drive/steering
 *    PS+Circle     = Enable drive/steering
 *    
 *    Nav1 Stick    = Drive, steer (no dead man switch)
 *       or
 *    L2+Nav1 Stick = Drive, steer (L2=dead man switch)
 * =============================================== */

/* =========================================================
 * This is a generic, parent class for all foot motor driver 
 * systems. It defines content common across all subclasses.
 * ========================================================= */
class FootMotor
{
  public:
    FootMotor(Buffer *);
    virtual void interpretController();
    virtual void calculateDrive() {};

  protected:
    Buffer* _buffer;
    long _currentMillis;
    long _previousMillis;
    bool _isDeadMan();
    void _initDeadMan();
    virtual void _stopFeet() {};

    #if defined(DEBUG_FOOT) || defined (DEBUG_ALL)
    String _className;
    #endif
};
#endif
