/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controlles.h - Library for supported controllers
 * Created by Brian Lubkeman, 17 December 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _CONTROLLERS_H_
#define _CONTROLLERS_H_

#include "Buffer.h"
#include <usbhub.h>
#include <BTD.h>
#include <PS4BT.h>
#include <PS3BT.h>

extern String output;
extern void printOutput(void);

// Controller device count

#ifdef PS3_NAVIGATION
const uint8_t MAX_NUMBER_OF_CONTROLLERS = 2;
#else
const uint8_t MAX_NUMBER_OF_CONTROLLERS = 1;
#endif


/* ==========================================
 *           Custom data structures
 * ========================================== */
struct CriticalFault_Struct {
  uint32_t badData;
  unsigned long lastMsgTime;
  unsigned long pluggedStateTime;
  bool reconnect;
};


/* ================================================================================
 *                             Parent Controller Class
 * ================================================================================ */

class Controller_Parent
{
  public:
    Controller_Parent(Buffer *);
    void begin(void);

    virtual ~Controller_Parent(void);
    virtual bool read(void) {};

  protected:
    Buffer * _buffer;
    USB _Usb;
    BTD _Btd;
    unsigned long _currentTime;
    unsigned long _lagTime;

  // onAttachInit functions
    // Each subclass must also define: static void _onInit(void)
    virtual void _onInitConnect(void) {};

  // Connection functions
    String _getMACAddress();
    bool _authorized(char * [], uint8_t);
    
    virtual void _connect() {};
    virtual void _disconnect() {};
    virtual void _setLed(void) {};

  // Critical fault detection variable and functions
    virtual void _initCriticalFault(uint8_t) {};
    virtual bool _detectCriticalFault() {};
    virtual void _resetCriticalFault() {};

  // Debugging
    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL) || defined(TEST_CONTROLLER)
    String _className;
    #endif
};


/* ================================================================================
 *                                  PS4 Controller
 * ================================================================================ */
 
class Controller_PS4 : public Controller_Parent
{
  public:
    Controller_PS4(Buffer *);
    void begin(void);
    bool read(void);

  // For use by attachOnInit
    static Controller_PS4 * Controller_PS4::anchor;

  private:
    PS4BT _controller;
    virtual void _connect(PS4BT *);
    virtual void _disconnect(PS4BT *);
    virtual void _setLed(void);

  // onAttachInit functions
    static void _onInit(void);
    virtual void _onInitConnect(void);

  // Critical fault detection variable and functions
    CriticalFault_Struct _faultData;
    unsigned long _lastReadTime;
    virtual void _initCriticalFault(void);
    virtual bool _detectCriticalFault(void);
    virtual void _resetCriticalFault(void);

  // Rumble functions
    unsigned long _rumbleStartTime;

  // Buffer functions
    void _setBuffer(void);
    void _updateBuffer(void);
};


/* ================================================================================
 *                                  PS3 Controller
 * ================================================================================ */

class Controller_PS3 : public Controller_Parent
{
  public:
    Controller_PS3(Buffer *);
    void begin(void);
    bool read(void);

  // For use by attachOnInit
    static Controller_PS3 * Controller_PS3::anchor;

  private:
    PS3BT _controller;
    virtual void _connect(PS3BT *);
    virtual void _disconnect(PS3BT *);
    virtual void _setLed(void);

  // onAttachInit functions
    static void _onInit(void);
    virtual void _onInitConnect(void);

  // Critical fault detection variable and functions
    CriticalFault_Struct _faultData;
    virtual void _initCriticalFault(void);
    virtual bool _detectCriticalFault(void);
    virtual void _resetCriticalFault(void);

  // Buffer functions
    void _setBuffer(void);
    void _updateBuffer(void);
};


/* ================================================================================
 *                          PS3 Move Navigation Controller
 * ================================================================================ */
 
class Controller_PS3Nav : public Controller_Parent
{
  public:
    Controller_PS3Nav(Buffer *);
    void begin(void);
    bool read(void);

  // For use by attachOnInit
    static Controller_PS3Nav * Controller_PS3Nav::anchor;

  private:
    PS3BT _controller;
    PS3BT _secondController;
    virtual void _connect(PS3BT *);
    virtual void _disconnect(PS3BT *);
    virtual void _setLed(void) {};

  // onAttachInit functions
    static void _onInit(void);
    virtual void _onInitConnect(void);

  // Critical fault detection variable and functions
    CriticalFault_Struct _faultData;
    CriticalFault_Struct _secondFaultData;
    virtual void _initCriticalFault(CriticalFault_Struct *);
    virtual bool _detectCriticalFault(CriticalFault_Struct *);
    virtual void _resetCriticalFault(CriticalFault_Struct *);

  // Buffer functions
    void _setBuffer(void);
    void _updateBuffer(void);
    void _crazyIvan(uint8_t, uint8_t, uint8_t);
};
#endif
