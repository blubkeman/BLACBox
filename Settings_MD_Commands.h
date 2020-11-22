/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings_MD_Commands.h - Library for Marcduino command settings
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */

#ifndef _SETTINGS_MD_COMMANDS_H_
#define _SETTINGS_MD_COMMANDS_H_

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

const int NUMBER_OF_BUTTON_COMBOS = 40;

// ================================================================================================
// The following maps Marcduino commands to button controls in accordance with the original
// SHADOW_MD code. Minor exceptions for the DEBUG_DRIVEman switch are managed in the peripheral libraries.
// ================================================================================================

#ifdef MD_STD_CONTROLS
#if defined(BODY_SOUND)

// -----------------------------------------------------
// These are the commands when both dome and body master
// are used and sound is sent to the body master.
// -----------------------------------------------------

const char * domeCommands[NUMBER_OF_BUTTON_COMBOS] = {
   cmd_DomeAwake      // UP              - Full Awake
  ,cmd_DomeAwake2     // RIGHT           - Full Awake+
  ,cmd_DomeQuiet      // DOWN            - Quiet Mode
  ,cmd_DomeAwake      // LEFT            - Mid Awake
  ,cmd_None           // SELECT+UP       - Volume Up
  ,cmd_HPLightsOff    // SELECT+RIGHT    - HP Lights Off
  ,cmd_None           // SELECT+DOWN     - Volume Down
  ,cmd_HPLightsOn     // SELECT+LEFT     - HP Lights On
  ,cmd_ScreamNS       // START+UP        - Scream
  ,cmd_FaintNS        // START+RIGHT     - Faint/Short Circuit
  ,cmd_None           // START+DOWN      - Disco
  ,cmd_FastWaveNS     // START+LEFT      - Fast Wave
  ,cmd_LogicsSW       // PS+UP           - Custom 1 (Play bank 8 track 8; Logics display "Star Wars")
  ,cmd_LogicsRandom   // PS+RIGHT        - Custom 2 (Play bank 8 track 9; Random logics display)
  ,cmd_LogicsRandom   // PS+DOWN         - Custom 3 (Play bank 8 track 10; Random logics display)
  ,cmd_LogicsRandom   // PS+LEFT         - Custom 4 (Play bank 8 track 11; Random logics display)
  ,cmd_CantinaDanceNS // L1+UP           - Cantina Dance
  ,cmd_Wave2NS        // L1+RIGHT        - Wave 2
  ,cmd_LeiaMsgNS      // L1+DOWN         - Leia Message
  ,cmd_WaveNS         // L1+LEFT         - Wave

  ,cmd_None           // TRIANGLE        - Open Panel #2 (body)
  ,cmd_None           // CIRCLE          - Close Panel #1 (body)
  ,cmd_None           // CROSS           - Close Panel #2 (body)
  ,cmd_None           // SQUARE          - Open Panel #1 (body)
  ,cmd_None           // SELECT+TRIANGLE - Volume Max
  ,cmd_OpenPanelAll   // SELECT+CIRCLE   - Open Panel All (dome)
  ,cmd_None           // SELECT+CROSS    - Volume Mid
  ,cmd_ClosePanelAll  // SELECT+SQUARE   - Close Panel All (dome)
  ,cmd_HPRandomOn     // START+TRIANGLE  - HP Random Movement
  ,cmd_HPLightsOff    // START+CIRCLE    - HP Lights Off
  ,cmd_HPReset        // START+CROSS     - HP Reset
  ,cmd_HPLightsOn     // START+SQUARE    - HP Lights On
  ,cmd_OpenPanel3     // PS+TRIANGLE     - Open Panel #3 (dome)
  ,cmd_ClosePanel4    // PS+CIRCLE       - Close Panel #4 (dome)
  ,cmd_ClosePanel3    // PS+CROSS        - Close Panel #3 (dome)
  ,cmd_OpenPanel4     // PS+SQUARE       - Open Panel #4 (dome)
  ,cmd_OpenPanel1     // L1+TRIANGLE     - Open Panel #1 (dome)
  ,cmd_ClosePanel2    // L1+CIRCLE       - Close Panel #2 (dome)
  ,cmd_ClosePanel1    // L1+CROSS        - Close Panel #1 (dome)
  ,cmd_OpenPanel2     // L1+SQUARE       - Open Panel #2 (dome)
};

