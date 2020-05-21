/*
 * Buffer.h - Library for controller inputs for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 21 May 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <controllerEnums.h>  // This is part of the USB_Host_Shield_Library_2.0

#define PS2 19  // Adding the PS button on a second Navigation to the array of controller buttons

/* From the USB Host Shield library, 
 * controllerEnums.h defines ButtonEnum enumeration.              |   These are my uses
 *      PS3       PS4       Wii       Wii U Pro  Xbox ONE  |  PS3 Nav1  PS3 Nav2
 *      --------  --------  --------  ---------  --------  |  --------  --------
 * 0  = UP        UP        UP        UP         UP        |  UP
 * 1  = RIGHT     RIGHT     RIGHT     RIGHT      RIGHT     |  DOWN
 * 2  = DOWN      DOWN      DOWN      DOWN       DOWN      |  RIGHT
 * 3  = LEFT      LEFT      LEFT      LEFT       LEFT      |  LEFT
 * 4  = SELECT    SHARE               BACK       BACK      |  SELECT (= CROSS of Nav1 or Nav2)
 * 5  = START     OPTIONS   PLUS      PLUS                 |  START (= CIRCLE of Nav1 or Nav2)
 * 6  = L3        L3        TWO       TWO                  |  L3
 * 7  = R3        R3        ONE       ONE                  |            R3 (= Nav2 L3)
 * 8  = L2        L2        MINUS     MINUS      BLACK     |  L2 (= L2 of Nav1 or Nav2)
 * 9  = R2        R2        HOME      HOME       WHITE     |
 * 10 = L1        L1        Z         Z                    |  L1 (= L1 of Nav1 or Nav2)
 * 11 = R1        R1        C         C                    |
 * 12 = TRIANGLE  TRIANGLE  B         B                    |            TRIANGLE (= Nav2 UP)
 * 13 = CIRCLE    CIRCLE    A         A                    |            CIRCLE (= Nav2 RIGHT)
 * 14 = CROSS     CROSS                          X         |            CROSS (= Nav2 DOWN)
 * 15 = SQUARE    SQUARE                         Y         |            SQUARE (= Nav2 LEFT)
 * 16 = PS        PS                  L          XBOX      |  PS
 * 17 = MOVE      TOUCHPAD            R          SYNC      |
 * 18 = T         T                   ZL                   |
 * 19 =                               ZR                   |            PS2 (= Nav2 PS)
 * 
 * 0 = LeftHatX
 * 1 = LeftHatY
 * 2 = RightHatX
 * 3 = RightHatY
 */

typedef struct {
  uint8_t button[20];  
  uint8_t hat[4];
  uint8_t prevHat[4];  // This is used in critical fault detection.
} InputData_Struct;

enum Status {
  PrimaryControllerConnected = 0,
  SecondaryControllerConnected = 1,
  FootMotorEnabled = 2,
  FootMotorStopped = 3,
  DomeMotorEnabled = 4,
  DomeMotorStopped = 5,
  DomeAutomationRunning = 6,
  HoloAutomationRunning = 7,
  CustomDomePanelRunning = 8
};

const uint8_t NUMBER_OF_STATUSES = 9;

extern String output;

/* ===========================================
 *           Buffer Class Definition
 * =========================================== */
class Buffer {

  private:
    InputData_Struct _input;    // Controller inputs
    int _cmd;
    bool _sendCommand;
    bool _status[NUMBER_OF_STATUSES];
    uint8_t _controllerCycle;

    #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
    String _className;
    #endif

    #ifdef TEST_CONTROLLER
    void _displayStick(String stick);
    void _displayButton(String dPadButton);
    #endif

  public:
    // =====================
    //      Constructor
    // =====================
    Buffer() {
      _controllerCycle = 0;

      #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
      _className = F("Buffer::");
      #endif

    };

