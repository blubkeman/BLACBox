/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DomeMotor_Syren10.cpp - Library for the Syren10 dome motor controller
 * Created by Brian Lubkeman, 16 June 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "DomeMotor.h"

const int SYREN10_BAUD_RATE = 9600;     // It is strongly recommended not to change this.

enum syren10_setting_index_e {
   iAddress   // 0 - Syren10 address.
};

/* ================================================================================
 *                          Syren10 Motor Controller Class
 * ================================================================================ */

// =====================
//      Constructor
// =====================
DomeMotor_Syren10::DomeMotor_Syren10 (
    Controller* pController,
    const byte settings[],
    const unsigned long timings[],
    const int syrenSettings[] )
  : DomeMotor(pController, settings, timings),
    m_syren(syrenSettings[iAddress], DomeMotor_Serial)
{
  m_controller = pController;
  m_syrenSettings = syrenSettings;
  m_domeStopped = true;
}

// ====================
//      Destructor
// ====================
DomeMotor_Syren10::~DomeMotor_Syren10(void) {}

// =================
//      begin()
// =================
void DomeMotor_Syren10::begin(void)
{
  // --------------------------------
  // Call the parent class's begin().
  // --------------------------------

  DomeMotor::begin();

  // -------------------------------------
  // Start communication with the Syren10.
  // -------------------------------------

  DomeMotor_Serial.begin(SYREN10_BAUD_RATE);
  m_syren.setTimeout(300);
  m_syren.stop();

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("DomeMotor_Syren10"), F("Syren10 motor controller started"));
  #endif
}

// ================
//      stop()
// ================
void DomeMotor_Syren10::stop(void)
{
  if ( m_domeStopped ) {
    return;
  }

  m_syren.stop();
  m_domeStopped = true;

  #if defined(DEBUG)
  Debug.print(DBG_INFO, F("DomeMotor_Syren10"), F("stop()"), F("Stopped dome motor"));
  #endif
}

// ========================
//      m_rotateDome()
// ========================
void DomeMotor_Syren10::m_rotateDome(int rotationSpeed)
{
  m_syren.motor(rotationSpeed);
  m_domeStopped = false;

  #if defined(DEBUG)
  if ( ! m_domeStopped ) {
    if ( rotationSpeed == 0 )
      Debug.print(DBG_VERBOSE, F("DomeMotor_Syren10"), F("m_rotateDome()"), F("Stopping dome"));
    else {
      if ( rotationSpeed < 0 ) {
        Debug.print(DBG_VERBOSE, F("DomeMotor_Syren10"), F("m_rotateDome()"), F("Rotate dome at speed "), (String)rotationSpeed);
      } else {
        Debug.print(DBG_VERBOSE, F("DomeMotor_Syren10"), F("m_rotateDome()"), F("Rotate dome at speed "), (String)rotationSpeed);
      }
    }
  }
  #endif
}
