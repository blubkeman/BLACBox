/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings.h - Library for user settings
 * Created by Brian Lubkeman, 16 June 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef __BLACBOX_SETTINGS_H__
#define __BLACBOX_SETTINGS_H__


// ========================================
//            Controller Settings
// ========================================

// Uncomment one of the following options for the controller
// you will be using after uploading this sketch.

//#define PS3_NAVIGATION
//#define PS3_CONTROLLER
#define PS4_CONTROLLER
//#define PS5_CONTROLLER

const int controllerSettings[] = {
   0      // Drive stick side.       : Set to 0=left, or 1=right.
 , 1      // Dome stick side         : Set to 0=left, or 1=right (opposite of the drive stick side).
 , 8      // Joystick dead zone      : Set to an integer (recommended < 10).
 , 3      // Number of peripherals   : Set to 1, 2, or 3; Used when processing a disconnect event.
};

const unsigned long controllerTimings[] = {
   300    // Lag time to kill motors : Set to a time in milliseconds. Kill the drive motors when lag time exceeds this value.
 , 10000  // Lag time to disconnect  : Set to a time in milliseconds. Disconnect the controller when lag exceeds this value.
 , 200    // Lag time to reconnect   : Set to a time in milliseconds. Reconnect the controller when lag exceeds this value.
 , 15     // Plugged short interval  : Set to a time in milliseconds. Part of critical fault detection.
 , 100    // Plugged long interval   : Set to a time in milliseconds. Part of critical fault detection.
};


// PS4 Pairing instructions:
// Set the following constant to true.  Upload the sketch.  Open the serial monitor.
// You should see "Please enable discovery of your device".  Press SHARE then press PS.
// Pairing should be complete.  Reset the constant to false.  Upload the sketch.  Press PS.

#if defined(PS4_CONTROLLER) || defined(PS5_CONTROLLER)
const bool pair = false;       // Perform controller pairing
#endif

// ========================================
//           Drive System Settings
// ========================================

const int driveMotorSettings[] = {
   0      // Motor driver            : Set to 0=Roboteq SBL2360 or SBL1360, 1=Sabertooth (not yet supported).
 , 0      // Use dead man switch     : Set to 0=false, 1=true.
 , 25     // Serial latency (in ms)  : Set to 25 ms for HardwareSerial, 50+ ms for SoftwareSerial.
 , 7      // Servo dead zone         : Similar to joystick dead zone, but for the servo value range.
};

const byte driveMotorPins[] = {
   44     // Drive pin #1 : Pulse1/Throttle (Roboteq) or Left foot (Sabertooth).
 , 45     // Drive pin #2 : Pulse2/Steering (Roboteq) or Right foot (Sabertooth).
 , 46     // Script pin   : Pulse3/Script (Roboteq only).
 , 47     // Deadman pin  : Digital pin for dead man switch.
};

const byte roboteqSettings[] = {
   0      // Roboteq communication mode : Set to 0=Pulse, 1=RS232 (Serial, not yet supported).  
 , 0      // Tank-style drive mixing    : Set to 0=false (for SBL2360), 1=true (for SBL1360).
};

const int sabertoothSettings[] = {
   70     // Drive speed, normal    : Set to a value between 0=stop, 127=full speed.
 , 110    // Drive speed, overdrive : Set to a value between 0=stop, 127=full speed.
 , 50     // Turn speed.            : Recommend beginner: 40 to 50, experienced: 50+
          //                           Higher values spin in place faster. Lower values are easier to control.
 , 1      // Ramping                : Lower values take longer to speed up. Change this by increments of 1.
 , 128    // Sabertooth address     : Values 128-135 allowed.  128 is typical.
 , 1      // Rotation inversion     : Set to -1 if you need to invert the rotation direction.
};


// ========================================
//              Dome Settings
// ========================================

const byte domeMotorSettings[] = {
   0      // Motor driver           : Set to 0=Syren10, no other driver supported yet.
 , 100    // Manual speed           : When sending to the dome motor driver over serial, set this to a number up to 127.
 , 49     // Automated speed min    : Minimum auto dome speed to allow automation to run.
 , 101    // Automated speed max    : Maximum auto dome speed to allow automation to run.
 , 100    // Automated speed        : Set to a number between the minimum and maximum allowed values.
 , 25     // Serial latency (in ms) : 25 ms for HardwareSerial, 50+ ms for SoftwareSerial.
};

const unsigned long domeMotorTimings[] = {
   1999   // Time for 360 turn min  :  Set to a time in milliseconds. Minimum time allowed for dome to turn a full 360 degrees.
 , 8001   // Time for 360 turn max  :  Set to a time in milliseconds. Maximum time allowed for dome to turn a full 360 degrees.
 , 2000   // Time for 360 turn      :  Set to a time between the minimum and maximum allowed values.
};

const int syrenSettings[] = {
   129    // Syren10 address.    :  Values 128-135 allowed.  129 is typical.
};

// ========================================
//            Marcduino Settings
// ========================================

const byte marcduinoSettings[] = {
   0    // FX control system           : Set to 0=Marcduino, no other system supported yet.
 , 1    // Marcduino body master used  : Set to 0=false, 1=true.
 , 1    // Marcduino sound master      : Set to 0=dome, 1=body.
 , 0    // Marcduino command set       : Set to 0=standard SHADOW+MD, 1=custom.
 , 0    // Sound board                 : Set to 0=Sparkfun MP3 Trigger, 1=CF III.
 , 0    // Magic panel used            : Set to 0=false, 1=true.
 , 2    // Body panels on servos       : Set to a value between 0 and 10.
 , 10   // Dome panels on servos       : Set to a value between 0 and 10.
 , 3    // Holoprojectors on servos    : Set to a value between 0 and 3.
 , 2    // HP automation delay min     : Set to a time in seconds. Minimum time between automated holoprojector movements.
 , 10   // HP automation delay max     : Set to a time in seconds. Maximum time between automated holoprojector movements.
 , 0    // Feather radio used          : Set to 0=false, 1=true.
};

#endif
