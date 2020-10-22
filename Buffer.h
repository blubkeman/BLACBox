/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Buffer.h - Library for controller inputs
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "Globals.h"
extern String output;

#include <controllerEnums.h>  // This is part of the USB_Host_Shield_Library_2.0
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
 * 8  = L2        L2        MINUS     MINUS      BLACK     |  L2        L2       (= L2 of Nav1 or Nav2)
 * 9  = R2        R2        HOME      HOME       WHITE     |
 * 10 = L1        L1        Z         Z                    |  L1        L1       (= L1 of Nav1 or Nav2)
 * 11 = R1        R1        C         C                    |
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

  // ==============================================
  //           Controller input functions
  // ==============================================
    void setButton(uint8_t, int);
    void setStick(uint8_t, int);
    void setPrevStick(uint8_t, int);

    uint8_t getButton(uint8_t);
    uint8_t getStick(uint8_t);
    uint8_t getPrevStick(uint8_t);

    void saveStick(void);
    void restoreStick(uint8_t);

    bool isStickOffCenter(uint8_t);
    bool isButtonModified(void);

  // ===================================
  //          Status functions
  // ===================================
    void setPrimaryConnected(bool);
    void setSecondaryConnected(bool);
    void setFootEnabled(bool);
    void setFootStopped(bool);
    void setDomeEnabled(bool);
    void setDomeStopped(bool);
    void setDomeAutomationRunning(bool);
    void setDomeCustomPanelRunning(bool);
    void setHoloAutomationRunning(bool);

    bool isPrimaryConnected(void);
    bool isSecondaryConnected(void);
    bool isFootEnabled(void);
    bool isFootStopped(void);
    bool isDomeEnabled(void);
    bool isDomeStopped(void);
    bool isDomeAutomationRunning(void);
    bool isDomeCustomPanelRunning(void);
    bool isHoloAutomationRunning(void);

  // =======================================
  //          Stop motor functions
  // =======================================
    void stopDomeMotor(void);
    void stopFootMotor(void);

    #ifdef TEST_CONTROLLER
  // ===================================
  //          Testing functions
  // ===================================
    void testInput(void);
    void scrollInput(void);
    #endif

  private:
    InputData_Struct _input;    // Controller inputs

  // ===============================
  //          Status flags
  // ===============================
    bool _primaryConnected;
    bool _secondaryConnected;
    bool _footEnabled;
    bool _footStopped;
    bool _domeEnabled;
    bool _domeStopped;
    bool _domeAutomationRunning;
    bool _domeCustomPanelRunning;
    bool _holoAutomationRunning;

  // ===================================
  //          Stop motor flags
  // ===================================
    bool _stopDomeMotor;
    bool _stopFootMotor;

    #ifdef TEST_CONTROLLER
  // ===================================
  //          Testing functions
  // ===================================
    void _displayStick(String stick);
    void _displayButton(String dPadButton);
    #endif

    #if defined(DEBUG_BUFFER) || defined(DEBUG_ALL) || defined(TEST_CONTROLLER)
  // ============================
  //          Debugging
  // ============================
    String _className;
    #endif
};
#endif
