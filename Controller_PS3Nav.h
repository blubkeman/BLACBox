/*
 * PS3_Nav.h - Library for PS3 Move Navigation controller for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 18 April 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _CONTROLLER_PS3NAV_H_
#define _CONTROLLER_PS3NAV_H_

#include "Globals.h"
#include <math.h>
#include <usbhub.h>
#include <BTD.h>
#include <PS3BT.h>

const int PRIMARY = 0;
const int SECONDARY = 1;
extern String output;

typedef struct {
  uint32_t badData;
  unsigned long lastMsgTime;
  String priority;
} CriticalFault_Struct;


/* ====================================
 *           Class Definition
 * ==================================== */
class BLACBox_PS3Nav
{
  private:
    Buffer* _buffer;
    USB _Usb;
    BTD _Btd;
    PS3BT _primary;
    PS3BT _secondary;
    CriticalFault_Struct _faultData[2];
    void _setBuffer();
    void _manageDisconnect();
    bool _isConnected(PS3BT *controller);

  // onAttachInit functions
    static void _onInitPrimary();
    static void _onInitSecondary();
    void _onInitValidate(uint8_t controllerNumber);
    void _swapControllers();

  // Critical fault detection functions
    bool _detectCriticalFault(PS3BT* controller, uint8_t controllerNumber);
    void _resetCriticalFault(PS3BT* controller, uint8_t controllerNumber);
    void _displayTimes(unsigned long, unsigned long, unsigned long);

  // Stop motor functions
    void _stopDomeMotor();
    void _stopFootMotors();
    void _stopAllMotors();

  public:
    // =====================
    //      Constructor
    // =====================
    BLACBox_PS3Nav(Buffer* pBuffer) : _Usb(), _Btd(&_Usb), _primary(&_Btd), _secondary(&_Btd) {
      // Prepare critical fault data for each controller.
      _faultData[PRIMARY].badData       = 0;
      _faultData[PRIMARY].lastMsgTime   = -1;
      _faultData[PRIMARY].priority      = "primary";
      _faultData[SECONDARY].badData     = 0;
      _faultData[SECONDARY].lastMsgTime = -1;
      _faultData[SECONDARY].priority    = "secondary";
      // Define a buffer to store the controller input.
      _buffer = pBuffer;    
    };

    // For attachOnInit
    static BLACBox_PS3Nav* BLACBox_PS3Nav::thisController;

    // =================
    //      begin()
    // =================
    void begin() {
      // Start the USB host.
      if (_Usb.Init() == -1) {
        Serial.println(F("OSC did not start"));
        while (1); //halt
      }
      Serial.println(F("Bluetooth Library Started"));
      // Prepare for starting the controller(s).
      thisController = this;
      _primary.attachOnInit(_onInitPrimary);
      _secondary.attachOnInit(_onInitSecondary);
    };

    // ================
    //      read()
    // ================
    bool read() {

      #ifdef TEST_CONTROLLER
      _buffer->inputRecvd = false;
      #endif

      // Finish cycling the status after disconnecting the primary controller.
      if (_buffer->getCycle() == 2) { _buffer->advanceCycle(); }

      // The more devices we have connected to the USB or BlueTooth, the 
      // more often Usb.Task needs to be called to eliminate latency.
      _Usb.Task();
      if (primaryIsConnected()) _Usb.Task();
      if (secondaryIsConnected()) _Usb.Task();

      // Make sure the motors are not running when we have no controllers.
      if (!primaryIsConnected() && !secondaryIsConnected()) {
        _stopAllMotors();
        return false;
      }

      // Look for a fault condition. When found do NOT process any controller data!
      if (_detectCriticalFault(&_primary, PRIMARY) || 
          _detectCriticalFault(&_secondary, SECONDARY))
        return false;

      // Get input from the controller(s).
      _setBuffer();

      // Look for disconnect commands from the user.
      // A disconnect may be issued using L2+PS.
      _manageDisconnect();

      // Optionally, display controller input to the serial monitor.
      return true;

    };

  /* =============================================
   *          Controller status function
   * ============================================= */
    bool primaryIsConnected()   { return _primary.PS3NavigationConnected; };
    bool secondaryIsConnected() { return _secondary.PS3NavigationConnected; };

};

