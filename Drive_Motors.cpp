/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Drive_Motors.cpp - Library for supported drive motor controllers
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Drive_Motors.h"


/* ================================================================================
 *                         Parent Controller Functions
 * ================================================================================ */

// =====================
//      Constructor
// =====================
DriveMotor::DriveMotor(Buffer * pBuffer)
{
  _buffer = pBuffer;

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
  _className = F("DriveMotor::");
  #endif
}


// ====================
//      Destructor
// ====================
DriveMotor::~DriveMotor() {}


// =================
//      begin()
// =================
void DriveMotor::begin()
{
  // --------------------------------
  // Set up a deadman switch, or not.
  // --------------------------------

  pinMode(DEADMAN_PIN,OUTPUT);

  #if defined(DEADMAN)

    #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
    output = _className+F("::begin()");
    output += F(" - Dead man switch enabled.");
    printOutput();
    #endif
  
    digitalWrite(DEADMAN_PIN,LOW);

  #else

    #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
    output = _className+F("::begin()");
    output += F(" - Dead man switch disabled.");
    printOutput();
    #endif
  
    digitalWrite(DEADMAN_PIN,HIGH);

  #endif
}


#if defined(SBL1360)
// ===================
//      _mixBHD()
// ===================
void DriveMotor::_mixBHD(byte stickX, byte stickY) {
/* =============================================================
 *  This is my interpretation of BigHappyDude's mixing function for differential (tank) style driving using two motors.  
 *  We will take the joystick's X and Y positions, mix these into a diamond matrix, and convert to a servo value range 
 *   (0-180) for the left and right foot motors.
 *  The maximum drive speed BHD used is excluded in this version as that is handled through a script in the Roboteq.
 *  If you wish to understand how this works, please see the comments in the mixBHD function implemented into the
 *   SHADOW_MD_Q85 code.
 * ============================================================= */

  if ( stickX == JOYSTICK_CENTER && stickY == JOYSTICK_CENTER ) {

    _leftFoot=SERVO_CENTER;
    _rightFoot=SERVO_CENTER;
    return;

  }

  int xInt = 0;
  int yInt = 0;

  if (stickY < JOYSTICK_CENTER) {
    yInt = map(stickY, JOYSTICK_MIN, (JOYSTICK_CENTER - JOYSTICK_DEAD_ZONE), 100, 1);
  } else {
    yInt = map(stickY, (JOYSTICK_CENTER + JOYSTICK_DEAD_ZONE), JOYSTICK_MAX, -1, -100);
  }

  if (stickX < JOYSTICK_CENTER) {
    xInt = map(stickX, JOYSTICK_MIN, (JOYSTICK_CENTER - JOYSTICK_DEAD_ZONE), -100, -1);
  } else {
    xInt = map(stickX, (JOYSTICK_CENTER + JOYSTICK_DEAD_ZONE), JOYSTICK_MAX, 1, 100);
  }

  float xFloat = xInt;
  float yFloat = yInt;

  if ( yInt > (xInt + 100) ) {
    xFloat = -100 / (1 - (yFloat / xFloat));
    yFloat = xFloat + 100;
  } else if ( yInt > (100 - xInt) ) {
    xFloat = -100 / (-1 - (yFloat / xFloat));
    yFloat = -xFloat + 100;
  } else if (yInt < (-xInt - 100)) {
    xFloat = 100 / (-1 - (yFloat / xFloat));
    yFloat = -xFloat - 100;
  } else if (yInt < (xInt - 100)) {
    xFloat = 100 / (1 - (yFloat / xFloat));
    yFloat = xFloat - 100;
  }

  float leftSpeed = ((xFloat + yFloat - 100) / 2) + 100;
  leftSpeed = (leftSpeed - 50) * 2;

  float rightSpeed = ((yFloat - xFloat - 100) / 2) + 100;
  rightSpeed = (rightSpeed - 50) * 2;

  _leftFoot=map(leftSpeed, -100, 100, SERVO_MAX, SERVO_MIN);
  _rightFoot=map(rightSpeed, -100, 100, SERVO_MAX, SERVO_MIN);
}
#endif


#if defined(SBL2360_SERIAL)
// ======================
//      _serialWrite()
// ======================
void DriveMotor::_serialWrite(String inStr)
{
  uint8_t something;
  if (DriveSerial.available()) {
    for (int i=0; i<inStr.length(); i++) {
      DriveSerial.write(inStr[i]);
    }
  }
}


// ======================
//      _serialRead()
// ======================
void DriveMotor::_serialRead(void)
{
  uint8_t something;
  if (DriveSerial.available()) {
    something = DriveSerial.read();
  }
}
#endif


