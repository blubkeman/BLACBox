/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS3.cpp - Library for supported controllers
 * Created by Brian Lubkeman, 5 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "Controller.h"

/* ================================================================================
 *                                  PS3 Controller
 * ================================================================================ */

// =====================
//      Constructor
// =====================
Controller_PS3::Controller_PS3(int settings[])
  : Controller(settings),
    m_controller(&m_Btd)
{
  #if defined(DEBUG)
  m_className = "Controller_PS3::";
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

  Controller::begin();

  // ------------------------
  // Set up the onInit event.
  // ------------------------

  anchor = this;
  m_controller.attachOnInit(m_onInit);

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  output = m_className+F("begin()");
  output += F(" - ");
  output += F("Ready to connect a");
  output += F(" PS3");
  output += F(" controller");

  output += F("\n  Drive stick:   ");
  driveStick.displayInit();
  output += F("\n  Dome stick:    ");
  domeStick.displayInit();
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

// ===================
//      m_connect
// ===================
void Controller_PS3::m_connect(void)
{
  // --------------------------------------------------------------
  // First, let's make sure we have a controller.  If not, drop it.
  // --------------------------------------------------------------

  if ( ! connected() ) {

    #if defined(DEBUG)
    output = m_className+F("m_connect()");
    output += F(" - ");
    output += F("Controller");
    output += F(" invalid");
    printOutput();
    #endif

    m_disconnect();
    return;
  }

  // --------------------------------------------------
  // Validate the MAC address of the controller against
  // our list of authorized devices.
  // --------------------------------------------------

  if ( ! m_authorized() ) {
    m_disconnect();
    return;
  }

  // --------------------
  // Complete connection.
  // --------------------

  m_setConnectionStatus(FULL);

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  if ( connectionStatus() > NONE ) {
    output = m_className+F("m_connect()");
    output += F(" - ");
    output += F("Controller");
    output += F(" connected");
    printOutput();
  }
  #endif
}

// ========================
//      m_disconnect()
// ========================
void Controller_PS3::m_disconnect(void)
{
  m_controller.setLedOff();
  m_controller.disconnect();
  m_setConnectionStatus(NONE);

  #if defined(DEBUG)
  output = m_className+F("m_disconnect()");
  output += F(" - ");
  output += F("Controller disconnected");
  printOutput();
  #endif
}

// ==================
//      setLed()
// ==================
void Controller_PS3::setLed(bool driveEnabled, byte speedProfile)
{
  m_controller.setLedOn(LED1);
}

// ==========================
//      m_getUsbStatus()
// ==========================
bool Controller_PS3::m_getUsbStatus(void)
{
  return ( m_controller.getStatus(Plugged) && m_controller.getStatus(Unplugged) );
}

// =====================
//      connected()
// =====================
bool Controller_PS3::connected(void)
{
  return m_controller.PS3Connected;
}

// ================
//      read()
// ================
bool Controller_PS3::read()
{
  // ------------------------------
  // Get input from the controller.
  // ------------------------------

  m_Usb.Task();
  if ( ! connected() ) {
    return false;
  }
  m_faultData[0].lastReadTime = millis();

  // -----------------------------------
  // Look for user-requested disconnect.
  // -----------------------------------

  if ( button.clicked(PS) && ( button.pressed(L2) || button.pressed(R2) ) ) {

    #if defined(DEBUG)
    output = F("\n");
    output += m_className+F("read()");
    output += F(" - ");
    output += F("Disconnecting due to user request");
    printOutput();
    #endif

    m_disconnect();
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
bool Controller_PS3::getButtonClick(int buttonEnum) { return m_controller.getButtonClick(buttonEnum); };
bool Controller_PS3::getButtonPress(int buttonEnum) { return m_controller.getButtonPress(buttonEnum); };
int Controller_PS3::getAnalogButton(int buttonEnum) { return m_controller.getAnalogButton(buttonEnum); };
int Controller_PS3::getAnalogHat(int stickEnum)     { return m_controller.getAnalogHat(stickEnum); }
