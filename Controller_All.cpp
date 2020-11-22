/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_All.cpp - Library for parent class of drive motor controllers
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Controller_All.h"


/* ===========================================================================
 *                          Controller_Parent class functions
 * =========================================================================== */

// =====================
//      Constructor
// =====================
Controller_Parent::Controller_Parent(Buffer * pBuffer) : _Usb(), _Btd(&_Usb)
{
  _buffer = pBuffer;

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  _className = F("Controller_PS3Nav::");
  #endif
}

// ====================
//      Destructor
// ====================
Controller_Parent::~Controller_Parent() {}

// =================
//      begin()
// =================
void Controller_Parent::begin()
{
  // -------------------
  // Start the USB host.
  // -------------------

  if (_Usb.Init() == -1) {
    Serial.println(F("OSC did not start"));
    while (1); //halt
  }
  Serial.println(F("Bluetooth Library Started"));
}

// =======================
//      _authorized()
// =======================
bool Controller_Parent::_authorized(char * pAddresses[], uint8_t arraySize)
{
  bool authorized = false;
  String btAddress = "";
  
  for(int8_t i = 5; i >= 0; i--) {
    if (btAddress.length() > 0) btAddress +=(":");
    if (_Btd.disc_bdaddr[i]<0x10) btAddress +="0";
    btAddress += String(_Btd.disc_bdaddr[i], HEX);
  }
  btAddress.toUpperCase();

  #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
  output = _className+F("_getMACAddress()");
  output += F(" - MAC address: ");
  output += btAddress;
  printOutput();
  #endif

  for (uint8_t i = 0; i < arraySize; i++) {
    if (btAddress == pAddresses[i]) {
      authorized = true;
      break;
    }
  }

  return authorized;
}
