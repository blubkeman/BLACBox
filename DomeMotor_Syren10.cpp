/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DomeMotor_Syren10.cpp - Library for the Syren10 dome motor controller
 * Created by Brian Lubkeman, 5 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "DomeMotor.h"

enum syren10_setting_index_e {
  iAddress,   // 0 - Syren10 address.
  iBaudRate   // 1 - Syren10 baud rate.
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
    m_syren(syrenSettings[iAddress], DomeMotorSerial)
{
  m_controller = pController;
  m_syrenSettings = syrenSettings;
  m_domeStopped = true;

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  m_className = "DomeMotor_Syren10::";
  #endif
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

  DomeMotorSerial.begin(m_syrenSettings[iBaudRate]);
  m_syren.setTimeout(300);
  m_syren.stop();

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  output = m_className+F("begin()");
  output += F(" - ");
  output += F("Syren10");
  output += F(" motor controller started.");
  printOutput();
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

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  output = m_className+F("stop()");
  output += F(" - ");
  output += F("Stopped dome motor");
  printOutput();
  #endif
}

// ========================
//      m_rotateDome()
// ========================
void DomeMotor_Syren10::m_rotateDome(int rotationSpeed)
{
  m_syren.motor(rotationSpeed);
  m_domeStopped = false;

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG)
  if ( ! m_domeStopped ) {
    output = m_className+F("m_rotateDome()");
    output += F(" - ");
    if ( rotationSpeed == 0 )
      output += F("Stopping dome.");
    else {
      output += F("Rotate dome at speed ");
      output += rotationSpeed;
      if ( rotationSpeed < 0 ) {
        output += F(" CCW.");
      } else {
        output += F(" CW.");
      }
    }
    printOutput();
  }
  #endif
}
