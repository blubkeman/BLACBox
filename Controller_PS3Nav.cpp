/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS3Nav.cpp - Library for the subclass for the PS3 Move Navigation controller
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Controllers.h"

#if defined(PS3_NAVIGATION)

/* ============================================
 *           Authorized MAC Addresses
 * ============================================ */
char * PS3NAV_MAC_ADDRESSES[NUMBER_OF_MAC_ADDRESSES] = {
  "xx:xx:xx:xx:xx:xx"
, "xx:xx:xx:xx:xx:xx"
};


/* ===========================================================================
 *                     Controller_PS3Nav subclass functions
 * =========================================================================== */

// =====================
//      Constructor
// =====================
Controller_PS3Nav::Controller_PS3Nav(Buffer * pBuffer) : Controller_Parent(pBuffer), _controller(&_Btd), _secondController(&_Btd)
{
  _buffer->buttonLabel[0] = "Up(Nav1)";
  _buffer->buttonLabel[1] = "Right(Nav1)";
  _buffer->buttonLabel[2] = "Down(Nav1)";
  _buffer->buttonLabel[3] = "Left(Nav1)";
  _buffer->buttonLabel[4] = "Cross";
  _buffer->buttonLabel[5] = "Circle";
  _buffer->buttonLabel[6] = "L3";
  _buffer->buttonLabel[7] = "R3";
  _buffer->buttonLabel[8] = "L2";
  _buffer->buttonLabel[9] = "R2";
  _buffer->buttonLabel[10] = "L1";
  _buffer->buttonLabel[11] = "R1";
  _buffer->buttonLabel[12] = "Up(Nav2)";
  _buffer->buttonLabel[13] = "Right(Nav2)";
  _buffer->buttonLabel[14] = "Down(Nav2)";
  _buffer->buttonLabel[15] = "Left(Nav2)";
  _buffer->buttonLabel[16] = "PS(Nav1)";
  _buffer->buttonLabel[17] = "";
  _buffer->buttonLabel[18] = "";
  _buffer->buttonLabel[19] = "PS(Nav2)";

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  _className = F("Controller_PS3Nav::");
  #endif
}

// =================
//      begin()
// =================
void Controller_PS3Nav::begin(void)
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
  _secondController.attachOnInit(_onInit);

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("begin()");
  output += F(" - PS3 Navigation controller started.");
  printOutput();
  #endif
}

// ================
//      read()
// ================
bool Controller_PS3Nav::read()
{

  // --------------------------------------------------------------------
  // The main program will handle stopping the motors if this read fails.
  //
  // Reasons this read may fail:
  // ---------------------------
  // 1. The primary controller is not connected.
  // 2. The primary controller throws a critical fault.
  // 3. The secondary controller throws a critical fault.
  // 4. The user disconnects the primary controller.
  // --------------------------------------------------------------------

  // ---------------------------------
  // Get input from the controller(s).
  // ---------------------------------

  _Usb.Task();
  if ( ! _controller.PS3NavigationConnected ) {

    // When using the deadman switch, if it was active when we lost the
    // controller, make certain the deadman switch is turned off.

    #if defined(DEADMAN)
    if ( digitalRead(DEADMAN_PIN) == HIGH )
      digitalWrite(DEADMAN_PIN, LOW);
    #endif

    if ( ! _faultData.reconnect )
      _faultData.reconnect = true;
    return false;
  } else {
    if ( _detectCriticalFault(&_faultData) ) {
      printOutput();
      return false;
    }
  }

  _Usb.Task();
  if ( ! _secondController.PS3NavigationConnected ) {
    if ( ! _secondFaultData.reconnect )
      _secondFaultData.reconnect = true;
  } else {
    if ( _detectCriticalFault(&_secondFaultData) ) {
      printOutput();
      return false;
    }
  }

  // --------------------------------------------------
  // If we get this far then ours controllers are good.
  // --------------------------------------------------

  // ------------------------------
  // Take a snapshot of the inputs.
  // ------------------------------

  _updateBuffer();

  // ----------------------------------------------
  // Respond to any user-issued disconnect (L2+PS).
  // ----------------------------------------------

  // -------------------------------------
  // Check the secondary controller first.
  // -------------------------------------

  if ( _buffer->getButton(PS2) && ( _buffer->getButton(R2) || _buffer->getButton(L2))) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("read()");
    output += F(" - Disconnecting due to user request");
    printOutput();
    #endif

    _disconnect(&_secondController);
  }

  // ---------------------------------
  // Now check the primary controller.
  // ---------------------------------

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
    printOutput();
    #endif

    anchor->_onInitConnect();
  }
}

