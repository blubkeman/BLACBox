/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller.cpp - Library for supported controllers
 * Created by Brian Lubkeman, 23 March 2021
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
Controller_Wrapper::Controller_Wrapper(void) : m_Usb(), m_Btd(&m_Usb)
{
  m_controllerConnected = NONE;
  m_disconnectCount = 0;

  #ifdef DEBUG
  m_className = "Controller_Wrapper::";
  #endif
}

// ====================
//      Destructor
// ====================
Controller_Wrapper::~Controller_Wrapper(void) {}

// =================
//      begin()
// =================
void Controller_Wrapper::begin(void)
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

// ============================
//      connectionStatus()
// ============================
byte Controller_Wrapper::connectionStatus(void)
{
  return m_controllerConnected;
}

// ============================
//      duringDisconnect()
// ============================
bool Controller_Wrapper::duringDisconnect(void)
{
  return (m_disconnectCount > 0 ? true : false);
}

// =================================
//      continueDisconnecting()
// =================================
void Controller_Wrapper::continueDisconnecting(void)
{
  // ----------------------------------------------------------------------------------
  // In loop(), each peripheral is given an opportunity to act on the disconnect event.
  // This function tracks how many peripherals have acted, and resets the count after
  // all have done so.
  // ----------------------------------------------------------------------------------

  if ( m_disconnectCount > NUMBER_OF_PERIPHERALS ) {
    m_disconnectCount = 0;
    return;
  }
  m_disconnectCount++;
}

// ========================
//      m_authorized()
// ========================
bool Controller_Wrapper::m_authorized(void)
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

  // ----------
  // Debugging.
  // ----------

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
//      m_setConnectionStatus()
// =================================
void Controller_Wrapper::m_setConnectionStatus(byte status)
{
  // -----------------------------------------------------------------------
  // Input: NONE (0), HALF (1) or FULL (2)
  // When we lose the controller, start the continueDisconnecting counter.
  // In loop(), each peripheral will be given a chance to act on this event.
  // -----------------------------------------------------------------------
  
  if ( m_controllerConnected != NONE && status == NONE ) {
    m_disconnectCount = 1;
  }
  m_controllerConnected = status;  
}

// ===============================
//      m_initCriticalFault()
// ===============================
void Controller_Wrapper::m_initCriticalFault(byte idx)
{
  m_faultData[idx].badData          = 0;
  m_faultData[idx].lastMsgTime      = millis();
  m_faultData[idx].pluggedStateTime = 0;
  m_faultData[idx].reconnect        = true;

  // ----------
  // Debugging.
  // ----------

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
void Controller_Wrapper::m_resetCriticalFault(byte idx)
{
  m_faultData[idx].badData = 0;
  m_faultData[idx].reconnect = true;
  m_faultData[idx].pluggedStateTime = 0;

  if (m_faultData[idx].lastReadTime == 0 || ! connected() )
    m_faultData[idx].lastMsgTime = -1;
  else
    m_faultData[idx].lastMsgTime = m_faultData[idx].lastReadTime;

  // ----------
  // Debugging.
  // ----------

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
bool Controller_Wrapper::m_detectCriticalFault(void)
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
      output += F("Disconnecting due to lag time.");  output += F("\n");
      output += F("  Current time:  ");  output += currentTime;  output += F("\n");
      output += F("  Last msg time: ");  output += lastMsgTime;  output += F("\n");
      output += F("  Lag:           ");  output += lagTime;    
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

    if ( m_getUsbStatus() && ! m_getUsbStatus() ) {

      // The controller's signal is confused. We'll give it two tries
      // to clear up. The first time through starts the state machine.
      // The second time through trips the critical fault.

      if ( m_faultData[0].pluggedStateTime > 0 ) {

        // ------------------------------------------------------------
        // Has the desired amount of time between failed checks passed?
        // ------------------------------------------------------------

        unsigned long interval = ( connected() ? PLUGGED_STATE_LONG_INTERVAL : PLUGGED_STATE_SHORT_INTERVAL);
        if ( currentTime > ( m_faultData[0].pluggedStateTime + interval )) {

          // -------------------------------------------------------
          // We have our second failed check.
          // Trigger the critical fault and reset the state machine.
          // -------------------------------------------------------

         m_faultData[0].badData++;
         m_faultData[0].pluggedStateTime = 0;

          #ifdef DEBUG
          output = m_className+F("m_detectCriticalFault()");
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
      output += F(" - ");
      output += F("Disconnecting due to excessive bad data.");
      #endif

      m_disconnect();
    }
  }

  return false;
}


// =============================
//      getButtonsPressed()
// =============================
int Controller_Wrapper::getButtonsPressed(void)
{
  int baseButton = -1;

  // ------------------------------
  // Look for pressed base buttons.
  // Stop when no base button is pressed.
  // ------------------------------

  if ( getButtonClick(UP) )            { baseButton = 0; }
  else if ( getButtonClick(RIGHT) )    { baseButton = 1; }
  else if ( getButtonClick(DOWN) )     { baseButton = 2; }
  else if ( getButtonClick(LEFT) )     { baseButton = 3; }
  else if ( getButtonClick(TRIANGLE) ) { baseButton = 4; }
  else if ( getButtonClick(CIRCLE) )   { baseButton = 5; }
  else if ( getButtonClick(CROSS) )    { baseButton = 6; }
  else if ( getButtonClick(SQUARE) )   { baseButton = 7; }
  else { return baseButton; }

  // ----------------------------
  // Return the calculated index.
  // ----------------------------

  return ( baseButton + m_getModifierButtons() );

}


#ifdef TEST_CONTROLLER
/* ================================================================================
 *                                 Testing Functions
 * ================================================================================ */

// ==========================
//      m_displayInput()
// ==========================
void Controller_Wrapper::m_displayInput()
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
    #if defined(PS5_CONTROLLER)
    btnLabel = "CREATE";
    #elif defined(PS4_CONTROLLER)
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
    #if defined(PS4_CONTROLLER) || defined(PS5_CONTROLLER)
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
String Controller_Wrapper::m_appendString(String inString, String addString)
{
  if ( inString != "" ) { inString += F("+"); }
  inString += addString;
  return inString;
}

// ==========================
//      m_displayStick()
// ==========================
void Controller_Wrapper::m_displayStick(String s, byte x, byte y)
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
void Controller_Wrapper::m_scrollInput()
{
  if ( ! connected() ) {
    return;
  }

  /* 
   * PS5 Controller = LX:x LY:x Up:x Rt:x Dn:x Lt:x L3:x L2:x L1:x PS:x Cr:x Op:x RX:x RY:x Tr:x Ci:x Cr:x Sq:x R3:x R2:x R1:x
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

  #if defined(PS5_CONTROLLER)
  output += F(" Cr:");  output += getButtonPress(4);
  output += F(" Op:");  output += getButtonPress(5);
  #elif defined(PS4_CONTROLLER)
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
