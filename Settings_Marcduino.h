/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings_MD_Commands.h - Library for Marcduino command settings
 * Created by Brian Lubkeman, 20 February 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */

#ifndef _SETTINGS_MARCDUINO_H_
#define _SETTINGS_MARCDUINO_H_

const char cmd_None[]            PROGMEM = "";
const char cmd_FullAwake[]       PROGMEM = ":SE11\r";  // Full Awake
const char cmd_FullAwake2[]      PROGMEM = ":SE14\r";  // Full Awake+
const char cmd_MidAwake[]        PROGMEM = ":SE13\r";  // Mid Awake
const char cmd_QuietMode[]       PROGMEM = ":SE10\r";  // Quiet Mode
const char cmd_DomeAwake[]       PROGMEM = ":CL00\r*RD00\r*OF00\r"; // Close panels, HP random, HP lights off
const char cmd_DomeAwake2[]      PROGMEM = ":CL00\r*RD00\r*ON00\r"; // Close panels, HP random, HP lights on
const char cmd_DomeQuiet[]       PROGMEM = ":CL00\r*ST00\r";        // Close panels, HP reset (random off, servos off, lights off)
const char cmd_BodyAwake[]       PROGMEM = ":CL00\r$R\r";           // Close panels, Sound random
const char cmd_BodyQuiet[]       PROGMEM = ":CL00\r$s\r";           // Close panels, Sound off
const char cmd_Scream[]          PROGMEM = ":SE01\r";  // Scream
const char cmd_ScreamNS[]        PROGMEM = ":SE51\r";  // Scream - No sound
const char cmd_ScreamSO[]        PROGMEM = "$S\r";     // Scream - Sound only
const char cmd_Wave[]            PROGMEM = ":SE02\r";  // Wave
const char cmd_WaveNS[]          PROGMEM = ":SE52\r";  // Wave - No sound
const char cmd_FastWave[]        PROGMEM = ":SE03\r";  // Fast Wave
const char cmd_FastWaveNS[]      PROGMEM = ":SE53\r";  // Fast Wave - No sound
const char cmd_Wave2[]           PROGMEM = ":SE04\r";  // Wave 2
const char cmd_Wave2NS[]         PROGMEM = ":SE54\r";  // Wave 2 - No sound
const char cmd_BeepCantina[]     PROGMEM = ":SE05\r";  // Beep Cantina
const char cmd_BeepCantinaNS[]   PROGMEM = ":SE55\r";  // Beep Cantina - No sound
const char cmd_Faint[]           PROGMEM = ":SE06\r";  // Faint/Short Circuit
const char cmd_FaintNS[]         PROGMEM = ":SE56\r";  // Faint/Short Circuit - No sound
const char cmd_FaintSO[]         PROGMEM = "$F\r";     // Faint/Short Circuit - Sound only
const char cmd_CantinaDance[]    PROGMEM = ":SE07\r";  // Cantina Dance
const char cmd_CantinaDanceNS[]  PROGMEM = ":SE57\r";  // Cantina Dance - No sound
const char cmd_CantinaShortSO[]  PROGMEM = "$c\r";     // Cantina Dance - Short sound only
const char cmd_CantinaLongSO[]   PROGMEM = "$C\r";     // Cantina Dance - Short sound only
const char cmd_LeiaMsg[]         PROGMEM = ":SE08\r";  // Leia Message
const char cmd_LeiaMsgNS[]       PROGMEM = ":SE00\r@0T6\r*HD01";  // Leia Message, no sound
const char cmd_LeiaMsgSO[]       PROGMEM = "$L\r";     // Leia Message - Sound only
const char cmd_Disco[]           PROGMEM = ":SE09\r";  // Disco
const char cmd_DiscoSO[]         PROGMEM = "$D\r";     // Disco - Sound only
const char cmd_VolumeDown[]      PROGMEM = "$-\r";     // Volume Down
const char cmd_VolumeMax[]       PROGMEM = "$f\r";     // Volume max
const char cmd_VolumeMid[]       PROGMEM = "$m\r";     // Volume mid
const char cmd_VolumeUp[]        PROGMEM = "$+\r";     // Volume Up
const char cmd_HPLightsOff[]     PROGMEM = "*OF00\r";  // Holos all lights off
const char cmd_HPLightsOn[]      PROGMEM = "*ON00\r";  // Holos all lights on
const char cmd_HPRandomOn[]      PROGMEM = "*RD00\r";  // Holos all random movement on
const char cmd_HPReset[]         PROGMEM = "*ST00\r";  // Holos all reset
const char cmd_OpenPanelAll[]    PROGMEM = ":OP00\r";  // Open all panels
const char cmd_OpenPanel1[]      PROGMEM = ":OP01\r";  // Open panel #1
const char cmd_OpenPanel2[]      PROGMEM = ":OP02\r";  // Open panel #2
const char cmd_OpenPanel3[]      PROGMEM = ":OP03\r";  // Open panel #3
const char cmd_OpenPanel4[]      PROGMEM = ":OP04\r";  // Open panel #4
const char cmd_ClosePanelAll[]   PROGMEM = ":CL00\r";  // Close all panels
const char cmd_ClosePanel1[]     PROGMEM = ":CL01\r";  // Close panel #1
const char cmd_ClosePanel2[]     PROGMEM = ":CL02\r";  // Close panel #2
const char cmd_ClosePanel3[]     PROGMEM = ":CL04\r";  // Close panel #4
const char cmd_ClosePanel4[]     PROGMEM = ":CL03\r";  // Close panel #3
const char cmd_Custom1[]         PROGMEM = "$88\r@0T10\r";  // Play bank 8 track 8, display "Star Wars"
const char cmd_Custom2[]         PROGMEM = "$89\r@0T1\r";   // Play bank 8 track 9, display random
const char cmd_Custom3[]         PROGMEM = "$810\r@0T1\r";  // Play bank 8 track 10, display random
const char cmd_Custom4[]         PROGMEM = "$811\r@0T1\r";  // Play bank 8 track 11, display random
const char cmd_Bank8Track8[]     PROGMEM = "$88\r";    // Play bank 8 track 8
const char cmd_Bank8Track9[]     PROGMEM = "$89\r";    // Play bank 8 track 9
const char cmd_Bank8Track10[]    PROGMEM = "$810\r";   // Play bank 8 track 10
const char cmd_Bank8Track11[]    PROGMEM = "$811\r";   // Play bank 8 track 10
const char cmd_LogicsSW[]        PROGMEM = "@0T10\r";  // Display "Star Wars"
const char cmd_LogicsRandom[]    PROGMEM = "@0T1\r";   // Display random