const char * bodyCommands[NUMBER_OF_BUTTON_COMBOS] = {
   cmd_BodyAwake      // UP              - Full Awake
  ,cmd_BodyAwake      // RIGHT           - Full Awake+
  ,cmd_BodyQuiet      // DOWN            - Quiet Mode
  ,cmd_BodyAwake      // LEFT            - Mid Awake
  ,cmd_VolumeUp       // SELECT+UP       - Volume Up
  ,cmd_None           // SELECT+RIGHT
  ,cmd_VolumeDown     // SELECT+DOWN     - Volume Down
  ,cmd_None           // SELECT+LEFT
  ,cmd_ScreamSO       // START+UP        - Scream
  ,cmd_FaintSO        // START+RIGHT     - Faint/Short Circuit
  ,cmd_DiscoSO        // START+DOWN      - Disco
  ,cmd_None           // START+LEFT      - Fast Wave
  ,cmd_Bank8Track8    // PS+UP           - Custom 1 (Play bank 8 track 8; Logics display "Star Wars")
  ,cmd_Bank8Track9    // PS+RIGHT        - Custom 2 (Play bank 8 track 9; Random logics display)
  ,cmd_Bank8Track10   // PS+DOWN         - Custom 3 (Play bank 8 track 10; Random logics display)
  ,cmd_Bank8Track11   // PS+LEFT         - Custom 4 (Play bank 8 track 11; Random logics display)
  ,cmd_None           // L1+UP           - Cantina Dance
  ,cmd_None           // L1+RIGHT        - Wave 2
  ,cmd_LeiaMsgSO      // L1+DOWN         - Leia Message
  ,cmd_None           // L1+LEFT         - Wave

  ,cmd_OpenPanel2     // TRIANGLE        - Open Panel #2 (body)
  ,cmd_ClosePanel1    // CIRCLE          - Close Panel #1 (body)
  ,cmd_ClosePanel2    // CROSS           - Close Panel #2 (body)
  ,cmd_OpenPanel1     // SQUARE          - Open Panel #1 (body)
  ,cmd_VolumeMax      // SELECT+TRIANGLE - Volume Max
  ,cmd_None           // SELECT+CIRCLE
  ,cmd_VolumeMid      // SELECT+CROSS    - Volume Mid
  ,cmd_None           // SELECT+SQUARE
  ,cmd_None, cmd_None, cmd_None, cmd_None // START+TRIANGLE, START+CIRCLE, START+CROSS, START+SQUARE
  ,cmd_None, cmd_None, cmd_None, cmd_None // PS+TRIANGLE, PS+CIRCLE, PS+CROSS, PS+SQUARE
  ,cmd_None, cmd_None, cmd_None, cmd_None // L1+TRIANGLE, L1+CIRCLE, L1+CROSS, L1+SQUARE
};

#else

// ------------------------------------------------------------------
// These are the dome commands when sound is sent to the dome master.
// ------------------------------------------------------------------

