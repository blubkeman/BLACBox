/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller.cpp - Library for supported controllers
 * Created by Brian Lubkeman, 10 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Controller.h"
//#include "../../Security.h"
#include "Security.h"

/* ================================================================================
 *                             Parent Controller Class
 * ================================================================================ */

// =====================
//      Constructor
// =====================
Controller::Controller(const int settings[], const unsigned long timings[])
  : m_Usb(),
    m_Btd(&m_Usb),
    driveStick(settings[iDriveSide], settings[iDeadZone], this),
    domeStick(settings[iDomeSide], settings[iDeadZone], this),
    button(this)
{
  pSettings = settings;
  pTimings = timings;
}

// ====================
//      Destructor
// ====================
Controller::~Controller(void) {}

// =================
//      begin()
// =================
void Controller::begin(void)
{
  // -------------------
  // Start the USB host.
  // -------------------

  if (m_Usb.Init() == -1) {

    #if defined(DEBUG)
    Debug.print(DBG_ERROR, F("Controller"), F("begin()"), F("OSC did not start"));
    #endif

    while (1);
  }

  #if defined(DEBUG)
  Debug.print(DBG_ERROR, F("Controller"), F("begin()"), F("Bluetooth Library Started"));
  #endif
}

// ============================
//      connectionStatus()
// ============================
byte Controller::connectionStatus(void)
{
  return m_connectionStatus;
}

// ===========================
//      isDisconnecting()
// ===========================
bool Controller::isDisconnecting(void)
{
  return (m_disconnectCount > 0 ? true : false);
}

// =========================
//      disconnecting()
// =========================
void Controller::disconnecting(void)
{
  // ----------------------------------------------------------------------------------
  // In loop(), each peripheral is given an opportunity to act on the disconnect event.
  // This function tracks how many peripherals have acted, and resets the count after
  // all have done so.
  // ----------------------------------------------------------------------------------

  if ( m_disconnectCount == pSettings[iPeripheralCount] ) {
    m_disconnectCount = 0;
    return;
  }
  m_disconnectCount++;
}

byte Controller::getType(void)
{
  return m_type;
}


// ========================
//      m_authorized()
// ========================
bool Controller::m_authorized(void)
{
  bool authorized = false;

  String btAddress = "";
  for(int8_t i = 5; i >= 0; i--) {
    if (btAddress.length() > 0) btAddress +=(":");
    if (m_Btd.disc_bdaddr[i]<0x10) btAddress +="0";
    btAddress += String(m_Btd.disc_bdaddr[i], HEX);
  }
  btAddress.toUpperCase();

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("Controller"), F("m_authorized()"), F("MAC address:"), btAddress);
  #endif

  for (byte i = 0; i < sizeof(authorizedMACAddresses); i++) {
    if (btAddress == m_getPgmString(authorizedMACAddresses[i])) {
      authorized = true;
      break;
    }
  }

  #if defined(DEBUG)
  if ( authorized ) {
    Debug.print(DBG_INFO, F("Controller"), F("m_authorized()"), F("Controller authorized"));
  } else {
    Debug.print(DBG_ERROR, F("Controller"), F("m_authorized()"), F("Controller unauthorized"));
  }
  #endif

  return authorized;
}

// =============================
//      m_getPgmString()
// =============================
static String Controller::m_getPgmString(const char * inValue)
{
  // This function takes in a pointer to a char array that has been
  // stored in program memory and returns its string content.

  String outValue = "";
  for (byte k = 0; k < strlen_P(inValue); k++) {
    outValue += (char)pgm_read_byte_near(inValue + k);
  }
  return outValue;
}

// =================================
//      m_setConnectionStatus()
// =================================
void Controller::m_setConnectionStatus(byte status)
{
  // -----------------------------------------------------------------------
  // Input: NONE (0), HALF (1) or FULL (2)
  // When we lose the controller, start the disconnecting counter.
  // In loop(), each peripheral will be given a chance to act on this event.
  // -----------------------------------------------------------------------
  
  if ( m_connectionStatus != NONE && status == NONE ) {
    m_disconnectCount = 1;
  }
  m_connectionStatus = status;  
}

