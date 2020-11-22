/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings.h - Library for user settings
 * Created by Brian Lubkeman, 22 November 2020
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
//#define TEST_CONTROLLER   // Uncomment to enable test mode for the controller. Comment out to disable.
//#define DEBUG_BUFFER      // For debugging the buffer. Uncomment to enable debug mode. Comment out to disable.
//#define DEBUG_CONTROLLER  // For debugging the controller. Uncomment to enable debug mode. Comment out to disable.
//#define DEBUG_MARCDUINO   // For debugging the Marcduino. Uncomment to enable debug mode. Comment out to disable.
//#define DEBUG_DOME        // For debugging the dome motor. Uncomment to enable debug mode. Comment out to disable.
//#define DEBUG_DRIVE        // For debugging the drive motors. Uncomment to enable debug mode. Comment out to disable.

//#define DEBUG_CRAZYIVAN   // For debugging the crazyIvan event (not included in DEBUG_ALL). Uncomment to enable debug mode. Comment out to disable.


/* ==================================================
 *             Configure your controller
 * ================================================== */
// Uncomment the line that best represents the controller you will be using.
// Make certain all other lines are commented out.

//#define PS3_NAVIGATION      // Using one or two PS3 Move Navigation controllers.
//#define PS3_CONTROLLER     // TODO: Test using one PS3 controller.
#define PS4_CONTROLLER     // TODO: Test using one PS4 controller.

// Step through the process to pair your controller(s) to the Bluetooth dongle.
// (See directions in the wiki found at astromech.net.) Note the MAC address
// of each controller. Edit the appropriate controller's *.cpp tab. Add each MAC
// address to the array of authorized addresses.  Adjust the number of MAC
// addresses to the number of elements in the array.

#if defined(PS3_NAVIGATION)
const uint8_t NUMBER_OF_MAC_ADDRESSES = 2;
#else
const uint8_t NUMBER_OF_MAC_ADDRESSES = 1;
#endif

// When drive is enabled or disabled, we cause the PS3 or PS4 controller
// to activate its rumble for a short period. This determines how long.

const int RUMBLE_DURATION = 500;    // This is in milliseconds. 500 = 1/2 second.

// Getting the joystick to rest perfect centered isn't always possible.
// Define the joystick's center point and a small range beyond that
// which we'll treat as center.

const uint8_t JOYSTICK_CENTER = 128;
const uint8_t JOYSTICK_DEBUG_DRIVE_ZONE = 10;

// One of my controllers frequently returns joystick coordinates 0,0 when
// the stick has not been touched. I'm calling this event a crazyIvan.
// The following constant is meant to adjust the sensitivity.

const uint8_t CRAZYIVAN_THRESHOLD = 50;

// If you have a relay module controlling your drive motor controller
// then uncomment this line to enable the DEBUG_DRIVEman code.


/* ==================================================
 *              Configure your Marcduino
 *              or Marcduino alternative
 * ================================================== */

// The use of Marcduino here implies the use of a dome master board at minimum.

#define MARCDUINO                           // Uncomment this when you have at least one Marcduino master.

#define MD_STD_CONTROLS                     // Uncomment this when you use the standard SHADOW_MD controls.
//#define MD_CUSTOM_CONTROLS                  // Uncomment this when you use your own custom Marcduino controls. (Advanced setting, requires custom coding)

const uint8_t NUMBER_OF_DOME_PANELS = 10;   // Specify how many dome panels are in use.
const uint8_t NUMBER_OF_HOLOPROJECTORS = 3; // Specify how many holoprojectors are in use. Always leave uncomment to avoid compiler errors.
const uint8_t AUTO_HP_DELAY_MIN = 2;        // Specify in seconds. Minimum time between automated holoprojector movements.
const uint8_t AUTO_HP_DELAY_MAX = 10;       // Specify in seconds. Maximum time between automated holoprojector movements.

const int MARCDUINO_BAUD_RATE = 9600;       // Do not change this!

// The use of a Marcduino body master board is optional.
// I also provide an I2C-based alternative to the body master.

#define MD_BODY_MASTER                    // Uncomment this line if you are using the optional Marcduino body master.
//#define QWIIC                               // Uncomment this line if you are using the I2C alternative.
                                            // Leave both commented out if you are using neither.

#define BODY_SOUND                          // Uncomment this when the sound board is located in the body.
                                            // Leave it commented out when located in the dome.

const uint8_t NUMBER_OF_BODY_PANELS = 2;    // Specify how many body panels are in use. Marcduino max = 10, Qwiic max = 16

#if defined(QWIIC)
const uint8_t QWIIC_MP3_INITIAL_VOLUME = 15;  // Set the desired starting volume level.
const uint8_t QWIIC_MP3_MIN_VOLUME = 1;       // Minimum volume.  Do not change this.
const uint8_t QWIIC_MP3_MAX_VOLUME = 31;      // Maximum volume.  Do not change this.

const uint8_t QWIIC_MP3_STARTUP_TRACK = 255;  // Set the desired starting sound track.
const uint8_t QWIIC_MP3_RANDOM_MIN = 1;       // Minimum track # for random sounds.
const uint8_t QWIIC_MP3_RANDOM_MAX = 10;      // Maximum track # for random sounds.

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!

const int QWIIC_SERVO_MIN   = 150;    // This is the 'minimum' pulse length count (out of 4096)
const int QWIIC_SERVO_MAX   = 600;    // This is the 'maximum' pulse length count (out of 4096)
const int QWIIC_SERVO_USMIN = 600;    // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
const int QWIIC_SERVO_USMAX = 2400;   // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
const int QWIIC_SERVO_FREQ  = 50;     // Analog servos run at ~50 Hz updates
#endif


/* ==================================================
 *             Configure your dome motor
 * ================================================== */
