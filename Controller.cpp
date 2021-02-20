/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller.cpp - Library for supported controllers
 * Created by Brian Lubkeman, 20 February 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "Controller.h"
#include "Security.h"


/* ================================================================================
 *                             Parent Controller Class
 * ================================================================================ */


// =====================
//      Constructor
// =====================
Controller_Parent::Controller_Parent(void) : m_Usb(), m_Btd(&m_Usb)
{
  m_controllerConnected = NONE;
  m_onDisconnectCount = 0;

  #ifdef DEBUG
  m_className = "Controller_Parent::";
  #endif
}

// ====================
//      Destructor
// ====================
Controller_Parent::~Controller_Parent(void) {}

// =================
//      begin()
// =================
void Controller_Parent::begin(void)
{
  // -------------------
  // Start the USB host.
  // -------------------

  if (m_Usb.Init() == -1) {

    #ifdef DEBUG
    output = m_className+F("begin()");
    output += F(" - ");
    output += F("OSC did not start");
    printOutput();
    #endif

    while (1); //halt
  }

  #ifdef DEBUG
  output = m_className+F("begin()");
  output += F(" - ");
  output += F("Bluetooth Library Started");
  printOutput();
  #endif
}

// ===================
//      m_connect
// ===================
void Controller_Parent::m_connect(void)
{
  // ------------------------------------------------------------------
  // First, let's make sure we have a PS4 controller.  If not, drop it.
  // ------------------------------------------------------------------

  if ( ! m_connected() ) {
    #ifdef DEBUG
    output = m_className+F("m_connect()");
    output += F(" - ");
    output += F("Controller");
    output += F(" invalid");
    printOutput();
    #endif
    m_disconnect();
    return;
  }

  // --------------------------------------------------------------------------------
  // Somehow, the USB Host Shield calls onInit twice per connection for the PS4 only.
  // The following use of connectionStatus() helps us process this content only once.
  // --------------------------------------------------------------------------------

  // --------------------------------------------------
  // Validate the MAC address of the controller against
  // our list of authorized devices.
  // --------------------------------------------------

  #if defined(PS4_CONTROLLER)
  if ( connectionStatus() > NONE ) {
  #endif

    if ( ! m_authorized() ) {
      m_disconnect();
      return;
    }

  #if defined(PS4_CONTROLLER)
  }
  #endif

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
  #if defined(PS4_CONTROLLER)
  if ( connectionStatus() > NONE ) {
  #endif
    output = m_className+F("m_connect()");
    output += F(" - ");
    output += F("Controller");
    output += F(" connected");
    printOutput();
  #if defined(PS4_CONTROLLER)
  }
  #endif
  #endif

  // --------------------
  // Complete connection.
  // --------------------

  setLed();
  m_setControllerStatus(FULL);
}

// ========================
//      m_authorized()
// ========================
bool Controller_Parent::m_authorized(void)
{
  bool authorized = false;

  String btAddress = "";
  for(int8_t i = 5; i >= 0; i--) {
    if (btAddress.length() > 0) btAddress +=(":");
    if (m_Btd.disc_bdaddr[i]<0x10) btAddress +="0";
    btAddress += String(m_Btd.disc_bdaddr[i], HEX);
  }
  btAddress.toUpperCase();

  #ifdef DEBUG
  output = F("\n");
  output += m_className+F("m_authorized()");
  output += F(" - ");
  output += F("MAC address: ");
  output += btAddress;
  printOutput();
  #endif

  for (byte i = 0; i < NUMBER_OF_MAC_ADDRESSES; i++) {
    if (btAddress == getPgmString(authorizedMACAddresses[i])) {
      authorized = true;
      break;
    }
  }

  #ifdef DEBUG
  output = m_className+F("m_authorized()");
  output += F(" - ");
  output += F("Controller");
  if ( authorized ) {
    output += F(" authorized");
  } else {
    output += F(" unauthorized");
  }
  printOutput();
  #endif

  return authorized;
}

// =================================
//      m_setControllerStatus()
// =================================
void Controller_Parent::m_setControllerStatus(byte n)
{
  m_controllerConnected = n;

  // -----------------------------------------------------------------------
  // When we lose the controller, start the onDisconnect counter.
  // In loop(), each peripheral will be given a chance to act on this event.
  // -----------------------------------------------------------------------
  
  if ( n != NONE ) {
    m_onDisconnectCount = 1;
  }
}

// ===============================
//      m_initCriticalFault()
// ===============================
void Controller_Parent::m_initCriticalFault(byte idx)
{
  m_faultData[idx].badData          = 0;
  m_faultData[idx].lastMsgTime      = millis();
  m_faultData[idx].pluggedStateTime = 0;
  m_faultData[idx].reconnect        = true;

  #ifdef DEBUG
  output = m_className+F("m_initCriticalFault()");
  output += F(" - ");
  output += F("Critical fault data");
  output += F(" initialized.");
  printOutput();
  #endif
}