const char * domeCommands[NUMBER_OF_BUTTON_COMBOS] = {
   cmd_FullAwake      // UP              - Full Awake
  ,cmd_FullAwake2     // RIGHT           - Full Awake+
  ,cmd_QuietMode      // DOWN            - Quiet Mode
  ,cmd_MidAwake       // LEFT            - Mid Awake
  ,cmd_VolumeUp       // SELECT+UP       - Volume Up
  ,cmd_HPLightsOff    // SELECT+RIGHT    - HP Lights Off
  ,cmd_VolumeDown     // SELECT+DOWN     - Volume Down
  ,cmd_HPLightsOn     // SELECT+LEFT     - HP Lights On
  ,cmd_Scream         // START+UP        - Scream
  ,cmd_Faint          // START+RIGHT     - Faint/Short Circuit
  ,cmd_Disco          // START+DOWN      - Disco
  ,cmd_FastWave       // START+LEFT      - Fast Wave
  ,cmd_Custom1        // PS+UP           - Custom 1 (Play bank 8 track 8; Logics display "Star Wars")
  ,cmd_Custom2        // PS+RIGHT        - Custom 2 (Play bank 8 track 9; Random logics display)
  ,cmd_Custom3        // PS+DOWN         - Custom 3 (Play bank 8 track 10; Random logics display)
  ,cmd_Custom4        // PS+LEFT         - Custom 4 (Play bank 8 track 11; Random logics display)
  ,cmd_CantinaDance   // L1+UP           - Cantina Dance
  ,cmd_Wave2          // L1+RIGHT        - Wave 2
  ,cmd_LeiaMsg        // L1+DOWN         - Leia Message
  ,cmd_Wave           // L1+LEFT         - Wave

  ,cmd_None           // TRIANGLE        - Open Panel #2 (body)
  ,cmd_None           // CIRCLE          - Close Panel #1 (body)
  ,cmd_None           // CROSS           - Close Panel #2 (body)
  ,cmd_None           // SQUARE          - Open Panel #1 (body)
  ,cmd_VolumeMax      // SELECT+TRIANGLE - Volume Max
  ,cmd_OpenPanelAll   // SELECT+CIRCLE   - Open Panel All (dome)
  ,cmd_VolumeMid      // SELECT+CROSS    - Volume Mid
  ,cmd_ClosePanelAll  // SELECT+SQUARE   - Close Panel All (dome)
  ,cmd_HPRandomOn     // START+TRIANGLE  - HP Random Movement
  ,cmd_HPLightsOff    // START+CIRCLE    - HP Lights Off
  ,cmd_HPReset        // START+CROSS     - HP Reset
  ,cmd_HPLightsOn     // START+SQUARE    - HP Lights On
  ,cmd_OpenPanel3     // PS+TRIANGLE     - Open Panel #3 (dome)
  ,cmd_ClosePanel4    // PS+CIRCLE       - Close Panel #4 (dome)
  ,cmd_ClosePanel3    // PS+CROSS        - Close Panel #3 (dome)
  ,cmd_OpenPanel4     // PS+SQUARE       - Open Panel #4 (dome)
  ,cmd_OpenPanel1     // L1+TRIANGLE     - Open Panel #1 (dome)
  ,cmd_ClosePanel2    // L1+CIRCLE       - Close Panel #2 (dome)
  ,cmd_ClosePanel1    // L1+CROSS        - Close Panel #1 (dome)
  ,cmd_OpenPanel2     // L1+SQUARE       - Open Panel #2 (dome)
};

// ------------------------------------------------------------------
// These are the body commands when sound is sent to the dome master.
// There are only four commands set when the body master is in use.
// There are no commands set when only the dome master is in use.
// ------------------------------------------------------------------

const char * bodyCommands[NUMBER_OF_BUTTON_COMBOS] = {
   cmd_None, cmd_None, cmd_None, cmd_None // UP, RIGHT, DOWN, LEFT
  ,cmd_None, cmd_None, cmd_None, cmd_None // SELECT+UP, SELECT+RIGHT, SELECT+DOWN, SELECT+LEFT
  ,cmd_None, cmd_None, cmd_None, cmd_None // START+UP, START+RIGHT, START+DOWN, START+LEFT
  ,cmd_None, cmd_None, cmd_None, cmd_None // PS+UP, PS+RIGHT, PS+DOWN, PS+LEFT
  ,cmd_None, cmd_None, cmd_None, cmd_None // L1+UP, L1+RIGHT, L1+DOWN, L1+LEFT
  ,cmd_None, cmd_None, cmd_None, cmd_None // TRIANGLE, CIRCLE, CROSS, SQUARE
  #ifdef MD_BODY_MASTER
  ,cmd_OpenPanel2     // TRIANGLE        - Open Panel #2 (body)
  ,cmd_ClosePanel1    // CIRCLE          - Close Panel #1 (body)
  ,cmd_ClosePanel2    // CROSS           - Close Panel #2 (body)
  ,cmd_OpenPanel1     // SQUARE          - Open Panel #1 (body)
  #else
  ,cmd_None           // SELECT+TRIANGLE
  ,cmd_None           // SELECT+CIRCLE
  ,cmd_None           // SELECT+CROSS
  ,cmd_None           // SELECT+SQUARE
  #endif
  ,cmd_None, cmd_None, cmd_None, cmd_None // START+TRIANGLE, START+CIRCLE, START+CROSS, START+SQUARE
  ,cmd_None, cmd_None, cmd_None, cmd_None // PS+TRIANGLE, PS+CIRCLE, PS+CROSS, PS+SQUARE
  ,cmd_None, cmd_None, cmd_None, cmd_None // L1+TRIANGLE, L1+CIRCLE, L1+CROSS, L1+SQUARE
};
#endif  // MD_BODY_MASTER && BODY_SOUND
#endif  // MD_STD_CONTROLS