// ========================
//      _onInitConnect
// ========================
void Controller_PS3Nav::_onInitConnect(void)
{
  // ---------------------------------------------------------------
  // Always try to assign as primary first.
  // Assign as secondary only when the primary is already connected.
  // ---------------------------------------------------------------

  if ( ! _buffer->isControllerConnected() ) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_onInitConnect()");
    output += F(" - Initiating connection with ");
    output += F("primary");
    output += F(" controller.");
    printOutput();
    #endif

    _connect(&_controller);

  } else if ( ! _buffer->isFullControllerConnected() ) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_onInitConnect()");
    output += F(" - Initiating connection with ");
    output += F("secondary");
    output += F(" controller.");
    printOutput();
    #endif

    _connect(&_secondController);
  }

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  printOutput();
  #endif
}


/* =======================================
 *          Connection functions
 * ======================================= */
 
// ==================
//      _connect
// ==================
void Controller_PS3Nav::_connect(PS3BT * pController)
{
  // ----------------------------------------------------------------------------------
  // First, let's make sure we have a PS3 Move Navigation controller.  If not, drop it.
  // ----------------------------------------------------------------------------------

  if ( ! pController->PS3NavigationConnected ) {

    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    output = _className+F("_connect()");
    output += F(" - Disconnecting");
    output += F(" invalid");
    output += F(" controller.");
    printOutput();
    #endif

    pController->disconnect();
    return;
  }

  // --------------------------------------------------
  // Get the MAC address of the controller and validate
  // against our list of authorized devices.
  // --------------------------------------------------

  if ( ! _authorized(PS3NAV_MAC_ADDRESSES, sizeof(PS3NAV_MAC_ADDRESSES)) ) {

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

    pController->disconnect();
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

  _buffer->setControllerConnected(HALF);
  _initCriticalFault(&_faultData);
  _buffer->setSpeedProfile(Walk);
  pController->setLedOn(LED1);

  // ---------------------------------------
  // Display which controller was connected.
  // ---------------------------------------

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_connect()");
  output += F(" - Controller connected.");
  printOutput();
  #endif
}

// =======================
//      _disconnect()
// =======================
void Controller_PS3Nav::_disconnect(PS3BT * pController)
{
  uint8_t priority = FIRST;
  if (pController == &_secondController)
    priority = SECOND;

  if ( priority == FIRST && _buffer->isFullControllerConnected() )
    _disconnect(&_secondController);

  if ( priority == FIRST ) {
    _buffer->setControllerConnected(NONE);
    _resetCriticalFault(&_faultData);
  } else if ( priority == SECOND ) {
    _buffer->setControllerConnected(HALF);
    _resetCriticalFault(&_secondFaultData);
  }

  pController->setLedOff(LED1);
  pController->disconnect();

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_disconnect()");
  output += F(" - ");
  if ( priority == FIRST )
    output += F("Primary");
  else
    output += F("Secondary");
  output += F(" controller disconnected");  
  printOutput();
  #endif
}

/* =====================================================
 *          Critical fault detection functions
 * ===================================================== */

// ==============================
//      _initCriticalFault()
// ==============================
void Controller_PS3Nav::_initCriticalFault(CriticalFault_Struct * pFaultData)
{
  pFaultData->badData          = 0;
  pFaultData->lastMsgTime      = millis();
  pFaultData->pluggedStateTime = 0;
  pFaultData->reconnect        = true;

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_initCriticalFault()");
  output += F(" - Critical fault data initialized.");
  printOutput();
  #endif
}

// ================================
//      _detectCriticalFault()
// ================================
bool Controller_PS3Nav::_detectCriticalFault(CriticalFault_Struct * pFaultData)
{
  uint8_t priority = FIRST;
  if (pFaultData == &_secondFaultData)
    priority = SECOND;

  if ( _controller.PS3NavigationConnected ) {
    _currentTime = millis();
    _lagTime = 0;
    pFaultData->lastMsgTime = _controller.getLastMessageTime();

    // -----------------------
    // Check for reconnection.
    // -----------------------

    if ( pFaultData->reconnect ) {
      if ( _lagTime < LAG_TIME_TO_RECONNECT ) {
        pFaultData->reconnect = false;
      }
      pFaultData->lastMsgTime = _currentTime;
    }

    // ----------
    // Check lag.
    // ----------

    if ( _currentTime >= pFaultData->lastMsgTime ) {
      _lagTime = _currentTime - pFaultData->lastMsgTime;
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
      
      if ( priority == FIRST )
        _disconnect(&_controller);
      else
        _disconnect(&_secondController);
      return true;
    }

    if ( priority == FIRST ) {

      // ---------------------------------------------------------------
      // This part is specific to the PS3 Navigation primary controller.
      // ---------------------------------------------------------------
      
      if ( _lagTime > LAG_TIME_TO_KILL_MOTORS && ! _buffer->isDriveStopped() ) {
  
        #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
        output = _className+F("_detectCriticalFault()");
        output += F(" - Stopping drive motors due to lag.");
        printOutput();
        #endif      
  
        return true;  // The actual code to stop the motor is in loop() when read() fails.
      }
    }
  
    // --------------------------
    // Check the PS3 signal data.
    // --------------------------

    if ( ! _controller.getStatus(Plugged) && ! _controller.getStatus(Unplugged) ) {

      // The controller's signal is confused. We'll give it two tries
      // to clear up. The first time through starts the state machine.
      // The second time through trips the critical fault.

      if (pFaultData->pluggedStateTime > 0) {

        // ------------------------------------------------------------
        // Has the desired amount of time between failed checks passed?
        // ------------------------------------------------------------
        
        unsigned long interval = (_buffer->isFullControllerConnected() ? PLUGGED_STATE_LONG_INTERVAL : PLUGGED_STATE_SHORT_INTERVAL);
        if ( _currentTime > ( pFaultData->pluggedStateTime + interval )) {

          // We have our second failed check.
          // Trigger the critical fault and reset the state machine.

          pFaultData->badData++;
          pFaultData->pluggedStateTime = 0;

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

        pFaultData->pluggedStateTime = millis();
      }

    } else {

      // --------------------------------------------------------
      // The controller is not confused. Reset the state machine.
      // --------------------------------------------------------

      if (pFaultData->badData > 0) {
        pFaultData->pluggedStateTime = 0;
        pFaultData->badData = 0;
      }
    }
  
    if (pFaultData->badData > 10) {

      // ----------------------------------
      // We have too much bad data coming
      // from the controller. Shut it down.
      // ----------------------------------

      #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
      output = _className+F("_detectCriticalFault()");
      output += F("\r\n - Disconnecting due to excessive bad data.");
      #endif

      if ( priority == FIRST )
        _disconnect(&_controller);
      else
        _disconnect(&_secondController);
      return true;
    }
  }

  return false;
}

// ===============================
//      _resetCriticalFault()
// ===============================
void Controller_PS3Nav::_resetCriticalFault(CriticalFault_Struct * pFaultData)
{
  uint8_t priority = FIRST;
  PS3BT * pController = &_controller;

  if (pFaultData == &_secondFaultData) {
    priority = SECOND;
    pController = &_secondController;
  }

  pFaultData->badData = 0;
  pFaultData->reconnect = true;
  pFaultData->pluggedStateTime = 0;

  if (pController->getLastMessageTime() == 0 || ! _controller.PS3NavigationConnected )
    pFaultData->lastMsgTime = -1;
  else
    pFaultData->lastMsgTime = pController->getLastMessageTime();

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_resetCriticalFault()");
  output += F(" - ");
  if ( priority == FIRST )
    output += F("Primary");
  else
    output += F("Secondary");
  output += F(" critical fault data reset.");
  printOutput();
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

  // ---------------------------------------------
  // Get button states for the primary controller.
  // ---------------------------------------------

  _buffer->setButton(UP,      _controller.getButtonClick(UP));
  _buffer->setButton(RIGHT,   _controller.getButtonClick(RIGHT));
  _buffer->setButton(DOWN,    _controller.getButtonClick(DOWN));
  _buffer->setButton(LEFT,    _controller.getButtonClick(LEFT));
  _buffer->setButton(SELECT,  _controller.getButtonPress(CROSS));
  _buffer->setButton(START,   _controller.getButtonPress(CIRCLE));
  _buffer->setButton(L3,      _controller.getButtonClick(L3));
  _buffer->setButton(L2,      _controller.getAnalogButton(L2));
  _buffer->setButton(L1,      _controller.getButtonPress(L1));
  _buffer->setButton(PS,      _controller.getButtonPress(PS));
  _buffer->setStick(LeftHatX, _controller.getAnalogHat(LeftHatX));
  _buffer->setStick(LeftHatY, _controller.getAnalogHat(LeftHatY));

  // -----------------------------------------------------------------------------
  // Get the button states for the secondary controller.
  // We can store some of these in the unused buttons from the primary controller.
  // -----------------------------------------------------------------------------

  if (_buffer->isFullControllerConnected()) {
    _buffer->setButton(TRIANGLE, _secondController.getButtonClick(UP));
    _buffer->setButton(CIRCLE,   _secondController.getButtonClick(RIGHT));
    _buffer->setButton(CROSS,    _secondController.getButtonClick(DOWN));
    _buffer->setButton(SQUARE,   _secondController.getButtonClick(LEFT));
    _buffer->setButton(SELECT,  (_controller.getButtonPress(CROSS) + _secondController.getButtonPress(CROSS)) > 0 ? 1 : 0);
    _buffer->setButton(START,   (_controller.getButtonPress(CIRCLE) + _secondController.getButtonPress(CIRCLE)) > 0 ? 1 : 0);
    _buffer->setButton(R3,       _secondController.getButtonClick(L3));
    _buffer->setButton(R2,       _secondController.getAnalogButton(L2));
    _buffer->setButton(R1,       _secondController.getButtonPress(L1));
    _buffer->setButton(PS2,      _secondController.getButtonPress(PS));
    _buffer->setStick(RightHatX, _secondController.getAnalogHat(LeftHatX));
    _buffer->setStick(RightHatY, _secondController.getAnalogHat(LeftHatY));
  }

  // -----------------------------------------
  // Check for and handle any crazyIvan event.
  // -----------------------------------------

  if (_controller.PS3NavigationConnected)
    _crazyIvan(FIRST, LeftHatX, LeftHatY);
  if (_secondController.PS3NavigationConnected)
    _crazyIvan(SECOND, RightHatX, RightHatY);

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
void Controller_PS3Nav::_updateBuffer(void)
{
  // ---------------------------------------------------------------
  // Save the previous joystick values for critical fault detection.
  // ---------------------------------------------------------------

  _buffer->saveStick();

  // ---------------------------------------------
  // Get button states for the primary controller.
  // ---------------------------------------------

  _buffer->updateButton(UP,     _controller.getButtonClick(UP));
  _buffer->updateButton(RIGHT,  _controller.getButtonClick(RIGHT));
  _buffer->updateButton(DOWN,   _controller.getButtonClick(DOWN));
  _buffer->updateButton(LEFT,   _controller.getButtonClick(LEFT));
  _buffer->updateButton(SELECT, _controller.getButtonPress(CROSS));
  _buffer->updateButton(START,  _controller.getButtonPress(CIRCLE));
  _buffer->updateButton(L3,     _controller.getButtonClick(L3));
  _buffer->updateButton(L2,     _controller.getAnalogButton(L2));
  _buffer->updateButton(L1,     _controller.getButtonPress(L1));
  _buffer->updateButton(PS,     _controller.getButtonPress(PS));
  _buffer->setStick(LeftHatX,   _controller.getAnalogHat(LeftHatX));
  _buffer->setStick(LeftHatY,   _controller.getAnalogHat(LeftHatY));

  // -----------------------------------------------------------------------------
  // Get the button states for the secondary controller.
  // We can store some of these in the unused buttons from the primary controller.
  // -----------------------------------------------------------------------------

  if (_buffer->isFullControllerConnected()) {
    _buffer->updateButton(TRIANGLE, _secondController.getButtonClick(UP));
    _buffer->updateButton(CIRCLE,   _secondController.getButtonClick(RIGHT));
    _buffer->updateButton(CROSS,    _secondController.getButtonClick(DOWN));
    _buffer->updateButton(SQUARE,   _secondController.getButtonClick(LEFT));
    _buffer->updateButton(SELECT,  (_controller.getButtonPress(CROSS) + _secondController.getButtonPress(CROSS)) > 0 ? 1 : 0);
    _buffer->updateButton(START,   (_controller.getButtonPress(CIRCLE) + _secondController.getButtonPress(CIRCLE)) > 0 ? 1 : 0);
    _buffer->updateButton(R3,       _secondController.getButtonClick(L3));
    _buffer->updateButton(R2,       _secondController.getAnalogButton(L2));
    _buffer->updateButton(R1,       _secondController.getButtonPress(L1));
    _buffer->updateButton(PS2,      _secondController.getButtonPress(PS));
    _buffer->setStick(RightHatX,    _secondController.getAnalogHat(LeftHatX));
    _buffer->setStick(RightHatY,    _secondController.getAnalogHat(LeftHatY));
  }

  // -----------------------------------------
  // Check for and handle any crazyIvan event.
  // -----------------------------------------

  if (_controller.PS3NavigationConnected)
    _crazyIvan(FIRST, LeftHatX, LeftHatY);
  if (_secondController.PS3NavigationConnected)
    _crazyIvan(SECOND, RightHatX, RightHatY);

  #ifdef TEST_CONTROLLER

  // ----------------------------------------------------------
  // Using only one of these at a time is strongly recommended.
  // ----------------------------------------------------------
 
  _buffer->displayInput();
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
    if (priority == FIRST)
      output += F(" - Primary");
    else if (priority == SECOND)
      output += F(" - Secondary");
    else
      output += F(" - Unknown");
    output += F(" stick");
    printOutput();
    #endif
  }
}

#endif    // defined(PS3_NAVIGATION)