// ================================
//      m_resetCriticalFault()
// ================================
void Controller_Parent::m_resetCriticalFault(byte idx)
{
  m_faultData[idx].badData = 0;
  m_faultData[idx].reconnect = true;
  m_faultData[idx].pluggedStateTime = 0;

  if (m_faultData[idx].lastReadTime == 0 || ! m_connected() )
    m_faultData[idx].lastMsgTime = -1;
  else
    m_faultData[idx].lastMsgTime = m_faultData[idx].lastReadTime;

  #ifdef DEBUG
  output = m_className+F("m_resetCriticalFault()");
  output += F(" - ");
  output += F("Critical fault data");
  output += F(" reset.");
  printOutput();
  #endif
}

// =================================
//      m_detectCriticalFault()
// =================================
bool Controller_Parent::m_detectCriticalFault(void)
{
  unsigned long currentTime;
  unsigned long lastMsgTime;
  unsigned long lagTime;
  
  if ( m_connected() ) {

    currentTime = millis();
    lastMsgTime = m_lastReadTime;
    lagTime = currentTime - lastMsgTime;

    // -----------------------
    // Check for reconnection.
    // -----------------------

    if (m_faultData[0].reconnect ) {
      if (lagTime < LAG_TIME_TO_RECONNECT ) {
       m_faultData[0].reconnect = false;
      }
      lastMsgTime = currentTime;
    }

    // -----------------------
    // Calculate or reset lag.
    // -----------------------

    if ( currentTime >= lastMsgTime ) {
      lagTime = currentTime - lastMsgTime;
    } else {
      lagTime = 0;
    }

    // ------------------------------
    // Disconnect after too much lag.
    // ------------------------------

    if ( lagTime > LAG_TIME_TO_DISCONNECT ) {

      #ifdef DEBUG
      output = m_className+F("m_detectCriticalFault()");
      output += F(" - ");
      output += F("Disconnecting due to lag time.");
      output += F("\n");
      output += F("  Current time:  ");
      output += currentTime;
      output += F("\n");
      output += F("  Last msg time: ");
      output += lastMsgTime;
      output += F("\n");
      output += F("  Lag:           ");
      output += lagTime;    
      #endif
      
      m_disconnect();
    }

    // -----------------------------------------
    // Stop the drive motors after too much lag.
    // -----------------------------------------

    if ( lagTime > LAG_TIME_TO_KILL_MOTORS && driveStopped == false ) {

      #ifdef DEBUG
      output = m_className+F("m_detectCriticalFault()");
      output += F(" - ");
      output += F("Stopping drive motors due to lag.");
      printOutput();
      #endif      

      return true;  // The actual code to stop the motor is in loop() when read() fails.
    }

    // --------------------------
    // Check the PS3 signal data.
    // --------------------------

    if ( m_getUsbPlugged() && ! m_getUsbPlugged() ) {

      // The controller's signal is confused. We'll give it two tries
      // to clear up. The first time through starts the state machine.
      // The second time through trips the critical fault.

      if ( m_faultData[0].pluggedStateTime > 0 ) {

        // ------------------------------------------------------------
        // Has the desired amount of time between failed checks passed?
        // ------------------------------------------------------------

        unsigned long interval = ( m_connected() ? PLUGGED_STATE_LONG_INTERVAL : PLUGGED_STATE_SHORT_INTERVAL);
        if ( currentTime > ( m_faultData[0].pluggedStateTime + interval )) {

          // -------------------------------------------------------
          // We have our second failed check.
          // Trigger the critical fault and reset the state machine.
          // -------------------------------------------------------

         m_faultData[0].badData++;
         m_faultData[0].pluggedStateTime = 0;

          #ifdef DEBUG
          output = m_className+F("m_detectCriticalFault()");
          output += F("\n");
          output += F(" - ");
          output += F("Invalid data from primary controller.");
          #endif

          return true;
        }
      } else {

        // ------------------------
        // Start the state machine.
        // ------------------------

       m_faultData[0].pluggedStateTime = millis();
      }

    } else {

      // --------------------------------------------------------
      // The controller is not confused. Reset the state machine.
      // --------------------------------------------------------

      if (m_faultData[0].badData > 0) {
       m_faultData[0].pluggedStateTime = 0;
       m_faultData[0].badData = 0;
      }
    }
  
    if (m_faultData[0].badData > 10) {

      // -----------------------------------------------------------------
      // Disconnect. We have too much bad data coming from the controller.
      // -----------------------------------------------------------------

      #ifdef DEBUG
      output = m_className+F("m_detectCriticalFault()");
      output += F("\n");
      output += F(" - ");
      output += F("Disconnecting due to excessive bad data.");
      #endif

      m_disconnect();
    }
  }

  return false;
}

// ============================
//      connectionStatus()
// ============================
byte Controller_Parent::connectionStatus(void)
{
  return m_controllerConnected;
}

// ============================
//      isControllerLost()
// ============================
bool Controller_Parent::isControllerLost(void)
{
  return (m_onDisconnectCount > 0 ? true : false);
}

// ========================
//      onDisconnect()
// ========================
void Controller_Parent::onDisconnect(void)
{
  // In loop(), each peripheral is given an opportunity to act on the disconnect event.
  // This function tracks how many peripherals have acted, and resets the count after
  // all have done so.

  if ( m_onDisconnectCount >= NUMBER_OF_PERIPHERALS ) {
    m_onDisconnectCount = 0;
    return;
  }
  m_onDisconnectCount++;
}