// ================================================================================================
// The following maps Marcduino commands to button controls using whatever custom choices the
// programmer chooses. Adjustments for the DEBUG_DRIVEman switch are managed in the peripheral libraries.
//
// Note: These controls are initially set to match the standard Marcduino controls. If you wish to
// change your controls, be do so here so that you retain the ability to switch back to standard
// with the switch of your settings.
// ================================================================================================

#ifdef MD_CUSTOM_CONTROLS
#if defined(MD_BODY_MASTER) && defined(BODY_SOUND)

// -----------------------------------------------------
// These are the commands when both dome and body master
// are used and sound is sent to the body master.
// -----------------------------------------------------

const char * domeCommands[NUMBER_OF_BUTTON_COMBOS] = {
   cmd_DomeAwake      // UP              - Full Awake
  ,cmd_DomeAwake2     // RIGHT           - Full Awake+
  ,cmd_DomeQuiet      // DOWN            - Quiet Mode
  ,cmd_DomeAwake      // LEFT            - Mid Awake
  ,cmd_None           // SELECT+UP       - Volume Up
  ,cmd_HPLightsOff    // SELECT+RIGHT    - HP Lights Off
  ,cmd_None           // SELECT+DOWN     - Volume Down
  ,cmd_HPLightsOn     // SELECT+LEFT     - HP Lights On
  ,cmd_ScreamNS       // START+UP        - Scream
  ,cmd_FaintNS        // START+RIGHT     - Faint/Short Circuit
  ,cmd_None           // START+DOWN      - Disco
  ,cmd_FastWaveNS     // START+LEFT      - Fast Wave
  ,cmd_LogicsSW       // PS+UP           - Custom 1 (Play bank 8 track 8; Logics display "Star Wars")
  ,cmd_LogicsRandom   // PS+RIGHT        - Custom 2 (Play bank 8 track 9; Random logics display)
  ,cmd_LogicsRandom   // PS+DOWN         - Custom 3 (Play bank 8 track 10; Random logics display)
  ,cmd_LogicsRandom   // PS+LEFT         - Custom 4 (Play bank 8 track 11; Random logics display)
  ,cmd_CantinaDanceNS // L1+UP           - Cantina Dance
  ,cmd_Wave2NS        // L1+RIGHT        - Wave 2
  ,cmd_LeiaMsgNS      // L1+DOWN         - Leia Message
  ,cmd_WaveNS         // L1+LEFT         - Wave

  ,cmd_None           // TRIANGLE        - Open Panel #2 (body)
  ,cmd_None           // CIRCLE          - Close Panel #1 (body)
  ,cmd_None           // CROSS           - Close Panel #2 (body)
  ,cmd_None           // SQUARE          - Open Panel #1 (body)
  ,cmd_None           // SELECT+TRIANGLE - Volume Max
  ,cmd_OpenPanelAll   // SELECT+CIRCLE   - Open Panel All (dome)
  ,cmd_None           // SELECT+CROSS    - Volume Mid
  ,cmd_ClosePanelAll  // SELECT+SQUARE   - Close Panel All (dome)
  ,cmd_HPRandomOn     // START+TRIANGLE  - HP Random Movement
  ,cmd_HPLightsOff    // START+CIRCLE    - HP Lights Off
  ,cmd_HPReset        // START+CROSS     - HP Reset
  ,cmd_HPLightsOn     // START+SQUARE    - HP Lights On
  ,cmd_OpenPanel3     // PS+TRIANGLE     - Open Panel #3 (dome)
  ,cmd_ClosePanel4    // PS+CIRCLE       - Close Panel #4 (dome)
  ,cmd_ClosePanel3    // PS+CROSS        - Close Panel #3 (dome)
  ,cmd_OpenPanel4     // PS+SQUARE       - Open Panel #4 (dome)
  ,cmd_OpenPanel1     // L1+TRIANGLE     - Open Panel #1 (dome)
  ,cmd_ClosePanel2    // L1+CIRCLE       - Close Panel #2 (dome)
  ,cmd_ClosePanel1    // L1+CROSS        - Close Panel #1 (dome)
  ,cmd_OpenPanel2     // L1+SQUARE       - Open Panel #2 (dome)
};

