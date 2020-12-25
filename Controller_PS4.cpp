/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS4.cpp - Library for the subclass for the PS4 controller
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Controllers.h"

#if defined(PS4_CONTROLLER)

/* ============================================
 *           Authorized MAC Addresses
 * ============================================ */
char * PS4_MAC_ADDRESSES[NUMBER_OF_MAC_ADDRESSES] = {
  "4C:B9:9B:21:63:3E"
};


/* ===========================================================================
 *                     Controller_PS4 subclass functions
 * =========================================================================== */

// =====================
//      Constructor
// =====================

// When pairing a PS4 controller for the first time, uncomment the line with "PAIR" as a parameter,
// and comment out the line without "PAIR" as a parameter. Upload the sketch and connect the controller.
// After pairing, switch the lines so that "PAIR" is not used. Upload the sketch again.

//Controller_PS4::Controller_PS4(Buffer * pBuffer) : Controller_Parent(pBuffer), _controller(&_Btd, PAIR)
Controller_PS4::Controller_PS4(Buffer * pBuffer) : Controller_Parent(pBuffer), _controller(&_Btd)
{
  _buffer->buttonLabel[0] = "Up";
  _buffer->buttonLabel[1] = "Right";
  _buffer->buttonLabel[2] = "Down";
  _buffer->buttonLabel[3] = "Left";
  _buffer->buttonLabel[4] = "Share";
  _buffer->buttonLabel[5] = "Options";
  _buffer->buttonLabel[6] = "L3";
  _buffer->buttonLabel[7] = "R3";
  _buffer->buttonLabel[8] = "L2";
  _buffer->buttonLabel[9] = "R2";
  _buffer->buttonLabel[10] = "L1";
  _buffer->buttonLabel[11] = "R1";
  _buffer->buttonLabel[12] = "Up";
  _buffer->buttonLabel[13] = "Right";
  _buffer->buttonLabel[14] = "Down";
  _buffer->buttonLabel[15] = "Left";
  _buffer->buttonLabel[16] = "PS";
  _buffer->buttonLabel[17] = "";
  _buffer->buttonLabel[18] = "Touchpad";
  _buffer->buttonLabel[19] = "";

  // ----------------------------
  // Initialize the rumble timer.
  // ----------------------------

  _rumbleStartTime = 0;

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  _className = F("Controller_PS4::");
  #endif
}

// =================
//      begin()
// =================
void Controller_PS4::begin(void)
{
  // -------------------
  // Start the USB Host.
  // -------------------

  Controller_Parent::begin();
  
  // ---------------------------
  // Setup for the attachOnInit.
  // ---------------------------

  anchor = this;
  _controller.attachOnInit(_onInit);

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("begin()");
  output += F(" - PS4 controller started.");
  printOutput();
  #endif
}

// ================
//      read()
// ================
bool Controller_PS4::read()
{
  // --------------------------------------------------------------------
  // The main program will handle stopping the motors if this read fails.
  //
  // Reasons this read may fail:
  // ---------------------------
  // 1. The controller is not connected.
  // 2. The controller throws a critical fault.
  // 4. The user disconnects the primary controller.
  // --------------------------------------------------------------------

  // ------------------------------
  // Get input from the controller.
  // ------------------------------

  _Usb.Task();
  if ( ! _controller.connected() ) {
    if ( ! _faultData.reconnect )
      _faultData.reconnect = true;
    return false;
  } else {

    // ---------------------------
    // Check for a critical fault.
    // ---------------------------

    _lastReadTime = millis();         // This is intended to be a replacement for getLastMessageTime() found in PS3BT.
    if ( _detectCriticalFault() ) {
      printOutput();
      return false;
    }
  }

  // -----------------------------------------------
  // If we get this far then our controller is good.
  // -----------------------------------------------

  // ------------------------------
  // Take a snapshot of the inputs.
  // ------------------------------

  _updateBuffer();

  // ----------------------------------------------
  // Respond to any user-issued disconnect (L2+PS).
  // ----------------------------------------------

  if ( _buffer->getButton(PS) && ( _buffer->getButton(R2) || _buffer->getButton(L2))) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("read()");
    output += F(" - Disconnecting due to user request");
    printOutput();
    #endif

    _disconnect(&_controller);
    return false;
  }

  // -------------
  // Read is done.
  // -------------

  if ( _buffer->isLedUpdateRequested() ) {
    _setLed();
  }

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
static void Controller_PS4::_onInit(void)
{
  if (anchor != NULL) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = F("Controller_PS4::_onInit() - Controller initialization started.");
    printOutput();
    #endif

    anchor->_onInitConnect();
  }
}

