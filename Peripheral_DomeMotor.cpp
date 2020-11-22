/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Dome_Motor.cpp - Library for a Syren 10 dome motor controller
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Peripheral_DomeMotor.h"

#if defined(SYREN10)

// =====================
//      Constructor
// =====================
Dome_Motor::Dome_Motor(Buffer* pBuffer) : _Syren(SYREN_ADDR, DomeSerial)
{
  _buffer = pBuffer;

  // -------------------
  // Rotation variables.
  // -------------------

  _previousTime = 0;
  _rotationStatus = 0;  // 0 = stopped, 1 = prepare to turn, 2 = turning

  // ---------------------
  // Automation variables.
  // ---------------------

  _turnDirection  = 1;  // 1 = positive turn, -1 negative turn
  _targetPosition = 0;  // (0 - 359) - degrees in a circle, 0 = home
  _stopTurnTime   = 0;  // millis() when next turn should stop
  _startTurnTime  = 0;  // millis() when next turn should start

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

  // -------------------------------------
  // This is used by the dome's interrupt.
  // -------------------------------------

  anchor = this;

  // ---------------------------------------
  // Random number seed for dome automation.
  // ---------------------------------------

  randomSeed(analogRead(0));

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
  int manualRotationSpeed = 0;
  unsigned long currentTime = millis();

  // Note: If at any point we lose both controllers, the dome motor
  // will be stopped through the use of an interrupt. Therefore,
  // we will not be testing for that condition here.

  // ------------------------------------------------------------------------
  // Flood control prevention
  // This is intentionally set to double the rate of the Drive Motor Latency.
  // ------------------------------------------------------------------------

  if ( (currentTime - _previousTime) < (SERIAL_LATENCY * 2) )
    return;

  _previousTime = currentTime;

  // ----------------------------------------
  // Look for dome automation enable/disable.
  // ----------------------------------------

/* ===============================================
 *  Enable/Disable commands
 * ===============================================
 *
 *  PS3 Navigation
 *  ==============
 *  L2+Circle = Enable dome automation
 *  L2+Cross  = Disable dome automation
 *
 *  PS4 Controller
 *  ==============
 *  L2+Share   = Enable dome automation
 *  L2+Options = Disable dome automation
 *
 * =============================================== */

  if ( _buffer->getButton(L2) > 10 ) {

    #if defined(DEBUG_DOME) || defined(DEBUG_ALL)
    output = _className+F("interpretController()");
    output += F(" - Dome automation");
    #endif

    if ( _buffer->isDomeAutomationRunning() ) {
      if ( _buffer->getButton(SELECT) ) {
    
        _automationOff();
  
        #if defined(DEBUG_DOME) || defined(DEBUG_ALL)
        output += F(" off");
        printOutput();
        #endif
      }
    } else {
      if ( _buffer->getButton(START) ) { 

        _automationOn();
  
        #if defined(DEBUG_DOME) || defined(DEBUG_ALL)
        output += F(" on");
        printOutput();
        #endif
      }
    }
  }

  // ---------------------------
  // Look for rotation commands.
  // ---------------------------

/* ===================================
 *  Dome rotation with variable speed
 * ===================================
 *
 *  Single-device controller (PS3, PS4, etc.) or 
 *  dual PS3Nav controllers (a.k.a. full controller)
 *  ================================================
 *  Right (or Nav2 Stick) = Rotate dome
 *  
 *  Single PS3Nav controller (a.k.a. half controller)
 *  =================================================
 *  L2+Nav1 Stick = Rotate dome
 *
 * =================================== */

  if ( _buffer->isFullControllerConnected() ) {
    if ( _buffer->isStickOffCenter(RightHatX) ) {
      manualRotationSpeed = _mapStick(_buffer->getStick(RightHatX));
    }
  } else if ( _buffer->isHalfControllerConnected() ) {
    if ( _buffer->getButton(L2) && _buffer->isStickOffCenter(LeftHatX) )
      manualRotationSpeed = _mapStick(_buffer->getStick(LeftHatX));
  }

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  if ( manualRotationSpeed != 0 ) {
    output = _className+F("interpretController()");
    output += F(" - Rotate dome at speed ");
    output += manualRotationSpeed;
    if ( manualRotationSpeed < 0 )
      output += F(" CCW.");
    else
      output += F(" CW.");
    printOutput();
  }
  #endif

  // -------------------------------------------
  // Turn off dome automation if manually moved.
  // -------------------------------------------

  if ( manualRotationSpeed != 0 && _buffer->isDomeAutomationRunning() ) {
    _automationOff();
  }

  // --------------------------
  // Send the rotation command.
  // --------------------------

  _rotateDome(manualRotationSpeed);
}


