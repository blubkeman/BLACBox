#include "Settings.h"
#include "Controller.h"
#include "DriveMotor.h"
#include "DomeMotor.h"
#include "Marcduino.h"

#if defined(PS4_CONTROLLER)
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
#endif

#if defined(SYREN10)
Syren10_DomeMotor domeMotor(&controller);
HardwareSerial &DomeMotorSerial = Serial2;
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

bool driveEnabled;
bool driveStopped;
byte speedProfile;
bool domeStopped;

#if defined(DEBUG) || defined(TEST_CONTROLLER)
String output;
#endif

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

//  controller.read();

  /* ========================
   *      DRIVE/STEERING
   * ======================== */
  if ( controller.read() ) {
    driveMotor.interpretController();
  }
    else {
    // Bad read.
    // Stop the motor when we lose the controller and the motor is still running.
    if ( controller.isControllerLost() ) {
      driveMotor.stop();
      controller.onDisconnect();
    }
  }

  /* =======================
   *      DOME ROTATION
   * ======================= */
  if ( controller.read() ) {
    domeMotor.interpretController();
    if ( domeMotor.isAutomationRunning() )
      domeMotor.automation();
  } else {
    // Bad read.
    // Stop the motor when we lose the controller and the motor is still running.
    if ( controller.isControllerLost() ) {
      domeMotor.stop();
      controller.onDisconnect();
    }
  }

  /* ===========================
   *      OTHER PERIPHERALS
   * =========================== */
  if ( controller.read() ) {
    marcduino.interpretController();
    if ( marcduino.isCustomPanelRunning() ) {
      marcduino.runCustomPanelSequence();
    }
  } else {
    // Bad read.
    // Put Marcduino into Quiet mode when we lose the controller.
    if ( controller.isControllerLost() ) {
      marcduino.quietMode();
      controller.onDisconnect();
    }
  }
}


/* ===================================================
 *           Support variables and functions
 * =================================================== */

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