// ======================
//      _setBuffer()
// ======================
void BLACBox_PS3Nav::_setBuffer() {
  // Set controller connection states.

  if (primaryIsConnected()) {
    _buffer->setStatus(Status::PrimaryControllerConnected, true);
    if (secondaryIsConnected()) {
      _buffer->setStatus(Status::SecondaryControllerConnected, true);
    } else {
      _buffer->setStatus(Status::SecondaryControllerConnected, false);
    }
  } else {
    _buffer->setStatus(Status::PrimaryControllerConnected, false);
    _buffer->setStatus(Status::SecondaryControllerConnected, false);
  }

  // Save the previous hat values for critical fault detection.

  _buffer->saveStick();

  // Get button states for a primary PS3 Move Navigation controller.

  _buffer->setButton(UP,     _primary.getButtonClick(UP));
  _buffer->setButton(RIGHT,  _primary.getButtonClick(RIGHT));
  _buffer->setButton(DOWN,   _primary.getButtonClick(DOWN));
  _buffer->setButton(LEFT,   _primary.getButtonClick(LEFT));
  _buffer->setButton(SELECT, _primary.getButtonPress(CROSS));
  _buffer->setButton(START,  _primary.getButtonPress(CIRCLE));
  _buffer->setButton(L3,     _primary.getButtonClick(L3));
  _buffer->setButton(L2,     _primary.getAnalogButton(L2));
  _buffer->setButton(L1,     _primary.getButtonPress(L1));
  _buffer->setButton(PS,     _primary.getButtonPress(PS));
  _buffer->setStick(LeftHatX,  _primary.getAnalogHat(LeftHatX));
  _buffer->setStick(LeftHatY,  _primary.getAnalogHat(LeftHatY));

  // Get button states for a secondary PS3 Move Navigation controller.
  // We can store some of these in the unused buttons from the primary controller.

  if (secondaryIsConnected()) {
    _buffer->setButton(TRIANGLE, _secondary.getButtonClick(UP));
    _buffer->setButton(CIRCLE,   _secondary.getButtonClick(RIGHT));
    _buffer->setButton(CROSS,    _secondary.getButtonClick(DOWN));
    _buffer->setButton(SQUARE,   _secondary.getButtonClick(LEFT));
    _buffer->setButton(SELECT,  (_primary.getButtonPress(CROSS) + _secondary.getButtonPress(CROSS)) > 0 ? 1 : 0);
    _buffer->setButton(START,   (_primary.getButtonPress(CIRCLE) + _secondary.getButtonPress(CIRCLE)) > 0 ? 1 : 0);
    _buffer->setButton(R3,       _secondary.getButtonClick(L3));
    _buffer->setButton(L2,      (_primary.getAnalogButton(L2) + _secondary.getAnalogButton(L2)) > 0 ? max(_primary.getAnalogButton(L2), _secondary.getAnalogButton(L2)) : 0);
    _buffer->setButton(L1,      (_primary.getButtonPress(L1) + _secondary.getButtonPress(L1)) > 0 ? 1 : 0);
    _buffer->setButton(PS2,      _secondary.getButtonPress(PS));
    _buffer->setStick(RightHatX, _secondary.getAnalogHat(LeftHatX));
    _buffer->setStick(RightHatY, _secondary.getAnalogHat(LeftHatY));
  }

  #ifdef TEST_CONTROLLER
  // Using only one of these at a time is recommended.
  _buffer->testInput();
  //_buffer.scrollInput();
  #endif
};

