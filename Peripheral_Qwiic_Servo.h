/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Qwiic_Servo.h - Library for an Adafruit PCA9685 16-Channel Servo Driver
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _PERIPHERAL_QWIIC_SERVO_H_
#define _PERIPHERAL_QWIIC_SERVO_H_

#include "Buffer.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

extern String output;
extern void printOutput(void);

const uint8_t CLOSED = 0;
const uint8_t OPENING = 1;
const uint8_t OPEN = 2;
const uint8_t CLOSING = 3;

/* ============================================
 *      Qwiic MP3 Trigger class definition
 * ============================================ */
class Qwiic_Servo
{
  public:
    Qwiic_Servo(Buffer * pBuffer);
    void begin(void);
    void interpretController(void);
    void move(void);

  private:
    Buffer * _buffer;
    Adafruit_PWMServoDriver _pwm;
    uint8_t _servoState[NUMBER_OF_BODY_PANELS];
    unsigned long _pulseLen[NUMBER_OF_BODY_PANELS];

    void _openPanel(uint8_t);
    void _closePanel(uint8_t);

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    String _className;
    #endif
};
#endif