    // =================
    //      begin()
    // =================
    void begin() {

      #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
      String functionName = _className+F("begin()");
      #endif

      // Initialize the array of buttons.
      for (uint8_t i = 0; i < 20; i++)
        this->setButton(i, 0);

      // Initialize the array of current and previous joystick positions at center.
      for (uint8_t i = 0; i < 4; i++) {
        this->setStick(i, 127);
        this->setPrevStick(i, 127);
      }

      // Initialize the statuses.
      setStatus(Status::PrimaryControllerConnected, false);
      setStatus(Status::SecondaryControllerConnected, false);
      // The following statuses are normally set in the respective constructors.
      // We only initialize these if they have not been activated in case other
      // classes need to know about them.
      #ifndef DRIVE
      setStatus(Status::FootMotorEnabled, false);
      setStatus(Status::FootMotorStopped, true);
      #endif
      #ifndef DOME
      setStatus(Status::DomeMotorEnabled, false);
      setStatus(Status::DomeMotorStopped, true);
      setStatus(Status::DomeAutomationRunning, false);
      setStatus(Status::CustomDomePanelRunning, false);
      #endif
      #ifndef HOLOPROJECTORS
      setStatus(Status::HoloAutomationRunning, false);
      #endif

      #ifdef BLACBOX_DEBUG
      output = functionName;
      output += F(" - Control buffer started.");
      Serial.println(output);
      #endif

    };

  // ==============================================
  //          Controller inputs functions
  // ==============================================
    void setButton(uint8_t i, int value)  { _input.button[i] = value; };
    uint8_t getButton(uint8_t i)          { return _input.button[i]; };
    
    void setStick(uint8_t i, int value)     { _input.hat[i] = value; };
    uint8_t getStick(uint8_t i)             { return _input.hat[i]; };

    void setPrevStick(uint8_t i, int value) { _input.prevHat[i] = value; };
    uint8_t getPrevStick(uint8_t i)         { return _input.prevHat[i]; };

    void saveStick() {
      for (uint8_t i = 0; i < 4; i++)
        _input.prevHat[i] = _input.hat[i];
    };
    void restoreStick(uint8_t i) { _input.hat[i] = _input.prevHat[i]; };

    bool isStickOffCenter(uint8_t i) {
      return (getStick(i) < 117 || getStick(i) > 137);
    };

    bool isButtonModified() {
      return (getButton(L1) && getButton(L2) && \
              getButton(R1) && getButton(R2) && \
              getButton(PS) && getButton(PS2));
    };

  /* ============================================
   *          Fault detection functions
   * ============================================ */

    // =====================
    //      crazyIvan()
    // =====================
    void crazyIvan(uint8_t xHat, uint8_t yHat) {

      #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
      String functionName = _className+F("crazyIvan()");
      #endif

    /* -----------------------------------------------------------------------------------
     * My PS3 Move Navigaton occasionally sends stick X,Y values 0,0 for no known reason.
     * I am calling this a "crazy Ivan" because why not?  I don't want this to be
     * processed, so when it occurs, I reset the X,Y values to their previous values.
     * 
     * This is called from within the critical fault detection function of the controller.
     * ----------------------------------------------------------------------------------- */
      if (this->getStick(xHat) == 0 && this->getPrevStick(xHat) > 50 && \
          this->getStick(yHat) == 0 && this->getPrevStick(yHat) > 50) {
        restoreStick(xHat);
        restoreStick(yHat);
        #ifdef BLACBOX_DEBUG
        output = F("Buffer::");
        output += F("crazyIvan - ");
        if (xHat = LeftHatX)
          output += F("Left");
        else
          output += F(" - Right");
        output += F(" Stick");
        Serial.println(output);
        #endif
      }
    };

  /* ===================================
   *          Status functions
   * =================================== */
    void setStatus(uint8_t statusNumber, bool value) { _status[statusNumber] = value; };
    bool isPrimaryConnected()       { return _status[Status::PrimaryControllerConnected]; };
    bool isSecondaryConnected()     { return _status[Status::SecondaryControllerConnected]; };
    bool isFootEnabled()            { return _status[Status::FootMotorEnabled]; };
    bool isFootStopped()            { return _status[Status::FootMotorStopped]; };
    bool isDomeEnabled()            { return _status[Status::DomeMotorEnabled]; };
    bool isDomeStopped()            { return _status[Status::DomeMotorStopped]; };
    bool isDomeAutomationRunning()  { return _status[Status::DomeAutomationRunning]; };
    bool isHoloAutomationRunning()  { return _status[Status::HoloAutomationRunning]; };
    bool isCustomDomePanelRunning() { return _status[Status::CustomDomePanelRunning]; };
    void advanceCycle()             { _controllerCycle = (_controllerCycle < 2 ? _controllerCycle++ : 0); };
    uint8_t getCycle()              { return _controllerCycle; };


