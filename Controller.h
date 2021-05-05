/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller.h - Library for supported controllers
 * Created by Brian Lubkeman, 5 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef __BLACBOX_CONTROLLER_H__
#define __BLACBOX_CONTROLLER_H__

#include <usbhub.h>
#include <BTD.h>
#include <PS5BT.h>
#include <PS4BT.h>
#include <PS3BT.h>
#include "controllerEnums.h"

#define DEBUG
//#define TEST_CONTROLLER

#if defined(DEBUG) || defined(TEST_CONTROLLER)
extern String output;
extern void printOutput(void);
#endif
extern String getPgmString(const char *);

struct CriticalFault_Struct {
  uint32_t badData;
  unsigned long lastReadTime;
  unsigned long lastMsgTime;
  unsigned long pluggedStateTime;
  bool reconnect;
};

enum controller_setting_index_e {
  iCntrlType,       // 0 - Controller type
  iDriveSide,       // 1 - Drive stick side
  iDomeSide,        // 2 - Dome stick side
  iDeadZone,        // 3 - Joystick dead zone
  iLagKillMotor,    // 4 - Lag time to kill drive motor
  iLagDisconnect,   // 5 - Lag time to disconnect
  iLagReconnect,    // 6 - Lag time to reconnect
  iShortInterval,   // 7 - USB plugged state, short interval
  iLongInterval,    // 8 - USB plugged state, long interval
  iPeripheralCount  // 9 - Number of peripherals (drive, dome, Marcduino)
};

enum connection_status_e {
  NONE,
  HALF,
  FULL
};

enum speed_profile_e {
  WALK,
  JOG,
  RUN,
  SPRINT
};

enum joystick_side_e {
  left,
  right
};

class Controller; // Class prototype

const int L4  = 4;
const int R4  = 5;
const int PS2 = 17;

/* ================================================================================
 *                                 Joystick Classes
 * ================================================================================ */
class Joystick
{
  protected:
    Controller* m_controller;
    byte m_getX(void);
    byte m_getY(void);
    String m_side(void);

    #if defined(DEBUG)
    String m_className;
    #endif

    #if defined(TEST_CONTROLLER)
    void m_appendString(String* inString);
    #endif

  public:
    Joystick(const byte side, const int deadZone, Controller* pController);
    ~Joystick(void);

    byte side;
    const int minValue = 0;
    const int center   = 127;
    const int maxValue = 255;
    int deadZone;

    void displayInit(void);

    #if defined(TEST_CONTROLLER)
    void display(String* out);
    #endif
};

class Joystick_Drive : public Joystick
{
  public:
    Joystick_Drive(const byte side, const int deadZone, Controller* pCcontroller);
    ~Joystick_Drive(void);

    int steering(void);
    int throttle(void);
};

class Joystick_Dome : public Joystick
{
  public:
    Joystick_Dome(const byte side, const int deadZone, Controller* pCcontroller);
    ~Joystick_Dome(void);

    byte rotation(void);
};

/* ================================================================================
 *                                   Button Class
 * ================================================================================ */
class Button
{
  private:
    Controller* m_controller;

    #if defined(DEBUG)
    String m_className;
    #endif

    #if defined(TEST_CONTROLLER)
    void m_appendString(String* inString, const String addString);
    #endif

  public:
    Button(Controller* pCcontroller);
    ~Button(void);

    bool clicked(int buttonEnum);
    bool pressed(int buttonEnum);
    byte analogValue(int buttonEnum);

    #if defined(TEST_CONTROLLER)
    bool hasBasePressed(void);
    void display(String* out);
    String label(int buttonEnum);
    String abbreviation(int buttonEnum);
    #endif
};

/* ================================================================================
 *                             Parent Controller Class
 * ================================================================================ */

class Controller
{
  protected:
    USB m_Usb;
    BTD m_Btd;
    connection_status_e m_connectionStatus;
    CriticalFault_Struct m_faultData[2];
    unsigned long m_lagTime;
    unsigned long m_lastReadTime;
    byte m_disconnectCount;

    bool m_authorized(void);
    void m_setConnectionStatus(byte n);
    void m_initCriticalFault(byte idx);
    void m_resetCriticalFault(byte idx);

    virtual void m_connect(void) {};
    virtual void m_disconnect(void) {};
    virtual bool m_getUsbStatus(void) {};
    virtual bool m_detectCriticalFault(void);

    #if defined(DEBUG)
    String m_className;
    #endif

    #if defined(TEST_CONTROLLER)
    void m_displayInput(void);
    #endif

