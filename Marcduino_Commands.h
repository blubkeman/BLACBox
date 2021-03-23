/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings_Marcduino_Commands.h - Library for Marcduino command settings
 * Created by Brian Lubkeman, 23 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */

#ifndef _SETTINGS_MARCDUINO_COMMANDS_H_
#define _SETTINGS_MARCDUINO_COMMANDS_H_

/* =======================================================================================
 * The following commands are those needed to reproduce the same functions as found in the 
 * original SHADOW_MD controller code.  It is not the exhaustive list of every possible 
 * command.  It is entirely possible to define additional commands.
 * 
 * To add additional commands, follow the example set by these already defined.  Please 
 * see the following website for a reference guide to Marcduino commands.
 * 
 * https://www.curiousmarc.com/r2-d2/marcduino-system/marcduino-software-reference/marcduino-command-reference
 * 
 * ======================================================================================= */

// This command is really just a placeholder. Please don't change it.

const char cmd_None[]            PROGMEM = "";

// ---------------
// Sound commands.
// ---------------

const char cmd_VolumeDown[]      PROGMEM = "$-\r";          // Volume Down
const char cmd_VolumeMax[]       PROGMEM = "$f\r";          // Volume max
const char cmd_VolumeMid[]       PROGMEM = "$m\r";          // Volume mid
const char cmd_VolumeUp[]        PROGMEM = "$+\r";          // Volume Up

// Pre-programmed sound-only sequences. (SO = Sound only)

const char cmd_ScreamSO[]        PROGMEM = "$S\r";          // Scream - Sound only
const char cmd_FaintSO[]         PROGMEM = "$F\r";          // Faint/Short Circuit - Sound only
const char cmd_CantinaShortSO[]  PROGMEM = "$c\r";          // Cantina Dance - Short sound only
const char cmd_CantinaLongSO[]   PROGMEM = "$C\r";          // Cantina Dance - Long sound only
const char cmd_LeiaMsgSO[]       PROGMEM = "$L\r";          // Leia Message - Sound only
const char cmd_DiscoSO[]         PROGMEM = "$D\r";          // Disco - Sound only

// Play specific tracks on specific banks. (See the Marcduino Command Reference web page for details on banks and tracks.)

const char cmd_Bank8Track8[]     PROGMEM = "$88\r";         // Play bank 8 track 8
const char cmd_Bank8Track9[]     PROGMEM = "$89\r";         // Play bank 8 track 9
const char cmd_Bank8Track10[]    PROGMEM = "$810\r";        // Play bank 8 track 10
const char cmd_Bank8Track11[]    PROGMEM = "$811\r";        // Play bank 8 track 11

// These are custom commands used by the standard Marcduino mappings for PS+[UP|RIGHT|DOWN|LEFT].

const char cmd_Custom1[]         PROGMEM = "$88\r@0T10\r";  // Play bank 8 track 8, display "Star Wars"
const char cmd_Custom2[]         PROGMEM = "$89\r@0T1\r";   // Play bank 8 track 9, display random
const char cmd_Custom3[]         PROGMEM = "$810\r@0T1\r";  // Play bank 8 track 10, display random
const char cmd_Custom4[]         PROGMEM = "$811\r@0T1\r";  // Play bank 8 track 11, display random

// ---------------
// Panel commands.
// ---------------

// These can be sent to either the dome or the body. The commands are the same for both.

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

// -----------------------
// Holoprojector commands.
// -----------------------

const char cmd_HPLightsOff[]     PROGMEM = "*OF00\r";  // Holos all lights off
const char cmd_HPLightsOn[]      PROGMEM = "*ON00\r";  // Holos all lights on
const char cmd_HPRandomOn[]      PROGMEM = "*RD00\r";  // Holos all random movement on
const char cmd_HPReset[]         PROGMEM = "*ST00\r";  // Holos all reset

// -----------------------
// Logic display commands.
// -----------------------

const char cmd_LogicsSW[]        PROGMEM = "@0T10\r";  // Display "Star Wars"
const char cmd_LogicsRandom[]    PROGMEM = "@0T1\r";   // Display random