#define SYREN10                           // Uncomment this when you use a Syren10 for your dome motor controller.

const int SYREN_ADDR = 129;               // Serial Address for Dome Syren
const int MOTOR_BAUD_RATE = 9600;         // Set the baud rate for the Syren motor controller, also used by Sabertooth,
                                          // for packetized options are: 2400, 9600, 19200 and 38400
const byte DOME_SPEED = 100;              // If using a speed controller for the dome, sets the top speed
                                          // Use a number up to 127 for serial
const int TIME_360_DOME_TURN = 4000;      // milliseconds for dome to complete 360 turn at DOME_AUTO_SPEED
const int TIME_360_DOME_TURN_MIN = 1999;  // Minimum time allowed for dome to complete a full 360 turn.
const int TIME_360_DOME_TURN_MAX = 8001;  // Maximum time allowed for dome to complete a full 360 turn.
                                          // Cut in half to reduce spin.  Offset for different rotation startups due to gearing.
const byte DOME_AUTO_SPEED = 100;         // Speed used when dome automation is active (1-127)
const byte DOME_AUTO_SPEED_MIN = 49;      // Minimum auto dome speed to allow automation to run.
const byte DOME_AUTO_SPEED_MAX = 101;     // Maximum auto dome speed to allow automation to run.

//const int INVERT_TURN_DIRECTION = -1;     // This may need to be set to 1 for some configurations

const int SERIAL_LATENCY = 25;  //This is a delay factor in ms to prevent queueing of the Serial data.
                                //25ms seems appropriate for HardwareSerial, values of 50ms or larger are needed for Softare Emulation


/* =========================================================
 *            Configure your drive motor options
 * ========================================================= */
// Set the following variable according to which system you use for your drive motor controller.

#define SBL2360_PULSE        // Uncomment this when you use a Roboteq SBL2360 AND communicate via Pulse command priority (PWM)
//#define SBL2360_SERIAL     // Uncomment this when you use a Roboteq SBL2360 and communicate via RS232 command priority (Serial)
//#define SBL1360            // Uncomment this when you use a Roboteq SBL1360 for your drive motor controller. (Not yet supported.)
//#define SABERTOOTH         // Uncomment this when you use a Sabertooth for your drive motor controller. (Not yet supported.)

// The following settings are fairly advanced.  Tinker with them only after you've
// studied the code to learn their use, or have talked with members for support.

const byte DRIVESPEED1 = 20;   // Set these to whatever speed work for you. 0-stop, 127-full speed.
const byte DRIVESPEED2 = 127;  // Recommend beginner: 50 to 75, experienced: 100 to 127.

const byte SERVO_CENTER = 90;  // This applies to SBL1360 and SBl2360 using Pulse command priority.

//
// The following are specific to the Roboteq SBL2360 using serial communication.
// -----------------------------------------------------------------------------

const int SBL2360_BAUD_RATE = 115200;     // I strongly recommend not changing this.
const int SBL2360_MAX_SPEED = 5000;       // Make this match the max speed set in the Roborun Utility.

//
// The following are specific to the Sabertooth.
// ---------------------------------------------

const int SABERTOOTH_ADDR = 128;       // Serial address for foot Sabertooth

const byte TURN_SPEED = 75; //50; // Set this to whatever speed work for you. 0-stop, 127-full speed.
                                  // The higher this number the faster it will spin in place, lower - easier to control.
                                  // Recommend beginner: 40 to 50, experienced: 50 and up, I like 75

const byte RAMPING = 6; //3;  // The lower this number the longer R2 will take to speedup
                              // or slow down, change this by increments of 1

const byte JOYSTICK_FOOT_DEBUG_DRIVE_ZONE_RANGE = 15; // For controllers that have centering problems,
                                               // use the lowest number with no drift

const byte DRIVE_DEBUG_DRIVE_BAND_RANGE = 10;         // Used to set the Sabertooth DEBUG_DRIVEZone for drive motors

//
// The following are specific to RC controllers.
//----------------------------------------------

const int STEERING_FACTOR = 100; // The larger SteeringFactor is the less senstitive steering is...
                                 // Smaller values give more accuracy in making fine steering corrections
                                 // XDist*sqrt(XDist+SteeringFactor)
//    const int LEFT_DIRECTION  = 1;   // change this if your motor is spinning the wrong way
//    const int RIGHT_DIRECTION = 0;   // change this if your motor is spinning the wrong way


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

const int PLUGGED_STATE_SHORT_INTERVAL = 15;  // This is the interval when no other controller is connected
const int PLUGGED_STATE_LONG_INTERVAL = 100; // This is the interval when another controller is connected.


/* ==================================================
 *                 Pin assignments
 * ================================================== */

const int DOME_INTERRUPT_PIN = 2;   // Do not change this!
const int DRIVE_INTERRUPT_PIN = 3;  // Do not change this!

#if defined(SABERTOOTH)
const int LEFT_FOOT_PIN   = 44;  // Connect this pin to motor controller for the left foot.
const int RIGHT_FOOT_PIN  = 45;  // Connect this pin to motor controller for the right foot.
#elif defined(SBL1360) || defined(SBL2360_PULSE)
const int THROTTLE_PIN    = 44;  // Connect this pin to the Roboteq input 1.
const int STEERING_PIN    = 45;  // Connect this pin to the Roboteq input 2.
#endif

#define DEADMAN
const uint8_t DEADMAN_PIN = 46;

#if defined(MD_BODY_MASTER)
const int SERIAL4_RX_PIN = 44;   // Connect this pin to the Marcduino body master for Rx (Serial mode).
const int SERIAL4_TX_PIN = 45;   // Connect this pin to the Marcduino body master for Tx (Serial mode).
#endif

#endif
