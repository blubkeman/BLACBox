/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Security.h - A list of authorized MAC addresses for the user's controllers
 * Created by Brian Lubkeman, 23 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 * =================================================================================
 *
 * BLACBox requires that you identify the controllers that are authorized to control
 *  the astromech. This is a safety precaution to ensure that no unauthorized user
 *  gets control of your droid.
 * BLACBox does not limit the number of controllers you can authorize. 
 *  It does not preassign PS3 Navigation controllers as "primary" or "secondary". 
 *  Whichever connects first is primary. The next one after that is secondary.
 */

#ifndef _SETTINGS_MAC_H_
#define _SETTINGS_MAC_H_

#include <Arduino.h>

/* ---------- Instructions ----------
 * With any given controller you want to authorize, put it through the pairing
 *  procedure as normal with the Arduino connected to a PC/laptop. The debugging
 *  lines will display the MAC address of the device. Note this address.
 * Work within the appropriate section below that represents the controller type.
 *  Create an array for each unique MAC address. Follow the examples.
 *  Update the NUMBER_OF_MAC_ADDRESSES to match the number of devices you've identified.
 *  Lastly, list each array name in the authorizedMACAddresses array.
 * ---------------------------------- */

// =================================================================
#if defined(PS3_NAVIGATION)

const char addr1[] PROGMEM = "00:06:F7:B8:57:01";
const char addr2[] PROGMEM = "E0:AE:5E:38:E0:CD";

const byte NUMBER_OF_MAC_ADDRESSES = 2;

const char * authorizedMACAddresses[NUMBER_OF_MAC_ADDRESSES] = { addr1, addr2 };

// =================================================================
#elif defined(PS3_CONTROLLER)

const char addr1[] PROGMEM = "38:C0:96:DD:15:DD";

const byte NUMBER_OF_MAC_ADDRESSES = 1;

const char * authorizedMACAddresses[NUMBER_OF_MAC_ADDRESSES] = { addr1 };

// =================================================================
#elif defined(PS4_CONTROLLER)

const char addr1[] PROGMEM = "4C:B9:9B:21:63:3E";

const byte NUMBER_OF_MAC_ADDRESSES = 1;

const char * authorizedMACAddresses[NUMBER_OF_MAC_ADDRESSES] = { addr1 };

// =================================================================
#endif

#endif
