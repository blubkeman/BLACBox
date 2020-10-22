/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Dome_Motor.cpp - Library for a Syren 10 dome motor controller
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Peripheral_Dome_Motor.h"


// =====================
//      Constructor
// =====================
Dome_Motor::Dome_Motor(Buffer* pBuffer) : _Syren(SYREN_ADDR, MotorSerial)
{
  _buffer = pBuffer;

  _buffer->setDomeEnabled(true);
  _buffer->setDomeStopped(true);
  _buffer->setDomeAutomationRunning(false);
  _buffer->setDomeCustomPanelRunning(false);
  
  _turnDirection  = 1;  // 1 = positive turn, -1 negative turn
  _targetPosition = 0;  // (0 - 359) - degrees in a circle, 0 = home
  _stopTurnTime   = 0;  // millis() when next turn should stop
  _startTurnTime  = 0;  // millis() when next turn should start
  _previousMillis = 0;
  _rotationStatus = 0;  // 0 = stopped, 1 = prepare to turn, 2 = turning

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  _className = F("Dome_Motor::");
  #endif
}


// =====================
//        begin()
// =====================
void Dome_Motor::begin()
{
  // Start Syren 10 dome motor.

  _Syren.autobaud();
  _Syren.setTimeout(300);

  // This is used by the dome's interrupt.

  anchor = this;

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("begin()");
  output += F(" - Dome motor started.");
  Serial.println(output);
  #endif
}


// =====================
//     automation()
// =====================
void Dome_Motor::automation()
{
  switch(_getCycle()) {
    case 0:
      _automationInit();
      break;
    case 1:
      _automationReady();
      break;
    case 2:
      _automationTurn();
      break;
  }
}


// ==============================
//     interpretController()
// ==============================
void Dome_Motor::interpretController()
{
  // First priority - Has the controller given reason to stop the motor?

  if (_buffer->isDomeStopped()) {
    _buffer->stopDomeMotor();
    return;
  }

  // Flood control prevention
  // This is intentionally set to double the rate of the Foot Motor Latency

  if ((millis() - _previousMillis) < (2*SERIAL_LATENCY) ) return;  

  // ===============================================
  //  Dome automation enable/disable
  // ===============================================
  //    L2+Circle = Enable dome automation
  //    L2+Cross  = Disable dome automation
  // ===============================================

  if ( (_buffer->isDomeAutomationRunning()) ) {
    if ( _buffer->getButton(L2) && _buffer->getButton(SELECT) ) {
      _automationOff();
      return;
    }
  } else {
    if ( _buffer->getButton(L2) && _buffer->getButton(START) ) {
      _automationOn();
      return;
    }
  }

  // Look for rotation commands.

  // ===============================================
  //  Dome rotation
  // ===============================================
  //  When there is a secondary controller.
  //    Nav2 Stick    = Rotate dome at variable speed and direction
  //  When there is no secondary controller.
  //    Nav1 Stick-L2 = Rotate dome at variable speed and direction
  //                    (L2 cannot be pressed. It is the dead man switch for foot motors.)
  //    Nav1 Cross    = Rotate dome CW at the fixed speed while driving
  //    Nav1 Circle   = Rotate dome CCW at the fixed speed while driving
  // ===============================================

  int manualRotationSpeed = 0;

  if ( _buffer->isSecondaryConnected() ) {

    if ( _buffer->isStickOffCenter(RightHatX) ) {

      // Rotate the dome in the direction and speed according to the secondary joystick.

      manualRotationSpeed = _mapStick(_buffer->getStick(RightHatX));
    }

  } else if ( _buffer->isPrimaryConnected() ) {

    // The primary controller handles dome rotation only when there
    // is no secondary controller. Priority is given to the stick
    // first then the button controls.

    if ( _buffer->isStickOffCenter(LeftHatX) && \
         _buffer->getButton(L2) == false ) {

      // Rotate the dome in the direction and speed according to the primary joystick.

      manualRotationSpeed = _mapStick(_buffer->getStick(LeftHatX));

    } else {

      if (_buffer->isButtonModified() == false ) {
        if ( _buffer->getButton(SELECT)) {
          manualRotationSpeed = DOME_AUTO_SPEED * -1; // Rotate the dome counterclockwise at a fixed speed.
        } else if ( _buffer->getButton(START) ) {
          manualRotationSpeed = DOME_AUTO_SPEED;      // Rotate the dome clockwise at a fixed speed.
        }
      }
    }
  }

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  if (manualRotationSpeed != 0) {
    output = _className+F("interpretController()");
    output += F(" - Rotate dome ");
    if (manualRotationSpeed < 0) {
      output += F("CCW");
    } else {
      output += F("CW");    
    }
    output += F(" at speed ");
    output += manualRotationSpeed;
    output += F(".");
    Serial.println(output);
  }
  #endif

  // Turn off dome automation if manually moved

  if (manualRotationSpeed != 0 && _buffer->isDomeAutomationRunning() == true) {
    _automationOff();
  }

  // Send the rotation command.

  _rotateDome(manualRotationSpeed);
}


// ===================
//      domeISR()
// ===================
static void Dome_Motor::domeISR()
{
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

  // Update the buffer's status.

  _buffer->setDomeStopped(true);

  // Reset the dome motor interrupt.

  digitalWrite(domeInterruptPin, HIGH);

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("_stopDome()");
  output += F(" - Dome motor stopped.");
  Serial.println(output);
  #endif
}


