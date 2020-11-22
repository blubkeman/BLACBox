/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_All.h - Library for parent class of all controllers
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _CONTROLLER_PARENT_H_
#define _CONTROLLER_PARENT_H_

#include "Buffer.h"
#include <usbhub.h>
#include <BTD.h>

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


/* ===========================================================
 * This is a generic, parent class for all controller systems.
 * It defines content common across all subclasses.
 * =========================================================== */
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

  // Critical fault detection variable and functions
    virtual void _initCriticalFault(uint8_t) {};
    virtual bool _detectCriticalFault() {};
    virtual void _resetCriticalFault() {};

  // Debugging
    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL) || defined(TEST_CONTROLLER)
    String _className;
    #endif
};
#endif
