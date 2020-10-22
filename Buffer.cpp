/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Buffer.cpp - Library for controller inputs for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */

#include "Arduino.h"
#include "Buffer.h"


// =====================
//      Constructor
// =====================
Buffer::Buffer(void)
{
  // Initialize the flags for stopping motors.

  _stopDomeMotor = false;
  _stopFootMotor = false;

  // Initialize statuses

  _primaryConnected = false;
  _secondaryConnected = false;
  _footEnabled = false;
  _footStopped = true;
  _domeEnabled = false;
  _domeStopped = true;
  _domeAutomationRunning = false;
  _domeCustomPanelRunning = false;
  _holoAutomationRunning = false;

  // Prepare for debugging

  #if defined(DEBUG_BUFFER) || defined(DEBUG_ALL)
  _className = F("Buffer::");
  #endif
}

// =================
//      begin()
// =================
void Buffer::begin(void)
{
  // Initialize the array of buttons and the arrays of
  // current and previous joystick positions to center.

  for (uint8_t i = 0; i < 20; i++) {
    setButton(i, 0);
    if (i < 4) {
      setStick(i, 127);
      setPrevStick(i, 127);
    }
  }

  #if defined(DEBUG_BUFFER) || defined(DEBUG_ALL)
  output = _className+F("begin()");
  output += F(" - Control buffer started.");
  Serial.println(output);
  #endif
}

// =============================================
//          Controller input functions
// =============================================
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

bool Buffer::isButtonModified()
{
  return (getButton(L1) && getButton(L2) && \
          getButton(R1) && getButton(R2) && \
          getButton(PS) && getButton(PS2));
}
bool Buffer::isStickOffCenter(uint8_t i)
{
  return (getStick(i) < 117 || getStick(i) > 137);
}

// ===================================
//          Status functions
// ===================================
void Buffer::setPrimaryConnected(bool b)       { _primaryConnected = b; };
void Buffer::setSecondaryConnected(bool b)     { _secondaryConnected = b; };
void Buffer::setFootEnabled(bool b)            { _footEnabled = b; };
void Buffer::setFootStopped(bool b)            { _footStopped = b; };
void Buffer::setDomeEnabled(bool b)            { _domeEnabled = b; };
void Buffer::setDomeStopped(bool b)            { _domeStopped = b; };
void Buffer::setDomeAutomationRunning(bool b)  { _domeAutomationRunning = b; };
void Buffer::setDomeCustomPanelRunning(bool b) { _domeCustomPanelRunning = b; };
void Buffer::setHoloAutomationRunning(bool b)  { _holoAutomationRunning = b; };

bool Buffer::isPrimaryConnected(void)       { return _primaryConnected; };
bool Buffer::isSecondaryConnected(void)     { return _secondaryConnected; };
bool Buffer::isFootEnabled(void)            { return _footEnabled; };
bool Buffer::isFootStopped(void)            { return _footStopped; };
bool Buffer::isDomeEnabled(void)            { return _domeEnabled; };
bool Buffer::isDomeStopped(void)            { return _domeStopped; };
bool Buffer::isDomeAutomationRunning(void)  { return _domeAutomationRunning; };
bool Buffer::isDomeCustomPanelRunning(void) { return _domeCustomPanelRunning; };
bool Buffer::isHoloAutomationRunning(void)  { return _holoAutomationRunning; };


// =======================================
//          Stop Motors functions
// =======================================
void Buffer::stopDomeMotor(void) { digitalWrite(domeInterruptPin, LOW); }
void Buffer::stopFootMotor(void) { digitalWrite(footInterruptPin, LOW); }


#ifdef TEST_CONTROLLER
// ===================================
//          Testing functions
// ===================================
void Buffer::_displayStick(String stick)
{
  output = _className+F("_displayStick()");
  if (stick == "Left") {
    output += " - LeftHatX: ";
    output += getStick(LeftHatX);
    output += " LeftHatY: ";
    output += getStick(LeftHatY);
  } else {
    output += " - RightHatX: ";
    output += getStick(RightHatX);
    output += " RightHatY: ";
    output += getStick(RightHatY);
  }
  Serial.println(output);
}

