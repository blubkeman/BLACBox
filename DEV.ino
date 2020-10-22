/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 *                          Last Revised Date: 22 October 2020
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
 *   4.  Achieved - Support the Marcduino system.
 *   5.  Achieved - Support sound sent to the optional Marcduino body master.
 *   6.  In progress - Support dome motor control using a Syren 10.
 *   7.  In progress - Support foot motor control using R/C without mixing (Roboteq 2360).
 *   8.  Future - Support foot motor control using R/C with mixing (Roboteq SBL1360).
 *   9.  Future - Support foot motor control using Sabertooth.
 *   10. Future - Support I2C-based peripherals.
 *   11. Future - Support additional controllers available through the USB Host Shield code.
 *
 * =======================================================================================
 * 
 * This rewrite was developed and tested on the following equipment.
 *   Arduino Mega 2560                (https://www.sparkfun.com/products/11061)
 *   Sainsmart USB host shield        (https://www.amazon.com/gp/product/B006J4G000)
 *   TRENDNet USB bluetooth dongle    (https://www.amazon.com/gp/product/B002AQSTXM)
 *   PS3 Move Navigation controller   (used, from www.gamestop.com)
 *   Compact Marcduino v1.5           (https://astromech.net/forums/showthread.php?30724-Compact-Marcduino-v1-5-BC-Approved-Continuous-Various-(Jan-2017)-Open)
 *   Sparkfun MP3 Trigger             (https://www.sparkfun.com/products/13720)
 *   Syren 10 motor controller        (https://www.dimensionengineering.com/products/syren10)
 *   Roboteq SBL2360 motor controller (https://www.roboteq.com/index.php/component/virtuemart/461/sbl2360-277-393-460-detail?Itemid=971)
 * Future equipment to be tested.
 *   Sabertooth motor controller      (https://www.dimensionengineering.com/products/sabertooth2x25)
 *   Sparkfun Qwiic MP3 Trigger       (https://www.sparkfun.com/products/15165)
 *   Adafruit I2C PWM driver          (https://www.adafruit.com/product/815)
 */

/* ===============================================
 *           Includes and instantiations
 * =============================================== */
#include "Globals.h"

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
// controller. By using the USB Host Shield Library 2.0, we should be able to implement
// any controller that library supports including the PS3, PS4, Xbox, and Wii.
// ------------------------
#ifdef PS3_NAVIGATION
#include "Controller_PS3Nav.h"
Controller_PS3Nav controller(&controlBuffer);
// This pointer helps us with the attachOnInit functions.
Controller_PS3Nav * Controller_PS3Nav::anchor = { NULL };
#endif

// ------------------------
// These are our Serial pins.
// ------------------------
#ifdef DOME or DRIVE
// Currently, the code assumes use of either a Syren 10 or a Sabertooth controller.
HardwareSerial &MotorSerial = Serial2;
#endif
#ifdef MARCDUINO
HardwareSerial &DomeSerial = Serial1;
#ifdef MD_BODY_MASTER
HardwareSerial &BodySerial = Serial3;
#endif
#endif

// ------------------------
// This is our dome motor.  It currently supports the Syren 10 motor controller.
// ------------------------
#ifdef DOME
#include "Peripheral_Dome_Motor.h"
Dome_Motor domeMotor(&controlBuffer);
Dome_Motor * Dome_Motor::anchor = { NULL };
#endif

// ------------------------
// This is our foot motor controller.  It supports the Roboteq SBL2360
// or other R/C controller that mixes by itself.
// Code for the Sabertooth and Roboteq SBL1360 or other R/C controller
// that requires remixing are planned for future support.
// ------------------------
#ifdef DRIVE
#include "Peripheral_RC_FootMotor.h"
RC_FootMotor footMotors(&controlBuffer);
RC_FootMotor * RC_FootMotor::anchor = { NULL };
#endif

// ------------------------
// This is our sound, logic display, holoprojectors, dome panels, and body panels.
// The Marcduino system is supported.  I hope to support a future alternative
// that uses I2C.
//   This is very much a work in progress.
// ------------------------
#ifdef MARCDUINO
#include "Peripheral_Marcduino.h"
Marcduino marcduino(&controlBuffer);
#endif


/* ================================================
 *           Declarations and definitions
 * ================================================ */
String output;


/* ================================
 *           Main Program
 * ================================ */
void setup() {
  // Start the serial monitor.

  Serial.begin(115200);
  #if !defined(__MIPSEL__)
  while (!Serial);
  #endif

  // Start our controller objects.

  controlBuffer.begin();
  controller.begin();

  #ifdef DOME or DRIVE
    MotorSerial.begin(MOTOR_BAUD_RATE); // This is for the Syren 10 and/or Sabertooth
    while (!MotorSerial);
    #ifdef DOME
      domeMotor.begin();
      // Setup an interrupt for stopping the dome motor.
      pinMode(domeInterruptPin, INPUT_PULLUP);
      digitalWrite(domeInterruptPin, HIGH);
      attachInterrupt(digitalPinToInterrupt(domeInterruptPin), domeMotor.domeISR, LOW);
    #endif
    #ifdef DRIVE
      footMotors.begin();
      // Setup an interrupt for stopping the foot motors.
      pinMode(footInterruptPin, INPUT_PULLUP);
      digitalWrite(footInterruptPin, HIGH);
      attachInterrupt(digitalPinToInterrupt(footInterruptPin), footMotors.footISR, LOW);
    #endif
  #endif

  // Start serial communications with the Marcduino dome master and optional body master.

  #ifdef MARCDUINO
  DomeSerial.begin(MARCDUINO_BAUD_RATE);
  #ifdef MD_BODY_MASTER
  BodySerial.begin(MARCDUINO_BAUD_RATE);
  #endif
  marcduino.begin();
  #endif

  // Reserve 200 bytes for the output string

  output.reserve(200);
}

void loop() {
  /* ========================
   *      DRIVE/STEERING
   * ======================== */
  #ifdef DRIVE
  if ( (controller.read()) ) {
    footMotors.interpretController();
  } else {
    // Bad read. Stop the motor.
    if ( ! controlBuffer.isFootStopped() )
      controlBuffer.stopFootMotor();
  }
  #endif

  /* =======================
   *      DOME ROTATION
   * ======================= */
  #ifdef DOME
  if ( (controller.read()) ) {
    domeMotor.interpretController();
    if (controlBuffer.isDomeAutomationRunning())
      domeMotor.automation();
  } else {
    // Bad read. Stop the motor.
    if ( ! controlBuffer.isDomeStopped() )
      controlBuffer.stopDomeMotor();
  }
  #endif

  /* ===========================
   *      OTHER PERIPHERALS
   * =========================== */
  #ifdef MARCDUINO
  if ( (controller.read()) ) {
    marcduino.interpretController();
    if (controlBuffer.isDomeCustomPanelRunning())
      marcduino.runCustomPanelSequence();
  }
  #endif
}
