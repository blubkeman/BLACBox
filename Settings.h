/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings.h - Library for user settings
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <Arduino.h>


/* ==================================================
 *                 Enable/Disable modes
 * ================================================== */
#define DEBUG_ALL         // For debugging everything. Uncomment to enable debug mode. Comment out to disable.
#define TEST_CONTROLLER   // Uncomment to enable test mode for the controller. Comment out to disable.
//#define DEBUG_BUFFER      // For debugging the buffer. Uncomment to enable debug mode. Comment out to disable.
//#define DEBUG_CONTROLLER  // For debugging the controller. Uncomment to enable debug mode. Comment out to disable.
//#define DEBUG_MARCDUINO   // For debugging the Marcduino. Uncomment to enable debug mode. Comment out to disable.
//#define DEBUG_DOME        // For debugging the dome motor. Uncomment to enable debug mode. Comment out to disable.
//#define DEBUG_FOOT        // For debugging the foot motors. Uncomment to enable debug mode. Comment out to disable.

//#define DEBUG_CRAZYIVAN   // For debugging the crazyIvan event (not included in DEBUG_ALL). Uncomment to enable debug mode. Comment out to disable.


/* =============================================
 *           Configure your controller
 * ============================================= */
// Uncomment the line that best represents the controller you will be using.
// Make certain all other lines are commented out.

#define PS3_NAVIGATION      // Using one or two PS3 Move Navigation controllers.
// #define PS3_CONTROLLER     // TODO: Future implementation.  Using one PS3 controller.
// #define PS4_CONTROLLER     // TODO: Future implementation.  Using one PS4 controller.

// Step through the process to pair your controller(s) to the Bluetooth dongle.
// (See directions in the wiki found at astromech.net.) Note the MAC address
// of each controller. Edit the Controller_PS3Nav.cpp tab listing each MAC
// address in the array of authorized addresses.

// If you have a relay module controlling your foot motor controller
// then uncomment this line to enable the deadman code.

#define DEADMAN
const uint8_t DEADMAN_PIN = 46;

// Getting the joystick to rest perfect centered isn't always possible.
// Define the joystick's center point and a small range beyond that
// which we'll treat as center.

const uint8_t JOYSTICK_CENTER = 127;
const uint8_t JOYSTICK_CENTER_OFFSET = 20;

// One of my controllers frequently returns joystick coordinates 0,0 when
// the stick has not been touched. I'm calling this event a crazyIvan.
// The following constant is meant to adjust the sensitivity.

#ifdef PS3_NAVIGATION
const uint8_t CRAZYIVAN_THRESHOLD = 50;
#endif


/* =============================================
 *           Configure fault detection
 * ============================================= */

// The following constants should be in milliseconds.

const int LAG_TIME_TO_KILL_MOTORS  = 300;
const int LAG_TIME_TO_DISCONNECT   = 10000;
const int LAG_TIME_TO_RECONNECT    = 200;

// Critical fault detection includes reviewing the controllers status for 'plugged' and 'unplugged'.
// We allow two attempts to get a clear update. I am using a state machine (a flag and timer) instead
// of the delay() command to control when a second attempt is made. The following constants determine
// how much time should pass between attempts based on the presence or absence of another controller.

const int PLUGGED_STATE_INTERVAL_ALONE = 15;  // This is the interval when no other controller is connected
const int PLUGGED_STATE_INTERVAL_OTHER = 100; // This is the interval when another controller is connected.


/* ==================================================
 *                  Active Peripherals
 * ================================================== */
// This is largely used for enabling/disabling broad functions during code development.
// Once in production, these definitions will likely be removed.

// Uncomment the elements you have running in your astromech.

//#define DOME               // Uncomment this when you have connected the foot motors and controller(s).
//#define DRIVE              // Uncomment this when you have connected the dome motor.
#define MARCDUINO          // Uncomment this when you have at least one Marcduino master.
#define MD_STD_CONTROLS    // Uncomment this when you use the standard SHADOW_MD controls.
//#define MD_CUSTOM_CONTROLS // Uncomment this when you use your own custom Marcduino controls. (Advanced setting)
#define MD_BODY_MASTER     // Uncomment this if you are using the optional Marcduino body master.
#define MD_BODY_SOUND      // Uncomment this if sound is sent to the body master.


/* =================================================
 *                 Serial Baud Rates
 * ================================================= */
const int MARCDUINO_BAUD_RATE = 9600; // Do not change this!
const int MOTOR_BAUD_RATE     = 9600; // Set the baud rate for the Syren motor controller, also used by Sabertooth,
                                      // for packetized options are: 2400, 9600, 19200 and 38400


/* =================================================
 *            Configure your sound options
 * ================================================= */
#ifndef MARCDUINO
// The following settings are intended for use with a SparkFun Qwiic MP3 Trigger.
// I may attempt to implement other sound systems in the future, but not now.

// Choose which sound track on the SD card will be played on startup.

const uint8_t STARTING_VOLUME = 2;
const uint8_t STARTUP_SOUND = 255;

// Set up random sound automation.  You can choose the time between
// random sounds (delay min and max) and a range of tracks on the
// SD card to be played (track min and max).

const uint8_t AUTO_SOUND_DELAY_MIN = 3;   // in seconds
const uint8_t AUTO_SOUND_DELAY_MAX = 25;  // in seconds
const uint8_t AUTO_SOUND_TRACK_MIN = 50;  // track #
const uint8_t AUTO_SOUND_TRACK_MAX = 110; // track #

// When the left DPad buttons (up, right, down, left) are used
// along with L1, a random track is triggered.  You can choose a
// range of tracks on the SD card to be played with each combo.