// =============================
//      _manageDisconnect()
// =============================
void BLACBox_PS3Nav::_manageDisconnect() {
  if (secondaryIsConnected() && _buffer->getButton(L2) && _buffer->getButton(PS2)) {
    #ifdef BLACBOX_DEBUG
    output = F("BLACBox_PS3Nav::");
    output += F("_manageDisconnect - ");
    output += F("Disconnecting secondary controller.");
    Serial.println(output);
    #endif
    _secondary.disconnect();
    _resetCriticalFault(&_secondary, SECONDARY);
  }

  if (primaryIsConnected() && _buffer->getButton(L2) && _buffer->getButton(PS)) {
    #ifdef BLACBOX_DEBUG
    output = F("BLACBox_PS3Nav::");
    output += F("_manageDisconnect - ");
    output += F("Disconnecting primary controller.");
    Serial.println(output);
    #endif
    _primary.disconnect();
    if (!_buffer->isSecondaryConnected()) { _buffer->advanceCycle(); }
    _resetCriticalFault(&_primary, PRIMARY);
  }

  if (!primaryIsConnected() && secondaryIsConnected()) {
    #ifdef BLACBOX_DEBUG
    output = F("BLACBox_PS3Nav::");
    output += F("_manageDisconnect - ");
    output += F("Moving secondary controller to primary.");
    Serial.println(output);
    #endif
    _swapControllers();
  }
};

// ========================
//      _isConnected()
// ========================
bool BLACBox_PS3Nav::_isConnected(PS3BT *controller) {
  return controller->PS3NavigationConnected;
};

/* =========================================
 *          attachOnInit functions
 * =========================================
 * Due to how the USB Host Shield library was written, the 'attachOnInit' function requires
 * our function to be static and have no arguments.  This prevents us from calling other,
 * non-static members of the PS3BT class.  To get around this, we define a static function
 * which in turn calls a non-static function. */

// ==========================
//      _onInitPrimary()
// ==========================
static void BLACBox_PS3Nav::_onInitPrimary() {
  // This is a static function called by 'attachOnInit'.
  if (BLACBox_PS3Nav::thisController != NULL) {
    BLACBox_PS3Nav::thisController->_onInitValidate(PRIMARY);
  }
};

// ============================
//      _onInitSecondary()
// ============================
static void BLACBox_PS3Nav::_onInitSecondary() {
  // This is a static function called by 'attachOnInit'.
  if (BLACBox_PS3Nav::thisController != NULL) {
    BLACBox_PS3Nav::thisController->_onInitValidate(SECONDARY);
  }
};

// =========================
//      _onInitValidate
// =========================
void BLACBox_PS3Nav::_onInitValidate(uint8_t controllerNumber) {
  // This is the non-static function which can call non-static members of the PS3BT class.
  // We cannot pass a pointer to the controller through the arguments. The compiler 
  // again complains about it being non-static. But we can pass the controller number 
  // and use that to determine which controller we're dealing with and thus set a 
  // pointer to it.  Tricksy!
  PS3BT* controller;
  if (controllerNumber == PRIMARY) controller = &_primary;
  else if (controllerNumber == SECONDARY) controller = &_secondary;

  // Now we can interact with the controller object.
  // First, let's make sure we have a PS3 Move Navigation controller.  If not, drop it.
  if (!_isConnected(controller)) {
    controller->disconnect();
    #ifdef BLACBOX_DEBUG
    output = F("BLACBox_PS3Nav::");
    output += F("_onInitValidate - ");
    output = F("Disconnecting invalid controller.");
    Serial.println(output);
    #endif
    return;
  }

  // We have a PS3 Move Navigation.  Finish its setup.
  controller->setLedOn(LED1);
  _buffer->advanceCycle();
  _resetCriticalFault(controller, controllerNumber);

  // Get the MAC address of the controller as identified through the Bluetooth dongle.
  String btAddress;
  for(int8_t i = 5; i >= 0; i--) {
    if (btAddress.length() > 0) btAddress +=(":");
    if (_Btd.disc_bdaddr[i]<0x10) btAddress +="0";
    btAddress += String(_Btd.disc_bdaddr[i], HEX);
  }
  btAddress.toUpperCase();

  // Display the MAC address on the serial monitor.
  #ifdef BLACBOX_DEBUG
  output = F("BLACBox_PS3Nav::");
  output += F("_onInitValidate - ");
  output += F("MAC address: ");
  output += btAddress;
  Serial.println(output);

  output = F("BLACBox_PS3Nav::");
  output += F("_onInitValidate - ");
  if (btAddress == PRIMARY_CONTROLLER_MAC || btAddress == BACKUP_CONTROLLER_MAC)
    output += F("Primary controller connected.");
  else
    if (controllerNumber == PRIMARY)
      output = F("Unidentified primary controller connected.");
    else
      output = F("Secondary controller connected.");
  Serial.println(output);
  #endif

  // We may have started the controllers in the wrong sequence.  Swap them if necessary.
  if (controllerNumber == SECONDARY && (btAddress == PRIMARY_CONTROLLER_MAC || btAddress == BACKUP_CONTROLLER_MAC)) {
    _swapControllers();
  }
};

