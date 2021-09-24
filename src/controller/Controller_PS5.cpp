/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS5.cpp - Library for supported controllers
 * Created by Brian Lubkeman, 16 June 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Controller.h"

/* ================================================================================
 *                                  PS5 Controller
 * ================================================================================ */

// =====================
//      Constructor
// =====================
Controller_PS5::Controller_PS5(const int settings[], const unsigned long timings[], bool pair=false)
  : Controller(settings, timings),
    m_controller(&m_Btd, pair)
{
  m_type = 3; // 0=PS3Nav, 1=PS3, 2=PS4, 3=PS5
}

// ====================
//      Destructor
// ====================
Controller_PS5::~Controller_PS5(void) {}

// =================
//      begin()
// =================
void Controller_PS5::begin(void)
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

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("Controller_PS5"), F("m_onInitConnect()"), F("Ready to connect a PS5 controller"));
  Debug.print(DBG_VERBOSE, F("\n  Drive stick: "), (String)driveStick.getSide());
  Debug.print(DBG_VERBOSE, F("\n    Dead zone: "), (String)driveStick.deadZone);
  Debug.print(DBG_VERBOSE, F("\n   Dome stick: "), (String)domeStick.getSide());
  Debug.print(DBG_VERBOSE, F("\n    Dead zone: "), (String)domeStick.deadZone);
  #endif
}

// ====================
//      m_onInit()
// ====================
static void Controller_PS5::m_onInit(void)
{
  if (anchor != NULL) {
    anchor->m_connect();
  }
}

// ===================
//      m_connect
// ===================
void Controller_PS5::m_connect(void)
{
  // --------------------------------------------------------------
  // First, let's make sure we have a controller.  If not, drop it.
  // --------------------------------------------------------------

  if ( ! connected() ) {

    #if defined(DEBUG)
    Debug.print(DBG_WARNING, F("Controller_PS5"), F("m_connect()"), F("Controller invalid"));
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

  #if defined(DEBUG)
  if ( connectionStatus() > NONE ) {
    Debug.print(DBG_INFO, F("Controller_PS5"), F("m_connect()"), F("Controller connected"));
  }
  #endif
}

// ========================
//      m_disconnect()
// ========================
void Controller_PS5::m_disconnect(void)
{
  m_controller.setLedOff();
  m_controller.disconnect();
  m_setConnectionStatus(NONE);

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("Controller_PS5"), F("m_disconnect()"), F("Controller disconnected"));
  #endif
}

// ==================
//      setLed()
// ==================
void Controller_PS5::setLed(bool driveEnabled, byte speedProfile)
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
      default: {
        #if defined(DEBUG)
        Debug.print(DBG_WARNING, F("Controller_PS5"), F("setLed()"), F("Speed profile unknown"));
        #endif
      }
    }
  }
}

// ==========================
//      m_getUsbStatus()
// ==========================
bool Controller_PS5::m_getUsbStatus(void)
{
  return true;
}

// =====================
//      connected()
// =====================
bool Controller_PS5::connected(void)
{
  return m_controller.connected();
}

// ================
//      read()
// ================
bool Controller_PS5::read()
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
    Debug.print(DBG_INFO, F("Controller_PS5"), F("read()"), F("Disconnecting due to user request"));
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
bool Controller_PS5::getButtonClick(int buttonEnum) { return m_controller.getButtonClick(buttonEnum); };
bool Controller_PS5::getButtonPress(int buttonEnum) { return m_controller.getButtonPress(buttonEnum); };
int Controller_PS5::getAnalogButton(int buttonEnum) { return m_controller.getAnalogButton(buttonEnum); };
int Controller_PS5::getAnalogHat(int stickEnum)     { return m_controller.getAnalogHat(stickEnum); }
