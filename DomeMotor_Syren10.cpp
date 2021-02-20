/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DomeMotor_Syren10.cpp - Library for the Syren10 dome motor controller
 * Created by Brian Lubkeman, 20 February 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "DomeMotor.h"

#if defined(SYREN10)
/* ================================================================================
 *                          Syren10 Motor Controller Class
 * ================================================================================ */

const int SYREN_ADDR = 129;  // Serial Address for Dome Syren

// =====================
//      Constructor
// =====================
#if defined(PS4_CONTROLLER)
Syren10_DomeMotor::Syren10_DomeMotor(Controller_PS4 * pController) : DomeMotor(), m_syren(SYREN_ADDR, DomeMotorSerial)
#else
Syren10_DomeMotor::Syren10_DomeMotor(Controller_PS3 * pController) : DomeMotor(), m_syren(SYREN_ADDR, DomeMotorSerial)
#endif
{
  m_controller = pController;

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
  m_className = "Syren10_DomeMotor::";
  #endif
}

// ====================
//      Destructor
// ====================
Syren10_DomeMotor::~Syren10_DomeMotor(void) {}

// =================
//      begin()
// =================
void Syren10_DomeMotor::begin(void)
{
  // -----------------------------------------------------------------------------
  // Call the parent class's begin(). It establishes our dead man switch, if used.
  // -----------------------------------------------------------------------------

  DomeMotor::begin();

  // -------------------------------------
  // Start communication with the Syren10.
  // -------------------------------------

  m_syren.autobaud();
  m_syren.setTimeout(300);
  m_syren.stop();

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
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
void Syren10_DomeMotor::stop(void)
{
  // --------------------------------------------------
  // When the motor is already not running, do nothing.
  // --------------------------------------------------

  if ( domeStopped ) {
    return;
  }

  // ----------------------
  // Send the stop command.
  // ----------------------

  m_syren.stop();

  // -----------------------------
  // Update the dome motor status.
  // -----------------------------

  domeStopped = true;

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
  output = m_className+F("stop()");
  output += F(" - ");
  output += F("Stopped dome motor");
  printOutput();
  #endif
}

// ========================
//      m_rotateDome()
// ========================
void Syren10_DomeMotor::m_rotateDome(int rotationSpeed)
{

  // ------------------------
  // Send the rotate command.
  // ------------------------

  m_syren.motor(rotationSpeed);

  // -----------------------------
  // Update the dome motor status.
  // -----------------------------

  domeStopped = false;

  // ----------
  // Debugging.
  // ----------

  #ifdef DEBUG
  if ( ! domeStopped ) {
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

#endif