// ============================
//      _isDeadmanPressed()
// ============================
bool DriveMotor::_isDeadmanPressed(void)
{
  // ---------------------------------------------
  // Check the deadman switch.  The deadman switch
  // varies a little for different controllers.
  // ---------------------------------------------

  #if defined(PS3_NAVIGATION)

    if ( _buffer->isFullControllerConnected() ) {

      // -------------------------------------------------------------
      // We have dual Nav controllers. L2 or R2 is the deadman switch.
      // -------------------------------------------------------------

      if ( _buffer->getButton(L2) || _buffer->getButton(R2) ) {
        digitalWrite(DEADMAN_PIN, HIGH);
        return true;
      } else {
        digitalWrite(DEADMAN_PIN, LOW);
        return false;
      }

    } else {

      // ----------------------------------------------------------
      // We have a single Nav controller. L1 is the deadman switch.
      // ----------------------------------------------------------

      if ( _buffer->getButton(L1) ) {
        digitalWrite(DEADMAN_PIN, HIGH);
        return true;
      } else {
        digitalWrite(DEADMAN_PIN, LOW);
        return false;
      }

    }

  #elif defined(PS3_CONTROLLER) || defined(PS4_CONTROLLER)

    // -----------------------------------------------------------
    // For PS3 or PS4 controllers, L2 or R2 is the deadman switch.
    // -----------------------------------------------------------

    if ( _buffer->getButton(L2) || _buffer->getButton(R2) ) {
      digitalWrite(DEADMAN_PIN, HIGH);
      return true;
    } else {
      digitalWrite(DEADMAN_PIN, LOW);
      return false;
    }

  #else

    // -------------------------------------------------------
    // We do not have a controller for which code is prepared.
    // -------------------------------------------------------

    return false;

  #endif
}


// ===============================
//      _toggleDriveEnabled()
// ===============================
void DriveMotor::_toggleDriveEnabled(void)
{
  if ( _buffer->getButton(PS) ) {

    if ( _buffer->getButton(START) && ! _buffer->isDriveEnabled() ) {
      _buffer->setDriveEnabled(true);
    } else if ( _buffer->getButton(SELECT) && _buffer->isDriveEnabled() ) {
      _buffer->setDriveEnabled(false);
    }

    _buffer->requestLedUpdate(true);
  }
}


// ===============================
//      _updateSpeedProfile()
// ===============================
void DriveMotor::_updateSpeedProfile(void)
{
  // ---------------------------------------------------
  // Update to the next speed profile in accordance with
  // the motor controller and remote control in use.
  // Speed profiles cycle through the following:
  //   1 = Walk
  //   2 = Jog
  //   3 = Run
  //   4 = Sprint
  // ---------------------------------------------------

  if ( _buffer->getButton(L1) && _buffer->getButton(L3) ) {

    uint8_t profile = _buffer->getSpeedProfile();

    #if (( defined(SBL2360_PULSE) || defined(SBL2360_SERIAL)|| defined(SBL1360) ) && ( defined(PS3_CONTROLLER) || defined(PS4_CONTROLLER) ))

    // ----------------------------------------------------------------------------
    // The full cycle is only available through a Roboteq controller using a script
    // that defines the max speed, acceleration, and deceleration of each profile.
    // Only the PS3 or PS4 controller offers feedback to tell the user when the
    // profile has changed.
    // ----------------------------------------------------------------------------

    if ( profile == Sprint ) {
      profile = 0;  // Reset the profile cycle.
    }
    profile++;
    
    #else

    // ---------------------------------------------------
    // For PS3 Navigations or motor controllers other than
    // a Roboteq, only two profiles are available.
    // ---------------------------------------------------

    if ( _buffer->getSpeedProfile() == Run ) {
      profile = Walk;
    } else {
      profile = Run;
    }

    #endif

    // ----------------------------------------------
    // Update the profile and provider user feedback.
    // ----------------------------------------------

    _buffer->setSpeedProfile(profile);
    _buffer->requestLedUpdate(true);
  }
}


// ==========================
//      _isUserDriving()
// ==========================
bool DriveMotor::_isUserDriving(void)
{
  // A user is driving only when:
  // 1. A controller is connected.
  // 2. The drive stick is enabled. (PS+Cirle/Options/Start toggles this.)
  // 4. When a deadman switch is in use, it must be pressed.

  if ( ! _buffer->isControllerConnected() )
    return false;

  if ( ! _buffer->isDriveEnabled() )
    return false;

  #if defined(DEADMAN)
  if ( ! _isDeadmanPressed() )
    return false;
  #endif
    
  return true;
}
