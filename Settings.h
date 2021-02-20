/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings.h - Library for user settings
 * Created by Brian Lubkeman, 20 February 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _SETTINGS_H_
#define _SETTINGS_H_


// ========================================
//           Debug & Testing Modes
// ========================================
#define DEBUG               // Uncomment to enable Serial monitor debugging.
//#define TEST_CONTROLLER     // Uncomment to enable display of controller buttons pressed.


// ========================================
//            Controller Settings
// ========================================

// Uncomment one of the following lines that represent
// which type of controller you will be using.

#define PS4_CONTROLLER
//#define PS3_CONTROLLER
//#define PS3_NAVIGATION

// Update this constant with the number of peripherals
// that have been enabled. The drive system, dome motor,
// and Marcduino system are the current peripherals.

const byte NUMBER_OF_PERIPHERALS = 2;

// ========================================
//           Drive System Settings
// ========================================

// Uncomment one of the following lines that represents the Roboteq
// motor controller you will be using for the drive system.

#define SBL2360
//#define SBL1360

// Uncomment one of the following lines that represents the
// input mode of the Roboteq motor controller.

#define PULSE
//#define RS232

// Uncomment the following line if you are using a dead
// man switch with the drive system.

//#define DEADMAN

// Choose which stick controls your drive system.
// Configure the following ONLY when using a PS3 or PS4 controller.
// DO NOT configure the following when using a PS3 Move Navigation.

const byte DRIVE_STICK = 0;     // This is the left stick (or PS3 Nav primary stick).
//const byte DRIVE_STICK = 1;   // This is the right stick (or PS3 Nav optional stick).

// ========================================
//           Dome Motor Settings
// ========================================

// Currently, only the Syren10 motor controller is supported.
// Comment this line out only if you do not use a dome motor.

#define SYREN10

// Modify these settings as appropriate for your system.

const byte DOME_SPEED = 100;              // If using a speed controller for the dome, this sets the top speed
                                          // Use a number up to 127 for serial.
const int TIME_360_DOME_TURN = 4000;      // Milliseconds for dome to complete 360 turn at DOME_AUTO_SPEED
const int TIME_360_DOME_TURN_MIN = 1999;  // Minimum time allowed for dome to complete a full 360 turn.
const int TIME_360_DOME_TURN_MAX = 8001;  // Maximum time allowed for dome to complete a full 360 turn.
                                          // Cut in half to reduce spin.  Offset for different rotation startups due to gearing.
const byte DOME_AUTO_SPEED = 100;         // Speed used when dome automation is active (1-127)
const byte DOME_AUTO_SPEED_MIN = 49;      // Minimum auto dome speed to allow automation to run.
const byte DOME_AUTO_SPEED_MAX = 101;     // Maximum auto dome speed to allow automation to run.

//const int INVERT_TURN_DIRECTION = -1;     // This may need to be set to 1 for some configurations

// ========================================
//            Marcduino Settings
// ========================================

// Uncomment the following line if you are using
// at least one Marcduino master board.

#define MARCDUINO

// Uncomment the following line if your build
// includes the optional body master board.

#if defined(MARCDUINO)
#define MD_BODY_MASTER
#endif

// Uncomment the following line if your build includes the
// optional body master board, and you use it to handle sound.

#if defined(MD_BODY_MASTER)
#define MD_BODY_SOUND
#endif

// Identify how many dome panels have the ability to be
// opened/closed. Marcduino supports up to 10 panels maximum.

const byte NUMBER_OF_DOME_PANELS = 10;

// ========================================
//          Miscellaneous Settings
// ========================================

// Set your pins here.

#if ( (defined(SBL2360) || defined(SBL1360)) && defined(PULSE) )
const byte PULSE1_PIN  = 44;
const byte PULSE2_PIN  = 45;      // PWM pins on the Arduino Mega include pins 2-13 and 44-46.
const byte SCRIPT_PIN  = 46;
#elif defined(SABERTOOTH)
const byte LEFTFOOT_PIN  = 44;
const byte RIGHTFOOT_PIN = 45;
#endif
const byte DEADMAN_PIN = 47;



const int SERIAL_LATENCY = 25;  //This is a delay factor in ms to prevent queueing of the Serial data.
                                //25ms seems appropriate for HardwareSerial, values of 50ms or larger are needed for Softare Emulation

// ==========================================
// Please do not change any of the following.
// ==========================================

// Speed profiles.

const byte WALK =   0;   // MaxSpeed=1400, Accel=12000, Decel=22000
const byte JOG  =   1;   // MaxSpeed=1600, Accel=16000, Decel=30000
const byte RUN  =   2;   // MaxSpeed=2400, Accel=16000, Decel=36000
const byte SPRINT = 3;   // MaxSpeed=3000, Accel=16000, Decel=40000

// Joystick positions.

const int JOYSTICK_MIN       = 0;
const int JOYSTICK_CENTER    = 128;
const int JOYSTICK_MAX       = 256;
const int JOYSTICK_DEAD_ZONE = 8;

// Servo positions.

const int SERVO_MIN       = 0;
const int SERVO_CENTER    = 90;
const int SERVO_MAX       = 180;
const int SERVO_DEAD_ZONE = 7;

// Controller connection statuses.

const byte NONE = 0;
const byte HALF = 1;  // Applies only to PS3 Move Navigation controllers.
const byte FULL = 2;
#endif
