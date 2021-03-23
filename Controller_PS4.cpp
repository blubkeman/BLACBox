/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS4.cpp - Library for PS4 controllers
 * Created by Brian Lubkeman, 23 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "Controller.h"


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
  m_className = "Controller_PS4::";
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

// ===================
//      m_connect
// ===================
void Controller_PS4::m_connect(void)
{
  // --------------------------------------------------------------
  // First, let's make sure we have a controller.  If not, drop it.
  // --------------------------------------------------------------

  if ( ! connected() ) {

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

  if ( connectionStatus() > NONE ) {

    // --------------------------------------------------
    // Validate the MAC address of the controller against
    // our list of authorized devices.
    // --------------------------------------------------

    if ( ! m_authorized() ) {
      m_disconnect();
      return;
    }
  }

  // --------------------
  // Complete connection.
  // --------------------

  setLed();
  m_setConnectionStatus(FULL);

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
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
void Controller_PS4::m_disconnect(void)
{
  m_controller.setLedOff();
  m_controller.disconnect();
  m_setConnectionStatus(NONE);

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

// ==========================
//      m_getUsbStatus()
// ==========================
bool Controller_PS4::m_getUsbStatus(void)
{
  return m_controller.getUsbStatus();
}

// =====================
//      m_connected
// =====================
bool Controller_PS4::connected(void)
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
  if ( ! connected() ) {
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
int Controller_PS4::m_getModifierButtons(void)
{
  int modifier = 0;

  if ( getButtonPress(L1) || 
       getButtonPress(R1) )           { modifier = 8;  }
  else if ( getButtonPress(SHARE) )   { modifier = 16; }
  else if ( getButtonPress(OPTIONS) ) { modifier = 24; }
  else if ( getButtonPress(PS) )      { modifier = 32; }

  return modifier;
}
#endif
