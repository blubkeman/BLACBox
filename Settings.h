/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings.h - Library for user settings
 * Created by Brian Lubkeman, 17 December 2020
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
//#define DEBUG_DRIVE       // For debugging the drive motors. Uncomment to enable debug mode. Comment out to disable.

//#define DEBUG_CRAZYIVAN   // For debugging the crazyIvan event (not included in DEBUG_ALL). Uncomment to enable debug mode. Comment out to disable.


/* ==================================================
 *             Configure your controller
 * ================================================== */
// Uncomment the line that best represents the controller you will be using.
// Make certain all other lines are commented out.

//#define PS3_NAVIGATION     // Using one or two PS3 Move Navigation controllers.
//#define PS3_CONTROLLER     // Using one PS3 controller.
#define PS4_CONTROLLER     // Using one PS4 controller.

// Step through the process to pair your controller(s) to the Bluetooth dongle.
// Note the MAC address of each controller. Edit the appropriate controller's
// *.cpp tab. Add each MAC address to the array of authorized addresses.

// Adjust the number of MAC addresses to the number of elements in the array.

#if defined(PS3_NAVIGATION)
const uint8_t NUMBER_OF_MAC_ADDRESSES = 2;
#else
const uint8_t NUMBER_OF_MAC_ADDRESSES = 1;
#endif

// Getting the joystick to rest perfectly centered isn't always possible.
// Define the joystick's center point and a small range beyond that
// which we'll treat as center.

const uint8_t JOYSTICK_MIN = 0;
const uint8_t JOYSTICK_CENTER = 128;
const uint8_t JOYSTICK_MAX = 255;
const uint8_t JOYSTICK_DEAD_ZONE = 7; // This value bring the joystick deadzone near the Roboteq's 8% deadzone.

// One of my controllers frequently returns joystick coordinates 0,0 when
// the stick has not been touched. I'm calling this event a crazyIvan.
// The following constant is meant to adjust the sensitivity.

const uint8_t CRAZYIVAN_THRESHOLD = 50;

// Uncomment this if you have added a deadman switch in front of your motor controller.
// Enabling this will require holding L1 to allow the left joystick to drive.

//#define DEADMAN


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

#define SBL2360_PULSE        // Uncomment this when you use a Roboteq SBL2360 AND communicate via Pulse command priority (PWM. Supported.)
//#define SBL2360_SERIAL     // Uncomment this when you use a Roboteq SBL2360 and communicate via RS232 command priority (Serial. Not yet supported.)
//#define SBL1360            // Uncomment this when you use a Roboteq SBL1360 for your drive motor controller. (Not yet supported.)
//#define SABERTOOTH         // Uncomment this when you use a Sabertooth for your drive motor controller. (Not yet supported.)

// This is an experiment. I suspect we don't need the use Servo.h for a
// Roboteq controller. Uncomment this to use it.  Uncomment to not use it.
// When uncommented, set the Roborun's Pulse Input range as: min=544, mid=1472, max=2400.
// When commented, set the Roborun's Pulse Input range as: min=0, mid=128, max=255.

//#define USE_SERVO

// The following are speed profiles.  Please do not change these.
// In a Roboteq controller, a script is used to set the max speed,
// acceleration, and deceleration.
//
// TO DO: Still need to work out how to manage this using a
// Sabertooth.
// ---------------------------------------------------------------

const uint8_t Walk   = 1;   // MaxSpeed=1400, Accel=12000, Decel=22000
const uint8_t Jog    = 2;   // MaxSpeed=1600, Accel=16000, Decel=30000
const uint8_t Run    = 3;   // MaxSpeed=2400, Accel=16000, Decel=36000
const uint8_t Sprint = 4;   // MaxSpeed=3000, Accel=16000, Decel=40000

//
// The following are specific to the Roboteq using serial communication.
// ---------------------------------------------------------------------

const int ROBOTEQ_BAUD_RATE = 115200;     // I strongly recommend not changing this.

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
const int PLUGGED_STATE_LONG_INTERVAL = 100;  // This is the interval when another controller is connected.


/* ==================================================
 *                 Pin assignments
 * ================================================== */

#if defined(SBL2360_PULSE)
const int THROTTLE_PIN    = 44;  // Connect this pin to the Roboteq pulse input 1 (DB25 pin 15).
const int STEERING_PIN    = 45;  // Connect this pin to the Roboteq pulse input 2 (DB25 pin 16).
#elif defined(SABERTOOTH) || defined(SBL1360)
const int LEFT_FOOT_PIN   = 44;  // Connect this pin to motor controller for the left foot.
const int RIGHT_FOOT_PIN  = 45;  // Connect this pin to motor controller for the right foot.
#endif

const int SCRIPT_PIN      = 46;  // Connect this pin to the Roboteq pulse input 4 (DB25 pin 17).

const int DEADMAN_PIN     = 47;


/* ==================================================
 *                     Servo Range
 * ================================================== */

const byte SERVO_MIN    = 0;    // The servo object outputs the value 544 microseconds.
const byte SERVO_CENTER = 90;   // The servo object outputs the value 1472 microseconds.
const byte SERVO_MAX    = 180;  // The servo object outputs the value 2400 microseconds.

/* If you are using a Roboteq motor controller and prefer the Pulse Input range values
 * to be more like the defaulted values of min=1000, mid=1500, max=2000 then use the
 * following settings instead. Follow this up with using the Roborun+ utility's
 * "Calibrate" tool to fine tune the inputs. It should end with min, mid, and max
 * values similar to the microseconds associated with each servo value.

const byte SERVO_MIN    = 44;  // The servo object outputs the value 998 microseconds, the integer nearest a value of 1000.
const byte SERVO_CENTER = 93;  // The servo object outputs the value 1503 microseconds, the integer nearest a value of 1500.
const byte SERVO_MAX    = 141; // The servo object outputs the value 1998 microseconds, the integer nearest a value of 2000.
*/

/*   Arduino      |     Roboteq
 * ---------------|-----------------
 * Servo   Servo  |  Pulse  Internal
 * Input  Output  |  Input       Use
 * -----  ------  |  -----  --------
 *     0     544  |    544     -1000
 *    90    1472  |   1472         0
 *   180    2400  |   2400      1000
 * 
 *            Alternate
 * ---------------|----------------
 * Servo  Servo   |  Pulse  Internal
 * Input  Output  |  Input    Use
 * -----  ------  |  -----  --------
 * 44      998    |   998    -1000      // This is the closest we can get to Roborun's default pulse input range min=1000.
 * 93     1503    |  1503        0      // This is the closest we can get to Roborun's default pulse input range mid=1500.
 * 141    1998    |  1998     1000      // This is the closest we can get to Roborun's default pulse input range max=2000.
 */

#endif