#if defined(PS4_CONTROLLER)
/* ================================================================================
 *                                  PS4 Controller
 * ================================================================================ */


// =====================
//      Constructor
// =====================
Controller_PS4::Controller_PS4(void) : Controller_Parent(), m_controller(&m_Btd)
{
  #ifdef DEBUG
  m_className = "Controller_PS4";
  #endif
}

// ====================
//      Destructor
// ====================
Controller_PS4::~Controller_PS4(void) {}

// =================
//      begin()
// =================
void Controller_PS4::begin(void)
{
  // -----------------------------------------------------------
  // Call the parent class begin() to start the USB Host Shield.
  // -----------------------------------------------------------

  Controller_Parent::begin();

  // ------------------------
  // Set up the onInit event.
  // ------------------------

  anchor = this;
  m_controller.attachOnInit(m_onInit);

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
  output = m_className+F("begin()");
  output += F(" - ");
  output += F("Ready to connect a");
  output += F(" PS4");
  output += F(" controller");
  printOutput();
  #endif
}

// ====================
//      m_onInit()
// ====================
static void Controller_PS4::m_onInit(void)
{
  if (anchor != NULL) {
    anchor->m_connect();
  }
}

// ========================
//      m_disconnect()
// ========================
void Controller_PS4::m_disconnect(void)
{
  m_controller.setLedOff();
  m_controller.disconnect();
  m_setControllerStatus(NONE);

  #ifdef DEBUG
  output = m_className+F("m_disconnect()");
  output += F(" - ");
  output += F("Controller disconnected");
  printOutput();
  #endif
}

// ==================
//      setLed()
// ==================
void Controller_PS4::setLed(void)
{
  if ( ! driveEnabled ) {
    m_controller.setLed(Red);
  } else {
    switch (speedProfile) {
      case WALK: {
        m_controller.setLed(Yellow);
        break;
      }
      case JOG: {
        m_controller.setLed(Green);
        break;
      }
      case RUN: {
        m_controller.setLed(Blue);
        break;
      }
      case SPRINT: {
        m_controller.setLed(Purple);
        break;
      }
    }
  }
}

// ===========================
//      m_getUsbPlugged()
// ===========================
bool Controller_PS4::m_getUsbPlugged(void)
{
  return m_controller.getUsbStatus();
}

// =====================
//      m_connected
// =====================
bool Controller_PS4::m_connected(void)
{
  return m_controller.connected();
}

// ================
//      read()
// ================
bool Controller_PS4::read()
{
  // ------------------------------
  // Get input from the controller.
  // ------------------------------

  m_Usb.Task();
  if ( ! m_connected() ) {
    return false;
  }
  m_faultData[0].lastReadTime = millis();

  // -----------------------------------
  // Look for user-requested disconnect.
  // -----------------------------------

  if ( getButtonPress(PS) && ( getButtonPress(L2) || getButtonPress(R2) ) ) {

    #ifdef DEBUG
    output = F("\n");
    output += m_className+F("read()");
    output += F(" - ");
    output += F("Disconnecting due to user request");
    printOutput();
    #endif

    m_disconnect();
  }

  #ifdef TEST_CONTROLLER
  //--------------------------
  // Testing controller input.
  // Uncomment only one of these at a time.
  //--------------------------
  m_displayInput();
  //m_scrollInput();
  #endif

  // -------------
  // Read is done.
  // -------------

  return true;
}

// ==============================
//      Get Button Functions
// ==============================
bool Controller_PS4::getButtonClick(int buttonEnum) { return m_controller.getButtonClick(buttonEnum); };
bool Controller_PS4::getButtonPress(int buttonEnum) { return m_controller.getButtonPress(buttonEnum); };
int Controller_PS4::getAnalogButton(int buttonEnum) { return m_controller.getAnalogButton(buttonEnum); };
int Controller_PS4::getAnalogHat(int stickEnum)  { return m_controller.getAnalogHat(stickEnum); }
#endif


#if defined(PS3_CONTROLLER)
/* ================================================================================
 *                                  PS3 Controller
 * ================================================================================ */
 

// =====================
//      Constructor
// =====================
Controller_PS3::Controller_PS3(void) : Controller_Parent(), m_controller(&m_Btd)
{
  #ifdef DEBUG
  m_className = "Controller_PS3";
  #endif
}

// ====================
//      Destructor
// ====================
Controller_PS3::~Controller_PS3(void) {}

// =================
//      begin()
// =================
void Controller_PS3::begin(void)
{
  // -----------------------------------------------------------
  // Call the parent class begin() to start the USB Host Shield.
  // -----------------------------------------------------------

  Controller_Parent::begin();

  // ------------------------
  // Set up the onInit event.
  // ------------------------

  anchor = this;
  m_controller.attachOnInit(m_onInit);

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
  output = m_className+F("begin()");
  output += F(" - ");
  output += F("Ready to connect a");
  output += F(" PS3");
  output += F(" controller");
  printOutput();
  #endif
}