// ================
//      stop()
// ================
void Dome_Motor::stop(void)
{
  // ----------------------------------------------------
  // Throw the interrupt that will stop the drive motors.
  // ----------------------------------------------------

  if ( ! _buffer->isDomeStopped() )
    digitalWrite(DOME_INTERRUPT_PIN, LOW);
}


// ===================
//      domeISR()
// ===================
static void Dome_Motor::domeISR()
{
  // -----------------------------------------------------
  // This is the interrupt service routine (ISR) used to
  // stop the dome motor wheneve we lose both controllers.
  // -----------------------------------------------------

  if (anchor != NULL)
    anchor->_stopDome();
}


// ====================
//      _stopDome()
// ====================
void Dome_Motor::_stopDome()
{
  // Stop the motor.

  _Syren.stop();

  // ---------------------------
  // Update the buffer's status.
  // ---------------------------

  _buffer->setDomeStopped(true);

  // -------------------------------
  // Reset the dome motor interrupt.
  // -------------------------------

  digitalWrite(DOME_INTERRUPT_PIN, HIGH);

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
  // ----------------------------------
  // Update the dome's movement status.
  // ----------------------------------

  if (rotationSpeed == 0)
    _buffer->setDomeStopped(true);
  else
    _buffer->setDomeStopped(false);

  // -----------------------------------
  // Send the command to the dome motor.
  // -----------------------------------

  _Syren.motor(rotationSpeed);

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


// =====================
//      _mapStick()
// =====================
int Dome_Motor::_mapStick(int stickPosition)
{
  // A joystick does not always hold itself perfectly centered when not
  // touched. To compensate for this, we redefine center to include a
  // small range off-center. This is called the DEBUG_DRIVE zone.  When the
  // stick is in the DEBUG_DRIVE zone treat it as being centered.

  int outValue = 0;
  if ( abs(stickPosition-128) >= JOYSTICK_DEBUG_DRIVE_ZONE )
    outValue = (map(stickPosition, 0, 255, -DOME_SPEED, DOME_SPEED));
  return outValue;
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
  // --------------------------------------------------
  // Do not run automation if settings are too far off.
  // --------------------------------------------------

  if ( TIME_360_DOME_TURN <= TIME_360_DOME_TURN_MIN || TIME_360_DOME_TURN >= TIME_360_DOME_TURN_MAX ||
       DOME_AUTO_SPEED <= DOME_AUTO_SPEED_MIN || DOME_AUTO_SPEED >= DOME_AUTO_SPEED_MAX ) {
    
    // ----------
    // Debugging.
    // ----------

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output = _className+F("automation()");
    output += F(" - Invalid settings.");
    output += F("\r\n  Turn time: ");   output += TIME_360_DOME_TURN;
    output += F("\t Min: ");            output += TIME_360_DOME_TURN_MIN;
    output += F("\t Max: ");            output += TIME_360_DOME_TURN_MAX;
    output += F("\r\n  Dome speed: ");  output += DOME_AUTO_SPEED;
    output += F("\t Min: ");            output += DOME_AUTO_SPEED_MIN;
    output += F("\t Max: ");            output += DOME_AUTO_SPEED_MAX;
    printOutput();
    #endif

    return;
  }

  // ------------------------------------------------------------
  // Call the function appropriate to the current rotation cycle.
  // ------------------------------------------------------------

  switch(_rotationStatus) {
    case STOPPED:
      _automationInit();
      break;
    case PREPARED:
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
  
  _rotationStatus = PREPARED;
   
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

    // ----------
    // Debugging.
    // ----------

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output = _className+F("_automationReady()");
    output += F(" - Ready to turn");
    printOutput();
    #endif

  // ---------------------------
  // Advance the rotation cycle.
  // ---------------------------

    _rotationStatus = TURNING;
  }
}


// ====================
//     _automationTurn()
// ====================
void Dome_Motor::_automationTurn(void)
{
  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("_automationTurn()");
  #endif

  if ( millis() < _stopTurnTime ) {

    // ------------------------------------------------------
    // Actively turn the dome until it reaches its stop time.
    // ------------------------------------------------------

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output += F(" - Turning.");
    #endif

    int autoRotationSpeed = DOME_AUTO_SPEED * _turnDirection;
    _rotateDome(autoRotationSpeed);
  } 
  else {

    // -------------------------------
    // Turn completed. Stop the motor.
    // -------------------------------

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output += F(" - Stop turning.");
    #endif

    _Syren.stop();

    // ---------------------------
    // Advance the rotation cycle.
    // ---------------------------

    _rotationStatus = STOPPED;
  }
  
  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  printOutput();
  #endif
}
#endif
