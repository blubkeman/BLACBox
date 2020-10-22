/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS3Nav.cpp - Library for a PS3 Move Navigation controller
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Controller_PS3Nav.h"

/* ============================================
 *           Authorized MAC Addresses
 * ============================================ */
const char * AUTHORIZED_MAC_ADDRESSES[2] = { "00:06:F7:B8:57:01", "E0:AE:5E:38:E0:CD" };


// =====================
//      Constructor
// =====================
Controller_PS3Nav::Controller_PS3Nav(Buffer * pBuffer) : _Usb(), _Btd(&_Usb), _primary(&_Btd), _secondary(&_Btd)
{
  // Access the buffer.

  _buffer = pBuffer;

  // Prepare for debugging

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  _className = F("Controller_PS3Nav::");
  #endif
}

// =================
//      begin()
// =================
void Controller_PS3Nav::begin(void)
{
  // Start the USB host.

  if (_Usb.Init() == -1) {
    Serial.println(F("OSC did not start"));
    while (1); //halt
  }
  Serial.println(F("Bluetooth Library Started"));

  // Setup the attachOnInit;

  anchor = this;
  _primary.attachOnInit(_onInit);
  _secondary.attachOnInit(_onInit);
}

// ================
//      read()
// ================
bool Controller_PS3Nav::read()
{
  // The more devices we have connected to the USB or BlueTooth, the 
  // more often Usb.Task needs to be called to eliminate latency.

  _Usb.Task();
  if (_primary.PS3NavigationConnected) _Usb.Task();
  if (_secondary.PS3NavigationConnected) _Usb.Task();

  // When we have no controllers,

  if ( ! _primary.PS3NavigationConnected &&  ! _secondary.PS3NavigationConnected ) {

    // make sure the buffer matches the controller status,

    if (_buffer->isPrimaryConnected())   { _disconnectController(&_primary, PRIMARY); }
    if (_buffer->isSecondaryConnected()) { _disconnectController(&_secondary, SECONDARY); }

    // stop any run-away motors,

    if ( ! _buffer->isFootStopped() ) { _buffer->stopFootMotor(); }
    if ( ! _buffer->isDomeStopped() ) { _buffer->stopDomeMotor(); }

    // and stop this read.

    return false;
  }

  // At this point, we have at least one controller connected.

  if ( ! _primary.PS3NavigationConnected ) {

    // Primary is disconnected.
    // Make sure the buffer matches the controller status, and stop a run-away motor.
    
    if (_buffer->isPrimaryConnected()) { _disconnectController(&_primary, PRIMARY); }
    if ( ! _buffer->isFootStopped() ) { _buffer->stopFootMotor(); }

  } else {

    // Primary is connected.
    // Handle for any 'awaiting reconnection' status.

    _checkReconnect(&_primary, PRIMARY);
  }
  
  if ( ! _secondary.PS3NavigationConnected ) {

    // Secondary is disconnected.
    // Make sure the buffer matches the controller status.
    // We do not check the dome motor because the primary controller can still control it.
    
    if (_buffer->isSecondaryConnected()) { _disconnectController(&_secondary, SECONDARY); }

  } else {

    // Secondary is connected.
    // Handle for any 'awaiting reconnection' status.

    _checkReconnect(&_secondary, SECONDARY);

  }

  // Look for critical faults. Stop this read when any are detected.

  if (_detectCriticalFault(&_primary, PRIMARY) || 
      _detectCriticalFault(&_secondary, SECONDARY)) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("read()");
    output += F(" - Critical fault detected.");
    Serial.println(output);
    #endif
    
    return false;
  }

  // Take a snapsot of the input from the controller(s).

  _setBuffer();

  // Look for and handle any disconnect command from the user.
  // A disconnect may be issued using L2+PS.

  if (_secondary.PS3NavigationConnected &&
      _buffer->getButton(L2) &&
      _buffer->getButton(PS2)) {

      #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
      output = _className+F("read()");
      output += F(" - User disconnected secondary controller");
      Serial.println(output);
      #endif
  
      _disconnectController(&_secondary, SECONDARY);
  }

  if (_primary.PS3NavigationConnected &&
      _buffer->getButton(L2) &&
      _buffer->getButton(PS)) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("read()");
    output += F(" - User disconnected primary controller");
    Serial.println(output);
    #endif

    _disconnectController(&_primary, PRIMARY);
  }

  // Optionally, display controller input to the serial monitor.

  #ifdef TEST_CONTROLLER
  if ( &_primary.PS3NavigationConnected || &_secondary.PS3NavigationConnected ) {

    // It is strongly recommended to uncomment only one of these two lines at a time.

    _buffer->testInput();
    //_buffer->scrollInputer;
  }
  #endif

  return true;
}


