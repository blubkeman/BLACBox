/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Security.h - A list of authorized MAC addresses for the user's controllers
 * Created by Brian Lubkeman, 16 June 2021
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

#ifndef __BLACBOX_SECURITY_H__
#define __BLACBOX_SECURITY_H__

#include <Arduino.h>

/* ---------- Instructions ----------
 * Connect the Arduino Mega to a computer running the Arduino IDE.
 * Step through the pairing procuedure.
 * With any given controller you want to authorize, go through the pairing
 *  procedure as normal. with the Arduino connected to a PC/laptop. The debugging
 *  lines will display the MAC address of the device. Note this address.
 *  
 * Add each unique MAC address follow the examples below.
 *  List each address to the authorizedMACAddresses array.
 * ---------------------------------- */

// =================================================================

const char ps3nav_addr1[] PROGMEM = "xx:xx:xx:xx:xx:xx";
const char ps3nav_addr2[] PROGMEM = "yy:yy:yy:yy:yy:yy";
const char ps3_addr1[]    PROGMEM = "zz:zz:zz:zz:zz:zz";
const char ps4_addr1[]    PROGMEM = "aa:aa:aa:aa:aa:aa";

const char * authorizedMACAddresses[] = {
  ps3nav_addr1,
  ps3nav_addr2,
  ps3_addr1,
  ps4_addr1
};
#endif
