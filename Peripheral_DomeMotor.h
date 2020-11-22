/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Dome_Motor.h - Library for a Syren 10 dome motor controller
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _PERIPHERAL_DOME_MOTOR_H_
#define _PERIPHERAL_DOME_MOTOR_H_

#include "Buffer.h"
#include <Sabertooth.h>

extern String output;
extern void printOutput(void);
extern HardwareSerial &DomeSerial;

// Dome automation stages

const uint8_t STOPPED = 0;
const uint8_t PREPARED = 1;
const uint8_t TURNING = 2;


/* ===============================================
 *  Dome Controls
 * ===============================================
 *  PS3 Navigation
 *
 *  Without DEBUG_DRIVEman Switch
 *  ======================
 *
 *  Single Controller
 *  -----------------
 *  L2+Cross      = Disable dome automation
 *  L2+Circle     = Enable dome automation
 *  L2+Nav1 Stick = Rotate dome at variable speed and direction
 *
 *  Dual Controller
 *  ---------------
 *  L2+Cross      = Disable dome automation
 *  L2+Circle     = Enable dome automation
 *  Nav2 Stick    = Rotate dome at variable speed and direction
 *
 *  With DEBUG_DRIVEman Switch
 *  ======================
 *
 *  Single Controller
 *  -----------------
 *  L1+Cross      = Disable dome automation
 *  L1+Circle     = Enable dome automation
 *  L1+Nav1 Stick = Rotate dome at variable speed and direction
 *    
 *  Dual Controller
 *  ---------------
 *  L1+Cross      = Disable dome automation
 *  L1+Circle     = Enable dome automation
 *  Nav2 Stick    = Rotate dome at variable speed and direction
 *    
 *  Custom addtion to Single Controller w/ or w/o DEBUG_DRIVEman Switch
 *  ------------------------------------------------------------
 *  Cross         = Rotate dome CW at the fixed speed
 *  Circle        = Rotate dome CCW at the fixed speed
 *
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
    void stop(void);
    static void domeISR(void);

  private:
    Buffer * _buffer;
    Sabertooth _Syren;

    int _turnDirection;
    float _targetPosition;
    unsigned long _stopTurnTime;
    unsigned long _startTurnTime;
    unsigned long _previousTime;
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
