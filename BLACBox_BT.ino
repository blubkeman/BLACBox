/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 *                          Last Revised Date: 23 March 2021
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
 *   2.  Achieved - Encapsulate code into libraries.
 *   3.  Achieved - Divide controllers and peripherals into classes.
 *   4.  Achieved - Support PS3 Move Navigation, PS3, PS4, and PS5 controllers.
 *   5.  Achieved - Support the Marcduino system.
 *   6.  Achieved - Support sound sent to either a Marcduino dome or the optional body master.
 *   7.  Achieved - Support drive motor control using Roboteq SBL2360 (without mixing).
 *   8.  Achieved - Support dome motor control using a Syren 10.
 *   9   Testing  - Support radio communication with dome electroncis instead of a slip ring.
 *   10. Future   - Support drive motor control using Roboteq SBL1360 (with mixing).
 *   11. Future   - Support drive motor control using Sabertooth.
 *   12. Future   - Support I2C-based peripherals to replace Marcduino body master.
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
#include "Settings.h"
#include "Controller.h"
#include "DriveMotor.h"
#include "DomeMotor.h"
#include "Marcduino.h"

#if defined(PS5_CONTROLLER)
Controller_PS5 controller;
Controller_PS5 * Controller_PS5::anchor = { NULL };
#elif defined(PS4_CONTROLLER)
Controller_PS4 controller;
Controller_PS4 * Controller_PS4::anchor = { NULL };
#elif defined(PS3_CONTROLLER)
Controller_PS3 controller;
Controller_PS3 * Controller_PS3::anchor = { NULL };
#elif defined(PS3_NAVIGATION)
Controller_PS3Nav controller;
Controller_PS3Nav * Controller_PS3Nav::anchor = { NULL };
#endif

#if defined(SBL2360) || defined(SBL1360)
Roboteq_DriveMotor driveMotor(&controller);
bool driveEnabled;
bool driveStopped;
byte speedProfile;
#endif

#if defined(SYREN10)
Syren10_DomeMotor domeMotor(&controller);
HardwareSerial &DomeMotorSerial = Serial2;
bool domeStopped;
#endif

#if defined(MARCDUINO)
Marcduino marcduino(&controller);
  #if defined(MD_BODY_MASTER)
  HardwareSerial &MD_Body_Serial = Serial3;
  #endif
#endif

#if defined(RS232)
HardwareSerial &DriveSerial = Serial1;
  #if defined(MARCDUINO)
  HardwareSerial &MD_Dome_Serial = Serial;
  #endif
#elif defined(MARCDUINO)
HardwareSerial &MD_Dome_Serial = Serial1;
#endif
#if defined(SYREN10)
#endif

#if defined(DEBUG) || defined(TEST_CONTROLLER)
String output;
#endif

/* ============================================================
 *                   M A I N   P R O G R A M
 * ============================================================ */

void setup() {

  // -------------------------
  // Start the serial monitor.
  // -------------------------

  #if defined(DEBUG) || defined(TEST_CONTROLLER)
  output.reserve(200);
  Serial.begin(115200);
  #if !defined(__MIPSEL__)
  while (!Serial);
  #endif
  #endif

  #ifdef DEBUG
  output = F("\n=============================================");
  output += F("\n");
  output += F("setup()");
  output += F(" - Starting");
  printOutput();
  #endif

  // ---------------------
  // Start the controller.
  // ---------------------

  controller.begin();

  // -----------------------------
  // Start our peripheral objects.
  // -----------------------------

  #if defined(SBL2360) || defined(SBL1360) || defined(SABERTOOTH)
  driveMotor.begin();
  #endif

  #if defined(SYREN10)
  domeMotor.begin();
  #endif

  #if defined(MARCDUINO)
  marcduino.begin();
  #endif

  // --------------
  // Setup is done.
  // --------------

  #ifdef DEBUG
  output = F("setup()");
  output += F(" - Complete");
  output += F("\n=============================================");
  printOutput();
  #endif
}

void loop() {

  /* ========================
   *      DRIVE/STEERING
   * ======================== */
  if ( controller.duringDisconnect() ) {
    // Stop the drive motors when we lose the controller.
    Serial.println("Disconnecting drive.");
    driveMotor.stop();
    controller.continueDisconnecting();
  } else if ( controller.read() ) {
    driveMotor.interpretController();
  }

  /* =======================
   *      DOME ROTATION
   * ======================= */
  if ( controller.duringDisconnect() ) {
    // Stop the dome motor when we lose the controller.
    Serial.println("Disconnecting dome.");
    domeMotor.stop();
    controller.continueDisconnecting();
  } else if ( controller.read() ) {
    domeMotor.interpretController();
    if ( domeMotor.isAutomationRunning() ) {
      domeMotor.runHoloAutomation();
    }
  }

  /* ===========================
   *          MARCDUINO
   * =========================== */
  if ( controller.duringDisconnect() ) {
    // Put Marcduino into Quiet mode when we lose the controller.
    Serial.println("Disconnecting Marcduino.");
    marcduino.quietMode();
    controller.continueDisconnecting();
  } else if ( controller.read() ) {
    marcduino.interpretController();
    if ( marcduino.isCustomPanelRunning() ) {
      marcduino.runCustomPanelRoutine();
    }
    marcduino.runHoloAutomation();
  }
}


/* ===================================================
 *           Support variables and functions
 * =================================================== */

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
/*
String getPgmString(const char inValue)
{
  // This function takes in a char array that has been
  // stored in program memory and returns its string content.

  Serial.print("inValue: ");
  Serial.println(inValue);
  String outValue = "";
  for (byte k = 0; k < sizeof(inValue); k++) {
    outValue += (char)pgm_read_byte_near(inValue + k);
  }
  return outValue;
}
*/

#if defined(DEBUG) || defined(TEST_CONTROLLER)
// =======================
//      printOutput()
// =======================
void printOutput(void) {
  if ( output != "" ) {
    if (Serial) Serial.println(output);
    output = "";
  }
}
#endif
