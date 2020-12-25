/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Dome_Motor.cpp - Library for a Syren 10 dome motor controller
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Dome_Motor.h"

#if defined(SYREN10)

// =====================
//      Constructor
// =====================
Dome_Motor::Dome_Motor(Buffer* pBuffer) : _Syren(SYREN_ADDR, DomeSerial)
{
  _buffer = pBuffer;

  _rotationStatus = STOPPED;
  _previousTime   = 0;

  _turnDirection  = 1;  // 1 = positive turn, -1 negative turn
  _targetPosition = 0;  // (0 - 359) - degrees in a circle, 0 = home
  _stopTurnTime   = 0;
  _startTurnTime  = 0;

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  _className = F("Dome_Motor::");
  #endif
}


// =====================
//        begin()
// =====================
void Dome_Motor::begin()
{
  // --------------------------
  // Start Syren 10 dome motor.
  // --------------------------

  _Syren.autobaud();
  _Syren.setTimeout(300);

  // ---------------------------------------------------
  // Prepare the random number seed for dome automation.
  // ---------------------------------------------------

  randomSeed(analogRead(0));

  // ==================================
  // Validate dome automation settings.
  // ==================================

  _automationSettingsInvalid = false;
  if ( TIME_360_DOME_TURN < TIME_360_DOME_TURN_MIN ||
       TIME_360_DOME_TURN > TIME_360_DOME_TURN_MAX ||
       DOME_AUTO_SPEED < DOME_AUTO_SPEED_MIN ||
       DOME_AUTO_SPEED > DOME_AUTO_SPEED_MAX ) {

    _automationSettingsInvalid = true;

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output = _className+F("begin()");
    output += F(" - Invalid settings.");
    output += F("\r\n  Turn time: ");   output += TIME_360_DOME_TURN;
    output += F("\t Min: ");            output += TIME_360_DOME_TURN_MIN;
    output += F("\t Max: ");            output += TIME_360_DOME_TURN_MAX;
    output += F("\r\n  Dome speed: ");  output += DOME_AUTO_SPEED;
    output += F("\t Min: ");            output += DOME_AUTO_SPEED_MIN;
    output += F("\t Max: ");            output += DOME_AUTO_SPEED_MAX;
    printOutput();
    #endif
  }

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("begin()");
  output += F(" - Syren10 started.");
  printOutput();
  #endif
}


// ==============================
//     interpretController()
// ==============================
void Dome_Motor::interpretController()
{
  // -------------------------------------------
  // When no controller is found stop the motor.
  // -------------------------------------------

  if ( ! _buffer->isControllerConnected() ) {
    stop();
    return;
  }

  // ----------------------------------------
  // Look for dome automation enable/disable.
  // ----------------------------------------

/* ===============================================
 *  Enable/Disable commands
 * ===============================================
 *
 *                            PS3 Navigation    PS3/PS4 Controller
 *                            ==============    ==================
 *  Enable dome automation    L2+Circle         L2+Share
 *  Disable dome automation   L2+Cross          L2+Options
 *
 * =============================================== */

  if ( _buffer->getButton(L2) > 10 ) {

    if ( _buffer->isDomeAutomationRunning() ) {

      if ( _buffer->getButton(SELECT) ) {
        _automationOff();
      }

    } else {

      if ( _buffer->getButton(START) ) { 
        _automationOn();
      }
    }
  }

  // ------------------------------------------------------------------------
  // Flood control. Don't overload the motor controller with excessive input.
  // ------------------------------------------------------------------------

  _currentTime = millis();
  if ( (_currentTime - _previousTime) < (SERIAL_LATENCY * 2) ) {
    return;
  }
  _previousTime = _currentTime;

  // ================================
  // Look for dome rotation commands.
  // ================================

/* ===================================
 *  Dome rotation with variable speed
 * ===================================
 *
 *                Single PS3 Nav    Dual PS3 Nav     PS3/PS4 Controller
 *                ==============    =============    ==================
 *  Rotate dome   L2+Stick          2nd Nav Stick    Right Stick
 *
 * =================================== */

  // --------------------------
  // Get the joystick position.
  // --------------------------

  uint8_t stickPosition = JOYSTICK_CENTER;

  if ( _buffer->isFullControllerConnected() ) {

    // This is a PS3, PS4, or dual PS3 Nav controller.
    stickPosition = _buffer->getStick(RightHatX);

  } else if ( _buffer->isHalfControllerConnected() ) {

    // This is a single PS3 Nav controller.
    stickPosition = _buffer->getStick(LeftHatX);

  }

  // -----------------------------------------------------
  // A stick within its dead zone is the same as centered.
  // Stop dome rotation when the stick is centered.
  // -----------------------------------------------------
  
  if ( abs(stickPosition - JOYSTICK_CENTER) < JOYSTICK_DEAD_ZONE ) {
    stop();
    return;
  }

  // --------------------------------------------------
  // Convert the joystick position to a rotation speed.
  // --------------------------------------------------

  int rotationSpeed = map(stickPosition, JOYSTICK_MIN, JOYSTICK_MAX, -DOME_SPEED, DOME_SPEED);

  // -------------------------------------------
  // Turn off dome automation if manually moved.
  // -------------------------------------------

  if ( rotationSpeed != 0 && _buffer->isDomeAutomationRunning() ) {
    _automationOff();
  }

  // --------------------------
  // Send the rotation command.
  // --------------------------

  _rotateDome(rotationSpeed);
}


