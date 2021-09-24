/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DriveMotor_Sabertooth.cpp - Library for the Sabertooth drive motor controller
 * Created by Brian Lubkeman, 16 June 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "DriveMotor.h"

const int SABERTOOTH_BAUD_RATE = 9600;     // It is strongly recommended not to change this.

extern HardwareSerial &DriveMotor_Serial;


/* ================================================================================
 *                         Sabertooth Motor Controller Class
 * ================================================================================ */

// =====================
//      Constructor
// =====================
DriveMotor_Sabertooth::DriveMotor_Sabertooth
  ( Controller* pController,
    const int settings[],
    const byte pins[],
    const byte sabertoothSettings[])
  : DriveMotor(pController, settings, pins)
  , m_sabertooth(sabertoothSettings[iAddress], DriveMotor_Serial)
{
  m_sabertoothSettings = sabertoothSettings;
}

// ====================
//      Destructor
// ====================
DriveMotor_Sabertooth::~DriveMotor_Sabertooth(void) {}

// =================
//      begin()
// =================
void DriveMotor_Sabertooth::begin(void)
{
  // -----------------------------------------------------------------------------
  // Call the parent class's begin(). It establishes our dead man switch, if used.
  // -----------------------------------------------------------------------------

  DriveMotor::begin();

  // ----------------------------------------
  // Start communication with the Sabertooth.
  // ----------------------------------------

  leftFootSignal.attach(m_pins[iDrivePin1]);
  leftFootSignal.write(m_servoCenter);

  rightFootSignal.attach(m_pins[iDrivePin2]);
  rightFootSignal.write(m_servoCenter);

  DriveMotor_Serial.begin(SABERTOOTH_BAUD_RATE);
  m_sabertooth.setTimeout(300);
  m_sabertooth.stop();

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("DriveMotor_Sabertooth"), F("begin()"), F("Sabertooth motor controller started"));
  #endif
}

// ================
//      stop()
// ================
void DriveMotor_Sabertooth::stop(void)
{
  // --------------------------------------------------
  // When the motor is already not running, do nothing.
  // --------------------------------------------------

  if ( driveStopped ) {
    return;
  }

  #if defined(DEBUG)
  Debug.print(DBG_WARNING, F("DriveMotor_Sabertooth"), F("stop()"), F("Stop drive motors"));
  #endif

  // ----------------------
  // Send the stop command.
  // ----------------------

  m_sabertooth.stop();

  // ------------------------
  // Update the drive status.
  // ------------------------

  driveStopped = true;
}

// ===================
//      m_drive()
// ===================
void DriveMotor_Sabertooth::m_drive(void)
{
  // ------------------------------
  // Account for the speed profile.
  // ------------------------------

  byte driveSpeed = 0;
  int stickSpeed = 0;

  if (speedProfile == RUN) {
    driveSpeed = m_sabertoothSettings[iOverdriveSpeed];
  } else if (speedProfile == WALK) {
    driveSpeed = m_sabertoothSettings[iDriveSpeed];
  }
  stickSpeed = (map(m_throttle, 0, 255, -driveSpeed, driveSpeed));

  // --------------------------
  // Factor in ramping up/down.
  // --------------------------

  if ( abs(m_throttle - m_driveStick->center) <  m_driveStick->deadZone ) {
    m_fastRampDown(&driveSpeed, &stickSpeed);
  } else {
    driveStopped = false;
    if ( driveSpeed < stickSpeed ) {
      m_rampUp(&driveSpeed, &stickSpeed);
    } else if ( driveSpeed > stickSpeed ) {
      m_rampDown(&driveSpeed, &stickSpeed);
    }
  }

  // ---------------------
  // Factor in spin speed.
  // ---------------------

  int turnNumber = 0;
  byte turnSpeed = m_sabertoothSettings[iTurnSpeed];

  if ( abs(driveSpeed) > 50)
    turnNumber = (map(m_driveStick->steering(), 54, 200, -(turnSpeed/4), (turnSpeed/4)));
  else if (turnNumber > 200)
    turnNumber = (map(m_driveStick->steering(), 201, 255, (turnSpeed/3), turnSpeed));
  else if (turnNumber <= 200 && turnNumber >= 54)
    turnNumber = (map(m_driveStick->steering(), 54, 200, -(turnSpeed/3), (turnSpeed/3)));
  else if (turnNumber < 54)
    turnNumber = (map(m_driveStick->steering(), 0, 53, -turnSpeed, -(turnSpeed/3)));
      
  if ( abs(turnNumber) > 5 ) {
    driveStopped = false;   
  }

  // 

  unsigned long currentTime = millis();

  if ( (currentTime - m_previousTime) > m_settings[iDriveLatency] ) {

    if ( driveSpeed != 0 || abs(turnNumber) > 5 ) {

      char buff[21];
      sprintf(buff, "Drive/Turn: %i/%i", driveSpeed, turnNumber);
      Debug.print(DBG_VERBOSE, F("DriveMotor_Sabertooth"), F("m_drive()"), buff);

      m_sabertooth.turn(turnNumber * m_sabertoothSettings[iInvertTurn]);
      m_sabertooth.drive(driveSpeed);

    } else {

      if ( ! driveStopped ) {
        stop();
      }

    }
  }

  m_previousTime = currentTime;
  return true;
}

void DriveMotor_Sabertooth::m_fastRampDown(byte driveSpeed, int stickSpeed)
{
  char buff[22];

  if ( abs(driveSpeed) > 50 ) {
    if ( driveSpeed > 0 ) {
      driveSpeed -= 3;
    } else {
      driveSpeed += 3;
    }
    sprintf(buff, "Drive/Stick: %i/%i", driveSpeed, stickSpeed);
    Debug.print(DBG_VERBOSE, F("DriveMotor_Sabertooth"), F("m_fastRampDown"), buff);
  } else if ( abs(driveSpeed) > 20 ) {
    if ( driveSpeed > 0 ) {
      driveSpeed -= 2;
    } else {
      driveSpeed += 2;
    }
    sprintf(buff, "Drive/Stick: %i/%i", driveSpeed, stickSpeed);
    Debug.print(DBG_VERBOSE, F("DriveMotor_Sabertooth"), F("m_fastRampDown"), buff);
  } else {
    driveSpeed = 0;
  }
}

void DriveMotor_Sabertooth::m_rampDown(byte driveSpeed, int stickSpeed)
{
  if ( (driveSpeed - stickSpeed) > (m_sabertoothSettings[iRamping]+1) ) {
    driveSpeed -= m_sabertoothSettings[iRamping];
    char buff[22];
    sprintf(buff, "Drive/Stick: %i/%i", driveSpeed, stickSpeed);
    Debug.print(DBG_VERBOSE, F("DriveMotor_Sabertooth"), F("m_rampDown"), buff);
  } else {
    driveSpeed = stickSpeed;
  }
}

void DriveMotor_Sabertooth::m_rampUp(byte driveSpeed, int stickSpeed)
{
  if ( (stickSpeed - driveSpeed) > (m_sabertoothSettings[iRamping]+1) ) {
    driveSpeed += m_sabertoothSettings[iRamping];
    char buff[22];
    sprintf(buff, "Drive/Stick: %i/%i", driveSpeed, stickSpeed);
    Debug.print(DBG_VERBOSE, F("DriveMotor_Sabertooth"), F("m_rampUp"), buff);
  } else {
    driveSpeed = stickSpeed;
  }
}