/* =========================================
 *          attachOnInit functions
 * =========================================
 * Due to how the USB Host Shield library was written, the 'attachOnInit' function requires
 * our function to be static and have no arguments.  This prevents us from calling other,
 * non-static members of the PS3BT class.  To get around this, we define a static function
 * (_onInit) which calls a non-static function (_onInitConnect). */

// ===================
//      _onInit()
// ===================
static void Controller_PS3Nav::_onInit(void)
{
  if (anchor != NULL) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = F("Controller_PS3Nav::_onInit() - Controller initialization started.");
    Serial.println(output);
    #endif

    anchor->_onInitConnect();
  }
}

// ========================
//      _onInitConnect
// ========================
void Controller_PS3Nav::_onInitConnect(void)
{
  // Always try to assign as primary first.
  // Assign as secondary only when the primary is already connected.

  if ( ! _buffer->isPrimaryConnected() ) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_onInitConnect()");
    output += F(" - Initiating connection with primary.");
    Serial.println(output);
    #endif

    _connectController(&_primary, PRIMARY);

  } else if ( ! _buffer->isSecondaryConnected() ) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_onInitConnect()");
    output += F(" - Initiating connection with secondary.");
    Serial.println(output);
    #endif

    _connectController(&_secondary, SECONDARY);
  }
}


/* =======================================
 *          Connection functions
 * ======================================= */
 
// ============================
//      _connectController
// ============================
void Controller_PS3Nav::_connectController(PS3BT * controller, uint8_t priority)
{
  // First, let's make sure we have a PS3 Move Navigation controller.  If not, drop it.

  if ( ! controller->PS3NavigationConnected ) {

    controller->disconnect();

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_connectController()");
    output += F(" - Disconnecting invalid controller.");
    Serial.println(output);
    #endif

    return;
  }

  // Get the MAC address of the controller.
  // Display the MAC address on the serial monitor before checking for authorization.

  String btAddress;
  for(int8_t i = 5; i >= 0; i--) {
    if (btAddress.length() > 0) btAddress +=(":");
    if (_Btd.disc_bdaddr[i]<0x10) btAddress +="0";
    btAddress += String(_Btd.disc_bdaddr[i], HEX);
  }
  btAddress.toUpperCase();

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_connectController()");
  output += F(" - MAC address: ");
  output += btAddress;
  Serial.println(output);
  #endif

  // Validate the MAC address against our authorized devices.

  bool authorized = false;

  for (uint8_t i = 0; i < sizeof(AUTHORIZED_MAC_ADDRESSES); i++) {
    if (btAddress == AUTHORIZED_MAC_ADDRESSES[i]) {
      authorized = true;
      break;
    }
  }

  if (! authorized) {

    // This is an unauthorized controller. Dump it.

    controller->disconnect();

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_connectController()");
    output += F(" - Disconnecting unauthorized controller.");
    Serial.println(output);
    #endif

    return;
  }

  // Display which controller was connected.

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_connectController()");
  if (priority == PRIMARY)
    output += F(" - Primary controller connected.");
  else if (priority == SECONDARY)
    output += F(" - Secondary controller connected.");
  Serial.println(output);
  #endif

  // Turn on the controller's LED light.

  controller->setLedOn(LED1);

  // Prepare the critical fault data.

  _initFaultData(priority);

  // Set the connected flag in the buffer so that peripherals may query the connection status.

  if (priority == PRIMARY) {
    _buffer->setPrimaryConnected(true);
  } else if (priority = SECONDARY) {
    _buffer->setSecondaryConnected(true);
  }

}

