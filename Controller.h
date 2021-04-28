/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller.h - Library for supported controllers
 * Created by Brian Lubkeman, 23 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <usbhub.h>
#include <BTD.h>
#include <PS5BT.h>
#include <PS4BT.h>
#include <PS3BT.h>
#include "Settings.h"

#if defined(DEBUG) || defined(TEST_CONTROLLER)
extern String output;
extern void printOutput(void);
#endif

extern bool driveEnabled;
extern bool driveStopped;
extern byte speedProfile;

extern String getPgmString(const char *);

#define PS2 17

const int LAG_TIME_TO_KILL_MOTORS  = 300;   // Kill motors when lag exceeds 0.3 seconds.
const int LAG_TIME_TO_DISCONNECT   = 10000; // Disconnect controller when lag exceeds 10 seconds.
const int LAG_TIME_TO_RECONNECT    = 200;   // Reconnect controller when lag exceed 0.2 seconds.

const int PLUGGED_STATE_SHORT_INTERVAL = 15;
const int PLUGGED_STATE_LONG_INTERVAL = 100;

struct CriticalFault_Struct {
  uint32_t badData;
  unsigned long lastReadTime;
  unsigned long lastMsgTime;
  unsigned long pluggedStateTime;
  bool reconnect;
};

#ifdef PS3_NAVIGATION
const byte NUMBER_OF_CONTROLLERS = 2;
#else
const byte NUMBER_OF_CONTROLLERS = 1;
#endif

/* ================================================================================
 *                             Parent Controller Class
 * ================================================================================ */
class Controller_Wrapper
{
  protected:
    USB m_Usb;
    BTD m_Btd;
    byte m_controllerConnected;
    CriticalFault_Struct m_faultData[NUMBER_OF_CONTROLLERS];
    unsigned long mcurrentTime;
    unsigned long m_lagTime;
    unsigned long m_lastReadTime;
    byte m_disconnectCount;

    #ifdef DEBUG
    String m_className;
    #endif

    bool m_authorized(void);
    void m_setConnectionStatus(byte n);
    void m_initCriticalFault(byte idx);
    void m_resetCriticalFault(byte idx);

    virtual void m_connect(void) {};
    virtual void m_disconnect(void) {};
    virtual bool m_getUsbStatus(void) {};
    virtual bool m_detectCriticalFault(void);
    virtual int  m_getModifierButtons(void) {};

    #ifdef TEST_CONTROLLER
    void m_displayInput(void);
    void m_scrollInput(void);
    String m_appendString(String inString, String addString);
    void m_displayStick(String s, byte x, byte y);
    #endif

  public:
    Controller_Wrapper(void);
    virtual ~Controller_Wrapper(void);

    void begin(void);
    byte connectionStatus(void);
    void continueDisconnecting(void);
    bool duringDisconnect(void);
    int  getButtonsPressed(void);

    virtual bool read(void) {};
    virtual bool connected(void) {};
    virtual bool getButtonClick(int buttonEnum) {};
    virtual bool getButtonPress(int buttonEnum) {};
    virtual int  getAnalogButton(int buttonEnum) {};
    virtual int  getAnalogHat(int stickEnum) {};
    virtual void setLed(void) {};
};


#if defined(PS4_CONTROLLER)
/* ================================================================================
 *                                  PS4 Controller
 * ================================================================================ */
class Controller_PS4 : public Controller_Wrapper
{
  private:
    PS4BT m_controller;

    static void m_onInit(void);

    virtual void m_connect(void);
    virtual void m_disconnect(void);
    virtual bool m_getUsbStatus(void);
    virtual bool connected(void);
    virtual int  m_getModifierButtons(void);

  public:
    static Controller_PS4 * Controller_PS4::anchor;

    Controller_PS4(void);
    virtual ~Controller_PS4(void);
    void begin(void);

    virtual bool read(void);
    virtual void setLed(void);
    virtual bool getButtonClick(int buttonEnum);
    virtual bool getButtonPress(int buttonEnum);
    virtual int  getAnalogButton(int buttonEnum);
    virtual int  getAnalogHat(int stickEnum);
};
#endif


#if defined(PS3_CONTROLLER)
/* ================================================================================
 *                                  PS3 Controller
 * ================================================================================ */
class Controller_PS3 : public Controller_Wrapper
{
  private:
    PS3BT m_controller;

    static void m_onInit(void);

    virtual void m_connect(void);
    virtual void m_disconnect(void);
    virtual bool m_getUsbStatus(void);
    virtual bool connected(void);
    virtual int  m_getModifierButtons(void);

  public:
    static Controller_PS3 * Controller_PS3::anchor;

    Controller_PS3(void);
    virtual ~Controller_PS3(void);
    void begin(void);

    virtual bool read(void);
    virtual void setLed(void);
    virtual bool getButtonClick(int buttonEnum);
    virtual bool getButtonPress(int buttonEnum);
    virtual int  getAnalogButton(int buttonEnum);
    virtual int  getAnalogHat(int stickEnum);
};
#endif



#if defined(PS3_NAVIGATION)
/* ================================================================================
 *                             PS3 Navigation Controller
 * ================================================================================ */
class Controller_PS3Nav : public Controller_Wrapper
{
  private:
    PS3BT m_controller;
    PS3BT m_secondController;

    static void m_onInit(void);
    void m_onInitConnect(void);

    virtual void m_connect(PS3BT * pController);
    virtual bool connected(PS3BT * pController);
    virtual void m_disconnect(PS3BT * pController);
    virtual bool m_getUsbStatus(void);
    virtual bool m_detectCriticalFault(PS3BT * pController);
    virtual int  m_getModifierButtons(void);

  public:
    static Controller_PS3Nav * Controller_PS3Nav::anchor;

    Controller_PS3Nav(void);
    virtual ~Controller_PS3Nav(void);
    void begin(void);

    virtual bool read(void);
    virtual void setLed(void);
    virtual bool getButtonClick(int buttonEnum);
    virtual bool getButtonPress(int buttonEnum);
    virtual int  getAnalogButton(int buttonEnum);
    virtual int  getAnalogHat(int stickEnum);
};
#endif


#if defined(PS5_CONTROLLER)
/* ================================================================================
 *                                  PS5 Controller
 * ================================================================================ */
class Controller_PS5 : public Controller_Wrapper
{
  private:
    PS5BT m_controller;

    static void m_onInit(void);

    virtual void m_connect(void);
    virtual void m_disconnect(void);
    virtual bool m_getUsbStatus(void);
    virtual bool connected(void);
    virtual int  m_getModifierButtons(void);

  public:
    static Controller_PS5 * Controller_PS5::anchor;

    Controller_PS5(void);
    virtual ~Controller_PS5(void);
    void begin(void);

    virtual bool read(void);
    virtual void setLed(void);
    virtual bool getButtonClick(int buttonEnum);
    virtual bool getButtonPress(int buttonEnum);
    virtual int  getAnalogButton(int buttonEnum);
    virtual int  getAnalogHat(int stickEnum);
};
#endif

#endif