const char * bodyCommands[NUMBER_OF_BUTTON_COMBOS] = {
   cmd_BodyAwake      // UP              - Full Awake
  ,cmd_BodyAwake      // RIGHT           - Full Awake+
  ,cmd_BodyQuiet      // DOWN            - Quiet Mode
  ,cmd_BodyAwake      // LEFT            - Mid Awake
  ,cmd_VolumeUp       // SELECT+UP       - Volume Up
  ,cmd_None           // SELECT+RIGHT
  ,cmd_VolumeDown     // SELECT+DOWN     - Volume Down
  ,cmd_None           // SELECT+LEFT
  ,cmd_ScreamSO       // START+UP        - Scream
  ,cmd_FaintSO        // START+RIGHT     - Faint/Short Circuit
  ,cmd_DiscoSO        // START+DOWN      - Disco
  ,cmd_None           // START+LEFT      - Fast Wave
  ,cmd_Bank8Track8    // PS+UP           - Custom 1 (Play bank 8 track 8; Logics display "Star Wars")
  ,cmd_Bank8Track9    // PS+RIGHT        - Custom 2 (Play bank 8 track 9; Random logics display)
  ,cmd_Bank8Track10   // PS+DOWN         - Custom 3 (Play bank 8 track 10; Random logics display)
  ,cmd_Bank8Track11   // PS+LEFT         - Custom 4 (Play bank 8 track 11; Random logics display)
  ,cmd_None           // L1+UP           - Cantina Dance
  ,cmd_None           // L1+RIGHT        - Wave 2
  ,cmd_LeiaMsgSO      // L1+DOWN         - Leia Message
  ,cmd_None           // L1+LEFT         - Wave

  ,cmd_OpenPanel2     // TRIANGLE        - Open Panel #2 (body)
  ,cmd_ClosePanel1    // CIRCLE          - Close Panel #1 (body)
  ,cmd_ClosePanel2    // CROSS           - Close Panel #2 (body)
  ,cmd_OpenPanel1     // SQUARE          - Open Panel #1 (body)
  ,cmd_VolumeMax      // SELECT+TRIANGLE - Volume Max
  ,cmd_None           // SELECT+CIRCLE
  ,cmd_VolumeMid      // SELECT+CROSS    - Volume Mid
  ,cmd_None           // SELECT+SQUARE
  ,cmd_None, cmd_None, cmd_None, cmd_None // START+TRIANGLE, START+CIRCLE, START+CROSS, START+SQUARE
  ,cmd_None, cmd_None, cmd_None, cmd_None // PS+TRIANGLE, PS+CIRCLE, PS+CROSS, PS+SQUARE
  ,cmd_None, cmd_None, cmd_None, cmd_None // L1+TRIANGLE, L1+CIRCLE, L1+CROSS, L1+SQUARE
};

#else

// ------------------------------------------------------------------
// These are the dome commands when sound is sent to the dome master.
// ------------------------------------------------------------------