// =================================
//      _disconnectController()
// =================================
void Controller_PS3Nav::_disconnectController(PS3BT * controller, uint8_t priority)
{
  // Set the connected flag in the buffer so that peripherals may query the connection status.

  if (priority == PRIMARY) {

    _buffer->setPrimaryConnected(false);
    _buffer->setFootEnabled(false);

  } else if (priority == SECONDARY) {

    _buffer->setSecondaryConnected(false);
    if ( ! _buffer->isPrimaryConnected() ) {
      _buffer->setDomeEnabled(false);
    }
  }

  // Complete the disconnect.

  controller->disconnect();
  controller->setLedOff(LED1);

  // Display which controller was disconnected.

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_disconnectController()");
  output += F(" - Disconnected ");
  output += _faultData[priority].priority;
  output += F(" controller.");
  Serial.println(output);
  #endif

  // Reset the critical fault data.

  _resetCriticalFault(controller, priority);

  // If we just dumped the primary controller and still have a secondary then
  // drop the secondary as well. This will force the droid into quiet mode
  // and require us to reconnect all controllers.

  if ( ! _buffer->isPrimaryConnected() && _buffer->isSecondaryConnected() )
    _disconnectController(&_secondary, SECONDARY);
}

/* =====================================================
 *          Critical fault detection functions
 * ===================================================== */

// ==========================
//      _initFaultData()
// ==========================
void Controller_PS3Nav::_initFaultData(uint8_t priority)
{
  _faultData[priority].badData            = 0;
  _faultData[priority].lastMsgTime        = -1;
  _faultData[priority].pluggedState       = false;
  _faultData[priority].pluggedStateTime   = 0;
  _faultData[priority].waitingToReconnect = false;

  if (priority == PRIMARY)
    _faultData[priority].priority = "Primary";
  else if (priority == SECONDARY)
    _faultData[priority].priority = "Secondary";

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_initFaultData()");
  output += F(" - Critical fault data initialized.");
  Serial.println(output);
  #endif
}


// ================================
//      _detectCriticalFault()
// ================================
bool Controller_PS3Nav::_detectCriticalFault(PS3BT * controller, uint8_t priority)
{
  bool fault = false;

  if ( ! controller->PS3NavigationConnected )
    return fault;

  _lagTime = 0;
  _currentTime = millis();

  //--------------------------------------------------------
  // Check for problems with lag time.
  //--------------------------------------------------------

  fault = _checkLag(controller, priority);
  if (fault) { return fault; }

  //--------------------------------------------------------
  // When the controller is confused about its plugged vs
  // unplugged status, increment our bad data counter.
  // Disconnect when counter reaches 10.
  //--------------------------------------------------------

  fault = _checkSignal(controller, priority);

  return fault;
}

// =====================
//      _checkLag()
// =====================
bool Controller_PS3Nav::_checkLag(PS3BT * controller, uint8_t priority)
{
  bool fault = false;

  // Calculate lag.

  if (_currentTime > _faultData[priority].lastMsgTime)
    _lagTime = _currentTime - _faultData[priority].lastMsgTime;
  else
    _lagTime = 0;

  // Stop motors due to lag.
  
  if (_lagTime > LAG_TIME_TO_KILL_MOTORS && priority == PRIMARY) {

    if ( ! _buffer->isFootStopped() ) { 
  
      #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
      output = _className+F("_checkLag() - ");
      output += F(" -  Stopping foot motors due to lag time.");
      Serial.println(output);
      #endif
  
      _buffer->stopFootMotor();
      _resetCriticalFault(controller, priority);
      fault = true;
    }
  
    if ( ! _buffer->isDomeStopped() && ! _buffer->isSecondaryConnected() ) {
  
      #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
      output = _className+F("_checkLag() - ");
      output += F(" -  Stopping dome motor due to lag time.");
      Serial.println(output);
      #endif
  
      _buffer->stopDomeMotor();
      _resetCriticalFault(controller, priority);
      fault = true;
    }
  }

  // Disconnect controller due to lag.

  if (_lagTime > LAG_TIME_TO_DISCONNECT) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_checkLag() - ");
    output += _faultData[priority].priority;
    output += F("\r\n  Disconnecting due to lag.");
    output += F("\r\n  Current time:  ");
    output += _currentTime;
    output += F("\r\n  Last msg time: ");
    output += _faultData[priority].lastMsgTime;
    output += F("\r\n  Lag:           ");
    output += _lagTime;    
    Serial.println(output);
    #endif

    _disconnectController(controller, priority);
    fault = true;
  }

  return fault;
}

