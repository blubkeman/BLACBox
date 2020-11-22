/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Qwiic_MP3.h - Library for a Sparkfun Qwiic MP3 Trigger
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _PERIPHERAL_QWIIC_MP3_H_
#define _PERIPHERAL_QWIIC_MP3_H_

#include "Buffer.h"
#include <Wire.h>
#include <SparkFun_Qwiic_MP3_Trigger_Arduino_Library.h>

extern String output;
extern void printOutput(void);


/* ============================================
 *      Qwiic MP3 Trigger class definition
 * ============================================ */
class Qwiic_MP3
{
  public:
    Qwiic_MP3(Buffer * pBuffer);
    void begin(void);
    void interpretController(void);

  private:
    Buffer * _buffer;
    MP3TRIGGER _mp3;
    uint8_t _volume;

    #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
    String _className;
    #endif
};
#endif
