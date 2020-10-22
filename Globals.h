/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Globals.h - Library for global definitions
 * Created by Brian Lubkeman, 22 October 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <Arduino.h>
#include "Settings.h"
#include "Buffer.h"

const uint8_t PRIMARY = 0;
const uint8_t SECONDARY = 1;

const int domeInterruptPin = 2;
const int footInterruptPin = 3;

#endif
