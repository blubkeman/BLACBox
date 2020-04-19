/*
 * Globals.h - Global variables for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 18 April 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <Arduino.h>
#include "Settings.h"
#include "Buffer.h"

typedef struct {
    //put your variable definitions here for the data you want to send
    //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
    int hpx; // hp movement
    int hpy; // hp movement
    int hpl; // hp light
    int hpa; // hp automation
    int dsp; // 100=no change, 0=random, 1=alarm, 4=whistle, 5=leia, 6=short circut, 10=EQ, 11=alarm2,
}  SEND_DATA_STRUCTURE;
// 21=speed1, 22=speed2, 23=speed3, 24=logics+, 25=logics-

enum PanelSetting {
  usePanel = 0,
  startDelay = 1,
  openTime = 2,
  panelStatus = 3,
  startTime = 4
};

enum PanelStatus {
  statusClosed = 0,
  statusOpen = 1,
  statusClose = 2
};

static int cmd;    // We use this to give directions to peripherals based on controller input.
static bool runningCustomRoutine = false;

String output = "";

#endif
