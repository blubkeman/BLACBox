/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Buffer.cpp - Library for controller inputs for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */

#include "Arduino.h"
#include "Buffer.h"

extern volatile bool domeStopped;
extern volatile bool driveStopped;


// =====================
//      Constructor
// =====================
Buffer::Buffer(void)
{
  // -----------------------------------------
  // Initialize the flags for stopping motors.
  // -----------------------------------------

  _stopDomeMotor = false;

  // --------------------
  // Initialize statuses.
  // --------------------

  _controllerConnected = NONE;
  _driveEnabled = true;
  _driveStopped = true;
  _overdriveEnabled = false;
  _domeStopped = true;
  _domeAutomationRunning = false;
  _domeCustomPanelRunning = false;
  _holoAutomationRunning = false;
  _speedProfile = 0;
  _updateRequested = false;

  // ---------------------
  // Prepare for debugging
  // ---------------------

  #if defined(DEBUG_BUFFER) || defined(DEBUG_ALL)
  _className = F("Buffer::");
  #endif
}

// =================
//      begin()
// =================
void Buffer::begin(void)
{
  // --------------------------------------------------
  // Initialize the array of buttons and the arrays of
  // current and previous joystick positions to center.
  // --------------------------------------------------

  for (uint8_t i = 0; i < 20; i++) {
    setButton(i, 0);
    if (i < 4) {
      setStick(i, 127);
      setPrevStick(i, 127);
    }
  }

  #if defined(DEBUG_BUFFER) || defined(DEBUG_ALL)
  output = F("\r\n==========================\r\n");
  output += _className+F("begin()");
  output += F(" - Control buffer started.");
  printOutput();
  #endif
}


// ==============================================
//           Controller input functions
// ==============================================
void Buffer::resetButtons(void) {
  for (uint8_t i = 0; i < sizeof(_input.button); i++)
    _input.button[i] = 0;
}
void Buffer::updateButton(uint8_t i, int v) {
  if (v > 0) { _input.button[i] = v; }
}
void Buffer::setButton(uint8_t i, int v)    { _input.button[i] = v; }
void Buffer::setStick(uint8_t i, int v)     { _input.stick[i] = v; }
void Buffer::setPrevStick(uint8_t i, int v) { _input.prevStick[i] = v; }

uint8_t Buffer::getButton(uint8_t i)    { return _input.button[i]; }
uint8_t Buffer::getStick(uint8_t i)     { return _input.stick[i]; }
uint8_t Buffer::getPrevStick(uint8_t i) { return _input.prevStick[i]; }

void Buffer::saveStick()
{
  for (uint8_t i = 0; i < 4; i++)
    _input.prevStick[i] = _input.stick[i];
}
void Buffer::restoreStick(uint8_t i)
{
  _input.stick[i] = _input.prevStick[i];
}

bool Buffer::isStickOffCenter(uint8_t i)
{
  return ( abs(getStick(i) - JOYSTICK_CENTER) >= JOYSTICK_DEAD_ZONE );
}


// ===============================================
//           Controller status functions
// ===============================================
void Buffer::setControllerConnected(uint8_t c) { _controllerConnected = c; };

bool Buffer::isFullControllerConnected(void) { return (_controllerConnected == 2 ? true : false); };
bool Buffer::isHalfControllerConnected(void) { return (_controllerConnected == 1 ? true : false); };
bool Buffer::isControllerConnected(void)     { return (_controllerConnected > 0  ? true : false); };


// ===============================================
//           Controller output functions
// ===============================================
void Buffer::requestLedUpdate(bool b) { _updateRequested = b; };
bool Buffer::isLedUpdateRequested(void) { return _updateRequested; };


// ================================================
//           Perpipheral status functions
// ================================================
void Buffer::setDriveEnabled(bool b) 
{
  _driveEnabled = b;
  requestLedUpdate(true);
}
void Buffer::setDriveStopped(bool b)           { _driveStopped = b; }
void Buffer::setDomeStopped(bool b)            { _domeStopped = b; }
void Buffer::setDomeAutomationRunning(bool b)  { _domeAutomationRunning = b; }
void Buffer::setDomeCustomPanelRunning(bool b) { _domeCustomPanelRunning = b; }
void Buffer::setHoloAutomationRunning(bool b)  { _holoAutomationRunning = b; }

void Buffer::setSpeedProfile(uint8_t i)
{
  _speedProfile = i;
  analogWrite(SCRIPT_PIN, i*50);
}