// ========================
//      _onInitConnect
// ========================
void Controller_PS4::_onInitConnect(void)
{
  // ---------------------------------------------------------------
  // Always try to assign as primary first.
  // Assign as secondary only when the primary is already connected.
  // ---------------------------------------------------------------

  if ( ! _buffer->isControllerConnected() ) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_onInitConnect()");
    output += F(" - Initiating connection with controller.");
    printOutput();
    #endif

    _connect(&_controller);
  }
}


/* =======================================
 *          Connection functions
 * ======================================= */
 
// ==================
//      _connect
// ==================
void Controller_PS4::_connect(PS4BT * pController)
{
  // ------------------------------------------------------------------
  // First, let's make sure we have a PS3 controller.  If not, drop it.
  // ------------------------------------------------------------------

  if ( ! _controller.connected() ) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_connect()");
    output += F(" - Disconnecting");
    output += F(" invalid");
    output += F(" controller.");
    printOutput();
    #endif

    _controller.setLedOff();
    _controller.disconnect();
    return;
  }

  // --------------------------------------------------
  // Get the MAC address of the controller and validate
  // against our list of authorized devices.
  // --------------------------------------------------

  if ( ! _authorized(PS4_MAC_ADDRESSES, sizeof(PS4_MAC_ADDRESSES)) ) {

    // --------------------------------------------
    // This is an unauthorized controller. Dump it.
    // --------------------------------------------

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_connect()");
    output += F(" - Disconnecting");
    output += F(" unauthorized");
    output += F(" controller.");
    printOutput();
    #endif

    _controller.setLedOff();
    _controller.disconnect();
    return;
  }

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  else
    output = _className+F("_connect()");
    output += F(" - Controller authorized.");
    printOutput();
  #endif

  // --------------------
  // Complete connection.
  // --------------------

  _buffer->setControllerConnected(FULL);
  _initCriticalFault();
  _buffer->setSpeedProfile(Walk);
  _buffer->requestLedUpdate(true);

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_connect()");
  output += F(" - Controller connected.");
  printOutput();
  #endif
}

// =======================
//      _disconnect()
// =======================
void Controller_PS4::_disconnect(PS4BT * pController)
{
  _buffer->setControllerConnected(NONE);
  _resetCriticalFault();

  _controller.setLedOff();
  _controller.disconnect();

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_disconnect()");
  output += F(" - Controller disconnected");  
  printOutput();
  #endif
}

// ===================
//      _setLed()
// ===================
void Controller_PS4::_setLed(void)
{
  _buffer->requestLedUpdate(false);

  if ( ! _buffer->isDriveEnabled() ) {
    _controller.setLed(Red);
  } else {
    switch ( _buffer->getSpeedProfile() ) {
      case 1 : {
        _controller.setLed(Yellow);
        break;
      }
      case 2 : {
        _controller.setLed(Green);
        break;
      }
      case 3 : {
        _controller.setLed(Blue);
        break;
      }
      case 4 : {
        _controller.setLed(Purple);
        break;
      }
    }
  }
}

/* =====================================================
 *          Critical fault detection functions
 * ===================================================== */

// ==============================
//      _initCriticalFault()
// ==============================
void Controller_PS4::_initCriticalFault(void)
{
  _faultData.badData          = 0;
  _faultData.lastMsgTime      = millis();
  _faultData.pluggedStateTime = 0;
  _faultData.reconnect        = true;

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_initCriticalFault()");
  output += F(" - Critical fault data initialized.");
  printOutput();
  #endif
}

