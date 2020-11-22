/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_DriveMotor_All.h - Library for parent class of drive motor controllers
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _PERIPHERAL_FOOTMOTOR_ALL_H_
#define _PERIPHERAL_FOOTMOTOR_ALL_H_

#include "Buffer.h"

extern String output;
extern HardwareSerial &DomeSerial;


/* =========================================================
 * This is a generic, parent class for all drive motor driver 
 * systems. It defines content common across all subclasses.
 * ========================================================= */
class DriveMotor
{
  public:
    DriveMotor(Buffer *);
    virtual ~DriveMotor(void);
    void begin(void);
    virtual void interpretController() {};
    void stop(void);

  protected:
    Buffer* _buffer;
    long _currentTime;
    long _previousTime;
    byte _driveSpeed1;
    byte _driveSpeed2;

    // DeadMan switch
    bool _isDeadmanPressed(void);

    // Drive functions
    bool _inOverdrive;
    void _toggleDriveSpeed(void);
    void _toggleDriveEnabled(void);
    bool _isUserDriving(void);
    virtual void _stopDrive(void) {};

    #if defined(DEBUG_DRIVE) || defined (DEBUG_ALL)
    String _className;
    #endif
};
#endif