const char * domeCommands[NUMBER_OF_BUTTON_COMBOS] = {
   cmd_FullAwake      // UP              - Full Awake
  ,cmd_FullAwake2     // RIGHT           - Full Awake+
  ,cmd_QuietMode      // DOWN            - Quiet Mode
  ,cmd_MidAwake       // LEFT            - Mid Awake
  ,cmd_VolumeUp       // SELECT+UP       - Volume Up
  ,cmd_HPLightsOff    // SELECT+RIGHT    - HP Lights Off
  ,cmd_VolumeDown     // SELECT+DOWN     - Volume Down
  ,cmd_HPLightsOn     // SELECT+LEFT     - HP Lights On
  ,cmd_Scream         // START+UP        - Scream
  ,cmd_Faint          // START+RIGHT     - Faint/Short Circuit
  ,cmd_Disco          // START+DOWN      - Disco
  ,cmd_FastWave       // START+LEFT      - Fast Wave
  ,cmd_Custom1        // PS+UP           - Custom 1 (Play bank 8 track 8; Logics display "Star Wars")
  ,cmd_Custom2        // PS+RIGHT        - Custom 2 (Play bank 8 track 9; Random logics display)
  ,cmd_Custom3        // PS+DOWN         - Custom 3 (Play bank 8 track 10; Random logics display)
  ,cmd_Custom4        // PS+LEFT         - Custom 4 (Play bank 8 track 11; Random logics display)
  ,cmd_CantinaDance   // L1+UP           - Cantina Dance
  ,cmd_Wave2          // L1+RIGHT        - Wave 2
  ,cmd_LeiaMsg        // L1+DOWN         - Leia Message
  ,cmd_Wave           // L1+LEFT         - Wave

  ,cmd_None           // TRIANGLE        - Open Panel #2 (body)
  ,cmd_None           // CIRCLE          - Close Panel #1 (body)
  ,cmd_None           // CROSS           - Close Panel #2 (body)
  ,cmd_None           // SQUARE          - Open Panel #1 (body)
  ,cmd_VolumeMax      // SELECT+TRIANGLE - Volume Max
  ,cmd_OpenPanelAll   // SELECT+CIRCLE   - Open Panel All (dome)
  ,cmd_VolumeMid      // SELECT+CROSS    - Volume Mid
  ,cmd_ClosePanelAll  // SELECT+SQUARE   - Close Panel All (dome)
  ,cmd_HPRandomOn     // START+TRIANGLE  - HP Random Movement
  ,cmd_HPLightsOff    // START+CIRCLE    - HP Lights Off
  ,cmd_HPReset        // START+CROSS     - HP Reset
  ,cmd_HPLightsOn     // START+SQUARE    - HP Lights On
  ,cmd_OpenPanel3     // PS+TRIANGLE     - Open Panel #3 (dome)
  ,cmd_ClosePanel4    // PS+CIRCLE       - Close Panel #4 (dome)
  ,cmd_ClosePanel3    // PS+CROSS        - Close Panel #3 (dome)
  ,cmd_OpenPanel4     // PS+SQUARE       - Open Panel #4 (dome)
  ,cmd_OpenPanel1     // L1+TRIANGLE     - Open Panel #1 (dome)
  ,cmd_ClosePanel2    // L1+CIRCLE       - Close Panel #2 (dome)
  ,cmd_ClosePanel1    // L1+CROSS        - Close Panel #1 (dome)
  ,cmd_OpenPanel2     // L1+SQUARE       - Open Panel #2 (dome)
};

// ------------------------------------------------------------------
// These are the body commands when sound is sent to the dome master.
// There are only four commands set when the body master is in use.
// There are no commands set when only the dome master is in use.
// ------------------------------------------------------------------

const char * bodyCommands[NUMBER_OF_BUTTON_COMBOS] = {
   cmd_None, cmd_None, cmd_None, cmd_None // UP, RIGHT, DOWN, LEFT
  ,cmd_None, cmd_None, cmd_None, cmd_None // SELECT+UP, SELECT+RIGHT, SELECT+DOWN, SELECT+LEFT
  ,cmd_None, cmd_None, cmd_None, cmd_None // START+UP, START+RIGHT, START+DOWN, START+LEFT
  ,cmd_None, cmd_None, cmd_None, cmd_None // PS+UP, PS+RIGHT, PS+DOWN, PS+LEFT
  ,cmd_None, cmd_None, cmd_None, cmd_None // L1+UP, L1+RIGHT, L1+DOWN, L1+LEFT
  ,cmd_None, cmd_None, cmd_None, cmd_None // TRIANGLE, CIRCLE, CROSS, SQUARE
  #ifdef MD_BODY_MASTER
  ,cmd_OpenPanel2     // TRIANGLE        - Open Panel #2 (body)
  ,cmd_ClosePanel1    // CIRCLE          - Close Panel #1 (body)
  ,cmd_ClosePanel2    // CROSS           - Close Panel #2 (body)
  ,cmd_OpenPanel1     // SQUARE          - Open Panel #1 (body)
  #else
  ,cmd_None           // SELECT+TRIANGLE
  ,cmd_None           // SELECT+CIRCLE
  ,cmd_None           // SELECT+CROSS
  ,cmd_None           // SELECT+SQUARE
  #endif
  ,cmd_None, cmd_None, cmd_None, cmd_None // START+TRIANGLE, START+CIRCLE, START+CROSS, START+SQUARE
  ,cmd_None, cmd_None, cmd_None, cmd_None // PS+TRIANGLE, PS+CIRCLE, PS+CROSS, PS+SQUARE
  ,cmd_None, cmd_None, cmd_None, cmd_None // L1+TRIANGLE, L1+CIRCLE, L1+CROSS, L1+SQUARE
};
#endif  // MD_BODY_MASTER && BODY_SOUND
#endif

#endif