  /* ====================================
   *          Testing functions
   * ==================================== */
    #ifdef TEST_CONTROLLER
    void testInput();
    void scrollInput();
    #endif

};

#ifdef TEST_CONTROLLER
void Buffer::_displayStick(String stick) {

  #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
  String functionName = _className+F("_displayStick()");
  #endif

  output = F("Buffer::testInput - ");
  if (stick == "Left") {
    output += "LeftHatX: ";
    output += getStick(LeftHatX);
    output += " LeftHatY: ";
    output += getStick(LeftHatY);
  } else {
    output += "RightHatX: ";
    output += getStick(RightHatX);
    output += " RightHatY: ";
    output += getStick(RightHatY);
  }
  Serial.println(output);
};

void Buffer::_displayButton(String dPadButton) {

  #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
  String functionName = _className+F("_displayButton");
  #endif

  output = F("Buffer::testInput - ");
  if (getButton(L1))          output += "L1 + ";
  else if (getButton(L2))     output += "L2 + ";
  else if (getButton(SELECT)) output += "CROSS + ";
  else if (getButton(START))  output += "CIRCLE + ";
  else if (getButton(PS) ||
           getButton(PS2))    output += "PS + ";
  output += dPadButton;
  Serial.println(output);
};

void Buffer::testInput() {

  #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
  String functionName = _className+F("testInput()");
  #endif

  if (getStick(LeftHatX) < 117 || getStick(LeftHatX) > 137 || \
      getStick(LeftHatY) < 117 || getStick(LeftHatY) > 137) {
    _displayStick("Left");
  }

  if (getButton(UP))    { _displayButton("UP(Nav1)"); }
  if (getButton(RIGHT)) { _displayButton("RIGHT(Nav1)"); }
  if (getButton(DOWN))  { _displayButton("DOWN(Nav1)"); }
  if (getButton(LEFT))  { _displayButton("LEFT(Nav1)"); }
  if (getButton(L3))    { _displayButton("L3(Nav1)"); }

  if (getStick(RightHatX) > 137 || getStick(RightHatX) < 117 || \
      getStick(RightHatY) > 137 || getStick(RightHatY) < 117) {
    _displayStick("Right");
  }

  if (getButton(TRIANGLE)) { _displayButton("UP(Nav2)"); }
  if (getButton(CIRCLE))   { _displayButton("RIGHT(Nav2)"); }
  if (getButton(CROSS))    { _displayButton("DOWN(Nav2)"); }
  if (getButton(SQUARE))   { _displayButton("LEFT(Nav2)"); }
  if (getButton(R3))       { _displayButton("L3(Nav2)"); }
};

void Buffer::scrollInput() {

  #if defined(BLACBOX_DEBUG) || defined (BLACBOX_VERBOSE)
  String functionName = _className+F("scrollInput()");
  #endif

//Nav1 - Up:x Rt:x Dn:x Lt:x L3:x L2:x L1:x PS:x | Nav2 - Up:x Rt:x Dn:x Lt:x R3:x PS:x
  output = "LX:";   output += getStick(LeftHatX);
  output += " LY:";  output += getStick(LeftHatY);
  output += " Up:";  output += getButton(UP);
  output += " Rt:";  output += getButton(RIGHT);
  output += " Dn:";  output += getButton(DOWN);
  output += " Lt:";  output += getButton(LEFT);
  output += " X:";   output += getButton(SELECT);
  output += " O:";   output += getButton(START);
  output += " L3:";  output += getButton(L3);
  output += " L2:";  output += getButton(L2);
  output += " L1:";  output += getButton(L1);
  output += " PS:";  output += getButton(PS);

  if ( isSecondaryConnected() ) {
    output += " RX:";  output += getStick(RightHatX);
    output += " RY:";  output += getStick(RightHatY);
    output += " 2Up:"; output += getButton(TRIANGLE);
    output += " 2Rt:"; output += getButton(CIRCLE);
    output += " 2Dn:"; output += getButton(CROSS);
    output += " 2Lt:"; output += getButton(SQUARE);
    output += " R3:";  output += getButton(R3);
    output += " R2:";  output += getButton(R2);
    output += " R1:";  output += getButton(R1);
    output += " 2PS:"; output += getButton(PS2);
  }

  Serial.println(output);

};
#endif

#endif
