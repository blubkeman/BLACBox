/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings.h - Library for user settings
 * Created by Brian Lubkeman, 5 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef __BLACBOX_SETTINGS_H__
#define __BLACBOX_SETTINGS_H__


// ========================================
//            Controller Settings
// ========================================

const int controllerSettings[] = {
   2      // controller type         : 0=PS3 Nav, 1=PS3, 2=PS4, 3=PS5
 , 0      // drive stick side        : 0=left; 1=right
 , 1      // dome stick side         : 0=left; 1=right
 , 8      // joystick dead zone      : joystick center +/- this number will be treated as center
 , 300    // lag time to kill motors : Kill motors when lag exceeds 0.3 seconds.
 , 10000  // lag time to disconnect  : Disconnect controller when lag exceeds 10 seconds.
 , 200    // lag time to reconnect   : Reconnect controller when lag exceed 0.2 seconds.
 , 15     // plugged short interval  : Part of critical fault detection.
 , 100    // plugged long interval   : Part of critical fault detection.
 , 3      // number of peripherals   : 1-3; Used when processing a disconnect event
};

// ========================================
//           Drive System Settings
// ========================================

const int driveMotorSettings[] = {
  0,  // Motor driver               : 0=Roboteq SBL2360 or SBL1360, 1=Sabertooth
  0,  // Roboteq communication mode : 0=Pulse, 1=RS232 (Serial)
  1,  // Use dead man switch        : 0=false, 1=true
  25, // Serial latency (in ms)     : 25 ms for HardwareSerial, 50+ ms for SoftwareSerial
  7,  // Servo dead zone            : Similar to joystick dead zone, but for the servo numerical range
  0   // Tank-style drive mixing    : 0=false (for SBL2360), 1=true (for SBL1360 or Sabertooth)
};

// Set your pins here. (PWM pins on the Arduino Mega include pins 2-13 and 44-46.)

const byte driveMotorPins[] = {
  44,     // Drive pin #1 - Pulse1/Throttle (Roboteq) or Left foot (Sabertooth).
  45,     // Drive pin #2 - Pulse2/Steering (Roboteq) or Right foot (Sabertooth).
  46,     // Script pin   - Pulse3/Script (Roboteq only).
  47      // Deadman pin  - Digital pin for dead man switch.
};

// ========================================
//              Dome Settings
// ========================================

const byte domeMotorSettings[] = {
  0,      // Motor driver  :  0=Syren10, no other driver supported yet
  100,    // If using a speed controller for the dome, this sets the top speed. Use a number up to 127 for serial.
  100,    // Speed used when dome automation is active (1-127)
  49,     // Minimum auto dome speed to allow automation to run.
  101,    // Maximum auto dome speed to allow automation to run.
  1,      // Invert turn direction. Set to -1 if inversion is needed.
  25      // Serial latency (in ms)  :  25 ms for HardwareSerial, 50+ ms for SoftwareSerial
};

const unsigned long domeMotorTimings[] = {
  2000,   // Milliseconds for dome to complete 360 turn at m_settings[iAutoSpeed]
  1999,   // Minimum time allowed for dome to complete a full 360 turn.
  8001    // Maximum time allowed for dome to complete a full 360 turn.
};

const int syrenSettings[] = {
  129,    // Syren10 address. : Values 128-135 allowed.  129 is typical.
  9600    // Syren10 baud rate.  DO NOT CHANGE THIS.
};

// ========================================
//            Marcduino Settings
// ========================================

const byte marcduinoSettings[] = {
  0,    // FX control system           : 0=Marcduino, no other system supported yet
  1,    // Marcduino body master used  : 0=false, 1=true
  1,    // Marcduino sound master      : 0=dome, 1=body
  0,    // Marcduino command set       : 0=standard SHADOW+MD, 1=custom
  0,    // Sound board                 : 0=Sparkfun MP3 Trigger, 1=CF III
  0,    // Magic panel used            : 0=false, 1=true
  2,    // Body panels on servos       : 
  10,   // Dome panels on servos       : 
  3,    // Holoprojectors on servos    : 
  2,    // HP automation delay min     : Specify in seconds. Minimum time between automated holoprojector movements.
  10,   // HP automation delay max     : Specify in seconds. Maximum time between automated holoprojector movements.
  0     // Feather radio used          : 0=false, 1=true
};

#endif
