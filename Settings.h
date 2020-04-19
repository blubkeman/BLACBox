/*
 * Settings.h - User settings for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 18 April 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
*/
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

/* Consolidated Arduino pin assignments
 * ============================================
 * *** When using Marcduino
 *    Pin 14 = Serial3 Tx to optional Marduino body master
 *    Pin 18 = Serial1 Tx to Marduino dome master/slave
 *
 * *** When NOT using Marcduino
 *    Pin 14 = Serial3 Tx to serial Teeces
 *    Pin 15 = Serial3 Rx from serial Teeces
 *    Pin 18 = Serial1 Tx to serial sound board
 *    Pin 19 = Serial1 Rx from serial sound board
 *    Pin 49 = Top utility arm servo
 *    Pin 50 = Bottom utility arm servo
 *
 * *** When using Syren10 dome motor and/or Sabertooth foot motor controller
 *    Pin 16 = Serial2 Tx to Syren 10/Sabertooth
 *    Pin 17 = Serial2 Rx from Syren 10/Sabertooth
 *
 * *** When using R/C foot motor controller instead of Sabertooth
 *    Pin 44 = Left foot motor
 *    Pin 45 = Right foot motor
 *
 * *** Coin slot LEDs are an independent system
 *    
 * Pin assignments on an Adafruit 16-channel servo board
 * =====================================================
 * *** When using holoprojectors with I2C and Adafruit servo board
 *    Pin 0  = Front holoprojector red
 *    Pin 1  = Front holoprojector green
 *    Pin 2  = Front holoprojector blue
 *    Pin 3  = Front holoprojector X axis
 *    Pin 4  = Front holoprojector Y axis
 *    Pin 5  = Back holoprojector red
 *    Pin 6  = Back holoprojector green
 *    Pin 7  = Back holoprojector blue
 *    Pin 8  = Back holoprojector X axis
 *    Pin 9  = Back holoprojector Y axis
 *    Pin 10 = Top holoprojector red
 *    Pin 11 = Top holoprojector green
 *    Pin 12 = Top holoprojector blue
 *    Pin 13 = Top holoprojector X axis
 *    Pin 14 = Top holoprojector Y axis  
 */

/* ==================================================
 *                 Enable/Disable modes
 * ================================================== */
#define TEST_CONTROLLER   // Uncomment to enable test mode for the controller.  Comment out to disable.
#define BLACBOX_DEBUG     // Uncomment to enable debug mode.  Comment out to disable.
//#define BLACBOX_VERBOSE   // Uncomment to enable more verbose debugging.  Comment out to disable.


/* =============================================
 *           Configure your controller
 * ============================================= */
// Uncomment the line that best represents the controller you will be using.
// Make certain all other lines are commented out.

#define PS3_NAVIGATION      // Using one or two PS3 Move Navigation controllers.
// #define PS3_CONTROLLER     // TODO: Future implementation.  Using one PS3 controller.
// #define PS4_CONTROLLER     // TODO: Future implementation.  Using one PS4 controller.

// Step through the process to pair your controller to the Bluetooth dongle.
// (See directions in the wiki found at astromech.net.) When pairing your
// primary controller, note the MAC address displayed in the serial monitor.
// Set that address in the following variable.

const String PRIMARY_CONTROLLER_MAC = "00:06:F7:B8:57:01";

// If you have a backup primary controller, step through the process to pair
// it to the Bluetooth dongle.  Note the MAC address display in the serial
// monitor, and record its value here.

const String BACKUP_CONTROLLER_MAC = "xx:xx:xx:xx:xx:xx";


/* ======================================================
 *            Configure your dome motor options
 * ====================================================== */
// These are fairly advanced settings.  Recommended: Do not tinker with these.

const byte I2C_DOME_ADDRESS = 0x01;
const int SYREN_ADDR = 129;    // Serial Address for Dome Syren

const byte DOME_SPEED = 100;   // If using a speed controller for the dome, sets the top speed
                               // Use a number up to 127 for serial

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

const int FOOT_CONTROLLER = 0;    // 0 = Sabertooth Serial
                                  // 1 = individual R/C output (for Q85/NEO motors with 1 controller for each foot, or Sabertooth Mode 2 Independant Mixing)

// The following settings are fairly advanced.  Tinker with them after you've
// studied the code to learn their use, or have talked with members for support.

const byte DRIVESPEED1 = 25;  // Set these to whatever speed work for you. 0-stop, 127-full speed.
const byte DRIVESPEED2 = 65;  // Recommend beginner: 50 to 75, experienced: 100 to 127, I like 100.

//
// The following are specific to the Sabertooth.
//
const int MOTOR_BAUD_RATE = 9600; // Set the baud rate for the Syren motor controller, also used by Sabertooth,
                                  // for packetized options are: 2400, 9600, 19200 and 38400

const int SABERTOOTH_ADDR = 128;       // Serial address for foot Sabertooth
const byte DRIVE_DEAD_BAND_RANGE = 10; // Used to set the Sabertooth DeadZone for foot motors