// ====================
//      m_onInit()
// ====================
static void Controller_PS3::m_onInit(void)
{
  if (anchor != NULL) {
    anchor->m_connect();
  }
}

// ========================
//      m_disconnect()
// ========================
void Controller_PS3::m_disconnect(void)
{
  m_controller.setLedOff();
  m_controller.disconnect();
  m_setControllerStatus(NONE);

  #ifdef DEBUG
  output = m_className+F("m_disconnect()");
  output += F(" - ");
  output += F("Controller disconnected");
  printOutput();
  #endif
}

// ==================
//      setLed()
// ==================
void Controller_PS3::setLed(void)
{
  m_controller.setLedOn(LED1);
}

// ===========================
//      m_getUsbPlugged()
// ===========================
bool Controller_PS3::m_getUsbPlugged(void)
{
  return ( m_controller.getStatus(Plugged) && m_controller.getStatus(Unplugged) );
}

// =====================
//      m_connected
// =====================
bool Controller_PS3::m_connected(void)
{
  return m_controller.PS3Connected;
}

// ==============================
//      Get Button Functions
// ==============================
bool Controller_PS3::getButtonClick(int buttonEnum) { return m_controller.getButtonClick(buttonEnum); };
bool Controller_PS3::getButtonPress(int buttonEnum) { return m_controller.getButtonPress(buttonEnum); };
int Controller_PS3::getAnalogButton(int buttonEnum) { return m_controller.getAnalogButton(buttonEnum); };
int Controller_PS3::getAnalogHat(int stickEnum)  { return m_controller.getAnalogHat(stickEnum); }

// ================
//      read()
// ================
bool Controller_PS3::read()
{
  // ------------------------------
  // Get input from the controller.
  // ------------------------------

  m_Usb.Task();
  if ( ! m_connected() ) {
    return false;
  }
  m_faultData[0].lastReadTime = millis();

  // -----------------------------------
  // Look for user-requested disconnect.
  // -----------------------------------

  if ( getButtonPress(PS) && ( getButtonPress(L2) || getButtonPress(R2) ) ) {

    #ifdef DEBUG
    output = F("\n");
    output += m_className+F("read()");
    output += F(" - ");
    output += F("Disconnecting due to user request");
    printOutput();
    #endif

    m_disconnect();
  }

  #ifdef TEST_CONTROLLER
  //--------------------------
  // Testing controller input.
  // Uncomment only one of these at a time.
  //--------------------------
  m_displayInput();
  //m_scrollInput();
  #endif

  // -------------
  // Read is done.
  // -------------

  return true;
}
#endif


#if defined(PS3_NAVIGATION)
/* ================================================================================
 *                             PS3 Navigation Controller
 * ================================================================================ */

// =====================
//      Constructor
// =====================
Controller_PS3Nav::Controller_PS3Nav(void) : Controller_Parent(), m_controller(&m_Btd), m_secondController(&m_Btd)
{
  #ifdef DEBUG
  m_className = "Controller_PS3Nav";
  #endif
}

// ====================
//      Destructor
// ====================
Controller_PS3Nav::~Controller_PS3Nav(void) {}

// =================
//      begin()
// =================
void Controller_PS3Nav::begin(void)
{
  // -----------------------------------------------------------
  // Call the parent class begin() to start the USB Host Shield.
  // -----------------------------------------------------------

  Controller_Parent::begin();

  // ------------------------
  // Set up the onInit event.
  // ------------------------

  anchor = this;
  m_controller.attachOnInit(m_onInit);
  m_secondController.attachOnInit(m_onInit);

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
  output = m_className+F("begin()");
  output += F(" - ");
  output += F("Ready to connect a");
  output += F(" PS3Nav");
  output += F(" controller");
  printOutput();
  #endif
}

// ====================
//      m_onInit()
// ====================
static void Controller_PS3Nav::m_onInit(void)
{
  if (anchor != NULL) {
    anchor->m_onInitConnect();
  }
}

// ========================
//      m_onInitConnect
// ========================
void Controller_PS3Nav::m_onInitConnect(void)
{
  // ---------------------------------------------------------------
  // Always try to assign as primary first.
  // Assign as secondary only when the primary is already connected.
  // ---------------------------------------------------------------

  if ( connectionStatus() == NONE ) {

    #ifdef DEBUG
    output = m_className+F("m_onInitConnect()");
    output += F(" - ");
    output += F("Initiating connection with ");
    output += F("primary");
    output += F(" controller");
    printOutput();
    #endif

    m_connect(&m_controller);

  } else if ( connectionStatus() != FULL ) {

    #ifdef DEBUG
    output = m_className+F("_onInitConnect()");
    output += F(" - ");
    output += F("Initiating connection with ");
    output += F("secondary");
    output += F(" controller");
    printOutput();
    #endif

    m_connect(&m_secondController);
  }
}
 
