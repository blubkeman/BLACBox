/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS3Nav.cpp - Library for PS3 Move Navigation controllers
 * Created by Brian Lubkeman, 22 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "Controller.h"


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
  m_className = "Controller_PS3Nav::";
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

  if ( ! connected(pController) ) {

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
//      m_getUsbStatus()
// ==========================
bool Controller_PS3Nav::m_getUsbStatus(void)
{
  return ( m_controller.getStatus(Plugged) && m_controller.getStatus(Unplugged) );
}

// =======================
//      connected()
// =======================
bool Controller_PS3Nav::connected(PS3BT * pController)
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
  if ( ! connected(&m_controller) ) {
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
  if ( ! connected(&m_secondController) ) {
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

// ================================
//      m_getModifierButtons()
// ================================
int Controller_PS3Nav::m_getModifierButtons(void)
{
  int modifier = 0;

  if ( getButtonPress(L1) || 
       getButtonPress(R1) )          { modifier = 8;  }
  else if ( getButtonPress(SELECT) ) { modifier = 16; }
  else if ( getButtonPress(START) )  { modifier = 24; }
  else if ( getButtonPress(PS) || 
            getButtonPress(PS2))     { modifier = 32; }

  return modifier;
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

    if ( ! m_getUsbStatus() ) {

      // The controller's signal is confused. We'll give it two tries
      // to clear up. The first time through starts the state machine.
      // The second time through trips the critical fault.

      if (m_faultData[idx].pluggedStateTime > 0) {

        // ------------------------------------------------------------
        // Has the desired amount of time between failed checks passed?
        // ------------------------------------------------------------
        
        unsigned long interval = ( connected(pController) ? PLUGGED_STATE_LONG_INTERVAL : PLUGGED_STATE_SHORT_INTERVAL);
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