// ================================
//      _detectCriticalFault()
// ================================
bool Controller_PS4::_detectCriticalFault(void)
{
  if ( _controller.connected() ) {
    _currentTime = millis();
    _lagTime = 0;
    _faultData.lastMsgTime = _lastReadTime;

    // -----------------------
    // Check for reconnection.
    // -----------------------

    if ( _faultData.reconnect ) {
      if ( _lagTime < LAG_TIME_TO_RECONNECT ) {
        _faultData.reconnect = false;
      }
      _faultData.lastMsgTime = _currentTime;
    }

    // ----------
    // Check lag.
    // ----------

    if ( _currentTime >= _faultData.lastMsgTime ) {
      _lagTime = _currentTime - _faultData.lastMsgTime;
    } else {
      _lagTime = 0;
    }

    if ( _lagTime > LAG_TIME_TO_DISCONNECT ) {

      #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
      output = _className+F("_detectCriticalFault()");
      output += F(" - Disconnecting due to lag time.");
      output += F("\r\n  Current time:  ");  output += _currentTime;
      output += F("\r\n  Last msg time: ");  output += _faultData.lastMsgTime;
      output += F("\r\n  Lag:           ");  output += _lagTime;    
      #endif
      
      _disconnect(&_controller);
    }

    if ( _lagTime > LAG_TIME_TO_KILL_MOTORS && ! _buffer->isDriveStopped() ) {

      #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
      output = _className+F("_detectCriticalFault()");
      output += F(" - Stopping drive motors due to lag.");
      printOutput();
      #endif      

      return true;  // The actual code to stop the motor is in loop() when read() fails.
    }

    // --------------------------
    // Check the PS3 signal data.
    // --------------------------

    if ( _controller.getUsbStatus() && ! _controller.getUsbStatus() ) {

      // The controller's signal is confused. We'll give it two tries
      // to clear up. The first time through starts the state machine.
      // The second time through trips the critical fault.

      if (_faultData.pluggedStateTime > 0) {

        // ------------------------------------------------------------
        // Has the desired amount of time between failed checks passed?
        // ------------------------------------------------------------

        unsigned long interval = (_buffer->isFullControllerConnected() ? PLUGGED_STATE_LONG_INTERVAL : PLUGGED_STATE_SHORT_INTERVAL);
        if ( _currentTime > ( _faultData.pluggedStateTime + interval )) {

          // -------------------------------------------------------
          // We have our second failed check.
          // Trigger the critical fault and reset the state machine.
          // -------------------------------------------------------

          _faultData.badData++;
          _faultData.pluggedStateTime = 0;

          #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
          output = _className+F("_detectCriticalFault()");
          output += F("\r\n - Invalid data from primary controller.");
          #endif

          return true;
        }
      } else {

        // ------------------------
        // Start the state machine.
        // ------------------------

        _faultData.pluggedStateTime = millis();
      }

    } else {

      // --------------------------------------------------------
      // The controller is not confused. Reset the state machine.
      // --------------------------------------------------------

      if (_faultData.badData > 0) {
        _faultData.pluggedStateTime = 0;
        _faultData.badData = 0;
      }
    }
  
    if (_faultData.badData > 10) {

      // ----------------------------------
      // We have too much bad data coming
      // from the controller. Shut it down.
      // ----------------------------------

      #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
      output = _className+F("_detectCriticalFault()");
      output += F("\r\n - Disconnecting due to excessive bad data.");
      #endif

      _disconnect(&_controller);
    }
  }

  return false;
}

// ===============================
//      _resetCriticalFault()
// ===============================
void Controller_PS4::_resetCriticalFault(void)
{
  _faultData.badData = 0;
  _faultData.reconnect = true;
  _faultData.pluggedStateTime = 0;

  if (_lastReadTime == 0 || ! _controller.connected() )
    _faultData.lastMsgTime = -1;
  else
    _faultData.lastMsgTime = _lastReadTime;

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_resetCriticalFault()");
  output += F(" - Critical fault data reset.");
  printOutput();
  #endif
}

/* ===================================
 *          Buffer functions
 * =================================== */

