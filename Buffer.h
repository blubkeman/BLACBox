/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Buffer.h - Library for controller inputs
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "Settings.h"
#include <controllerEnums.h>  // This is part of the USB_Host_Shield_Library_2.0

extern String output;
extern void printOutput(void);

// Connected controller types

const uint8_t NONE = 0;   // This represents no connected controllers.
const uint8_t HALF = 1;   // This represents a single PS3 Move Navigation controller connected.
const uint8_t FULL = 2;   // This represents either a single-device controller or dual PS3 Move Navigation controllers connected.

#define PS2 19  // Adding the PS button on a second Navigation to the array of controller buttons

/* From the USB Host Shield library,                       |
 * controllerEnums.h defines ButtonEnum enumeration.       |   These are my uses
 *      PS3       PS4       Wii       Wii U Pro  Xbox ONE  |  PS3 Nav1  PS3 Nav2
 *      --------  --------  --------  ---------  --------  |  --------  --------
 * 0  = UP        UP        UP        UP         UP        |  UP
 * 1  = RIGHT     RIGHT     RIGHT     RIGHT      RIGHT     |  DOWN
 * 2  = DOWN      DOWN      DOWN      DOWN       DOWN      |  RIGHT
 * 3  = LEFT      LEFT      LEFT      LEFT       LEFT      |  LEFT
 * 4  = SELECT    SHARE               BACK       BACK      |  SELECT    SELECT   (= CROSS of Nav1 or Nav2)
 * 5  = START     OPTIONS   PLUS      PLUS                 |  START     START    (= CIRCLE of Nav1 or Nav2)
 * 6  = L3        L3        TWO       TWO                  |  L3
 * 7  = R3        R3        ONE       ONE                  |            R3       (= Nav2 L3)
 * 8  = L2        L2        MINUS     MINUS      BLACK     |  L2
 * 9  = R2        R2        HOME      HOME       WHITE     |            R2       (= Nav2 L2)
 * 10 = L1        L1        Z         Z                    |  L1
 * 11 = R1        R1        C         C                    |            R1       (= Nav2 L1)
 * 12 = TRIANGLE  TRIANGLE  B         B                    |            TRIANGLE (= Nav2 UP)
 * 13 = CIRCLE    CIRCLE    A         A                    |            CIRCLE   (= Nav2 RIGHT)
 * 14 = CROSS     CROSS                          X         |            CROSS    (= Nav2 DOWN)
 * 15 = SQUARE    SQUARE                         Y         |            SQUARE   (= Nav2 LEFT)
 * 16 = PS        PS                  L          XBOX      |  PS
 * 17 = MOVE      TOUCHPAD            R          SYNC      |
 * 18 = T         T                   ZL                   |
 * 19 =                               ZR                   |            PS2      (= Nav2 PS)
 * 
 * 0 = LeftHatX
 * 1 = LeftHatY
 * 2 = RightHatX
 * 3 = RightHatY
 */

/* ==========================================
 *           Custom data structures
 * ========================================== */
typedef struct {
  uint8_t button[20];
  uint8_t stick[4];
  uint8_t prevStick[4];  // This is used in critical fault detection.
} InputData_Struct;


/* ====================================
 *           Class Definition
 * ==================================== */
class Buffer
{
  public:
    Buffer(void);
    void begin(void);

    char * buttonLabel[20];

  // Controller input functions
    void resetButtons(void);
    void updateButton(uint8_t, int);
    uint8_t getButton(uint8_t);
    void setButton(uint8_t, int);

    void setStick(uint8_t, int);
    uint8_t getStick(uint8_t);
    void setPrevStick(uint8_t, int);
    uint8_t getPrevStick(uint8_t);
    void saveStick(void);
    void restoreStick(uint8_t);

    bool isStickOffCenter(uint8_t);

  // Controller status
    void setControllerConnected(uint8_t);   // Pass 0=NONE, 1=HALF (PS3Nav only), 2=FULL

    bool isFullControllerConnected(void);   // Returns true when _controllerConnected == FULL
    bool isHalfControllerConnected(void);   // Returns true when _controllerConnected == HALF
    bool isControllerConnected(void);       // Returns true when _controllerConnected == HALF or FULL (not NONE)

  // Controller output
    void requestLedUpdate(bool);
    bool isLedUpdateRequested(void);

  // Peripheral status
    void setDriveEnabled(bool);
    void setDriveStopped(bool);
    void setDomeStopped(bool);
    void setDomeAutomationRunning(bool);
    void setDomeCustomPanelRunning(bool);
    void setHoloAutomationRunning(bool);
    void setSpeedProfile(uint8_t);

    bool isDriveEnabled(void);
    bool isDriveStopped(void);
    bool isOverdriveEnabled(void);
    bool isDomeStopped(void);
    bool isDomeAutomationRunning(void);
    bool isDomeCustomPanelRunning(void);
    bool isHoloAutomationRunning(void);
    bool isBodyPanelRunning(void);
    uint8_t getSpeedProfile(void);

  //  Motor functions
    void stopDomeMotor(void);

  // Testing inputs
    #ifdef TEST_CONTROLLER
    void displayInput(void);
    void scrollInput(void);
    #endif

  private:
    InputData_Struct _input;    // Controller inputs

  // Status flags
    uint8_t _controllerConnected;   // 0=NONE, 1=HALF, 2=FULL
    bool _driveEnabled;
    volatile bool _driveStopped;
    bool _overdriveEnabled;
    volatile bool _domeStopped;
    bool _domeAutomationRunning;
    bool _domeCustomPanelRunning;
    bool _holoAutomationRunning;
    bool _bodyPanelRunning;
    bool _updateRequested;
    uint8_t _speedProfile;

  // Motor controls
    bool _stopDomeMotor;

    #if defined(DEBUG_BUFFER) || defined(DEBUG_ALL) || defined(TEST_CONTROLLER)

  // Debugging
    String _className;
    #endif

  // Testing inputs
    #ifdef TEST_CONTROLLER
    void _displayButtons(uint8_t, uint8_t);
    void _displayStick(String, uint8_t, uint8_t);
    #endif
};
#endif
