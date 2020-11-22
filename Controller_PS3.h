/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS3.h - Library for the subclass for the PS3 controller
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _CONTROLLER_PS3NAV_H_
#define _CONTROLLER_PS3NAV_H_

#include "Buffer.h"
#include "Controller_All.h"
#include <PS3BT.h>

extern String output;
extern void printOutput(void);


/* ===========================================================
 * This is a test subclass for specifice controller system.
 * =========================================================== */
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
#endif  // _CONTROLLER_PS3NAV_H_