void Buffer::_displayButton(String dPadButton)
{
  output = _className+F("_displayButton");
  if (getButton(L1))          output += "L1 + ";
  else if (getButton(L2))     output += "L2 + ";
  else if (getButton(SELECT)) output += "CROSS + ";
  else if (getButton(START))  output += "CIRCLE + ";
  else if (getButton(PS) ||
           getButton(PS2))    output += "PS + ";
  output += dPadButton;
  Serial.println(output);
}

void Buffer::testInput()
{
  uint8_t centerPlus = JOYSTICK_CENTER + JOYSTICK_CENTER_OFFSET;
  uint8_t centerMinus = JOYSTICK_CENTER - JOYSTICK_CENTER_OFFSET;

  if (isPrimaryConnected()) {

    uint8_t leftX = getStick(LeftHatX);
    uint8_t leftY = getStick(LeftHatY);
    if ( (leftX < centerMinus || leftX > centerPlus ||
          leftY < centerMinus || leftY > centerPlus) ) {
      _displayStick("Left");
    }
  
    if (getButton(UP))    { _displayButton("UP(Nav1)"); }
    if (getButton(RIGHT)) { _displayButton("RIGHT(Nav1)"); }
    if (getButton(DOWN))  { _displayButton("DOWN(Nav1)"); }
    if (getButton(LEFT))  { _displayButton("LEFT(Nav1)"); }
    if (getButton(L3))    { _displayButton("L3(Nav1)"); }
  }

  if (isSecondaryConnected()) {

    uint8_t rightX = getStick(RightHatX);
    uint8_t rightY = getStick(RightHatY);
    if ( (rightX < centerMinus || rightX > centerPlus ||
          rightY < centerMinus || rightY > centerPlus) ) {
      _displayStick("Right");
    }
  
    if (getButton(TRIANGLE)) { _displayButton("UP(Nav2)"); }
    if (getButton(CIRCLE))   { _displayButton("RIGHT(Nav2)"); }
    if (getButton(CROSS))    { _displayButton("DOWN(Nav2)"); }
    if (getButton(SQUARE))   { _displayButton("LEFT(Nav2)"); }
    if (getButton(R3))       { _displayButton("L3(Nav2)"); }
  }
}

void Buffer::scrollInput()
{
  //Nav1 - Up:x Rt:x Dn:x Lt:x L3:x L2:x L1:x PS:x | Nav2 - Up:x Rt:x Dn:x Lt:x R3:x PS:x

  output = "";
  
  if ( isPrimaryConnected() ) {
    output += "LX:";   output += getStick(LeftHatX);
    output += " LY:";  output += getStick(LeftHatY);
    output += " Up:";  output += getButton(UP);
    output += " Rt:";  output += getButton(RIGHT);
    output += " Dn:";  output += getButton(DOWN);
    output += " Lt:";  output += getButton(LEFT);
    output += " X:";   output += getButton(SELECT);
    output += " O:";   output += getButton(START);
    output += " L3:";  output += getButton(L3);
    output += " L2:";  output += getButton(L2);
    output += " L1:";  output += getButton(L1);
    output += " PS:";  output += getButton(PS);
  }

  if ( isSecondaryConnected() ) {
    output += " RX:";  output += getStick(RightHatX);
    output += " RY:";  output += getStick(RightHatY);
    output += " 2Up:"; output += getButton(TRIANGLE);
    output += " 2Rt:"; output += getButton(CIRCLE);
    output += " 2Dn:"; output += getButton(CROSS);
    output += " 2Lt:"; output += getButton(SQUARE);
    output += " R3:";  output += getButton(R3);
    output += " R2:";  output += getButton(R2);
    output += " R1:";  output += getButton(R1);
    output += " 2PS:"; output += getButton(PS2);
  }

  Serial.println(output);
}
#endif
