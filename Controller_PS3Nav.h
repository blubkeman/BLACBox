/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Controller_PS3Nav.h - Library for a PS3 Move Navigation controller
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _CONTROLLER_PS3NAV_H_
#define _CONTROLLER_PS3NAV_H_

#include "Globals.h"
#include <usbhub.h>
#include <BTD.h>
#include <PS3BT.h>

extern String output;


/* ==========================================
 *           Custom data structures
 * ========================================== */
struct CriticalFault_Struct {
  uint32_t badData;
  unsigned long lastMsgTime;
  String priority;
  bool pluggedState;
  unsigned long pluggedStateTime;
  bool waitingToReconnect;
};


/* ====================================
 *           Class Definition
 * ==================================== */
class Controller_PS3Nav
{
  public:
    Controller_PS3Nav(Buffer *);
    void begin(void);
    bool read(void);

  // For use by attachOnInit
    static Controller_PS3Nav * Controller_PS3Nav::anchor;;

  private:
    Buffer * _buffer;
    USB _Usb;
    BTD _Btd;
    PS3BT _primary;
    PS3BT _secondary;

  // Connection functions
    void _connectController(PS3BT *, uint8_t);
    void _disconnectController(PS3BT *, uint8_t);

  // onAttachInit functions
    static void _onInit(void);
    void _onInitConnect(void);

  // Critical fault detection variable and functions
    CriticalFault_Struct _faultData[2];
    unsigned long _lagTime;
    unsigned long _currentTime;
    void _initFaultData(uint8_t);
    bool _detectCriticalFault(PS3BT *, uint8_t);
    void _resetCriticalFault(PS3BT *, uint8_t);
    bool _checkLag(PS3BT *, uint8_t);
    void _checkReconnect(PS3BT *, uint8_t);
    bool _checkSignal(PS3BT *, uint8_t);

  // Buffer functions
    void _setBuffer(void);
    void _crazyIvan(uint8_t, uint8_t, uint8_t);

  // Debugging
    #if defined(DEBUG_CONTROLLER) || defined(DEBUG_ALL)
    String _className;
    #endif
};
#endif
