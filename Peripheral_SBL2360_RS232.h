/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_SBL2360_RS232.h - Library for the Roboteq SBL2360 drive motor controller using command priority RS232
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _PERIPHERAL_SBL2360_RS232_H_
#define _PERIPHERAL_SBL2360_RS232_H_

#include "Buffer.h"
#include "Peripheral_DriveMotor_All.h"

extern String output;
extern void printOutput(void);
extern HardwareSerial &DriveSerial;


/* ===============================================================
 * This is a child class for the Roboteq SBL2360 motor controller.
 * This is the version using RS232 command priority.
 * =============================================================== */
class SBL2360_Serial : public DriveMotor
{
  public:
    SBL2360_Serial(Buffer *);
    virtual ~SBL2360_Serial(void);
    void begin(void);
    virtual void interpretController(void);

  // For use by attachInterrupt
    static SBL2360_Serial * SBL2360_Serial::anchor;
    static void driveISR(void);

  protected:
    int _previousSpeed;
    int _previousSteering;
    int _previousThrottle;
    long _previousTime;

    virtual void _runDrive(void);
    virtual void _stopDrive(void);

    void _serialWrite(String);
    void _serialRead(void);
};
#endif

/* ===============================================
 *  Serial wiring between SBL2360 and Arduino Mega
 * ===============================================
 *
 *  DB25 connector       Arduino Mega
 *  --------------       ------------
 *  Pin 2 - Tx      -->  Pin 18 (Rx)
 *  Pin 3 - Rx      <--  Pin 19 (Tx)
 *  Pin 5 - Ground  ---  GND
 *  Pin x - 5VOut
 *
 *  There needs to be a TTL to RS232 adapter between these connections.
 *    Place a 4.7K Ohm resister in front of Pin 2.
 *    Place a 1N5819 shotsky diode between the Pin 2/Pin 18 wire and 
 *      the ground wire, cathode towards the Pin 2/Pin 18 wire.
 *    Place a 1N5819 shotsky diode between the Pin 2/Pin 18 wire and 
 *      the 5VOut pin, cathode towards the 5VOut pin.
 *
 *  The RS232 output must be set to 'inverted' in the Roborun Utility.
 *    From the Console send:  ^RSBR 5
 *    This is specific to baud rate 115200. See the SBL2360 manual if
 *      you have changed to a different baud rate.
 *    Make sure the controller response is:  +
 *    From the Console send:  ~RSBR
 *    If the response is - or a value different than what was
 *      entered, then an inverted signal is not supported.
 *    From the Console send:  %EESAV
 *      This will save the settings change.
 *
 *  Make sure at least one of the "Input Command Modes and Priorities"
 *    in the Console is set to Serial.
 *
 *  Commands
 *    Command Priorities
 *      Priority 1: RS232 (Serial, from Arduino or Roborun Utility)
 *  Motor 1 Configuration
 *    Number of Poles: 8
 *    Closed Loop Feedback Sensor: Hall Sensor
 *  Motor 1 Output
 *    Speed & Acceleration
 *      Max Speed (RPM): 5000
 *      Acceleration (RPM/s): 5000
 *      Deceleration (RPM/s): 5000
 *  Motor 2 Configuration
 *    Direction: Inverted
 *    Number of Poles: 8
 *    Closed Loop Feedback Sensor: Hall Sensor
 *  Motor 2 Output
 *    Speed & Acceleration
 *      Max Speed (RPM): 5000
 *      Acceleration (RPM/s): 5000
 *      Deceleration (RPM/s): 5000
 *  General
 *    Mixing: Mixed Mode 3
 *
 *  Commands are case-insensitive and terminated by a carriage return ('\r').
 *    The underscore character ('_') is interpreted as a carriage return.
 *    All characters lower than 0x20 (space) are ignored.
 *
 *  Commands that expect no reply send: +\r  as an acknowledgement.
 *  All valid characters received are echoed back. We can use this to verify
 *    our commands sent.
 *  If we receive:  -\r  then there has been an error.
 *
 *  The SBL2360 has a watchdog setting that will stop the motor if it fail to
 *    receive a valid command for a set period (default = 1 second). We can
 *    send a QRY character (0x05). The response will be an ACK character (0x06).
 *
 *  Runtime commands start with:             !
 *  Runtime queries start with:              ?
 *  Maintenance commands start with:         %
 *  Configuration read commands start with:  ~
 *  Comfiguration write commands start with: ^
 * =============================================== */


/* ===============================================
 *  Drive Controls
 * ===============================================
 *  PS3 Navigation (single or dual)
 *
 *  PS+Cross  = Disable drive/steering
 *  PS+Circle = Enable drive/steering
 *
 *  Without DEBUG_DRIVEman Switch
 *  ======================
 *     Nav1 Stick = Drive, steer, 'normal' speed
 *  L2+Nav1 Stick = Drive, steer, 'turbo' speed
 *
 *  With DEBUG_DRIVEman Switch
 *  ======================
 *     L1+Nav1 Stick = Drive, steer, 'normal' speed
 *  L2+L1+Nav1 Stick = Drive, steer, 'turbo' speed
 *
 * =============================================== */
