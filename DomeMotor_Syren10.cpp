/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * DomeMotor_Syren10.cpp - Library for the Syren10 dome motor controller
 * Created by Brian Lubkeman, 22 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include <Arduino.h>
#include "DomeMotor.h"


#if defined(SYREN10)
/* ================================================================================
 *                          Syren10 Motor Controller Class
 * ================================================================================ */

// We use packetized serial mode. Make certain dip switches 1 and 2 are down.
// Dip switch 3 should up *unless and only* if you use Lipo batteries. Then it should be down.

// Set the dip switches 4, 5, and 6 according to the address to be used.
// 128 = 4,5,6 all up     132 = 6 down; 4,5 up
// 129 = 4 down; 5,6 up   133 = 6,4 down; 5 up
// 130 = 5 down; 4,6 up   134 = 6,5 down; 4 up
// 131 = 4,5 down; 6 up   135 = 4,5,6 all down

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
  // --------------------------------
  // Call the parent class's begin().
  // --------------------------------

  DomeMotor::begin();

  // -------------------------------------
  // Start communication with the Syren10.
  // -------------------------------------

  DomeMotorSerial.begin(9600);
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