// ===================
//      m_connect
// ===================
void Controller_PS3Nav::m_connect(PS3BT * pController)
{
  // ----------------------------------------------------------------------------------
  // First, let's make sure we have a PS3 Move Navigation controller.  If not, drop it.
  // ----------------------------------------------------------------------------------

  if ( ! m_connected(pController) ) {

    #ifdef DEBUG
    output = m_className+F("m_connect()");
    output += F(" - ");
    output += F("Controller");
    output += F(" invalid");
    printOutput();
    #endif

    m_disconnect(pController);
    return;
  }

  // --------------------------------------------------
  // Validate the MAC address of the controller against
  // our list of authorized devices.
  // --------------------------------------------------

  if ( ! m_authorized() ) {
    m_disconnect(pController);
    return;
  }

  // ---------------------------------------
  // Display which controller was connected.
  // ---------------------------------------

  #ifdef DEBUG
  output = m_className+F("m_connect()");
  output += F(" - ");
  if ( pController == &m_controller ) {
    output += F("Primary");
  } else if ( pController == &m_secondController ) {
    output += F("Secondary");
  }
  output += F(" controller");
  output += F(" connected");
  printOutput();
  #endif

  // --------------------
  // Complete connection.
  // --------------------

  pController->setLedOn(LED1);
  if ( pController == &m_controller ) {
    m_setControllerStatus(HALF);
  } else if ( pController == &m_secondController ) {
    m_setControllerStatus(FULL);
  }
}

// ========================
//      m_disconnect()
// ========================
void Controller_PS3Nav::m_disconnect(PS3BT * pController)
{
  // -------------------------------------------------------------
  // When primary is disconnected, first disconnect the secondary.
  // -------------------------------------------------------------

  if ( pController == &m_controller && connectionStatus() == FULL ) {
    m_disconnect(&m_secondController);
  }

  // ------------------------------
  // Perform the actual disconnect.
  // ------------------------------

  pController->setLedOff(LED1);
  pController->disconnect();

  // -----------------------------
  // Update the connection status.
  // -----------------------------

  if ( pController == &m_controller ) {
    m_setControllerStatus(NONE);
  } else if ( pController == &m_secondController ) {
    m_setControllerStatus(HALF);
  }

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
  output = m_className+F("m_disconnect()");
  output += F(" - ");
  if ( pController == &m_controller ) {
    output += F("Primary");
  } else {
    output += F("Secondary");
  }
  output += F(" controller");
  output += F(" disconnected");
  printOutput();
  #endif
}

// ==================
//      setLed()
// ==================
void Controller_PS3Nav::setLed(void)
{
  m_controller.setLedOn(LED1);
}

// ==========================
//      m_takeSnapshot()
// ==========================
void Controller_PS3Nav::m_takeSnapshot(void)
{
}

// ===========================
//      m_getUsbPlugged()
// ===========================
bool Controller_PS3Nav::m_getUsbPlugged(void)
{
  return ( m_controller.getStatus(Plugged) && m_controller.getStatus(Unplugged) );
}

// =======================
//      m_connected()
// =======================
bool Controller_PS3Nav::m_connected(PS3BT * pController)
{
  return pController->PS3NavigationConnected;
}

// ================
//      read()
// ================
bool Controller_PS3Nav::read()
{

  // -------------------------------
  // Get input from the controllers.
  // -------------------------------

  m_Usb.Task();
  if ( ! m_connected(&m_controller) ) {
    // _onDisconnect();
    return false;
  } else {
    if ( m_detectCriticalFault(&m_controller) ) {
      printOutput();
      return false;
    }
  }
  m_faultData[0].lastReadTime = millis();

  m_Usb.Task();
  if ( ! m_connected(&m_secondController) ) {
    if ( ! m_faultData[1].reconnect )
      m_faultData[1].reconnect = true;
  } else {
    if ( m_detectCriticalFault(&m_secondController) ) {
      printOutput();
      return false;
    }
  }
  m_faultData[1].lastReadTime = millis();

  // -----------------------------------
  // Look for user-requested disconnect.
  // -----------------------------------

  if ( getButtonPress(L2) || getButtonPress(L2) ) {

    if ( getButtonPress(PS2) ) {
  
      #ifdef DEBUG
      output = F("\n");
      output += m_className+F("read()");
      output += F(" - ");
      output += F("Disconnecting");
      output += F(" secondary");
      output += F(" due to user request");
      printOutput();
      #endif
  
      m_disconnect(&m_secondController);

    } else if ( getButtonPress(PS) ) {
  
      #ifdef DEBUG
      output = F("\n");
      output += m_className+F("read()");
      output += F(" - ");
      output += F("Disconnecting");
      output += F(" due to user request");
      printOutput();
      #endif

      m_disconnect(&m_controller);
    }
  }

  #ifdef TEST_CONTROLLER
  //--------------------------
  // Testing controller input.
  // Uncomment only one of these at a time.
  //--------------------------
  m_displayInput();
  //m_scrollInput();
  #endif

  // -------------
  // Read is done.
  // -------------

  return true;

}

