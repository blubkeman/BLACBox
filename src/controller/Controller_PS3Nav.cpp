/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS3Nav.cpp - Library for supported controllers
 * Created by Brian Lubkeman, 16 June 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Controller.h"

/* ================================================================================
 *                                  PS3Nav Controller
 * ================================================================================ */

// =====================
//      Constructor
// =====================
Controller_PS3Nav::Controller_PS3Nav(const int settings[], const unsigned long timings[])
  : Controller(settings, timings),
    m_controller(&m_Btd),
    m_secondController(&m_Btd)
{
  m_type = 0; // 0=PS3Nav, 1=PS3, 2=PS4, 3=PS5
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

  Controller::begin();

  // ------------------------
  // Set up the onInit event.
  // ------------------------

  anchor = this;
  m_controller.attachOnInit(m_onInit);
  m_secondController.attachOnInit(m_onInit);

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("Controller_PS3Nav"), F("m_onInitConnect()"), F("Ready to connect a PS3 Nav controller"));
  Debug.print(DBG_VERBOSE, F("\n  Drive stick: "), (String)driveStick.getSide());
  Debug.print(DBG_VERBOSE, F("\n    Dead zone: "), (String)driveStick.deadZone);
  Debug.print(DBG_VERBOSE, F("\n   Dome stick: "), (String)domeStick.getSide());
  Debug.print(DBG_VERBOSE, F("\n    Dead zone: "), (String)domeStick.deadZone);
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

    #if defined(DEBUG)
    Debug.print(DBG_INFO, F("Controller_PS3Nav"), F("m_onInitConnect()"), F("Initiating connection with primary controller"));
    #endif

    m_connect(&m_controller);

  } else if ( connectionStatus() != FULL ) {

    #if defined(DEBUG)
    Debug.print(DBG_INFO, F("Controller_PS3Nav"), F("m_onInitConnect()"), F("Initiating connection with secondary controller"));
    #endif

    m_connect(&m_secondController);
  }
}