#if defined(MAGIC_PANEL)
// ---------------------
// Magic panel commands.
// ---------------------

const char cmd_MagicPanelOn      PROGMEM = "*MO99\r";   // Magic panel turned on
const char cmd_MagicPanelOff     PROGMEM = "*MO00\r";   // Magic panel turned off
//const char cmd_MagicPanelTimed   PROGMEM = "*M0xx\r";   // Uncomment and change xx to a number of seconds from 01 to 98.
//const char cmd_MagicPanelFlicker PROGMEM = "*MFxx\r";   // Uncomment and change xx to a number of seconds from 01 to 99.
#endif

// ---------------------------------
// Pre-programmed full system modes.
// ---------------------------------

// Pre-programmed modes for the dome with sound.

const char cmd_FullAwake[]       PROGMEM = ":SE11\r";  // Full Awake (panel close, random sound, random holo movement, holo lights off)
const char cmd_FullAwake2[]      PROGMEM = ":SE14\r";  // Awake+     (panel close, random sound, random holo movement, holo lights on)
const char cmd_MidAwake[]        PROGMEM = ":SE13\r";  // Mid Awake  (panel close, random sound, holos reset)
const char cmd_QuietMode[]       PROGMEM = ":SE10\r";  // Quiet Mode (panel close, stop sounds,  holos reset)

// Pre-programmed modes for the body with sound.

const char cmd_BodyAwake[]       PROGMEM = ":CL00\r$R\r";           // Close panels, Sound random
const char cmd_BodyQuiet[]       PROGMEM = ":CL00\r$s\r";           // Close panels, Sound off

// Pre-programmed modes for the dome without sound.

const char cmd_DomeAwake[]       PROGMEM = ":CL00\r*RD00\r*OF00\r"; // Close panels, HP random, HP lights off
const char cmd_DomeAwake2[]      PROGMEM = ":CL00\r*RD00\r*ON00\r"; // Close panels, HP random, HP lights on
const char cmd_DomeQuiet[]       PROGMEM = ":CL00\r*ST00\r";        // Close panels, HP reset (random off, servos off, lights off)
const char cmd_DomeMidAwake[]    PROGMEM = ":CL00\r$R\r";           // Close panels, Sound random

// -------------------------
// Pre-programmed sequences.
// -------------------------

// For the dome with sound.

const char cmd_Scream[]          PROGMEM = ":SE01\r";  // Scream
const char cmd_Wave[]            PROGMEM = ":SE02\r";  // Wave
const char cmd_FastWave[]        PROGMEM = ":SE03\r";  // Fast Wave
const char cmd_Wave2[]           PROGMEM = ":SE04\r";  // Wave 2
const char cmd_BeepCantina[]     PROGMEM = ":SE05\r";  // Beep Cantina
const char cmd_Faint[]           PROGMEM = ":SE06\r";  // Faint/Short Circuit
const char cmd_CantinaDance[]    PROGMEM = ":SE07\r";  // Cantina Dance
const char cmd_LeiaMsg[]         PROGMEM = ":SE08\r";  // Leia Message
const char cmd_Disco[]           PROGMEM = ":SE09\r";  // Disco

// For the dome without sound. (NS = No sound)

const char cmd_ScreamNS[]        PROGMEM = ":SE51\r";  // Scream - No sound
const char cmd_WaveNS[]          PROGMEM = ":SE52\r";  // Wave - No sound
const char cmd_FastWaveNS[]      PROGMEM = ":SE53\r";  // Fast Wave - No sound
const char cmd_Wave2NS[]         PROGMEM = ":SE54\r";  // Wave 2 - No sound
const char cmd_BeepCantinaNS[]   PROGMEM = ":SE55\r";  // Beep Cantina - No sound
const char cmd_FaintNS[]         PROGMEM = ":SE56\r";  // Faint/Short Circuit - No sound
const char cmd_CantinaDanceNS[]  PROGMEM = ":SE57\r";  // Cantina Dance - No sound
const char cmd_LeiaMsgNS[]       PROGMEM = ":SE00\r@0T6\r*HD01";  // Leia Message, no sound

#endif