bool Buffer::isDriveEnabled(void)
{
  #if defined(PS3_NAVIGATION)
  // -------------------------------------------------
  // When we have only one PS3Nav controller, L2+Stick
  // rotates the dome. Drive is temporarily disabled.
  // -------------------------------------------------
  if ( isHalfControllerConnected() && getButton(L2) )
    return false;
  #endif

  return _driveEnabled;
}
bool Buffer::isDriveStopped(void)           { return _driveStopped; }
bool Buffer::isOverdriveEnabled(void)       { return _overdriveEnabled; }
bool Buffer::isDomeStopped(void)            { return _domeStopped; }
bool Buffer::isDomeAutomationRunning(void)  { return _domeAutomationRunning; }
bool Buffer::isDomeCustomPanelRunning(void) { return _domeCustomPanelRunning; }
bool Buffer::isHoloAutomationRunning(void)  { return _holoAutomationRunning; }
bool Buffer::isBodyPanelRunning(void)       { return _bodyPanelRunning; }
uint8_t Buffer::getSpeedProfile(void)       { return _speedProfile; }


// ===========================================
//           Motor control functions
// ===========================================
void Buffer::stopDomeMotor(void) {  }


#ifdef TEST_CONTROLLER
// ========================
//      displayInput()
// ========================
void Buffer::displayInput(void)
{
  if ( ! isControllerConnected() )
    return;

  output = "";

  _displayStick("Left", getStick(LeftHatX), getStick(LeftHatY));
  _displayStick("Right", getStick(RightHatX), getStick(RightHatY));

  _displayButtons(UP, LEFT);          // Display UP, RIGHT, DOWN, LEFT
  _displayButtons(TRIANGLE, SQUARE);  // Display TRIANGLE, CIRCLE, CROSS, SQUARE
  _displayButtons(SELECT, R3);        // Display SELECT, START, L3, R3

  if ( output != "") {
    output = _className+F("displayInput() - ")+output;
    printOutput();
  }
}

// ===========================
//      _displayButtons()
// ===========================
void Buffer::_displayButtons(uint8_t iStart, uint8_t iEnd)
{
  for (uint8_t i = iStart; i <= iEnd; i++) {
    if (getButton(i) > 0) {

      if ( output != "" ) { output += " + "; }
        output += buttonLabel[i];

      uint8_t btn;
      uint8_t modifiers[] = { SELECT, START, L2, R2, L1, R1, PS, PS2 };

      for (uint8_t j = 0; j < sizeof(modifiers); j++) {
        btn = modifiers[j];
        if ( btn >= iStart && btn <= iEnd ) { continue; }
        if (getButton(btn) > 0) {
          output += " + ";
          output += buttonLabel[btn];
        }
      }
    }
  }
}

// =========================
//      _displayStick()
// =========================
void Buffer::_displayStick(String s, uint8_t x, uint8_t y)
{
  uint8_t centerPlus = (JOYSTICK_CENTER + JOYSTICK_DEAD_ZONE);
  uint8_t centerMinus = (JOYSTICK_CENTER - JOYSTICK_DEAD_ZONE);

  if ( (x < centerMinus || x > centerPlus || y < centerMinus || y > centerPlus) ) {

    if ( output != "" ) { output += " + "; }
    output += s;
    output += F(": ");
    output += x;
    output += F(",");
    output += y;

    // This will show the pressed state of L2, R2, L1 and R1.
  
    for (uint8_t i = L2; i <= R1 ; i++) {
      if ( getButton(i) > 0 ) {
        output += " + ";
        output += buttonLabel[i];
      }
    }
  }
}

// =======================
//      scrollInput()
// =======================
void Buffer::scrollInput(void)
{
  //LX:x LY:x Up:x Rt:x Dn:x Lt:x L3:x L2:x L1:x PS:x [X:x O:x | Sl:x St:x] [RX:x RY:x Tr:x Ci:x Cr:x Sq:x R3:x R2:x R1:]

  output = "";
  
  if ( isControllerConnected() ) {
    output += "LX:";   output += getStick(LeftHatX);
    output += " LY:";  output += getStick(LeftHatY);
    output += " Up:";  output += getButton(UP);
    output += " Rt:";  output += getButton(RIGHT);
    output += " Dn:";  output += getButton(DOWN);
    output += " Lt:";  output += getButton(LEFT);
    output += " L3:";  output += getButton(L3);
    output += " L2:";  output += getButton(L2);
    output += " L1:";  output += getButton(L1);
    output += " PS:";  output += getButton(PS);

    #if defined(PS3_NAVIGATION)
    output += " X:";   output += getButton(SELECT);
    output += " O:";   output += getButton(START);
    #else
    output += " Sl:";   output += getButton(SELECT);
    output += " St:";   output += getButton(START);
    #endif
  }

  if ( isFullControllerConnected() ) {
    output += " RX:"; output += getStick(RightHatX);
    output += " RY:"; output += getStick(RightHatY);
    output += " Tr:"; output += getButton(TRIANGLE);
    output += " Ci:"; output += getButton(CIRCLE);
    output += " Cr:"; output += getButton(CROSS);
    output += " Sq:"; output += getButton(SQUARE);
    output += " R3:"; output += getButton(R3);
    output += " R2:"; output += getButton(R2);
    output += " R1:"; output += getButton(R1);
    #ifdef PS3_NAVIGATION
    output += " 2PS:"; output += getButton(PS2);
    #endif
  }

  printOutput();
}
#endif