const uint8_t RANDOM_SOUND_UP_MIN    = 50;  // 50-82 = Chatter sounds
const uint8_t RANDOM_SOUND_UP_MAX    = 82;
const uint8_t RANDOM_SOUND_RIGHT_MIN = 83;  // 83-110 = Idle sounds
const uint8_t RANDOM_SOUND_RIGHT_MAX = 110;
const uint8_t RANDOM_SOUND_DOWN_MIN  = 111; // 111-148 = Acknowledge, Action Completed, Engage, Hacking Progress
const uint8_t RANDOM_SOUND_DOWN_MAX  = 148;
const uint8_t RANDOM_SOUND_LEFT_MIN  = 149; // 149-226 = Alert, Attack, Hurt, Pain, Shock, Threat Warning
const uint8_t RANDOM_SOUND_LEFT_MAX  = 226;
#endif


/* ======================================================
 *            Configure your PWM driver options
 * ====================================================== */
#ifndef MARCDUINO
// In theory the internal oscillator is 25MHz but it really isn't
// that precise. You can 'calibrate' by tweaking this number till
// you get the frequency you're expecting!
const int OSCILLATOR_FREQ = 27000000;

const int SERVO_FREQ = 50; // Analog servos run at ~50 Hz updates

const int SERVOMIN = 150;  // This is the 'minimum' pulse length count (out of 4096)
const int SERVOMAX = 600;  // This is the 'maximum' pulse length count (out of 4096)
#endif


/* =======================================================
 *            Configure your dome panel settings
 * ======================================================= */
#ifdef MARCDUINO
const uint8_t NUMBER_OF_DOME_PANELS = 10; // Specify how many dome panels are in use
#endif


/* =======================================================
 *            Configure your body panel settings
 * ======================================================= */
#ifdef BODY_PANELS
const uint8_t NUMBER_OF_BODY_PANELS = 10; // Specify how many body panels are in use.
#endif


/* ======================================================
 *            Configure your dome motor options
 * ====================================================== */
// These are fairly advanced settings.  Recommended: Do not tinker with these.

const int SYREN_ADDR = 129;    // Serial Address for Dome Syren

const byte DOME_SPEED = 100;   // If using a speed controller for the dome, sets the top speed
                               // Use a number up to 127 for serial

const byte I2C_DOME_ADDRESS = 0x01;

const byte JOYSTICK_DOME_DEAD_ZONE_RANGE = 10;  // For controllers that centering problems, use the lowest number with no drift

const int INVERT_TURN_DIRECTION = -1;   // This may need to be set to 1 for some configurations
const byte DOME_AUTO_SPEED = 127;       // Speed used when dome automation is active (1- 127)
const int TIME_360_DOME_TURN_LEFT = 1000; // milliseconds for dome to complete 360 turn at DOME_AUTO_SPEED
const int TIME_360_DOME_TURN_RIGHT = 300; // milliseconds for dome to complete 360 turn at DOME_AUTO_SPEED
                                          // Cut in half to reduce spin.  Offset for different rotation startups due to gearing.
const int SERIAL_LATENCY = 25;  //This is a delay factor in ms to prevent queueing of the Serial data.
                                //25ms seems appropriate for HardwareSerial, values of 50ms or larger are needed for Softare Emulation


/* =========================================================
 *            Configure your foot motor options
 * ========================================================= */
// Set the following variable according to which system you use for your foot motor controller.

const int FOOT_CONTROLLER = 1;    // 0 = Sabertooth Serial
                                  // 1 = R/C output; Roboteq SBL2360 uses this
                                  // 2 = R/C output with remixing; Roboteq SBL1360 uses this
// R/C uses Servo objects _leftFootSignal (pin 44 aka throttle) and _rightFootSignal (pin 45 aka direction)

// The following settings are fairly advanced.  Tinker with them only after you've
// studied the code to learn their use, or have talked with members for support.

const byte DRIVESPEED1 = 25;  // Set these to whatever speed work for you. 0-stop, 127-full speed.
const byte DRIVESPEED2 = 65;  // Recommend beginner: 50 to 75, experienced: 100 to 127.

//
// The following are specific to the Sabertooth.
//
const int SABERTOOTH_ADDR = 128;       // Serial address for foot Sabertooth

const byte TURN_SPEED = 75; //50; // Set this to whatever speed work for you. 0-stop, 127-full speed.
                                  // The higher this number the faster it will spin in place, lower - easier to control.
                                  // Recommend beginner: 40 to 50, experienced: 50 and up, I like 75

const byte RAMPING = 6; //3;  // The lower this number the longer R2 will take to speedup
                              // or slow down, change this by increments of 1

const byte JOYSTICK_FOOT_DEAD_ZONE_RANGE = 15; // For controllers that have centering problems,
                                               // use the lowest number with no drift

const byte DRIVE_DEAD_BAND_RANGE = 10;         // Used to set the Sabertooth DeadZone for foot motors

//
// The following are specific to RC controllers.
//
const int LEFT_FOOT_PIN   = 44;  // connect this pin to motor controller for left foot (R/C mode)
const int RIGHT_FOOT_PIN  = 45;  // connect this pin to motor controller for right foot (R/C mode)
const int STEERING_FACTOR = 100; // The larger SteeringFactor is the less senstitive steering is...
                                 // Smaller values give more accuracy in making fine steering corrections
                                 // XDist*sqrt(XDist+SteeringFactor)
//    const int LEFT_DIRECTION  = 1;   // change this if your motor is spinning the wrong way
//    const int RIGHT_DIRECTION = 0;   // change this if your motor is spinning the wrong way


#endif
