/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Drive_Motors.h - Library for supported drive motor controllers
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _DRIVE_MOTORS_H_
#define _DRIVE_MOTORS_H_

#include "Buffer.h"
#include <Sabertooth.h>

#if defined(USE_SERVO)
#include <Servo.h>
#endif

extern String output;
extern HardwareSerial &DriveSerial;


/* ================================================================================
 *                             Parent Drive Motor Class
 * ================================================================================ */

class DriveMotor
{
  public:
    DriveMotor(Buffer *);
    virtual ~DriveMotor(void);
    void begin(void);
    virtual void interpretController() {};
    virtual void stop(void) {};

  protected:
    Buffer * _buffer;
    long _currentTime;
    long _previousTime;

    // DeadMan switch
    bool _isDeadmanPressed(void);

    // Drive functions
    void _toggleDriveEnabled(void);
    void _updateSpeedProfile(void);
    bool _isUserDriving(void);

    // Serial functions
    void _serialWrite(String);
    void _serialRead(void);

    #if defined(SBL1360)
    int _leftFoot;
    int _rightFoot;
    void _mixBHD(byte, byte);
    #endif
    
    #if defined(DEBUG_DRIVE) || defined (DEBUG_ALL)
    String _className;
    #endif
};


/* ================================================================================
 *                           Roboteq SBL2360 or SBL1360
 * ================================================================================ */
class Roboteq_DriveMotors : public DriveMotor
{
  public:
    Roboteq_DriveMotors(Buffer *);
    virtual ~Roboteq_DriveMotors(void);
    void begin(void);
    virtual void interpretController(void);
    virtual void stop(void);

  protected:
    int _steering;
    int _throttle;
    int _previousSteering;
    int _previousThrottle;
    void _drive(void);

    #if defined(SBL2360_PULSE) && defined(USE_SERVO)
    Servo _steeringSignal;
    Servo _throttleSignal;
    void _servoWrite(void);
    #endif

    #if defined(SBL1360) && defined(USE_SERVO)
    Servo _leftFootSignal;
    Servo _rightFootSignal;
    void _servoWrite(void);
    #endif
};


/* ================================================================================
 *                                    Sabertooth
 * ================================================================================ */
class Sabertooth_DriveMotors : public DriveMotor
{
  public:
    Sabertooth_DriveMotors(Buffer *);
    virtual ~Sabertooth_DriveMotors(void);
    void begin(void);
    virtual void interpretController(void);
    virtual void stop(void);

  protected:
    Sabertooth ST;
    int _steering;
    int _throttle;
    int _previousSteering;
    int _previousThrottle;
    void _drive(void);

    // Sabertooth-specific variables and methods to be determined.

    #if defined(USE_SERVO)
    Servo _leftFootSignal;
    Servo _rightFootSignal;
    void _servoWrite(void);
    #endif
};
#endif
