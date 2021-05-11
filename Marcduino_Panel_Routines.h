/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Marcduino_Panel_Routines.h - Library for Marcduino custom panel routines.
 * Created by Brian Lubkeman, 10 May 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */

#ifndef _MARCDUINO_PANEL_ROUTINES_H_
#define _MARCDUINO_PANEL_ROUTINES_H_

// ----------------------
// Custom Panel Routines.
// ----------------------

// In this sample routine, panels #1-#6 are used.  One second after starting the routine,
// panel #1 begins opening.  Half a second later, panel #1 begins closing while panel #2
// begins opening. Each panel in sequence follows the same pattern.  The whole sequence
// is complete in 5 seconds.

Panel_Routine_Struct sampleRoutine[] = {
  { false, 1, 1000, 500 },
  { false, 2, 1500, 500 },
  { false, 3, 2000, 500 },
  { false, 4, 2500, 500 },
  { false, 5, 3000, 500 },
  { false, 6, 3500, 500 }
};

#endif