// ===========================
//      _checkReconnect()
// ===========================
void Controller_PS3Nav::_checkReconnect(PS3BT * controller, uint8_t priority)
{
  _faultData[priority].lastMsgTime = (unsigned long)controller->getLastMessageTime();

  if (_faultData[priority].waitingToReconnect) {

    _lagTime = (_currentTime - _faultData[priority].lastMsgTime);

    if (_lagTime < LAG_TIME_TO_RECONNECT) {

      _faultData[priority].waitingToReconnect = false;

      #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
      output = _className+F("_checkReconnect()");
      output += F(" - ");
      output += _faultData[priority].priority;
      output += F("  reconnected.");
      Serial.println(output);
      #endif
    }

    _faultData[priority].lastMsgTime = _currentTime;
  }
}

// ========================
//      _checkSignal()
// ========================
bool Controller_PS3Nav::_checkSignal(PS3BT * controller, uint8_t priority)
{
  bool fault = false;

  if ( controller->getStatus(Plugged) == controller->getStatus(Unplugged) ) {

    // We want to check this status twice before deciding it truly is confused, but we need a
    // little time to pass between checks. Original SHADOW code used delay() and repeated code
    // to accomplish the two attempts. I am using a state machine (a flag and a timer) to avoid
    // the full blown pause caused by delay().

    if (_faultData[priority].pluggedState) {

      // When we get inside this condition, we're into our second attempt, but we still
      // need to give it a little time, an interval. Vary the timer's interval based on
      // whether or not there is another controller present.

      unsigned long interval = PLUGGED_STATE_INTERVAL_ALONE;
      if ( (priority == PRIMARY && _buffer->isSecondaryConnected()) || 
           (priority == SECONDARY && _buffer->isPrimaryConnected()) ) {
        interval = PLUGGED_STATE_INTERVAL_OTHER;
      }

      if (_currentTime > _faultData[priority].pluggedStateTime + interval) {

        // When we get this far, then we're still seeing a confused status, and we've
        // passed the prescribed time interval. Increment our bad data counter, and
        // reset the state machine.

        #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
        output = _className+F("_checkSignal()");
        output += F(" - ");
        output += _faultData[priority].priority;
        output += F("\r\n  Invalid data from controller.");
        Serial.println(output);
        #endif

        _faultData[priority].badData++;
        _faultData[priority].pluggedState = false;
        fault = true;
      }
    }

    // Here, we activate our state machine and start the timer.

    _faultData[priority].pluggedState = true;
    _faultData[priority].pluggedStateTime = millis();
    
  } else {

    // At this point, there is no confusion over the status of plugged vs unplugged.
    // Reset the state machine if it had been activated.

    if (_faultData[priority].pluggedState) {
      _faultData[priority].pluggedState = false;
      _faultData[priority].badData = 0;
    }
  }

  if (_faultData[priority].badData > 10) {

    // We have too much bad data coming from the controller.
    // Shut it down.

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_checkSignal()");
    output += F(" - ");
    output += _faultData[priority].priority;
    output += F("\r\n  Disconnecting due to excessive bad data.");
    Serial.println(output);
    #endif

    _disconnectController(controller, priority);
    fault = true;
  }

  return fault;
}

// ===============================
//      _resetCriticalFault()
// ===============================
void Controller_PS3Nav::_resetCriticalFault(PS3BT * controller, uint8_t priority)
{
  _faultData[priority].badData = 0;
  if (controller->getLastMessageTime() == 0)
    _faultData[priority].lastMsgTime = -1;
  else if ( ! controller->PS3NavigationConnected )
    _faultData[priority].lastMsgTime = -1;
  else
    _faultData[priority].lastMsgTime = controller->getLastMessageTime();
  _faultData[priority].waitingToReconnect = true;

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_resetCriticalFault()");
  output += F(" - ");
  output += _faultData[priority].priority;
  output += F(" critical fault data reset.");
  Serial.println(output);
  #endif
}


/* ===================================
 *          Buffer functions
 * =================================== */

