/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Qwiic_Servo.cpp - Library for a Sparkfun Qwiic MP3 Trigger
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Peripheral_Qwiic_Servo.h"

#if defined(QWIIC)

// =====================
//      Constructor
// =====================
Qwiic_Servo::Qwiic_Servo(Buffer* pBuffer) : _pwm(0x04, Wire)
{
  _buffer = pBuffer;

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  _className = F("Qwiic_Servo::");
  #endif
}


// =====================
//        begin()
// =====================
void Qwiic_Servo::begin()
{
  _pwm.begin();

  /*
   * In theory the internal oscillator (clock) is 25MHz but it really isn't
   * that precise. You can 'calibrate' this by tweaking this number until
   * you get the PWM update frequency you're expecting!
   * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
   * is used for calculating things like writeMicroseconds()
   * Analog servos run at ~50 Hz updates, It is importaint to use an
   * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
   * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
   *    the I2C PCA9685 chip you are setting the value for.
   * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
   *    expected value (50Hz for most ESCs)
   * Setting the value here is specific to each individual I2C PCA9685 chip and
   * affects the calculations for the PWM update frequency. 
   * Failure to correctly set the int.osc value will cause unexpected PWM results
   */

  _pwm.setOscillatorFrequency(27000000);
  _pwm.setPWMFreq(QWIIC_SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);

  // --------------------------
  // Initialize state machines.
  // --------------------------

  for (uint8_t i = 0; i < NUMBER_OF_BODY_PANELS; i++) {
    _servoState[i] = CLOSED;
    _pulseLen[i] = QWIIC_SERVO_MIN;
  }

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("begin()");
  output += F(" - Qwiic servos started.");
  printOutput();
  #endif
}


// ==============================
//     interpretController()
// ==============================
void Qwiic_Servo::interpretController()
{
  #if defined(MD_STD_CONTROLS)

  /* =====================================================
   * Based on standard Marcduino commands, these are the
   * button combinations that trigger a sound action.
   *
   * TRIANGLE - Open body panel #2
   * CROSS    - Close body panel #2
   * SQUARE   - Open body panel #1
   * CIRCLE   - Close body panel #1
  /* ===================================================== */

  if ( ! ( _buffer->getButton(SELECT) && _buffer->getButton(START) && _buffer->getButton(PS) && _buffer->getButton(L1) )) {
    if ( _buffer->getButton(SQUARE) )        { _servoState[0] = OPENING; }
    else if ( _buffer->getButton(CIRCLE) )   { _servoState[0] = CLOSING; }
    else if ( _buffer->getButton(TRIANGLE) ) { _servoState[1] = OPENING; }
    else if ( _buffer->getButton(CROSS) )    { _servoState[1] = CLOSING; }
  }

  #endif
}


// ====================
//        move()
// ====================
void Qwiic_Servo::move()
{
  for (uint8_t i = 0; i < NUMBER_OF_BODY_PANELS; i++) {

    if ( _servoState[i] == OPENING ) {

      _pulseLen[i]++;
      _pwm.setPWM(i, 0, _pulseLen[i]);
      if ( (uint8_t)_pulseLen[i] == QWIIC_SERVO_MAX )
        _servoState[i] = OPEN;

    } else if ( _servoState[i] == CLOSING ) {

      _pulseLen[i]--;
      _pwm.setPWM(i, 0, _pulseLen[i]);
      if ( (uint8_t)_pulseLen[i] == QWIIC_SERVO_MIN )
        _servoState[i] = CLOSED;

    }
  }
}
#endif
