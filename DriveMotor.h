/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DriveMotor.h - Library for supported drive motor controllers
 * Created by Brian Lubkeman, 10 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef __BLACBOX_DRIVE_MOTOR_H__
#define __BLACBOX_DRIVE_MOTOR_H__

#include <Servo.h>
//#include "../Controller/Controller.h"
#include "Controller.h"
#include "DebugUtils.h"

#define DEBUG

extern HardwareSerial &DriveMotor_Serial;

enum driveMotor_setting_index_e {
   iMotorDriver   // 0 - Motor driver.
 , iDeadMan       // 1 - Use dead man switch.
 , iDriveLatency  // 2 - Serial latency.
 , iServoDeadZone // 3 - Servo dead zone
};

enum driveMotor_pin_index_e {
   iDrivePin1   // 0 - Drive pin #1 (steering/left foot)
 , iDrivePin2   // 1 - Drive pin #2 (throttle/right foot)
 , iScriptPin   // 2 - Script pin (Roboteq only)
 , iDeadManPin  // 3 - Dead man switch pin
};

enum roboteq_setting_index_e {
   iCommMode      // 0 - Roboteq communication mode.
 , iMixing        // 1 - Tank-style mixing
};

enum sabertooth_setting_index_e {
   iDriveSpeed      // 0 - Normal drive speed
 , iOverdriveSpeed  // 1 - Overdrive speed
 , iTurnSpeed       // 2 - Turn speed
 , iRamping         // 3 - Ramping
 , iAddress         // 4 - Sabertooth address
 , iInvertTurn      // 5 - Invert turn direction.
};


/* ================================================================================
 *                             Parent Drive Motor Class
 * ================================================================================ */
class DriveMotor
{
  protected:
    Controller * m_controller;
    int* m_settings;
    byte* m_pins;

    Joystick_Drive* m_driveStick;
    Button* m_button;

    bool driveEnabled;
    bool driveStopped;
    byte speedProfile;
    byte prevConnStatus;

    const int m_servoMin = 0;
    const int m_servoCenter = 90;
    const int m_servoMax = 180;

    unsigned long m_previousTime;
    int m_throttle;
    int m_input1;
    int m_previousInput1;
    int m_steering;
    int m_input2;
    int m_previousInput2;

    bool m_isDeadmanPressed(void);
    void m_setSpeedProfile(void);

    virtual void m_drive(void) {};
    virtual void m_writeScript(void) {};

  public:
    DriveMotor(Controller* pController, const int settings[], const byte pins[]);
    virtual ~DriveMotor(void);
    void begin(void);
    void interpretController(void);
    virtual void stop(void) {};
};

/* ================================================================================
 *                           Roboteq SBL2360 or SBL1360
 * ================================================================================ */
class DriveMotor_Roboteq : public DriveMotor
{
  protected:
    Servo m_pulse1Signal;
    Servo m_pulse2Signal;
    Servo m_scriptSignal;
    byte* m_roboteqSettings;

    void m_analogToServo(int steering, int throttle);
    void m_writePulse(int input1, int input2);
    void m_writePulse(int input);
    void m_mixBHD(byte stickX, byte stickY);
    void m_writeSerial(String inStr);

    virtual void m_drive(void);
    virtual void m_writeScript(void);

  public:
    DriveMotor_Roboteq(Controller* pController, const int settings[], const byte pins[], const byte roboteqSettings[]);
    virtual ~DriveMotor_Roboteq(void);
    void begin(void);
    virtual void stop(void);
};

/* ================================================================================
 *                                    Sabertooth
 * ================================================================================ */
#include <Sabertooth.h>
class DriveMotor_Sabertooth : public DriveMotor
{
  protected:
    Sabertooth m_sabertooth;
    Servo leftFootSignal;
    Servo rightFootSignal;
    byte* m_sabertoothSettings;

    void m_fastRampDown(byte driveSpeed, int stickSpeed);
    void m_rampDown(byte driveSpeed, int stickSpeed);
    void m_rampUp(byte driveSpeed, int stickSpeed);

    virtual void m_drive(void);

  public:
    DriveMotor_Sabertooth(Controller* pController, const int settings[], const byte pins[], const byte sabertoothSettings[]);
    virtual ~DriveMotor_Sabertooth(void);
    void begin(void);
    virtual void stop(void);
};
#endif