  public:
    Controller(const int settings[]);
    ~Controller(void);

    int* pSettings;
    Joystick_Drive driveStick;
    Joystick_Dome domeStick;
    Button button;

    void begin(void);
    byte connectionStatus(void);
    void disconnecting(void);
    bool isDisconnecting(void);
    int  getType(void);

    virtual bool read(void) {};
    virtual bool connected(void) {};
    virtual bool getButtonClick(int buttonEnum) {};
    virtual bool getButtonPress(int buttonEnum) {};
    virtual int  getAnalogButton(int buttonEnum) {};
    virtual int  getAnalogHat(int stickEnum) {};
    virtual void setLed(bool driveEnabled, byte speedProfile) {};
};

/* ================================================================================
 *                                  PS3Nav Controller
 * ================================================================================ */
class Controller_PS3Nav : public Controller
{
  private:
    PS3BT m_controller;
    PS3BT m_secondController;

    static void m_onInit(void);
    void m_onInitConnect(void);

    virtual void m_connect(PS3BT * pController);
    virtual void m_disconnect(PS3BT * pController);
    virtual bool m_getUsbStatus(void);
    virtual bool m_detectCriticalFault(PS3BT * pController);

    #if defined(DEBUG)
    String m_className;
    #endif

  public:
    Controller_PS3Nav(int settings[]);
    virtual ~Controller_PS3Nav(void);

    static Controller_PS3Nav* Controller_PS3Nav::anchor;

    void begin(void);

    virtual bool read(void);
    virtual bool connected(PS3BT * pController);
    virtual bool getButtonClick(int buttonEnum);
    virtual bool getButtonPress(int buttonEnum);
    virtual int  getAnalogButton(int buttonEnum);
    virtual int  getAnalogHat(int stickEnum);
    virtual void setLed(bool driveEnabled, byte speedProfile);
};

/* ================================================================================
 *                                  PS3 Controller
 * ================================================================================ */
class Controller_PS3 : public Controller
{
  private:
    PS3BT m_controller;

    static void m_onInit(void);

    virtual void m_connect(void);
    virtual void m_disconnect(void);
    virtual bool m_getUsbStatus(void);

    #if defined(DEBUG)
    String m_className;
    #endif

  public:
    Controller_PS3(int settings[]);
    virtual ~Controller_PS3(void);

    static Controller_PS3* Controller_PS3::anchor;

    void begin(void);

    virtual bool read(void);
    virtual bool connected(void);
    virtual bool getButtonClick(int buttonEnum);
    virtual bool getButtonPress(int buttonEnum);
    virtual int  getAnalogButton(int buttonEnum);
    virtual int  getAnalogHat(int stickEnum);
    virtual void setLed(bool driveEnabled, byte speedProfile);
};

/* ================================================================================
 *                                  PS4 Controller
 * ================================================================================ */
class Controller_PS4 : public Controller
{
  private:
    PS4BT m_controller;

    static void m_onInit(void);

    virtual void m_connect(void);
    virtual void m_disconnect(void);
    virtual bool m_getUsbStatus(void);

    #if defined(DEBUG)
    String m_className;
    #endif

  public:
    Controller_PS4(int settings[]);
    virtual ~Controller_PS4(void);

    static Controller_PS4* Controller_PS4::anchor;

    void begin(void);

    virtual bool read(void);
    virtual bool connected(void);
    virtual bool getButtonClick(int buttonEnum);
    virtual bool getButtonPress(int buttonEnum);
    virtual int  getAnalogButton(int buttonEnum);
    virtual int  getAnalogHat(int stickEnum);
    virtual void setLed(bool driveEnabled, byte speedProfile);
};

/* ================================================================================
 *                                  PS5 Controller
 * ================================================================================ */
class Controller_PS5 : public Controller
{
  private:
    PS5BT m_controller;

    static void m_onInit(void);

    virtual void m_connect(void);
    virtual void m_disconnect(void);
    virtual bool m_getUsbStatus(void);

    #if defined(DEBUG)
    String m_className;
    #endif

  public:
    Controller_PS5(int settings[]);
    virtual ~Controller_PS5(void);

    static Controller_PS5* Controller_PS5::anchor;

    void begin(void);

    virtual bool read(void);
    virtual bool connected(void);
    virtual bool getButtonClick(int buttonEnum);
    virtual bool getButtonPress(int buttonEnum);
    virtual int  getAnalogButton(int buttonEnum);
    virtual int  getAnalogHat(int stickEnum);
    virtual void setLed(bool driveEnabled, byte speedProfile);
};

#endif