// =======================
//      _rotateDome()
// =======================
void Dome_Motor::_rotateDome(int rotationSpeed)
{
  unsigned long currentMillis = millis();

  //Flood control prevention
  //This is intentionally set to double the rate of the Foot Motor Latency

  if ( (currentMillis - _previousMillis) <= (2*SERIAL_LATENCY) )
    return;

  // Update the dome's movement status.

  if (rotationSpeed == 0)
//    _buffer->setStatus(DomeMotorStopped, true);
    _buffer->setDomeStopped(true);
  else
//    _buffer->setStatus(DomeMotorStopped, false);
    _buffer->setDomeStopped(false);

  // Send the command to the dome motor.

  _Syren.motor(rotationSpeed);

  // Prepare for the next cycle.

  _previousMillis = currentMillis;

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("_rotateDome()");
  if (rotationSpeed < 0)
    output += F(" - Spinning dome left at speed: ");
  else if (rotationSpeed > 0)
    output += F(" - Spinning dome right at speed: ");
  else
    output += F(" - Stopping dome spin speed: ");
  output += rotationSpeed;
  Serial.println(output);
  #endif

}


// =====================
//      _mapStick()
// =====================
int Dome_Motor::_mapStick(int stickPosition)
{
  int ret;

  // A joystick does not always hold itself perfectly centered when not
  // touched. To compensate for this, we redefine center to include a
  // small range off-center. This is called the dead zone.  When the
  // stick is in the dead zone treat it as being centered.

  if ( abs(stickPosition-128) < JOYSTICK_DOME_DEAD_ZONE_RANGE ) {
    ret = 0;
  } else {
    ret = (map(stickPosition, 0, 255, -DOME_SPEED, DOME_SPEED));
  }

  return ret;
}


// ===========================================
//          Rotation cycle functions
// ===========================================
void Dome_Motor::_nextCycle()   { _rotationStatus = (_rotationStatus < 2 ? _rotationStatus++ : 0); }
uint8_t Dome_Motor::_getCycle() { return _rotationStatus; }
void Dome_Motor::_resetCycle()  { _rotationStatus = 0; }


// =========================
//     _automationOff()
// =========================
void Dome_Motor::_automationOff()
{
//  _buffer->setStatus(Status::DomeAutomationRunning, false);
  _buffer->setDomeAutomationRunning(false);
  _resetCycle();
  _targetPosition = 0;

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("automationOff()");
  output += F(" - Dome automation disabled.");
  Serial.println(output);
  #endif
}


// ========================
//     _automationOn()
// ========================
void Dome_Motor::_automationOn()
{
//  _buffer->setStatus(Status::DomeAutomationRunning, true);
  _buffer->setDomeAutomationRunning(true);

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("automationOn()");
  output += F(" - Dome automation enabled.");
  Serial.println(output);
  #endif
}


// ========================
//     _automationInit()
// ========================
void Dome_Motor::_automationInit(void)
{
  if (_targetPosition == 0) {

    // Dome is currently in the home position - prepare to turn.

    // Set the target position to a random degree of a 360 degree
    // circle shaving off the first and last 5 degrees

    _targetPosition = random(5,354);

    // Turn the dome in the positive (CW) or negative (CCW) direction.

    _startTurnTime = millis() + (random(3, 10) * 1000);
    if (_targetPosition < 180) {
      _turnDirection = 1;
      _stopTurnTime = _startTurnTime + ((_targetPosition / 360) * TIME_360_DOME_TURN_RIGHT);
    }
    else {
      _turnDirection = -1;
      _stopTurnTime = _startTurnTime + (((360 - _targetPosition) / 360) * TIME_360_DOME_TURN_LEFT);
    }

  } else {

    // Dome is not in the home position - send it back to home

    _startTurnTime = millis() + (random(3, 10) * 1000);
    if (_targetPosition < 180) {
      _turnDirection = -1;
      _stopTurnTime = _startTurnTime + ((_targetPosition / 360) * TIME_360_DOME_TURN_LEFT);
    }
    else {
      _turnDirection = 1;
      _stopTurnTime = _startTurnTime + (((360 - _targetPosition) / 360) * TIME_360_DOME_TURN_RIGHT);
    }
    _targetPosition = 0;
  }

  _nextCycle();
   
  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  output = _className+F("_automationInit()");
  output += F(" - Dome Automation: Initial Turn Set");
  Serial.println(output);
  output = F("\tCurrent Time: ");
  output += millis();
  Serial.println(output);
  output = F("\tNext Start Time: ");
  output += _startTurnTime;
  Serial.println(output);
  output = F("\tNext Stop Time: ");
  output += _stopTurnTime;
  Serial.println(output);
  output = F("\tDome Target Position: ");
  output += _targetPosition;
  Serial.println(output);
  #endif
}


// =========================
//     _automationReady()
// =========================
void Dome_Motor::_automationReady(void)
{
  if (_startTurnTime < millis()) {
    _nextCycle();

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output = _className+F("_automationReady()");
    output += F(" - Dome Automation: Ready To Start Turn");
    Serial.println(output);
    #endif
  }
}


// ====================
//     _automationTurn()
// ====================
void Dome_Motor::_automationTurn(void)
{
  if (_stopTurnTime > millis()) {

    // Actively turn the dome until it reaches its stop time.

    int autoRotationSpeed = DOME_AUTO_SPEED * _turnDirection;
    _rotateDome(autoRotationSpeed);

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output = _className+F("_automationTurn()");
    output += F(" - Turning Now!!");
    Serial.println(output);
    #endif
  } 
  else {

    // Turn completed. Stop the motor.

    _nextCycle();
    _Syren.stop();

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    output = _className+F("_automationTurn()");
    output = F(" - STOP TURN!!");
    Serial.println(output);
    #endif
  }
}