// ===============================
//      m_initCriticalFault()
// ===============================
void Controller::m_initCriticalFault(byte idx)
{
  m_faultData[idx].badData          = 0;
  m_faultData[idx].lastMsgTime      = millis();
  m_faultData[idx].pluggedStateTime = 0;
  m_faultData[idx].reconnect        = true;

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("Controller"), F("m_initCriticalFault()"), F("Critical fault data initialized."));
  #endif
}

// ================================
//      m_resetCriticalFault()
// ================================
void Controller::m_resetCriticalFault(byte idx)
{
  m_faultData[idx].badData = 0;
  m_faultData[idx].reconnect = true;
  m_faultData[idx].pluggedStateTime = 0;

  if (m_faultData[idx].lastReadTime == 0 || ! connected() )
    m_faultData[idx].lastMsgTime = -1;
  else
    m_faultData[idx].lastMsgTime = m_faultData[idx].lastReadTime;

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("Controller"), F("m_initCriticalFault()"), F("Critical fault data reset."));
  #endif
}

// =================================
//      m_detectCriticalFault()
// =================================
bool Controller::m_detectCriticalFault(void)
{
  unsigned long currentTime;
  unsigned long lastMsgTime;
  unsigned long lagTime;
  
  if ( connected() ) {

    currentTime = millis();
    lastMsgTime = m_lastReadTime;
    lagTime = currentTime - lastMsgTime;

    // -----------------------
    // Check for reconnection.
    // -----------------------

    if (m_faultData[0].reconnect ) {
      if (lagTime < pTimings[iLagReconnect] ) {
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

    if ( lagTime > pTimings[iLagDisconnect] ) {

      #if defined(DEBUG)
      Debug.print(DBG_WARNING, F("Controller"), F("m_detectCriticalFault()"), F("Disconnecting due to lag time."));
      Debug.print(DBG_WARNING, F("  Current time:  "), (String)currentTime);
      Debug.print(DBG_WARNING, F("  Last msg time: "), (String)lastMsgTime);
      Debug.print(DBG_WARNING, F("  Lag:           "), (String)lagTime);
      #endif
      
      m_disconnect();
    }

    // -----------------------------------------
    // Stop the drive motors after too much lag.
    // -----------------------------------------

    if ( lagTime > pTimings[iLagKillMotor] ) {

      #if defined(DEBUG)
      Debug.print(DBG_WARNING, F("Controller"), F("m_detectCriticalFault()"), F("Stopping drive motors due to lag."));
      #endif      

      return true;  // The actual code to stop the motor is in loop() when read() fails.
    }

    // --------------------------
    // Check the PS3 signal data.
    // --------------------------

    if ( m_getUsbStatus() && ! m_getUsbStatus() ) {

      // The controller's signal is confused. We'll give it two tries
      // to clear up. The first time through starts the state machine.
      // The second time through trips the critical fault.

      if ( m_faultData[0].pluggedStateTime > 0 ) {

        // ------------------------------------------------------------
        // Has the desired amount of time between failed checks passed?
        // ------------------------------------------------------------

        unsigned long interval = ( connected() ? pTimings[iLongInterval] : pTimings[iShortInterval]);
        if ( currentTime > ( m_faultData[0].pluggedStateTime + interval )) {

          // -------------------------------------------------------
          // We have our second failed check.
          // Trigger the critical fault and reset the state machine.
          // -------------------------------------------------------

         m_faultData[0].badData++;
         m_faultData[0].pluggedStateTime = 0;

          #if defined(DEBUG)
          Debug.print(DBG_WARNING, F("Controller"), F("m_detectCriticalFault()"), F("Invalid data from primary controller."));
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

      #if defined(DEBUG)
      Debug.print(DBG_WARNING, F("Controller"), F("m_detectCriticalFault()"), F("Disconnecting due to excessive bad data."));
      #endif

      m_disconnect();
    }
  }

  return false;
}

/* ================================================================================
 *                                   Button Class
 * ================================================================================ */

// =====================
//      Constructor
// =====================
Button::Button(Controller* pController)
{
  m_controller = pController;
}

// ====================
//      Destructor
// ====================
Button::~Button(void) {}

// ============================
//      Get button actions
// ============================
bool Button::clicked(int buttonEnum)     { return m_controller->getButtonClick(buttonEnum); }
bool Button::pressed(int buttonEnum)     { return m_controller->getButtonPress(buttonEnum); }
byte Button::analogValue(int buttonEnum) { return m_controller->getAnalogButton(buttonEnum); }

/* ================================================================================
 *                                  Joystick Class
 * ================================================================================ */

// =====================
//      Constructor
// =====================
Joystick::Joystick(const byte argSide, const int argDeadZone, Controller* pController)
{
  m_controller = pController;
  side = argSide;
  deadZone = argDeadZone;
}

// ====================
//      Destructor
// ====================
Joystick::~Joystick(void) {}

// ===================
//      getSide()
// ===================
String Joystick::getSide(void)
{
  if ( side == 0 ) {
    return "Left";
  } else {
    return "Right";
  }
}

// ==================
//      m_getX()
// ==================
byte Joystick::m_getX(void)
{
  if ( side == 0 ) {
    return m_controller->getAnalogHat(LeftHatX);
  } else if ( side == 1 ) {
    return m_controller->getAnalogHat(RightHatX);
  } else {
    return center;
  }
}

// ==================
//      m_getY()
// ==================
byte Joystick::m_getY(void)
{
  if ( side == 0 ) {
    return m_controller->getAnalogHat(LeftHatY);
  } else if ( side == 1 ) {
    return m_controller->getAnalogHat(RightHatY);
  } else {
    return center;
  }
}

/* ================================================================================
 *                               Joystick_Drive Class
 * ================================================================================ */

// =====================
//      Constructor
// =====================
Joystick_Drive::Joystick_Drive(byte side, int deadZone, Controller* pController) : Joystick(side, deadZone, pController) {}

// ====================
//      Destructor
// ====================
Joystick_Drive::~Joystick_Drive(void) {}

// ====================
//      steering()
// ====================
int Joystick_Drive::steering(void)
{
  return m_getX();
}

// ====================
//      throttle()
// ====================
int Joystick_Drive::throttle(void)
{
  return m_getY();
}

/* ================================================================================
 *                                Joystick_Dome Class
 * ================================================================================ */

// =====================
//      Constructor
// =====================
Joystick_Dome::Joystick_Dome(byte side, int deadZone, Controller* pController) : Joystick(side, deadZone, pController) {}

// ====================
//      Destructor
// ====================
Joystick_Dome::~Joystick_Dome(void) {}

// ====================
//      rotation()
// ====================
byte Joystick_Dome::rotation(void)
{
  return m_getX();
}

#if defined(TEST_CONTROLLER)
/* ================================================================================
 *                            Test Controller Functions
 * ================================================================================ */

// ==========================
//      m_displayInput()
// ==========================
void Controller::m_displayInput(void)
{
  String output = "";

  if ( connectionStatus() != NONE ) {
    if ( button.hasBasePressed() ) {
      button.display(&output);
    }
    if ( (driveStick.side == 0 && ! button.pressed(L1)) ||
         (driveStick.side == 1 && ! button.pressed(R1)) ) {
      driveStick.display(&output);
    }
    if ( connectionStatus() == FULL ) {
      if ( (domeStick.side == 0 && ! button.pressed(L1)) ||
           (domeStick.side == 1 && ! button.pressed(R1)) ) {
        domeStick.display(&output);
      }
    }
  }

  Debug.print(DBG_VERBOSE, output);
}

// ==========================
//      hasBasePressed()
// ==========================
bool Button::hasBasePressed(void)
{
  bool out = false;  
  int list[12] = { L4, R4, L3, R3, UP, RIGHT, DOWN, LEFT, TRIANGLE, CIRCLE, CROSS, SQUARE };

  for (int i = 0; i < 12; i++) {
    if ( pressed(list[i]) ) {
      if ( i < 2 ) {
        // CREATE|SHARE|SELECT|OPTIONS|START|CROSS(PS3Nav)|CIRCLE(PS3Nav).
        // These are base buttons when combined with PS|PS2 or L2|R2.
        if ( pressed(PS) || pressed(PS2) || pressed(L2) || pressed(R2) ) {
          out = true;
        }
      } else {
        // All others are always base buttons.
        out = true;
      }
    }
  }

  return out;
}

// ===================
//      display()
// ===================
void Button::display(String* out)
{
  int list[18] = { L1, R1, L2, R2, PS, PS2, L4, R4, L3, R3, UP, RIGHT, DOWN, LEFT, TRIANGLE, CIRCLE, CROSS, SQUARE };
  for (int i = 0; i < 18; i++) {
    if ( pressed(list[i]) ) {
      m_appendString(out, (String)label(list[i]));
    }
  }
}

// ==========================
//      m_appendString()
// ==========================
void Button::m_appendString(String* pValue, const String addString)
{
  if ( pValue->length() > 0 ) {
    pValue->concat("+");
  }
  pValue->concat(addString);
}

// =================
//      label()
// =================
String Button::label(int buttonEnum)
{
  switch (buttonEnum) {
    case UP:       { return "UP"; }
    case RIGHT:    { return "RIGHT"; }
    case DOWN:     { return "DOWN"; }
    case LEFT:     { return "LEFT"; }
    case L3:       { return "L3"; }
    case R3:       { return "R3"; }
    case L2:       { return "L2"; }
    case R2:       { return "R2"; }
    case L1:       { return "L1"; }
    case R1:       { return "R1"; }
    case TRIANGLE: { return "TRIANGLE"; }
    case CIRCLE:   { return "CIRCLE"; }
    case CROSS:    { return "CROSS"; }
    case SQUARE:   { return "SQUARE"; }
    case PS:       { return "PS"; }
    case PS2:      { return "PS2"; }
    case 4: {
      switch (m_controller->getType()) {
        case 0: { return "CROSS";  break; }
        case 1: { return "SELECT"; break; }
        case 2: { return "SHARE";  break; }
        case 3: { return "CREATE"; break; }
        default: { return "UNKNOWN"; }
      }
    }
    case 5: {
      switch (m_controller->getType()) {
        case 0: { return "CIRCLE";  break; }
        case 1: { return "START";   break; }
        case 2: { return "OPTIONS"; break; }
        case 3: { return "OPTIONS"; break; }
        default: { return "UNKNOWN"; }
      }
    }
    default: { return "UNKNOWN"; }
  }
}

// ========================
//      abbreviation()
// ========================
String Button::abbreviation(int buttonEnum)
{
  switch (buttonEnum) {
    case UP:       { return "Up"; }
    case RIGHT:    { return "Rt"; }
    case DOWN:     { return "Dn"; }
    case LEFT:     { return "Lt"; }
    case L3:       { return "L3"; }
    case R3:       { return "R3"; }
    case L2:       { return "L2"; }
    case R2:       { return "R2"; }
    case L1:       { return "L1"; }
    case R1:       { return "R1"; }
    case TRIANGLE: { return "Tri"; }
    case CIRCLE:   { return "Cir"; }
    case CROSS:    { return "Cro"; }
    case SQUARE:   { return "Sq"; }
    case PS:       { return "PS"; }
    case PS2:      { return "PS2"; }
    case 4: {
      switch (m_controller->getType()) {
        case 0: { return "X";  break; }
        case 1: { return "Sl"; break; }
        case 2: { return "Sh"; break; }
        case 3: { return "Cr"; break; }
        default: { return "Unk"; }
      }
    }
    case 5: {
      switch (m_controller->getType()) {
        case 0: { return "O";   break; }
        case 1: { return "St";  break; }
        case 2: { return "Opt"; break; }
        case 3: { return "Opt"; break; }
        default: { return "Unk"; }
      }
    }
  }
}

// ===================
//      display()
// ===================
void Joystick::display(String* out)
{
  if ( (abs(m_getX() - center) >= deadZone) ||
       (abs(m_getY() - center) >= deadZone) ) {
    m_appendString(out);
  }
}

// ==========================
//      m_appendString()
// ==========================
void Joystick::m_appendString(String* pValue)
{
  if ( pValue->length() > 0 ) {
    pValue->concat(F("+"));
  }
  pValue->concat(getSide());
  pValue->concat(F(": "));
  pValue->concat(m_getX());
  pValue->concat(F(","));
  pValue->concat(m_getY());
}
#endif