// ==========================
//      getButtonClick()
// ==========================
bool Controller_PS3Nav::getButtonClick(int buttonEnum)
{
  switch (buttonEnum) {
    case UP:       { return m_controller.getButtonClick(buttonEnum); }
    case RIGHT:    { return m_controller.getButtonClick(buttonEnum); }
    case DOWN:     { return m_controller.getButtonClick(buttonEnum); }
    case LEFT:     { return m_controller.getButtonClick(buttonEnum); }
    case L3:       { return m_controller.getButtonClick(buttonEnum); }
    case TRIANGLE: { return m_secondController.getButtonClick(UP); }
    case CIRCLE:   { return m_secondController.getButtonClick(RIGHT); }
    case CROSS:    { return m_secondController.getButtonClick(DOWN); }
    case SQUARE:   { return m_secondController.getButtonClick(LEFT); }
    case R3:       { return m_secondController.getButtonClick(L3); }
    default:       { return false; }
  }
  
}

// ==========================
//      getButtonPress()
// ==========================
bool Controller_PS3Nav::getButtonPress(int buttonEnum)
{
  switch (buttonEnum) {
    case L1:     { return m_controller.getButtonPress(buttonEnum); }
    case PS:     { return m_controller.getButtonPress(buttonEnum); }
    case R1:     { return m_controller.getButtonPress(L1); }
    case PS2:    { return m_controller.getButtonPress(PS); }
    case SELECT: { return m_controller.getButtonPress(buttonEnum) || m_secondController.getButtonPress(buttonEnum); }
    case START:  { return m_controller.getButtonPress(buttonEnum) || m_secondController.getButtonPress(buttonEnum); }
    default:       { return false; }
  }
  return m_controller.getButtonPress(buttonEnum);
}

// ===========================
//      getAnalogButton()
// ===========================
int Controller_PS3Nav::getAnalogButton(int buttonEnum)
{
  switch (buttonEnum) {
    case L2: { return m_controller.getAnalogButton(buttonEnum); }
    case R2: { return m_controller.getAnalogButton(L2); }
    default: { return 0; }
  }
}

// ========================
//      getAnalogHat()
// ========================
int Controller_PS3Nav::getAnalogHat(int stickEnum)
{
  int pos = JOYSTICK_CENTER;
  if ( stickEnum == LeftHatX || stickEnum == LeftHatY ) {
    pos = m_controller.getAnalogHat(stickEnum);
  } else if ( stickEnum == RightHatX ) {
    pos = m_secondController.getAnalogHat(LeftHatX);
  } else if ( stickEnum == RightHatY ) {
    pos = m_secondController.getAnalogHat(LeftHatY);
  }
  return pos;
}

// =================================
//      m_detectCriticalFault()
// =================================
bool Controller_PS3Nav::m_detectCriticalFault(PS3BT * pController)
{
  unsigned long currentTime;
  unsigned long lastMsgTime;
  unsigned long lagTime;

  byte idx = 0;
  if ( pController == &m_secondController ) {
    idx = 1;
  }

  if ( m_connected(pController) ) {
    currentTime = millis();
    m_faultData[idx].lastMsgTime = pController->getLastMessageTime();
    lagTime = 0;

    // -----------------------
    // Check for reconnection.
    // -----------------------

    if ( m_faultData[idx].reconnect ) {
      if ( lagTime < LAG_TIME_TO_RECONNECT ) {
        m_faultData[idx].reconnect = false;
      }
      m_faultData[idx].lastMsgTime = currentTime;
    }

    // ----------
    // Check lag.
    // ----------

    if ( currentTime >= m_faultData[idx].lastMsgTime ) {
      lagTime = currentTime - m_faultData[idx].lastMsgTime;
    } else {
      lagTime = 0;
    }

    // ------------------------------
    // Disconnect after too much lag.
    // ------------------------------

    if ( lagTime > LAG_TIME_TO_DISCONNECT ) {

      #ifdef DEBUG
      output = m_className+F("m_detectCriticalFault()");
      output += F(" - ");
      output += F("Disconnecting due to lag time.");
      output += F("\n");
      output += F("  Current time:  ");
      output += currentTime;
      output += F("\n");
      output += F("  Last msg time: ");
      output += lastMsgTime;
      output += F("\n");
      output += F("  Lag:           ");
      output += lagTime;    
      #endif
      
      m_disconnect(pController);
      return true;
    }

    // ---------------------------------------------------------------
    // Stop the drive motors after too much lag.
    // This part is specific to the PS3 Navigation primary controller.
    // ---------------------------------------------------------------

    if ( idx == 0 ) {
      if ( lagTime > LAG_TIME_TO_KILL_MOTORS && driveStopped == false ) {
  
        #ifdef DEBUG
        output = m_className+F("m_detectCriticalFault()");
        output += F(" - ");
        output += F("Stopping drive motors due to lag.");
        printOutput();
        #endif      
  
        return true;  // The actual code to stop the motor is in loop() when read() fails.
      }
    }
  
    // --------------------------
    // Check the PS3 signal data.
    // --------------------------

    if ( ! m_getUsbPlugged() ) {

      // The controller's signal is confused. We'll give it two tries
      // to clear up. The first time through starts the state machine.
      // The second time through trips the critical fault.

      if (m_faultData[idx].pluggedStateTime > 0) {

        // ------------------------------------------------------------
        // Has the desired amount of time between failed checks passed?
        // ------------------------------------------------------------
        
        unsigned long interval = ( m_connected(pController) ? PLUGGED_STATE_LONG_INTERVAL : PLUGGED_STATE_SHORT_INTERVAL);
        if ( currentTime > ( m_faultData[idx].pluggedStateTime + interval )) {

          // We have our second failed check.
          // Trigger the critical fault and reset the state machine.

          m_faultData[idx].badData++;
          m_faultData[idx].pluggedStateTime = 0;

          #ifdef DEBUG
          output = m_className+F("m_detectCriticalFault()");
          output += F("\n");
          output += F(" - ");
          output += F("Invalid data from primary controller.");
          #endif

          return true;
        }
      } else {

        // ------------------------
        // Start the state machine.
        // ------------------------

        m_faultData[idx].pluggedStateTime = millis();
      }

    } else {

      // --------------------------------------------------------
      // The controller is not confused. Reset the state machine.
      // --------------------------------------------------------

      if (m_faultData[idx].badData > 0) {
        m_faultData[idx].pluggedStateTime = 0;
        m_faultData[idx].badData = 0;
      }
    }
  
    if (m_faultData[idx].badData > 10) {

      // ----------------------------------
      // We have too much bad data coming
      // from the controller. Shut it down.
      // ----------------------------------

      #ifdef DEBUG
      output = m_className+F("m_detectCriticalFault()");
      output += F("\n");
      output += F(" - ");
      output += F("Disconnecting due to excessive bad data.");
      #endif

      m_disconnect(pController);
      return true;
    }
  }

  return false;
}
#endif