typedef struct {
  const char domeCommand;
  const char bodyCommand;
} ButtonMapping_Struct;

/*
 * 0 = UP     4 = TRIANGLE   8 = L1,R1
 * 1 = RIGHT  5 = CIRCLE    16 = SHARE/SELECT/CROSS
 * 2 = DOWN   6 = CROSS     24 = OPTIONS/START/CIRCLE
 * 3 = LEFT   7 = SQUARE    32 = PS,PS2
 */

ButtonMapping_Struct buttonMap[] {
  {cmd_DomeAwake      ,cmd_BodyAwake}    // UP          - Full Awake
 ,{cmd_DomeAwake2     ,cmd_BodyAwake}    // RIGHT       - Full Awake+
 ,{cmd_DomeQuiet      ,cmd_BodyQuiet}    // DOWN        - Quiet Mode
 ,{cmd_DomeQuiet      ,cmd_BodyAwake}    // LEFT        - Mid Awake
 ,{cmd_None           ,cmd_OpenPanel2}   // TRIANGLE    - Open Panel #2 (body)
 ,{cmd_None           ,cmd_ClosePanel1}  // CIRCLE      - Close Panel #1 (body)
 ,{cmd_None           ,cmd_ClosePanel2}  // CROSS       - Close Panel #2 (body)
 ,{cmd_None           ,cmd_OpenPanel1}   // SQUARE      - Open Panel #1 (body)
 
 ,{cmd_CantinaDanceNS ,cmd_None}         // L1+UP       - Cantina Dance
 ,{cmd_Wave2NS        ,cmd_None}         // L1+RIGHT    - Wave 2
 ,{cmd_LeiaMsgNS      ,cmd_LeiaMsgSO}    // L1+DOWN     - Leia Message
 ,{cmd_WaveNS         ,cmd_None}         // L1+LEFT     - Wave
 ,{cmd_OpenPanel1     ,cmd_None}         // L1+TRIANGLE - Open Panel #1 (dome)
 ,{cmd_ClosePanel2    ,cmd_None}         // L1+CIRCLE   - Close Panel #2 (dome)
 ,{cmd_ClosePanel1    ,cmd_None}         // L1+CROSS    - Close Panel #1 (dome)
 ,{cmd_OpenPanel2     ,cmd_None}         // L1+SQUARE   - Open Panel #2 (dome)

 ,{cmd_None           ,cmd_VolumeUp}     // SELECT+UP       - Volume Up
 ,{cmd_HPLightsOff    ,cmd_None}         // SELECT+RIGHT    - HP Lights Off
 ,{cmd_None           ,cmd_VolumeDown}   // SELECT+DOWN     - Volume Down
 ,{cmd_HPLightsOn     ,cmd_None}         // SELECT+LEFT     - HP Lights On
 ,{cmd_None           ,cmd_VolumeMax}    // SELECT+TRIANGLE - Volume Max
 ,{cmd_OpenPanelAll   ,cmd_None}         // SELECT+CIRCLE   - Open Panel All (dome)
 ,{cmd_None           ,cmd_VolumeMid}    // SELECT+CROSS    - Volume Mid
 ,{cmd_ClosePanelAll  ,cmd_None}         // SELECT+SQUARE   - Close Panel All (dome)

 ,{cmd_ScreamNS       ,cmd_ScreamSO}     // START+UP        - Scream
 ,{cmd_FaintNS        ,cmd_FaintSO}      // START+RIGHT     - Faint/Short Circuit
 ,{cmd_None           ,cmd_DiscoSO}      // START+DOWN      - Disco
 ,{cmd_FastWaveNS     ,cmd_None}         // START+LEFT      - Fast Wave
 ,{cmd_HPRandomOn     ,cmd_None}         // START+TRIANGLE  - HP Random Movement
 ,{cmd_HPLightsOff    ,cmd_None}         // START+CIRCLE    - HP Lights Off
 ,{cmd_HPReset        ,cmd_None}         // START+CROSS     - HP Reset
 ,{cmd_HPLightsOn     ,cmd_None}         // START+SQUARE    - HP Lights On

 ,{cmd_LogicsSW       ,cmd_Bank8Track8}  // PS+UP           - Custom 1 (Play bank 8 track 8; Logics display "Star Wars")
 ,{cmd_LogicsRandom   ,cmd_Bank8Track9}  // PS+RIGHT        - Custom 2 (Play bank 8 track 9; Random logics display)
 ,{cmd_LogicsRandom   ,cmd_Bank8Track10} // PS+DOWN         - Custom 3 (Play bank 8 track 10; Random logics display)
 ,{cmd_LogicsRandom   ,cmd_Bank8Track11} // PS+LEFT         - Custom 4 (Play bank 8 track 11; Random logics display)
 ,{cmd_OpenPanel3     ,cmd_None}         // PS+TRIANGLE     - Open Panel #3 (dome)
 ,{cmd_ClosePanel4    ,cmd_None}         // PS+CIRCLE       - Close Panel #4 (dome)
 ,{cmd_ClosePanel3    ,cmd_None}         // PS+CROSS        - Close Panel #3 (dome)
 ,{cmd_OpenPanel4     ,cmd_None}         // PS+SQUARE       - Open Panel #4 (dome)
};

#endif
