/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Dome_Motor.h - Library for a Syren 10 dome motor controller
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _PERIPHERAL_DOME_MOTOR_H_
#define _PERIPHERAL_DOME_MOTOR_H_

#include "Globals.h"
#include <Sabertooth.h>

extern String output;
extern HardwareSerial &MotorSerial;


/* ===============================================
 *  Dome Controls
 * ===============================================
 *  PS3 Navigation
 *
 *    Single Controller
 *    -----------------
 *    L2+Cross      = Disable dome automation
 *    L2+Circle     = Enable dome automation
 *    L2+Nav1 Stick = Rotate dome at variable speed and direction
 *    Cross         = Rotate dome CW at the fixed speed
 *    Circle        = Rotate dome CCW at the fixed speed
 *
 *    Dual Controller
 *    ---------------
 *    L2+Cross      = Disable dome automation
 *    L2+Circle     = Enable dome automation
 *    Nav2 Stick    = Rotate dome at variable speed and direction
 * =============================================== */


/* ============================================
 *           Dome_Motor class definition
 * ============================================ */
class Dome_Motor
{
  public:
    Dome_Motor(Buffer * pBuffer);
    void begin(void);
    void automation(void);
    void interpretController(void);

  // For use by attachInterrupt
    static Dome_Motor * Dome_Motor::anchor;
    static void domeISR(void);

  private:
    Buffer * _buffer;
    Sabertooth _Syren;

    int _turnDirection;
    float _targetPosition;
    unsigned long _stopTurnTime;
    unsigned long _startTurnTime;
    unsigned long _previousMillis;
    int _rotationStatus;

    void _nextCycle(void);
    uint8_t _getCycle(void);
    void _resetCycle(void);

    void _automationOff(void);
    void _automationOn(void);
    void _automationInit(void);
    void _automationReady(void);
    void _automationTurn(void);

    void _rotateDome(int);
    void _stopDome(void);
    int _mapStick(int);

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    String _className;
    #endif
};
#endif