const byte TURN_SPEED = 75; //50; // Set this to whatever speed work for you. 0-stop, 127-full speed.
                                  // The higher this number the faster it will spin in place, lower - easier to control.
                                  // Recommend beginner: 40 to 50, experienced: 50 and up, I like 75

const byte RAMPING = 6; //3;  // The lower this number the longer R2 will take to speedup
                              // or slow down, change this by increments of 1

const byte JOYSTICK_FOOT_DEAD_ZONE_RANGE = 15; // For controllers that have centering problems,
                                               // use the lowest number with no drift

//
// The following are specific to RC controllers.
//
const int LEFT_FOOT_PIN   = 44;  // connect this pin to motor controller for left foot (R/C mode)
const int RIGHT_FOOT_PIN  = 45;  // connect this pin to motor controller for right foot (R/C mode)
//    const int LEFT_DIRECTION  = 1;   // change this if your motor is spinning the wrong way
//    const int RIGHT_DIRECTION = 0;   // change this if your motor is spinning the wrong way
//    const int STEERING_FACTOR = 100; // The larger SteeringFactor is the less senstitive steering is...
//                                     // Smaller values give more accuracy in making fine steering corrections
//                                     // XDist*sqrt(XDist+SteeringFactor)


/* ==================================================
 *                  Active Peripherals
 * ================================================== */
// This is largely used for enabling/disabling broad functions during code development.
// Once in production, these definitions will likely be removed.

// Uncomment the elements you have running in your astromech.

//#define DRIVE             // Uncomment this when you have connected the foot motors and controller(s).
//#define DOME              // Uncomment this when you have connected the dome motor.
#define MARCDUINO         // Uncomment this when you have at least one Marcduino master.
//#define HOLOPROJECTORS    // Uncomment this if you are using at least 1 holoprojector. Max 3 holos.
//#define LOGIC_DISPLAYS    // Uncomment this if you are using Teeces logic display.
//#define DOME_PANELS       // Uncomment this if you are using at least 1 dome panel. Max 10 panels.
#define BODY_PANELS       // Uncomment this if you are using at least 1 body panel. Max 10 panels.
//#define MAGIC_PANEL       // Uncomment this if you are using a magic panel in the dome.
#define MD_BODY_MASTER    // Uncomment this if you are using the optional Marcduino body master.
#define MD_BODY_SOUND     // Uncomment this if sound is sent to the body master.
const int MARCDUINO_BAUD_RATE = 9600; // Do not change this!


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


/* =========================================================
 *            Configure your holoprojector options
 * ========================================================= */
#ifdef HOLOPROJECTORS
const uint8_t NUMBER_OF_HOLOPROJECTORS = 3;

// ========== Advanced Holoprojector Settings ==========

const bool isFrontHoloManuallyControlled = true;
const int HOLO_DELAY = 20000; // up to 20 second delay

// Servo range settings.

const int HOLO_SERVO_CTR = 300;

const int HOLO_FRONT_X_SERVO_MIN = 265; //250; //150;  // Issues with resin holo...
const int HOLO_FRONT_X_SERVO_MAX = 315; //350; //600;  // Issues with resin holo...
const int HOLO_FRONT_Y_SERVO_MIN = 250; //200; //150;  // Issues with resin holo...
const int HOLO_FRONT_Y_SERVO_MAX = 330; //400; //600;  // Issues with resin holo...

const int HOLO_BACK_X_SERVO_MIN = 275; //250; //150;
const int HOLO_BACK_X_SERVO_MAX = 325; //350; //600; 
const int HOLO_BACK_Y_SERVO_MIN = 250; //200; //150;
const int HOLO_BACK_Y_SERVO_MAX = 350; //400; //600; 

const int HOLO_TOP_X_SERVO_MIN = 275; //250; //150;
const int HOLO_TOP_X_SERVO_MAX = 325; //350; //600; 
const int HOLO_TOP_Y_SERVO_MIN = 250; //200; //150;
const int HOLO_TOP_Y_SERVO_MAX = 350; //400; //600;

// Set up random sound automation.  You can choose the
// time between movements (delay min and max).

const uint8_t AUTO_HP_DELAY_MIN = 3;   // in seconds
const uint8_t AUTO_HP_DELAY_MAX = 25;  // in seconds
#endif


/* =======================================================
 *            Configure your dome panel settings
 * ======================================================= */
#ifdef MARCDUINO
const uint8_t NUMBER_OF_DOME_PANELS = 10; // Specify how many dome panels are in use
#endif


/* =======================================================
 *            Configure your magic panel options
 * ======================================================= */
#ifdef MAGIC_PANEL

#ifndef MARCDUINO
const uint8_t MAGIC_PANEL_DRIVER_NUMBER = 1;  // Change this to 2 only if your magic panel on the driver for your dome panels.
const uint8_t MAGIC_PANEL_HEADER = 15;        // Which servo header number hosts the magic panel?
#endif

#endif


/* =======================================================
 *            Configure your body panel settings
 * ======================================================= */
#ifdef BODY_PANELS
const uint8_t NUMBER_OF_BODY_PANELS = 10; // Specify how many body panels are in use.
#endif


#endif