void Controller_PS4::_setBuffer(void)
{
  // ---------------------------------------------------------------
  // Save the previous joystick values for critical fault detection.
  // ---------------------------------------------------------------

  _buffer->saveStick();

  // ---------------------------------------------
  // Get button states for the primary controller.
  // ---------------------------------------------

  _buffer->setButton(UP,       _controller.getButtonClick(UP));
  _buffer->setButton(RIGHT,    _controller.getButtonClick(RIGHT));
  _buffer->setButton(DOWN,     _controller.getButtonClick(DOWN));
  _buffer->setButton(LEFT,     _controller.getButtonClick(LEFT));
  _buffer->setButton(SELECT,   _controller.getButtonPress(SHARE));
  _buffer->setButton(START,    _controller.getButtonPress(OPTIONS));
  _buffer->setButton(L3,       _controller.getButtonClick(L3));
  _buffer->setButton(R3,       _controller.getButtonClick(R3));
  _buffer->setButton(L2,       _controller.getAnalogButton(L2));
  _buffer->setButton(R2,       _controller.getAnalogButton(R2));
  _buffer->setButton(L1,       _controller.getButtonPress(L1));
  _buffer->setButton(R1,       _controller.getButtonPress(R1));
  _buffer->setButton(PS,       _controller.getButtonPress(PS));
  _buffer->setStick(LeftHatX,  _controller.getAnalogHat(LeftHatX));
  _buffer->setStick(LeftHatY,  _controller.getAnalogHat(LeftHatY));
  _buffer->setButton(TRIANGLE, _controller.getButtonClick(TRIANGLE));
  _buffer->setButton(CIRCLE,   _controller.getButtonClick(CIRCLE));
  _buffer->setButton(CROSS,    _controller.getButtonClick(CROSS));
  _buffer->setButton(SQUARE,   _controller.getButtonClick(SQUARE));
  _buffer->setStick(RightHatX, _controller.getAnalogHat(RightHatX));
  _buffer->setStick(RightHatY, _controller.getAnalogHat(RightHatY));

  #ifdef TEST_CONTROLLER

  // ----------------------------------------------------------
  // Using only one of these at a time is strongly recommended.
  // ----------------------------------------------------------
 
  _buffer->displayInput();
  //_buffer->scrollInput();
  #endif
}

// =========================
//      _updateBuffer()
// =========================
void Controller_PS4::_updateBuffer(void)
{
  // ---------------------------------------------------------------
  // Save the previous joystick values for critical fault detection.
  // ---------------------------------------------------------------

  _buffer->saveStick();

  // ---------------------------------------------
  // Get button states for the primary controller.
  // ---------------------------------------------

  _buffer->updateButton(UP,       _controller.getButtonClick(UP));
  _buffer->updateButton(RIGHT,    _controller.getButtonClick(RIGHT));
  _buffer->updateButton(DOWN,     _controller.getButtonClick(DOWN));
  _buffer->updateButton(LEFT,     _controller.getButtonClick(LEFT));
  _buffer->updateButton(SELECT,   _controller.getButtonPress(SHARE));
  _buffer->updateButton(START,    _controller.getButtonPress(OPTIONS));
  _buffer->updateButton(L3,       _controller.getButtonClick(L3));
  _buffer->updateButton(R3,       _controller.getButtonClick(R3));
  _buffer->updateButton(L2,       _controller.getAnalogButton(L2));
  _buffer->updateButton(R2,       _controller.getAnalogButton(R2));
  _buffer->updateButton(L1,       _controller.getButtonPress(L1));
  _buffer->updateButton(R1,       _controller.getButtonPress(R1));
  _buffer->updateButton(PS,       _controller.getButtonPress(PS));
  _buffer->setStick(LeftHatX,     _controller.getAnalogHat(LeftHatX));
  _buffer->setStick(LeftHatY,     _controller.getAnalogHat(LeftHatY));
  _buffer->updateButton(TRIANGLE, _controller.getButtonClick(TRIANGLE));
  _buffer->updateButton(CIRCLE,   _controller.getButtonClick(CIRCLE));
  _buffer->updateButton(CROSS,    _controller.getButtonClick(CROSS));
  _buffer->updateButton(SQUARE,   _controller.getButtonClick(SQUARE));
  _buffer->setStick(RightHatX,    _controller.getAnalogHat(RightHatX));
  _buffer->setStick(RightHatY,    _controller.getAnalogHat(RightHatY));

  #ifdef TEST_CONTROLLER

  // ----------------------------------------------------------
  // Using only one of these at a time is strongly recommended.
  // ----------------------------------------------------------
 
  _buffer->displayInput();
  //_buffer->scrollInput();
  #endif
}

#endif    // defined(PS4_CONTROLLER)
