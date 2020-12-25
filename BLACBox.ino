/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 *                          Last Revised Date: 17 December 2020
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
 *   4.  Achieved - Support PS3 Move Navigation, PS3, and PS4 controllers.
 *   5.  Achieved - Support the Marcduino system.
 *   6.  Achieved - Support sound sent to the optional Marcduino body master.
 *   7.  Achieved - Support drive motor control using Roboteq SBL2360 (without mixing).
 *   8.  Testing  - Support dome motor control using a Syren 10.
 *   9   Testing  - Support radio communication with dome electroncis instead of a slip ring.
 *   12. In progress - Support I2C-based peripherals to replace Marcduino body master.
 *   10. Future   - Support drive motor control using Roboteq SBL1360 (with mixing).
 *   11. Future   - Support drive motor control using Sabertooth.
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
 *   Roboteq SBL2360 motor controller (https://www.roboteq.com/products/products-brushless-dc-motor-controllers/sbl-family/sbl2360t-detail)
 *   Adafruit Feather M0 RFM69HCW     (https://www.adafruit.com/product/3176)
 * Future equipment to be tested.
 *   Roboteq SBL1360 motor controller (https://www.roboteq.com/products/products-brushless-dc-motor-controllers/sbl-family/sbl1360-277-detail)
 *   Sabertooth motor controller      (https://www.dimensionengineering.com/products/sabertooth2x25)
 *   Sparkfun Qwiic MP3 Trigger       (https://www.sparkfun.com/products/15165)
 *   Adafruit I2C PWM driver          (https://www.adafruit.com/product/815)
 */

/* ===============================================
 *           Includes and instantiations
 * =============================================== */
#include "Settings.h"

// ------------------------
// This is our control buffer. This is what allows us to
// decouple the controller code from the peripheral code.
// ------------------------
#include "Buffer.h"
Buffer controlBuffer;

// ------------------------
// This is our controller. The PS3 Move Navigation, PS3, and PS4 controllers are currently supported.
// ------------------------
#include "Controllers.h"

#if defined(PS3_NAVIGATION)
Controller_PS3Nav controller(&controlBuffer);
Controller_PS3Nav * Controller_PS3Nav::anchor = { NULL }; // This pointer helps us with the attachOnInit functions.

#elif defined(PS3_CONTROLLER)
Controller_PS3 controller(&controlBuffer);
Controller_PS3 * Controller_PS3::anchor = { NULL }; // This pointer helps us with the attachOnInit functions.

#elif defined(PS4_CONTROLLER)
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
#include "Dome_Motor.h"

#if defined(SYREN10)
Dome_Motor domeMotor(&controlBuffer);
#endif

// ------------------------
// This is our drive motor controller. The Roboteq SBL2360 controller
// using Pulse Input command priority is currently supported.
// ------------------------
#include "Drive_Motors.h"

#if defined(SBL2360_PULSE) || defined (SBL2360_SERIAL) || defined(SBL1360)
Roboteq_DriveMotors driveMotors(&controlBuffer);

#elif defined(SABERTOOTH)
Sabertooth_DriveMotors driveMotors(&controlBuffer);

#endif

// ------------------------
// This is our sound, logic display, holoprojectors, dome panels, and body panels.
// The Marcduino system is supported.  I hope to support a future alternative
// that uses I2C.
// ------------------------
#if defined(MARCDUINO)
#include "Marcduino.h"
Marcduino marcduino(&controlBuffer);
#endif

// -------------------------------------------------------------------------------
// This is experimental. Trying to add I2C support via Sparkfun's Qwiic system.
// The goal is to replace a Marcduino body master with an I2C alternative to free
// up one of the Serial pins on the Arduino Mega. That would make supporting the
// Roboteq motor controllers in RS232 (Serial) command priority a bit easier.
// -------------------------------------------------------------------------------
#if defined(QWIIC)
#include <Wire.h>
#include "Qwiic_MP3_Trigger.h"
Qwiic_MP3 mp3(&controlBuffer);

#include "Qwiic_Servo.h"
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
  #endif

  #if defined(SBL2360_PULSE) || defined(SBL2360_SERIAL) || defined(SBL1360) || defined(SABERTOOTH)
    driveMotors.begin();
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
  if ( controller.read() ) {
    driveMotors.interpretController();
  } else {
    driveMotors.stop();   // Bad read. Stop the motor if it's running.
  }
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
