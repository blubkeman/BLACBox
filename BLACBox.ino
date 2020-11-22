/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 *                          Last Revised Date: 22 November 2020
 *                          Revised By: Brian E. Lubkeman
 *  Inspired by the PADAWAN (danf), SHADOW (KnightShade), SHADOW_MD (vint43) effort
 * =================================================================================
 *
 * This program is free software: you can redistribute it and/or modify it for
 * your personal use and the personal use of other astromech club members.  
 *
 * This program is distributed in the hope that it will be useful 
 * as a courtesy to fellow astromech club members wanting to develop
 * their own droid control system.
 *
 * IT IS OFFERED WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You are using this software at your own risk and, as a fellow club member, it is
 * expected you will have the proper experience / background to handle and manage that 
 * risk appropriately.  It is completely up to you to insure the safe operation of
 * your droid and to validate and test all aspects of your droid control system.
 *
 * =======================================================================================
 * 
 * This is a heavily modified version of the S.H.A.D.O.W. controller system. 
 * It is an ambitious rewrite with the following goals.
 * 
 *   1.  Achieved - Improve the success rate of connecting my PS3 Move Navigation controllers.
 *   2.  Achieved - Decouple controller code from peripheral code through the use of an intermediary buffer.
 *   3.  Achieved - Encapsulate code into libraries.
 *   4.  Achieved - Support PS3 Move Navigation and PS4 controllers.
 *   5.  Achieved - Support the Marcduino system.
 *   6.  Achieved - Support sound sent to the optional Marcduino body master.
 *   7.  Testing  - Support dome motor control using a Syren 10.
 *   8.  Testing  - Support drive motor control using R/C without mixing (Roboteq SBL2360).
 *   9   Testing  - Support radio communication with dome electroncis instead of a slip ring.
 *   12. In progress - Support I2C-based peripherals to replace Marcduino body master.
 *   10. Future   - Support drive motor control using R/C with mixing (Roboteq SBL1360).
 *   11. Future   - Support drive motor control using Sabertooth.
 *   13. Future   - Support PS3 controllers available through the USB Host Shield code.
 *
 * =======================================================================================
 * 
 * This rewrite was developed and tested on the following equipment.
 *   Arduino Mega 2560                (https://www.sparkfun.com/products/11061)
 *   Sainsmart USB host shield        (https://www.amazon.com/gp/product/B006J4G000)
 *   TRENDNet USB bluetooth dongle    (https://www.amazon.com/gp/product/B002AQSTXM)
 *   PS3 Move Navigation controller   (used, from www.gamestop.com)
 *   PS4 controller                   (new, from www.gamestop.com)
 *   Compact Marcduino v1.5           (https://astromech.net/forums/showthread.php?30724-Compact-Marcduino-v1-5-BC-Approved-Continuous-Various-(Jan-2017)-Open)
 *   Sparkfun MP3 Trigger             (https://www.sparkfun.com/products/13720)
 *   Syren 10 motor controller        (https://www.dimensionengineering.com/products/syren10)
 *   Roboteq SBL2360 motor controller (https://www.roboteq.com/index.php/component/virtuemart/461/sbl2360-277-393-460-detail?Itemid=971)
 *   Adafruit Feather M0 RFM69HCW     (https://www.adafruit.com/product/3176)
 * Future equipment to be tested.
 *   Sabertooth motor controller      (https://www.dimensionengineering.com/products/sabertooth2x25)
 *   Sparkfun Qwiic MP3 Trigger       (https://www.sparkfun.com/products/15165)
 *   Adafruit I2C PWM driver          (https://www.adafruit.com/product/815)
 */

/* ===============================================
 *           Includes and instantiations
 * =============================================== */
#include "Settings.h"
#include "Buffer.h"

// ------------------------
// This is our control buffer. This is what allows us to decouple the controller
// code from the peripheral code. With this, I should be able to swap controllers
// from PS3 Nav to PS3 or PS4 after I create the appropriate header file and code
// for said controller and without rewriting all the peripheral code.
// ------------------------
#include "Buffer.h"
Buffer controlBuffer;

// ------------------------
// This is our controller. The PS3 Move Navigation is currently the only supported
// controller. The PS3 controller needs testing. By using the USB Host Shield
// Library 2.0, we should be able to implement any controller that library supports
// including the PS4, Xbox, and Wii.
// ------------------------
#if defined(PS3_NAVIGATION)
#include "Controller_PS3Nav.h"
Controller_PS3Nav controller(&controlBuffer);
Controller_PS3Nav * Controller_PS3Nav::anchor = { NULL }; // This pointer helps us with the attachOnInit functions.

#elif defined(PS3_CONTROLLER)
#include "Controller_PS3.h"
Controller_PS3 controller(&controlBuffer);
Controller_PS3 * Controller_PS3::anchor = { NULL }; // This pointer helps us with the attachOnInit functions.

#elif defined(PS4_CONTROLLER)
#include "Controller_PS4.h"
Controller_PS4 controller(&controlBuffer);
Controller_PS4 * Controller_PS4::anchor = { NULL }; // This pointer helps us with the attachOnInit functions.

#endif

// ------------------------
// These are our Serial pins.
// ------------------------
#if defined(SBL2360_SERIAL)
HardwareSerial &DriveSerial = Serial1;
  #if defined(MARCDUINO)
  HardwareSerial &MD_DomeSerial = Serial;
  #endif
#elif defined(MARCDUINO)
HardwareSerial &MD_DomeSerial = Serial1;
#endif

#if defined(SYREN10)
HardwareSerial &DomeSerial = Serial2;
#endif

#if defined(MARCDUINO) && defined(MD_BODY_MASTER)
HardwareSerial &MD_BodySerial = Serial3;
#endif

// ------------------------
// This is our dome motor.  It currently supports the Syren 10 motor controller.
// ------------------------
#if defined(SYREN10)
#include "Peripheral_DomeMotor.h"
Dome_Motor domeMotor(&controlBuffer);
Dome_Motor * Dome_Motor::anchor = { NULL };     // This pointer helps us with the interrupt service routine (ISR).
#endif

// ------------------------
// This is our drive motor controller. It supports the Roboteq SBL2360 controller using Pulse command priority.
// ------------------------
#if defined(SBL2360_PULSE)
#include "Peripheral_SBL2360_Pulse.h"
SBL2360_PWM driveMotors(&controlBuffer);
SBL2360_PWM * SBL2360_PWM::anchor = { NULL }; // This pointer helps us with the interrupt service routine (ISR)

#elif defined (SBL2360_SERIAL)
#include "Peripheral_SBL2360_RS232.h"
SBL2360_Serial driveMotors(&controlBuffer);
SBL2360_Serial * SBL2360_Serial::anchor = { NULL }; // This pointer helps us with the interrupt service routine (ISR)

/* ----- TO DO: Future development -----
 *
#elif defined(SBL1360)
#include "Peripheral_SBL1360.h"
SBL1360_DriveMotor driveMotors(&controlBuffer);
SBL1360_DriveMotor * SBL1360_DriveMotor::anchor = { NULL }; // This pointer helps us with the interrupt service routine (ISR)

#elif defined (GENERIC_RC)
#include "Peripheral_RC_Drive.h"
RC_DriveMotor driveMotors(&controlBuffer);
RC_DriveMotor * RC_DriveMotor::anchor = { NULL }; // This pointer helps us with the interrupt service routine (ISR)

#elif defined(SABERTOOTH)
#include "Peripheral_Sabertooth.h"
Sabertooth_DriveMotor driveMotors(&controlBuffer);
Sabertooth_DriveMotor * Sabertooth_DriveMotor::anchor = { NULL }; // This pointer helps us with the interrupt service routine (ISR)
*/
#endif

// ------------------------
// This is our sound, logic display, holoprojectors, dome panels, and body panels.
// The Marcduino system is supported.  I hope to support a future alternative
// that uses I2C.
// ------------------------
#if defined(MARCDUINO)
#include "Peripheral_Marcduino.h"
Marcduino marcduino(&controlBuffer);
#endif

// -------------------------------------------------------------------------------
// This is experimental. Trying to add in I2C support via Sparkfun's Qwiic system.
// The goal is to replace a Marcduino body master with an I2C alternative to free
// up one of the Serial pins on the Arduino Mega.
// -------------------------------------------------------------------------------
#if defined(QWIIC)
#include <Wire.h>
#include "Peripheral_Qwiic_MP3.h"
Qwiic_MP3 mp3(&controlBuffer);

#include "Peripheral_Qwiic_Servo.h"
Qwiic_Servo bodyPanels(&controlBuffer);
#endif

/* ================================
 *           Main Program
 * ================================ */
void setup() {

  // ----------------------------------------
  // Reserve 200 bytes for the output string.
  // ----------------------------------------

  output.reserve(200);

  // -------------------------
  // Start the serial monitor.
  // -------------------------

  #if defined(DEBUG_ALL)
  Serial.begin(115200);
  #if !defined(__MIPSEL__)
  while (!Serial);
  #endif
  #endif

  // ----------
  // Start I2C.
  // ----------
  #if defined(QWIIC)
  Wire.begin();   // Starting I2C with the Arduino Mega as master.
  #endif

  // -----------------------------
  // Start our controller objects.
  // -----------------------------

  controlBuffer.begin();
  controller.begin();

  // -----------------------------
  // Start our peripheral objects.
  // -----------------------------

  #if defined(SYREN10) || defined(SABERTOOTH)
    DomeSerial.begin(MOTOR_BAUD_RATE); // This is for the Syren 10 and/or Sabertooth
    while (!DomeSerial);
  #endif

  #ifdef SYREN10
    domeMotor.begin();
    // Setup an interrupt for stopping the dome motor.
    pinMode(DOME_INTERRUPT_PIN, INPUT_PULLUP);
    digitalWrite(DOME_INTERRUPT_PIN, HIGH);
    attachInterrupt(digitalPinToInterrupt(DOME_INTERRUPT_PIN), domeMotor.domeISR, LOW);
  #endif

  #if defined(SBL2360_PULSE) || defined(SBL2360_SERIAL) || defined(SBL1360) || defined(GENERIC_RC) || defined(SABERTOOTH)
    driveMotors.begin();
    // Setup an interrupt for stopping the drive motors.
    pinMode(DRIVE_INTERRUPT_PIN, INPUT_PULLUP);
    digitalWrite(DRIVE_INTERRUPT_PIN, HIGH);
    attachInterrupt(digitalPinToInterrupt(DRIVE_INTERRUPT_PIN), driveMotors.driveISR, LOW);
  #endif

  #if defined(MARCDUINO)
  MD_DomeSerial.begin(MARCDUINO_BAUD_RATE);
  while (!MD_DomeSerial);
    #if defined(MD_BODY_MASTER)
    MD_BodySerial.begin(MARCDUINO_BAUD_RATE);
    while (!MD_BodySerial);
    #endif
  marcduino.begin();
  #endif

  #if defined(QWIIC)
  mp3.begin();
  bodyPanels.begin();
  #endif

  // --------------
  // Setup is done.
  // --------------

  output = F("Setup() - Complete.");
  printOutput();
}

void loop() {

  // Refresh the controller input.

  controlBuffer.resetButtons();

  /* ========================
   *      DRIVE/STEERING
   * ======================== */
  #if defined(SBL2360_PULSE) || defined(SBL2360_SERIAL) || defined(SBL1360) || defined(GENERIC_RC) || defined(SABERTOOTH)
  if ( controller.read() )
    driveMotors.interpretController();
  else   
    driveMotors.stop();   // Bad read. Stop the motor if it's running.
  #endif

  /* =======================
   *      DOME ROTATION
   * ======================= */
  #if defined(SYREN10)
  if ( controller.read() ) {
    domeMotor.interpretController();
    if ( controlBuffer.isDomeAutomationRunning() )
      domeMotor.automation();
  } else
    domeMotor.stop();   // Bad read. Stop the motor if it's running.
  #endif

  /* ===========================
   *      OTHER PERIPHERALS
   * =========================== */
  #if defined(MARCDUINO)
  if ( controller.read() ) {
    marcduino.interpretController();
    if ( controlBuffer.isDomeCustomPanelRunning() )
      marcduino.runCustomPanelSequence();
  }
  #endif

  #if defined(QWIIC)
  if ( controller.read() )
    mp3.interpretController();

  if ( controller.read() ) {
    bodyPanels.interpretController();
    if ( controlBuffer.isBodyPanelRunning() )
      bodyPanels.move();
  }
  #endif
}


/* ===================================================
 *           Support variables and functions
 * =================================================== */
String output;

// =======================
//      printOutput()
// =======================
void printOutput(void) {
  if ( output != "" ) {
    if (Serial) Serial.println(output);
    output = "";
  }
}

// ===========================
//      getPgmString()
// ===========================
String getPgmString(const char * inValue)
{
  // This function takes in a pointer to a char array that has been
  // stored in program memory and returns its string content.

  String outValue = "";
  for (byte k = 0; k < strlen_P(inValue); k++) {
    outValue += (char)pgm_read_byte_near(inValue + k);
  }
  return outValue;
}

String getPgmString(const char inValue)
{
  // This function takes in a char array that has been
  // stored in program memory and returns its string content.

  String outValue = "";
  for (byte k = 0; k < sizeof(inValue); k++) {
    outValue += (char)pgm_read_byte_near(inValue + k);
  }
  return outValue;
}