// ============================
//      _swapControllers()
// ============================
void BLACBox_PS3Nav::_swapControllers() {
  #ifdef BLACBOX_DEBUG
  output = F("BLACBox_PS3Nav::");
  output += F("_swapControllers - ");
  output += F("Controllers started in reverse order.\r\nSwapping their priorities.");
  Serial.println(output);
  #endif

  // Swap the controllers' priority.
  PS3BT temp = _primary;
  _primary = _secondary;
  _secondary = temp;

  // Reset the onInit functions.
  _primary.attachOnInit(_onInitPrimary);
  _secondary.attachOnInit(_onInitSecondary); 

  // Reset to avoid a false critical fault.
  _resetCriticalFault(&_primary, PRIMARY);
  _resetCriticalFault(&_secondary, SECONDARY);

  _buffer->begin();
};

/* =====================================================
 *          Critical fault detection functions
 * ===================================================== */

// ================================
//      _detectCriticalFault()
// ================================
bool BLACBox_PS3Nav::_detectCriticalFault(PS3BT* controller, uint8_t controllerNumber) {

  // Skip critical fault detection when the controller is not connected.
  if (!_isConnected(controller) || _faultData[controllerNumber].lastMsgTime <= 0) {
    return false;
  }

  // Calculate the lag time.
  _faultData[controllerNumber].lastMsgTime = (unsigned long)controller->getLastMessageTime();
  unsigned long currentTime = millis();
  unsigned long lagTime = (currentTime - _faultData[controllerNumber].lastMsgTime);

  // Skip checking lag when we get bad data from the controller.
  if (lagTime < 0) {
    lagTime = 0;
    _faultData[controllerNumber].badData++;
    #ifdef BLACBOX_DEBUG
    output = F("BLACBox_PS3Nav::");
    output += F("_detectCriticalFault - ");
    output += F("Waiting for ");
    output += _faultData[controllerNumber].priority;
    output += F(" controller data.");
    Serial.println(output);
    #endif
  }
  else if (_faultData[controllerNumber].lastMsgTime > 0) {
    
    // Check lag in the primary controller.

    if (controllerNumber == PRIMARY) {
      // After 1/2 of a second lag, stop all motors but do not disconnect the controller yet.
      // I found that processing a command through MarcDuino can take up to 509-510ms, so I reset
      // the lag time threshold to that level.
      if (lagTime > 510) {
        /*
         * TODO: Work out isFootMotorStopped, isDomeMotorStopped
         */
//        if (!isFootMotorStopped || !isDomeMotorStopped) {
          #ifdef BLACBOX_DEBUG
          output = F("BLACBox_PS3Nav::");
          output += F("_detectCriticalFault - ");
          output += F("It has been 510ms since we heard from the primary controller.");
          Serial.println(output);
          _displayTimes(_faultData[controllerNumber].lastMsgTime, currentTime, lagTime);
          output = F("CRITICAL FAULT: Shutting down motors, and watching for a new message.");
          Serial.println(output);
          #endif
          _stopAllMotors();
//        }
        return true;
      }

      // After 30 seconds lag, disconnect the primary controller.

      if (lagTime > 30000) {
        #ifdef BLACBOX_DEBUG
        output = F("BLACBox_PS3Nav::");
        output += F("_detectCriticalFault - ");
        output += F("It has been 30s since we heard from the primary controller.");
        Serial.println(output);
        _displayTimes(_faultData[controllerNumber].lastMsgTime, currentTime, lagTime);
        output = F("CRITICAL FAULT: Disconnecting the controller.");
        Serial.println(output);
        #endif
        controller->disconnect();
        return true;
      }
    }

    // Check lag in the secondary controller.

    if (controllerNumber == SECONDARY) {
      // After 10 seconds lag, stop the dome motor and disconnect the secondary controller.
  
      if (lagTime > 10000) {

        #ifdef BLACBOX_DEBUG
        output = F("BLACBox_PS3Nav::");
        output += F("_detectCriticalFault - ");
        output += F("It has been 10s since we heard from the secondary controller.");
        Serial.println(output);
        _displayTimes(_faultData[controllerNumber].lastMsgTime, currentTime, lagTime);
        output = F("CRITICAL FAULT: Shutting down dome motor, and disconnecting the controller.");
        Serial.println(output);
        #endif
        _stopDomeMotor();
        controller->disconnect();
        return true;
      }
    }
  }

  // Now, let's see if the content is good.

  if (!controller->getStatus(Plugged) && !controller->getStatus(Unplugged)) {
    // The controller says it is neither plugged nor unplugged.
    // Wait 10ms, Update USB, and try again
    delay(10);
    _Usb.Task();

    if (!controller->getStatus(Plugged) && !controller->getStatus(Unplugged)) {
      #ifdef BLACBOX_DEBUG
      output = F("BLACBox_PS3Nav::");
      output += F("_detectCriticalFault - ");
      output += F("Invalid data from the ");
      output += _faultData[controllerNumber].priority;
      output += F(" controller.");
      Serial.println(output);
      #endif
      _faultData[controllerNumber].badData++;
      return true;
    }

  }
  else if (_faultData[controllerNumber].badData > 0) {
    // Reset badData count for this controller.
    _faultData[controllerNumber].badData = 0;
  }

  if (_faultData[controllerNumber].badData > 10) {
    // We're getting too much garbage. Disconnect the controller.
    #ifdef BLACBOX_DEBUG
    output = F("BLACBox_PS3Nav::");
    output += F("_detectCriticalFault - ");
    output += F("Too much bad data coming from the ");
    output += _faultData[controllerNumber].priority;
    output += F(" controller.");
    Serial.println(output);
    output = F("CRITICAL FAULT: Disconnecting the controller.");
    Serial.println(output);
    #endif
    if (controllerNumber == PRIMARY) _stopFootMotors();
    _stopDomeMotor();
    controller->disconnect();
    return true;
  }

  // When a joystick suddenly jumps to 0,0 ignore
  // the input and reset to the previous value.
  _buffer->crazyIvan(LeftHatX, LeftHatY);
  _buffer->crazyIvan(RightHatX, RightHatY);
  
  // If we get this far, there is no fault detected.
  return false;
};