#ifdef TEST_CONTROLLER
/* ================================================================================
 *                                 Testing Functions
 * ================================================================================ */

// ==========================
//      m_displayInput()
// ==========================
void Controller_Parent::m_displayInput()
{
  output = "";
  bool hasBase = false;
  bool hasStick = false;
  String btnLabel = "";

  if ( getButtonPress(L1) )  { output = m_appendString(output, "L1"); }
  if ( getButtonPress(R1) )  { output = m_appendString(output, "R1"); }
  if ( getButtonPress(L2) )  { output = m_appendString(output, "L2"); }
  if ( getButtonPress(R2) )  { output = m_appendString(output, "R2"); }
  if ( getButtonPress(PS) )  { output = m_appendString(output, "PS"); }
  if ( getButtonPress(PS2) ) { output = m_appendString(output, "PS2"); }

  if ( getButtonClick(4) ) {
    #if defined(PS4_CONTROLLER)
    btnLabel = "SHARE";
    #elif defined(PS3_CONTROLLER)
    btnLabel = "SELECT";
    #elif defined(PS3_NAVIGATION)
    btnLabel = "CROSS";
    #endif
    if ( getButtonPress(PS) || getButtonPress(PS2) || (getAnalogButton(L2) > 10) || (getAnalogButton(R2) > 10) ) {
      hasBase = true;
    }
    output = m_appendString(output, btnLabel);
  }
  if ( getButtonClick(5) ) {
    #if defined(PS4_CONTROLLER)
    btnLabel = "OPTIONS";
    #elif defined(PS3_CONTROLLER)
    btnLabel = "START";
    #elif defined(PS3_NAVIGATION)
    btnLabel = "CIRCLE";
    #endif
    if ( getButtonPress(PS) || getButtonPress(PS2) || (getAnalogButton(L2) > 10) || (getAnalogButton(R2) > 10) ) {
      hasBase = true;
    }
    output = m_appendString(output, btnLabel);
  }

  if ( getButtonClick(L3) ) {
    hasBase = true;
    output = m_appendString(output, "L3");
  }
  if ( getButtonClick(R3) ) {
    hasBase = true;
    output = m_appendString(output, "R3");
  }
  if ( getButtonClick(UP) ) {
    hasBase = true;
    output = m_appendString(output, "UP");
  }
  if ( getButtonClick(RIGHT) ) {
    hasBase = true;
    output = m_appendString(output, "RIGHT");
  }
  if ( getButtonClick(DOWN) ) {
    hasBase = true;
    output = m_appendString(output, "DOWN");
  }
  if ( getButtonClick(LEFT) ) {
    hasBase = true;
    output = m_appendString(output, "LEFT");
  }

  if ( getButtonClick(TRIANGLE) ){
    #if defined(PS_NAVIGATION)
    btnLabel = "UP(Nav2)";
    #else
    btnLabel = "TRIANGLE";
    #endif
    hasBase = true;
    output = m_appendString(output, btnLabel);
  }
  if ( getButtonClick(CIRCLE) ){
    #if defined(PS_NAVIGATION)
    btnLabel = "RIGHT(Nav2)";
    #else
    btnLabel = "CIRCLE";
    #endif
    hasBase = true;
    output = m_appendString(output, btnLabel);
  }
  if ( getButtonClick(CROSS) ) {
    #if defined(PS_NAVIGATION)
    btnLabel = "DOWN(Nav2)";
    #else
    btnLabel = "CROSS";
    #endif
    hasBase = true;
    output = m_appendString(output, btnLabel);
  }
  if ( getButtonClick(SQUARE) ) {
    #if defined(PS_NAVIGATION)
    btnLabel = "LEFT(Nav2)";
    #else
    btnLabel = "SQUARE";
    #endif
    hasBase = true;
    output = m_appendString(output, btnLabel);
  }

  // ------------------
  // Display the stick.
  // ------------------

  int posX = getAnalogHat(LeftHatX);
  int posY = getAnalogHat(LeftHatY);

  if ( (abs(posX - JOYSTICK_CENTER) >= JOYSTICK_DEAD_ZONE) || 
       (abs(posY - JOYSTICK_CENTER) >= JOYSTICK_DEAD_ZONE) ) {
    if ( ! getButtonPress(L1) ) {
      hasStick = true;
      m_displayStick("Left", posX, posY);
    }
  }

  if ( connectionStatus() == FULL ) {

    posX = getAnalogHat(RightHatX);
    posY = getAnalogHat(RightHatY);

    if ( (abs(posX - JOYSTICK_CENTER) >= JOYSTICK_DEAD_ZONE) || 
         (abs(posY - JOYSTICK_CENTER) >= JOYSTICK_DEAD_ZONE) ) {
      if ( ! getButtonPress(R1) ) {
        hasStick = true;
        m_displayStick("Right", posX, posY);
      }
    }
  }

  if ( hasBase || hasStick ) {
    printOutput();
  }
}