// ======================
//      _setBuffer()
// ======================
void Controller_PS3Nav::_setBuffer(void)
{
  // Save the previous joystick values for critical fault detection.

  _buffer->saveStick();

  // Get button states for the primary controller.

  _buffer->setButton(UP,      _primary.getButtonClick(UP));
  _buffer->setButton(RIGHT,   _primary.getButtonClick(RIGHT));
  _buffer->setButton(DOWN,    _primary.getButtonClick(DOWN));
  _buffer->setButton(LEFT,    _primary.getButtonClick(LEFT));
  _buffer->setButton(SELECT,  _primary.getButtonPress(CROSS));
  _buffer->setButton(START,   _primary.getButtonPress(CIRCLE));
  _buffer->setButton(L3,      _primary.getButtonClick(L3));
  _buffer->setButton(L2,      _primary.getAnalogButton(L2));
  _buffer->setButton(L1,      _primary.getButtonPress(L1));
  _buffer->setButton(PS,      _primary.getButtonPress(PS));
  _buffer->setStick(LeftHatX, _primary.getAnalogHat(LeftHatX));
  _buffer->setStick(LeftHatY, _primary.getAnalogHat(LeftHatY));

  // Get the button states for the secondary controller.
  // We can store some of these in the unused buttons from the primary controller.

  if (_buffer->isSecondaryConnected()) {
    _buffer->setButton(TRIANGLE, _secondary.getButtonClick(UP));
    _buffer->setButton(CIRCLE,   _secondary.getButtonClick(RIGHT));
    _buffer->setButton(CROSS,    _secondary.getButtonClick(DOWN));
    _buffer->setButton(SQUARE,   _secondary.getButtonClick(LEFT));
    _buffer->setButton(SELECT,  (_primary.getButtonPress(CROSS) + _secondary.getButtonPress(CROSS)) > 0 ? 1 : 0);
    _buffer->setButton(START,   (_primary.getButtonPress(CIRCLE) + _secondary.getButtonPress(CIRCLE)) > 0 ? 1 : 0);
    _buffer->setButton(R3,       _secondary.getButtonClick(L3));
    _buffer->setButton(L2,      (_primary.getAnalogButton(L2) + _secondary.getAnalogButton(L2)) > 0 ? max(_primary.getAnalogButton(L2), _secondary.getAnalogButton(L2)) : 0);
    _buffer->setButton(L1,      (_primary.getButtonPress(L1) + _secondary.getButtonPress(L1)) > 0 ? 1 : 0);
    _buffer->setButton(PS2,      _secondary.getButtonPress(PS));
    _buffer->setStick(RightHatX, _secondary.getAnalogHat(LeftHatX));
    _buffer->setStick(RightHatY, _secondary.getAnalogHat(LeftHatY));
  }

  // Check for and handle any crazyIvan event.

  if (_primary.PS3NavigationConnected)
    _crazyIvan(PRIMARY, LeftHatX, LeftHatY);
  if (_secondary.PS3NavigationConnected)
    _crazyIvan(SECONDARY, RightHatX, RightHatY);

  #ifdef TEST_CONTROLLER

  // Using only one of these at a time is strongly recommended.
 
  _buffer->testInput();
  //_buffer->scrollInput();

  #endif
}


// ======================
//      _crazyIvan()
// ======================
void Controller_PS3Nav::_crazyIvan(uint8_t priority, uint8_t x, uint8_t y)
{
/* -----------------------------------------------------------------------------------
 * My PS3 Move Navigaton occasionally sends stick X,Y values 0,0 for no known reason.
 * I am calling this a "crazy Ivan" because why not?  I don't want this to be
 * processed, so when it occurs, I reset the X,Y values to their previous values.
 * 
 * This is called from within the _setBuffer() function of the controller.
 * ----------------------------------------------------------------------------------- */

  if (_buffer->getStick(x) == 0 && _buffer->getPrevStick(x) > CRAZYIVAN_THRESHOLD && \
      _buffer->getStick(y) == 0 && _buffer->getPrevStick(y) > CRAZYIVAN_THRESHOLD) {
    _buffer->restoreStick(x);
    _buffer->restoreStick(y);

    #ifdef DEBUG_CRAZYIVAN
    output = _className+F("_crazyIvan()");
    if (priority == PRIMARY)
      output += F(" - Primary");
    else if (priority == SECONDARY)
      output += F(" - Secondary");
    else
      output += F(" - Unknown");
    output += F(" stick");
    Serial.println(output);
    #endif
  }
}