// ===============================
//      _resetCriticalFault()
// ===============================
void BLACBox_PS3Nav::_resetCriticalFault(PS3BT* controller, uint8_t controllerNumber) {
  _faultData[controllerNumber].badData = 0;
  if (!_isConnected(controller) || controller->getLastMessageTime() == 0)
    _faultData[controllerNumber].lastMsgTime = -1;
  else
    _faultData[controllerNumber].lastMsgTime = controller->getLastMessageTime();
};

// =========================
//      _displayTimes()
// =========================
void BLACBox_PS3Nav::_displayTimes(unsigned long msgTime, unsigned long currTime, unsigned long lagTime) {
  output = F("\tlastMsgTime: ");      output += msgTime;
  output += F("\r\n\tcurrentTime: "); output += currTime;
  output += F("\r\n\tlagTime:     "); output += lagTime;
  Serial.println(output);
}

/* =======================================
 *          Stop motor functions
 * ======================================= */

// ==========================
//      _stopDomeMotor()
// ==========================
void BLACBox_PS3Nav::_stopDomeMotor() {
/*
 * TODO: Work out isFootMotorStopped, stopFeet()
 * 
  stopFeet();
 */
  _buffer->setStatus(FootMotorStopped, true);
};

// ===========================
//      _stopFootMotors()
// ===========================
void BLACBox_PS3Nav::_stopFootMotors() {
/*
 * TODO: Work out SyR.stop();
 * 
  SyR.stop();
 */
};

// ==========================
//      _stopAllMotors()
// ==========================
void BLACBox_PS3Nav::_stopAllMotors() {
  _stopFootMotors();
  _stopDomeMotor();
};

#endif
