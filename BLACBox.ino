/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 *                          Last Revised Date: 24 September 2021
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
 *   10. Future   - Support drive motor control using Sabertooth.
 *   11. Future   - Support drive motor control using Roboteq SBL1360 (with mixing).
 *   12. Future   - Support I2C-based FX system to replace Marcduino body master.
 *
 * =======================================================================================
 *
 * This rewrite was developed and tested on the following equipment.
 *   Arduino Mega 2560                (https://www.sparkfun.com/products/11061)
 *   Sainsmart USB host shield        (https://www.amazon.com/gp/product/B006J4G000)
 *   TRENDNet USB bluetooth dongle    (https://www.amazon.com/gp/product/B002AQSTXM)
 *   PS3 Move Navigation controller   (used, from www.gamestop.com)
 *   PS3 controller                   (used, from www.gamestop.com)
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
 *
 * =======================================================================================
 * 
 * The hardest part of getting this code to work with a Roboteq motor driver was understanding
 * the conversions from analog input to Roboteq Pulse inputs. The secret came when reading the
 * Servo.h library. It quietly converts degrees into milliseconds. Once I found this, it was
 * easy to understand the Pulse inputs that were needed. Here are the conversions that take
 * place along the way.
 * 
 * Analog Input   Converted   Converted to  |  Roboteq Pulse
 * from joystick  to degrees  milliseconds  |  Input Ranges
 * -------------  ----------  ------------  |  -------------
 * Min = 0        Min = 0     Min = 544     |  Min = 544
 * Mid = 127      Mid = 90    Mid = 1472    |  Mid = 1472
 * Max = 255      Max = 180   Max = 2400    |  Max = 2400
 * 
 */
#include "Settings.h"
#include "src/toolbox/DebugUtils.h"
#include "src/controller/Controller.h"
#include "src/domeMotor/DomeMotor.h"
#include "src/driveMotor/DriveMotor.h"
#include "src/marcduino/Marcduino.h"

#if defined(PS3_NAVIGATION)
Controller_PS3Nav controller(controllerSettings, controllerTimings);
Controller_PS3Nav* Controller_PS3Nav::anchor = { NULL };
#elif defined(PS3_CONTROLLER)
Controller_PS3 controller(controllerSettings, controllerTimings);
Controller_PS3* Controller_PS3::anchor = { NULL };
#elif defined(PS4_CONTROLLER)
Controller_PS4 controller(controllerSettings, controllerTimings, pair);
Controller_PS4* Controller_PS4::anchor = { NULL };
#elif defined(PS5_CONTROLLER)
Controller_PS5 controller(controllerSettings, controllerTimings, pair);
Controller_PS5* Controller_PS5::anchor = { NULL };
#endif

DomeMotor_Syren10 domeMotor(&controller, domeMotorSettings, domeMotorTimings, syrenSettings);
DriveMotor_Roboteq driveMotor(&controller, driveMotorSettings, driveMotorPins, roboteqSettings);
Marcduino marcduino(&controller, marcduinoSettings);

// Rearrange the Serial configurations to fit your electronics.

HardwareSerial &MD_Body_Serial    = Serial3;
HardwareSerial &DomeMotor_Serial  = Serial2;  // Connects to Syren10.
HardwareSerial &DriveMotor_Serial = Serial2;  // Daisy chain Syren10 to Sabertooth. Roboteq RS232/Serial is not yet supported.
HardwareSerial &MD_Dome_Serial    = Serial1;


/* ============================================================
 *                   M A I N   P R O G R A M
 * ============================================================ */

void setup() {

  // -------------------------
  // Start the serial monitor.
  // -------------------------

  #if defined(DEBUG) || defined(TEST_CONTROLLER)
  Serial.begin(115200);
  #if !defined(__MIPSEL__)
  while (!Serial);
  #endif

  Debug.setDebugLevel(DBG_VERBOSE);
  Debug.print(DBG_INFO, F("============================================="));
  #endif

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("BLACBox"), F("setup()"), F("Starting"));
  #endif

  // ---------------------
  // Start the controller.
  // ---------------------

  controller.begin();

  // -----------------------------
  // Start our peripheral objects.
  // -----------------------------

  driveMotor.begin();
  domeMotor.begin();
  marcduino.begin();

  // --------------
  // Setup is done.
  // --------------

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("BLACBox"), F("setup()"), F("Complete"));
  #endif

  #if defined(DEBUG) || defined(TEST_CONTROLLER)
  Debug.print(DBG_INFO, F("============================================="));
  #endif  
}

void loop() {

  /* ========================
   *      DRIVE/STEERING
   * ======================== */
  if ( controller.isDisconnecting() ) {
    // Stop the drive motors when we lose the controller.
    driveMotor.stop();
    Serial.println(F("Disconnected drive."));
    controller.disconnecting();
  } else if ( controller.read() ) {
    driveMotor.interpretController();
  }

  /* =======================
   *      DOME ROTATION
   * ======================= */
  if ( controller.isDisconnecting() ) {
    // Stop the dome motor when we lose the controller.
    domeMotor.stop();
    Serial.println(F("Disconnected dome."));
    controller.disconnecting();
  } else if ( controller.read() ) {
    domeMotor.interpretController();
    if ( domeMotor.isAutomationRunning() ) {
      domeMotor.runAutomation();
    }
  }

  /* ===========================
   *          MARCDUINO
   * =========================== */
  if ( controller.isDisconnecting() ) {
    // Put Marcduino into Quiet mode when we lose the controller.
    marcduino.quietMode();
    Serial.println(F("Disconnected Marcduino."));
    controller.disconnecting();
  } else if ( controller.read() ) {
    marcduino.interpretController();
    if ( marcduino.isCustomPanelRunning() ) {
      marcduino.runCustomPanelRoutine();
    }
    marcduino.runAutomation();
  }
}
