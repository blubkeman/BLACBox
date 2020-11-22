/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_DriveMotor_All.cpp - Library for parent class of drive motor controllers
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Peripheral_DriveMotor_All.h"


/* ===========================================================================
 *                          DriveMotor class functions
 * =========================================================================== */

// =====================
//      Constructor
// =====================
DriveMotor::DriveMotor(Buffer * pBuffer)
{
  _buffer = pBuffer;
  _inOverdrive = false;
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
  pinMode(DEADMAN_PIN,OUTPUT);

  #if defined(DEADMAN)

    #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
    output = _className+F("::begin()");
    output += F(" - DEBUG_DRIVE man switch enabled.");
    #endif
  
    digitalWrite(DEADMAN_PIN,LOW);

  #else

    #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
    output = _className+F("::begin()");
    output += F(" - DEBUG_DRIVE man switch disabled.");
    #endif
  
    digitalWrite(DEADMAN_PIN,HIGH);

  #endif
}

// ================
//      stop()
// ================
void DriveMotor::stop(void)
{
  // Throw the interrupt that will stop the drive motors.

  if ( ! _buffer->isDriveStopped() )
    digitalWrite(DRIVE_INTERRUPT_PIN, LOW);
}

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

    // ---------------------------------------------------------
    // We do not have a controller for which I've prepared code.
    // ---------------------------------------------------------

    return false;

  #endif
}

// ================================
//      _toggleDriveEnabled()
// ================================
void DriveMotor::_toggleDriveEnabled(void)
{
  #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
  output = _className+F("_checkDriveEnabled()");
  #endif

  if ( _buffer->getButton(PS) ) {

    if ( _buffer->getButton(START) && ! _buffer->isDriveEnabled() ) {

      #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
      output += F(" - Enable drive.");
      printOutput();
      #endif

      _buffer->setDriveEnabled(true);
      _buffer->_setRumbledRequested(true);

    } else if ( _buffer->getButton(SELECT) && _buffer->isDriveEnabled() ) {

      #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
      output += F(" - Disable drive.");
      printOutput();
      #endif

      _buffer->setDriveEnabled(false);
      _buffer->_setRumbledRequested(true);
    }
  }
}

// =============================
//      _toggleDriveSpeed()
// =============================
void DriveMotor::_toggleDriveSpeed(void)
{
  if ( _buffer->getButton(L1) && _buffer->getButton(L3) ) {
    _inOverdrive = ( ! _inOverdrive );
    _buffer->_setRumbledRequested(true);

    #if defined(DEBUG_DRIVE) || defined(DEBUG_ALL)
    output = _className+F("");
    output += F(" - _toggleDriveSpeed");
    if ( _inOverdrive )
      output += F(": Turbo speed");
    else
      output += F(": Normal speed");
    printOutput();
    #endif
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
  // 3. The drive stick's position is outside of its DEBUG_DRIVE zone range.
  // 4. When a deadman switch is in use, it must be pressed.

  if ( ! _buffer->isControllerConnected() )
    return false;

  if ( ! _buffer->isDriveEnabled() )
    return false;

  if ( ! _buffer->isStickOffCenter(LeftHatX) || ! _buffer->isStickOffCenter(LeftHatY) )
    return false;

  #if defined(DEADMAN)
  if ( ! _isDeadmanPressed() )
    return false;
  #endif
    
  return true;
}
