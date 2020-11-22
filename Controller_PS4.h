/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS4.h - Library for the subclass for the PS4 controller
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _CONTROLLER_PS3NAV_H_
#define _CONTROLLER_PS3NAV_H_

#include "Buffer.h"
#include "Controller_All.h"
#include <PS4BT.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

extern String output;
extern void printOutput(void);


/* ===========================================================
 * This is a test subclass for specifice controller system.
 * =========================================================== */
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
#endif  // _CONTROLLER_PS3NAV_H_
