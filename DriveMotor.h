/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DriveMotor.h - Library for supported drive motor controllers
 * Created by Brian Lubkeman, 23 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef __BLACBOX_DRIVE_MOTOR_H__
#define __BLACBOX_DRIVE_MOTOR_H__

#include <Servo.h>
#include "Controller.h"

#if defined(DEBUG)
extern String output;
extern void printOutput(void);
#endif
extern HardwareSerial &DriveSerial;

enum driveMotor_setting_index_e {
  iMotorDriver,   // 0 - Motor driver.
  iCommMode,      // 1 - Roboteq communication mode.
  iDeadMan,       // 2 - Use dead man switch.
  iDriveLatency,  // 3 - Serial latency.
  iServoDeadZone, // 4 - Servo dead zone
  iMixing         // 5 - Tank-style mixing
};

enum driveMotor_pin_index_e {
  iDrivePin1,  // 0 - Drive pin #1 (steering/left foot)
  iDrivePin2,  // 1 - Drive pin #2 (throttle/right foot)
  iScriptPin,  // 2 - Script pin (Roboteq only)
  iDeadManPin  // 3 - Dead man switch pin
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

    Joystick_Drive* m_joystick;
    Button* m_button;

    bool driveEnabled;
    bool driveStopped;
    byte speedProfile;

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
    void m_mixBHD(byte stickX, byte stickY);
    void m_serialWrite(String inStr);

    #if defined(DEBUG)
    String m_className;
    #endif

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

    void m_mapInputs(int steering, int throttle);
    void m_writePulse(int input1, int input2);
    void m_writePulse(int input);

    virtual void m_drive(void);
    virtual void m_writeScript(void);

  public:
    DriveMotor_Roboteq(Controller* pController, const int settings[], const byte pins[]);
    virtual ~DriveMotor_Roboteq(void);
    void begin(void);
    virtual void stop(void);
};
#endif