// ===================
//      m_connect
// ===================
void Controller_PS3Nav::m_connect(PS3BT * pController)
{
  // --------------------------------------------------------------
  // First, let's make sure we have a controller.  If not, drop it.
  // --------------------------------------------------------------

  if ( ! connected(pController) ) {

    #if defined(DEBUG)
    Debug.print(DBG_WARNING, F("Controller_PS3Nav"), F("m_connect()"), F("Controller invalid"));
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

  #if defined(DEBUG)
  if ( pController == &m_controller ) {
    Debug.print(DBG_INFO, F("Controller_PS3Nav"), F("m_connect()"), F("Primary controller connected"));
  } else if ( pController == &m_secondController ) {
    Debug.print(DBG_INFO, F("Controller_PS3Nav"), F("m_connect()"), F("Secondary controller connected"));
  }
  #endif

  // --------------------
  // Complete connection.
  // --------------------

  if ( pController == &m_controller ) {
    m_setConnectionStatus(HALF);
  } else if ( pController == &m_secondController ) {
    m_setConnectionStatus(FULL);
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
    m_setConnectionStatus(NONE);
  } else if ( pController == &m_secondController ) {
    m_setConnectionStatus(HALF);
  }

  #if defined(DEBUG)
  if ( pController == &m_controller ) {
    Debug.print(DBG_INFO, F("Controller_PS3Nav"), F("m_connect()"), F("Primary controller disconnected"));
  } else {
    Debug.print(DBG_INFO, F("Controller_PS3Nav"), F("m_connect()"), F("Secondary controller disconnected"));
  }
  #endif
}

// ==================
//      setLed()
// ==================
void Controller_PS3Nav::setLed(bool driveEnabled, byte speedProfile)
{
  m_controller.setLedOn(LED1);
}

// ==========================
//      m_getUsbStatus()
// ==========================
bool Controller_PS3Nav::m_getUsbStatus(void)
{
  return ( m_controller.getStatus(Plugged) && m_controller.getStatus(Unplugged) );
}

// =====================
//      connected()
// =====================
bool Controller_PS3Nav::connected(PS3BT * pController)
{
  return pController->PS3NavigationConnected;
}

// ================
//      read()
// ================
bool Controller_PS3Nav::read()
{
  // ------------------------------
  // Get input from the controller.
  // ------------------------------

  m_Usb.Task();
  if ( ! connected(&m_controller) ) {
    // _continueDisconnecting();
    return false;
  } else {
    if ( m_detectCriticalFault(&m_controller) ) {
      return false;
    }
  }
  m_faultData[0].lastReadTime = millis();

  m_Usb.Task();
  if ( ! connected(&m_secondController) ) {
    if ( ! m_faultData[1].reconnect )
      m_faultData[1].reconnect = true;
  } else {
    if ( m_detectCriticalFault(&m_secondController) ) {
      return false;
    }
  }
  m_faultData[1].lastReadTime = millis();

  // -----------------------------------
  // Look for user-requested disconnect.
  // -----------------------------------

  if ( getButtonPress(L2) || getButtonPress(R2) ) {

    if ( getButtonPress(PS2) ) {

      #if defined(DEBUG)
      String msg = F("Disconnecting");
      msg += F(" secondary");
      msg += F(" due to user request");
      Debug.print(DBG_INFO, F("Controller_PS3Nav"), F("read()"), msg);
      #endif

      m_disconnect(&m_secondController);

    } else if ( getButtonPress(PS) ) {

      #if defined(DEBUG)
      Debug.print(DBG_INFO, F("Controller_PS3Nav"), F("read()"), F("Disconnecting"), F(" due to user request"));
      #endif

      m_disconnect(&m_controller);
    }
  }

  #if defined(TEST_CONTROLLER)
  //--------------------------
  // Testing controller input.
  //--------------------------
  m_displayInput();   // Uncomment only one of these at a time.
  //m_scrollInput();  // Uncomment only one of these at a time.
  #endif

  // -------------
  // Read is done.
  // -------------

  return true;
}

// ==============================
//      Get Button Functions
// ==============================
bool Controller_PS3Nav::getButtonClick(int buttonEnum)
{
  switch (buttonEnum) {
    case UP:       { return m_controller.getButtonClick(buttonEnum); }
    case RIGHT:    { return m_controller.getButtonClick(buttonEnum); }
    case DOWN:     { return m_controller.getButtonClick(buttonEnum); }
    case LEFT:     { return m_controller.getButtonClick(buttonEnum); }
    case TRIANGLE: { return m_secondController.getButtonClick(UP); }
    case CIRCLE:   { return m_secondController.getButtonClick(RIGHT); }
    case CROSS:    { return m_secondController.getButtonClick(DOWN); }
    case SQUARE:   { return m_secondController.getButtonClick(LEFT); }
    case L4:       { return (m_controller.getButtonClick(buttonEnum) || m_secondController.getButtonClick(buttonEnum)); }
    case R4:       { return (m_controller.getButtonClick(buttonEnum) || m_secondController.getButtonClick(buttonEnum)); }
    case L3:       { return m_controller.getButtonClick(buttonEnum); }
    case R3:       { return m_secondController.getButtonClick(L3); }
    case L1:       { return m_controller.getButtonClick(buttonEnum); }
    case R1:       { return m_secondController.getButtonClick(L1); }
    case L2:       { return m_controller.getButtonClick(buttonEnum); }
    case R2:       { return m_secondController.getButtonClick(L2); }
    case PS:       { return m_controller.getButtonClick(buttonEnum); }
    case PS2:      { return m_secondController.getButtonClick(PS); }
    default:       { return false; }
  }
}

bool Controller_PS3Nav::getButtonPress(int buttonEnum)
{
  switch (buttonEnum) {
    case UP:       { return m_controller.getButtonPress(buttonEnum); }
    case RIGHT:    { return m_controller.getButtonPress(buttonEnum); }
    case DOWN:     { return m_controller.getButtonPress(buttonEnum); }
    case LEFT:     { return m_controller.getButtonPress(buttonEnum); }
    case TRIANGLE: { return m_secondController.getButtonPress(UP); }
    case CIRCLE:   { return m_secondController.getButtonPress(RIGHT); }
    case CROSS:    { return m_secondController.getButtonPress(DOWN); }
    case SQUARE:   { return m_secondController.getButtonPress(LEFT); }
    case L4:       { return (m_controller.getButtonPress(buttonEnum) || m_secondController.getButtonPress(buttonEnum)); }
    case R4:       { return (m_controller.getButtonPress(buttonEnum) || m_secondController.getButtonPress(buttonEnum)); }
    case L3:       { return m_controller.getButtonPress(buttonEnum); }
    case R3:       { return m_secondController.getButtonPress(L3); }
    case L1:       { return m_controller.getButtonPress(buttonEnum); }
    case R1:       { return m_secondController.getButtonPress(L1); }
    case L2:       { return m_controller.getButtonPress(buttonEnum); }
    case R2:       { return m_secondController.getButtonPress(L2); }
    case PS:       { return m_controller.getButtonPress(buttonEnum); }
    case PS2:      { return m_secondController.getButtonPress(PS); }
    default:       { return false; }
  }
  return m_controller.getButtonPress(buttonEnum);
}
int Controller_PS3Nav::getAnalogButton(int buttonEnum)
{
  return m_controller.getAnalogButton(buttonEnum);
}
int Controller_PS3Nav::getAnalogHat(int stickEnum)
{
  return m_controller.getAnalogHat(stickEnum);
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

  if ( connected(pController) ) {
    currentTime = millis();
    m_faultData[idx].lastMsgTime = pController->getLastMessageTime();
    lagTime = 0;

    // -----------------------
    // Check for reconnection.
    // -----------------------

    if ( m_faultData[idx].reconnect ) {
      if ( lagTime < pTimings[iLagReconnect] ) {
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

    if ( lagTime > pTimings[iLagDisconnect] ) {

      #if defined(DEBUG)
      Debug.print(DBG_WARNING, F("Controller_PS3Nav"), F("m_detectCriticalFault()"), F("Disconnecting due to lag time."));
      Debug.print(DBG_VERBOSE, F("  Current time:  "), (String)currentTime);
      Debug.print(DBG_VERBOSE, F("  Last msg time: "), (String)lastMsgTime);
      Debug.print(DBG_VERBOSE, F("  Lag:           "), (String)lagTime);
      #endif
      
      m_disconnect(pController);
      return true;
    }

    // ---------------------------------------------------------------
    // Stop the drive motors after too much lag.
    // This part is specific to the PS3 Navigation primary controller.
    // ---------------------------------------------------------------

    if ( idx == 0 ) {
      if ( lagTime > pTimings[iLagKillMotor] ) {
  
        #if defined(DEBUG)
        Debug.print(DBG_WARNING, F("Controller_PS3Nav"), F("m_detectCriticalFault()"), F("Stopping drive motors due to lag."));
        #endif      
  
        return true;  // The actual code to stop the motor is in loop() when read() fails.
      }
    }
  
    // --------------------------
    // Check the PS3 signal data.
    // --------------------------

    if ( ! m_getUsbStatus() ) {

      // The controller's signal is confused. We'll give it two tries
      // to clear up. The first time through starts the state machine.
      // The second time through trips the critical fault.

      if (m_faultData[idx].pluggedStateTime > 0) {

        // ------------------------------------------------------------
        // Has the desired amount of time between failed checks passed?
        // ------------------------------------------------------------
        
        unsigned long interval = ( connected(pController) ? pTimings[iLongInterval] : pTimings[iShortInterval]);
        if ( currentTime > ( m_faultData[idx].pluggedStateTime + interval )) {

          // We have our second failed check.
          // Trigger the critical fault and reset the state machine.

          m_faultData[idx].badData++;
          m_faultData[idx].pluggedStateTime = 0;

          #if defined(DEBUG)
          Debug.print(DBG_WARNING, F("Controller_PS3Nav"), F("m_detectCriticalFault()"), F("Invalid data from primary controller."));
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

      #if defined(DEBUG)
      Debug.print(DBG_WARNING, F("Controller_PS3Nav"), F("m_detectCriticalFault()"), F("Disconnecting due to bad data."));
      #endif

      m_disconnect(pController);
      return true;
    }
  }

  return false;
}