// ================
//      stop()
// ================
void Dome_Motor::stop(void)
{
  if ( _buffer->isDomeStopped() ) { return; }

  _Syren.stop();
  _buffer->setDomeStopped(true);

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("_stopDome()");
  output += F(" - Dome motor stopped.");
  printOutput();
  #endif
}


// =======================
//      _rotateDome()
// =======================
void Dome_Motor::_rotateDome(int rotationSpeed)
{
  _Syren.motor(rotationSpeed);
  _buffer->setDomeStopped(false);

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  if ( ! _buffer->isDomeStopped() ) {
    output = _className+F("_rotateDome()");
    if ( rotationSpeed == 0 )
      output += F(" - Stopping dome.");
    else {
      output += F(" - Rotate dome at speed ");
      output += rotationSpeed;
      if ( rotationSpeed < 0 )
        output += F(" CCW.");
      else
        output += F(" CW.");
    }
    printOutput();
  }
  #endif
}


// ============================================
//          Dome automation functions
// ============================================

// =========================
//     _automationOff()
// =========================
void Dome_Motor::_automationOff()
{
  _buffer->setDomeAutomationRunning(false);
  _rotationStatus = STOPPED;
  _targetPosition = 0;

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("automationOff()");
  output += F(" - Dome automation disabled.");
  printOutput();
  #endif
}


// ========================
//     _automationOn()
// ========================
void Dome_Motor::_automationOn()
{
  _buffer->setDomeAutomationRunning(true);

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("automationOn()");
  output += F(" - Dome automation enabled.");
  printOutput();
  #endif
}


// =====================
//     automation()
// =====================
void Dome_Motor::automation()
{
  // ----------------------------------------------
  // Do not run automation if settings are invalid.
  // ----------------------------------------------

  if ( _automationSettingsInvalid ) { return; }

  // ------------------------------------------------------------
  // Call the function appropriate to the current rotation cycle.
  // ------------------------------------------------------------

  switch(_rotationStatus) {
    case STOPPED:
      _automationInit();
      break;
    case READY:
      _automationReady();
      break;
    case TURNING:
      _automationTurn();
      break;
  }
}


// ========================
//     _automationInit()
// ========================
void Dome_Motor::_automationInit(void)
{

  if ( _targetPosition == 0 ) {

    // -----------------------------------------------
    // Previous position was home. Set a new position.
    // -----------------------------------------------

    _targetPosition = random(5,354);
    _startTurnTime = millis() + (random(3, 11) * 1000); // Wait 3-10 seconds before turning.
    if ( _targetPosition < 180 ) {
      _stopTurnTime = _startTurnTime + ((_targetPosition / 360) * TIME_360_DOME_TURN);
      _turnDirection = 1;
    } else {
      _stopTurnTime = _startTurnTime + (((360 - _targetPosition) / 360) * TIME_360_DOME_TURN);
      _turnDirection = -1;
    }

  } else {

    // ----------------------------------------------
    // Previous positon was not home. Return to home.
    // ----------------------------------------------

    _startTurnTime = millis() + (random(1,6) * 1000); // Wait 1-5 seconds before returning home.
    if ( _targetPosition < 180 ) {
      _stopTurnTime = _startTurnTime + ((_targetPosition / 360) * TIME_360_DOME_TURN);
    } else {
      _stopTurnTime = _startTurnTime + (((360 - _targetPosition) / 360) * TIME_360_DOME_TURN);
    }
    _turnDirection = _turnDirection * -1;
    _targetPosition = 0;
  }

  // ---------------------------
  // Advance the rotation cycle.
  // ---------------------------
  
  _rotationStatus = READY;
   
  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("_automationInit()");
  output += F(" - Turn set");
  output += F("\r\n  Current Time: ");    output += millis();
  output += F("\r\n  Next Start Time: "); output += _startTurnTime;
  output += F("\r\n  Next Stop Time: ");  output += _stopTurnTime;
  output += F("\r\n  Target Position: "); output += _targetPosition;
  printOutput();
  #endif
}


// =========================
//     _automationReady()
// =========================
void Dome_Motor::_automationReady(void)
{
  if (_startTurnTime < millis()) {

    // ---------------------------
    // Advance the rotation cycle.
    // ---------------------------

    _rotationStatus = TURNING;

    // ----------
    // Debugging.
    // ----------

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output = _className+F("_automationReady()");
    output += F(" - Ready to turn");
    printOutput();
    #endif
  }
}


// ====================
//     _automationTurn()
// ====================
void Dome_Motor::_automationTurn(void)
{
  if ( millis() < _stopTurnTime ) {

    // ------------------------------------------------------
    // Actively turn the dome until it reaches its stop time.
    // ------------------------------------------------------

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output = _className+F("_automationTurn()");
    output += F(" - Turning.");
    printOutput();
    #endif

    int rotationSpeed = DOME_AUTO_SPEED * _turnDirection;
    _rotateDome(rotationSpeed);
  } 
  else {

    // -------------------------------
    // Turn completed. Stop the motor.
    // -------------------------------

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output = _className+F("_automationTurn()");
    output += F(" - Stop turning.");
    printOutput();
    #endif

    stop();

    // ---------------------------
    // Advance the rotation cycle.
    // ---------------------------

    _rotationStatus = STOPPED;
  }
}
#endif