// ==========================
//      m_appendString()
// ==========================
String Controller_Parent::m_appendString(String inString, String addString)
{
  if ( inString != "" ) { inString += F("+"); }
  inString += addString;
  return inString;
}

// ==========================
//      m_displayStick()
// ==========================
void Controller_Parent::m_displayStick(String s, byte x, byte y)
{
  if ( output != "" ) {
    output += " + ";
  }
  output += s;
  output += F(": ");
  output += x;
  output += F(",");
  output += y;
}

// =========================
//      m_scrollInput()
// =========================
void Controller_Parent::m_scrollInput()
{
  if ( ! m_connected() ) {
    return;
  }

  /* 
   * PS4 Controller = LX:x LY:x Up:x Rt:x Dn:x Lt:x L3:x L2:x L1:x PS:x Sh:x Op:x RX:x RY:x Tr:x Ci:x Cr:x Sq:x R3:x R2:x R1:x
   * PS3 Controller = LX:x LY:x Up:x Rt:x Dn:x Lt:x L3:x L2:x L1:x PS:x Sl:x St:x RX:x RY:x Tr:x Ci:x Cr:x Sq:x R3:x R2:x R1:x
   * PS3 Navigation = LX:x LY:x Up:x Rt:x Dn:x Lt:x L3:x L2:x L1:x PS:x X:x O:x [RX:x RY:x Up2:x Rt2:x Dn2:x Lt2:x R3:x R2:x R1:x PS2:x]
   */

  output = F("LX:");    output += getAnalogHat(LeftHatX);
  output += F(" LY:");  output += getAnalogHat(LeftHatX);
  output += F(" Up:");  output += getButtonClick(UP);
  output += F(" Rt:");  output += getButtonClick(RIGHT);
  output += F(" Dn:");  output += getButtonClick(DOWN);
  output += F(" Lt:");  output += getButtonClick(LEFT);
  output += F(" L1:");  output += getButtonPress(L1);
  output += F(" L2:");  output += getAnalogButton(L2);
  output += F(" L3:");  output += getButtonClick(L3);
  output += F(" PS:");  output += getButtonPress(PS);

  #if defined(PS4_CONTROLLER)
  output += F(" Sh:");  output += getButtonPress(4);
  output += F(" Op:");  output += getButtonPress(5);
  #elif defined(PS3_CONTROLLER)
  output += F(" Sl:");  output += getButtonPress(4);
  output += F(" St:");  output += getButtonPress(5);
  #elif defined(PS3_NAVIGATION)
  output += F(" X:");   output += getButtonPress(4);
  output += F(" O:");   output += getButtonPress(5);
  #endif

  if ( connectionStatus() == FULL ) {
    output += F(" RX:");  output += getAnalogHat(RightHatX);
    output += F(" RY:");  output += getAnalogHat(RightHatX);
    #if defined(PS3_NAVIGATION)
    output += F(" Up2:");  output += getButtonClick(TRIANGLE);
    output += F(" Rt2:");  output += getButtonClick(CIRCLE);
    output += F(" Dn2:");  output += getButtonClick(CROSS);
    output += F(" Lf2:");  output += getButtonClick(SQUARE);
    #else
    output += F(" Tr:");  output += getButtonClick(TRIANGLE);
    output += F(" Cr:");  output += getButtonClick(CIRCLE);
    output += F(" Ci:");  output += getButtonClick(CROSS);
    output += F(" Sq:");  output += getButtonClick(SQUARE);
    #endif
    output += F(" R1:");  output += getButtonPress(R1);
    output += F(" R2:");  output += getAnalogButton(R2);
    output += F(" R3:");  output += getButtonClick(R3);
  }

  printOutput();
}
#endif
