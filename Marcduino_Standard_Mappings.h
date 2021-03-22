/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings_Marcduino_Std.h - Library for Marcduino command settings
 * Created by Brian Lubkeman, 22 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 * =================================================================================
 * 
 * ---------- I M P O R T A N T ----------
 * This library is intended to provide the button-to-command mappings needed to
 * reproduce the same controls as the original SHADOW-MD code.  Please do not change
 * the contents of this file.
 * 
 * If you wish to customize the controls, please make your changes in the tab for
 * Settings_Marcduino_Custom.h, and uncomment the line in Settings.h that says:
 * 
 * #define MD_CUSTOM_CMDS
 * 
 */

#ifndef _SETTINGS_MARCDUINO_STD_H_
#define _SETTINGS_MARCDUINO_STD_H_

#include "Marcduino_Commands.h"

/* ===============================================================================
 * This data structure when used as an array allows us to map a button combination
 * (represented by the array's index) to one command for the dome and one command
 * for the body.
 * =============================================================================== */

typedef struct {
  const char * domeCommand;
  const char * bodyCommand;
} ButtonMapping_Struct;


#if defined(MD_BODY_MASTER) && defined(MD_BODY_SOUND)
// ----------------------------------------------
// These mappings are used when the optional body
// master is in use and controls the sound.
// ----------------------------------------------
ButtonMapping_Struct buttonMap[] {
//  Dome                Body                Button Combo       Comment
// -----------------   ---------------      --------------     -----------------------
  {cmd_DomeAwake      ,cmd_BodyAwake}    // UP               - Full Awake
 ,{cmd_DomeAwake2     ,cmd_BodyAwake}    // RIGHT            - Full Awake+
 ,{cmd_DomeQuiet      ,cmd_BodyQuiet}    // DOWN             - Quiet Mode
 ,{cmd_DomeQuiet      ,cmd_BodyAwake}    // LEFT             - Mid Awake
 ,{cmd_None           ,cmd_OpenPanel2}   // TRIANGLE         - Open Panel #2 (body)
 ,{cmd_None           ,cmd_ClosePanel1}  // CIRCLE           - Close Panel #1 (body)
 ,{cmd_None           ,cmd_ClosePanel2}  // CROSS            - Close Panel #2 (body)
 ,{cmd_None           ,cmd_OpenPanel1}   // SQUARE           - Open Panel #1 (body)
 
 ,{cmd_CantinaDanceNS ,cmd_None}         // L1+UP            - Cantina Dance
 ,{cmd_Wave2NS        ,cmd_None}         // L1+RIGHT         - Wave 2
 ,{cmd_LeiaMsgNS      ,cmd_LeiaMsgSO}    // L1+DOWN          - Leia Message
 ,{cmd_WaveNS         ,cmd_None}         // L1+LEFT          - Wave
 ,{cmd_OpenPanel1     ,cmd_None}         // L1+TRIANGLE      - Open Panel #1 (dome)
 ,{cmd_ClosePanel2    ,cmd_None}         // L1+CIRCLE        - Close Panel #2 (dome)
 ,{cmd_ClosePanel1    ,cmd_None}         // L1+CROSS         - Close Panel #1 (dome)
 ,{cmd_OpenPanel2     ,cmd_None}         // L1+SQUARE        - Open Panel #2 (dome)

 ,{cmd_None           ,cmd_VolumeUp}     // SELECT+UP        - Volume Up
 ,{cmd_HPLightsOff    ,cmd_None}         // SELECT+RIGHT     - HP Lights Off
 ,{cmd_None           ,cmd_VolumeDown}   // SELECT+DOWN      - Volume Down
 ,{cmd_HPLightsOn     ,cmd_None}         // SELECT+LEFT      - HP Lights On
 ,{cmd_None           ,cmd_VolumeMax}    // SELECT+TRIANGLE  - Volume Max
 ,{cmd_OpenPanelAll   ,cmd_None}         // SELECT+CIRCLE    - Open Panel All (dome)
 ,{cmd_None           ,cmd_VolumeMid}    // SELECT+CROSS     - Volume Mid
 ,{cmd_ClosePanelAll  ,cmd_None}         // SELECT+SQUARE    - Close Panel All (dome)

 ,{cmd_ScreamNS       ,cmd_ScreamSO}     // START+UP         - Scream
 ,{cmd_FaintNS        ,cmd_FaintSO}      // START+RIGHT      - Faint/Short Circuit
 ,{cmd_None           ,cmd_DiscoSO}      // START+DOWN       - Disco
 ,{cmd_FastWaveNS     ,cmd_None}         // START+LEFT       - Fast Wave
 ,{cmd_HPRandomOn     ,cmd_None}         // START+TRIANGLE   - HP Random Movement
 ,{cmd_HPLightsOff    ,cmd_None}         // START+CIRCLE     - HP Lights Off
 ,{cmd_HPReset        ,cmd_None}         // START+CROSS      - HP Reset
 ,{cmd_HPLightsOn     ,cmd_None}         // START+SQUARE     - HP Lights On

 ,{cmd_LogicsSW       ,cmd_Bank8Track8}  // PS+UP            - Custom 1 (Play bank 8 track 8; Logics display "Star Wars")
 ,{cmd_LogicsRandom   ,cmd_Bank8Track9}  // PS+RIGHT         - Custom 2 (Play bank 8 track 9; Random logics display)
 ,{cmd_LogicsRandom   ,cmd_Bank8Track10} // PS+DOWN          - Custom 3 (Play bank 8 track 10; Random logics display)
 ,{cmd_LogicsRandom   ,cmd_Bank8Track11} // PS+LEFT          - Custom 4 (Play bank 8 track 11; Random logics display)
 ,{cmd_OpenPanel3     ,cmd_None}         // PS+TRIANGLE      - Open Panel #3 (dome)
 ,{cmd_ClosePanel4    ,cmd_None}         // PS+CIRCLE        - Close Panel #4 (dome)
 ,{cmd_ClosePanel3    ,cmd_None}         // PS+CROSS         - Close Panel #3 (dome)
 ,{cmd_OpenPanel4     ,cmd_None}         // PS+SQUARE        - Open Panel #4 (dome)
};

#elif defined(MD_BODY_MASTER) && !defined(MD_BODY_SOUND)
// -------------------------------------------------------------------------
// These mappings are used when the optional body master is in use but sound
// is sent to the dome master.  Basically, it only runs body panels.
// -------------------------------------------------------------------------
ButtonMapping_Struct buttonMap[] {
//  Dome                Body                 Button Combo      Comment
// -----------------   ---------------       --------------    -----------------------
  {cmd_FullAwake      ,cmd_ClosePanelAll} // UP              - Full Awake
 ,{cmd_FullAwake2     ,cmd_ClosePanelAll} // RIGHT           - Full Awake+
 ,{cmd_QuietMode      ,cmd_ClosePanelAll} // DOWN            - Quiet Mode
 ,{cmd_MidAwake       ,cmd_ClosePanelAll} // LEFT            - Mid Awake
 ,{cmd_None           ,cmd_OpenPanel2}    // TRIANGLE        - Open Panel #2 (body)
 ,{cmd_None           ,cmd_ClosePanel1}   // CIRCLE          - Close Panel #1 (body)
 ,{cmd_None           ,cmd_ClosePanel2}   // CROSS           - Close Panel #2 (body)
 ,{cmd_None           ,cmd_OpenPanel1}    // SQUARE          - Open Panel #1 (body)
 
 ,{cmd_CantinaDance   ,cmd_None}          // L1+UP           - Cantina Dance
 ,{cmd_Wave2          ,cmd_None}          // L1+RIGHT        - Wave 2
 ,{cmd_LeiaMsg        ,cmd_None}          // L1+DOWN         - Leia Message
 ,{cmd_Wave           ,cmd_None}          // L1+LEFT         - Wave
 ,{cmd_OpenPanel1     ,cmd_None}          // L1+TRIANGLE     - Open Panel #1 (dome)
 ,{cmd_ClosePanel2    ,cmd_None}          // L1+CIRCLE       - Close Panel #2 (dome)
 ,{cmd_ClosePanel1    ,cmd_None}          // L1+CROSS        - Close Panel #1 (dome)
 ,{cmd_OpenPanel2     ,cmd_None}          // L1+SQUARE       - Open Panel #2 (dome)

 ,{cmd_VolumeUp       ,cmd_None}          // SELECT+UP       - Volume Up
 ,{cmd_HPLightsOff    ,cmd_None}          // SELECT+RIGHT    - HP Lights Off
 ,{cmd_VolumeDown     ,cmd_None}          // SELECT+DOWN     - Volume Down
 ,{cmd_HPLightsOn     ,cmd_None}          // SELECT+LEFT     - HP Lights On
 ,{cmd_VolumeMax      ,cmd_None}          // SELECT+TRIANGLE - Volume Max
 ,{cmd_OpenPanelAll   ,cmd_None}          // SELECT+CIRCLE   - Open Panel All (dome)
 ,{cmd_VolumeMid      ,cmd_None}          // SELECT+CROSS    - Volume Mid
 ,{cmd_ClosePanelAll  ,cmd_None}          // SELECT+SQUARE   - Close Panel All (dome)

 ,{cmd_Scream         ,cmd_None}          // START+UP        - Scream
 ,{cmd_Faint          ,cmd_None}          // START+RIGHT     - Faint/Short Circuit
 ,{cmd_None           ,cmd_None}          // START+DOWN      - Disco
 ,{cmd_FastWave       ,cmd_None}          // START+LEFT      - Fast Wave
 ,{cmd_HPRandomOn     ,cmd_None}          // START+TRIANGLE  - HP Random Movement
 ,{cmd_HPLightsOff    ,cmd_None}          // START+CIRCLE    - HP Lights Off
 ,{cmd_HPReset        ,cmd_None}          // START+CROSS     - HP Reset
 ,{cmd_HPLightsOn     ,cmd_None}          // START+SQUARE    - HP Lights On

 ,{cmd_Custom1        ,cmd_None}          // PS+UP           - Custom 1 (Play bank 8 track 8; Logics display "Star Wars")
 ,{cmd_Custom2        ,cmd_None}          // PS+RIGHT        - Custom 2 (Play bank 8 track 9; Random logics display)
 ,{cmd_Custom3        ,cmd_None}          // PS+DOWN         - Custom 3 (Play bank 8 track 10; Random logics display)
 ,{cmd_Custom4        ,cmd_None}          // PS+LEFT         - Custom 4 (Play bank 8 track 11; Random logics display)
 ,{cmd_OpenPanel3     ,cmd_None}          // PS+TRIANGLE     - Open Panel #3 (dome)
 ,{cmd_ClosePanel4    ,cmd_None}          // PS+CIRCLE       - Close Panel #4 (dome)
 ,{cmd_ClosePanel3    ,cmd_None}          // PS+CROSS        - Close Panel #3 (dome)
 ,{cmd_OpenPanel4     ,cmd_None}          // PS+SQUARE       - Open Panel #4 (dome)
};

#elif !defined(MD_BODY_MASTER)
// ------------------------------------------------------------
// These mappings are used when only the dome master is in use.
// ------------------------------------------------------------
ButtonMapping_Struct buttonMap[] {
//  Dome                Body                 Button Combo      Comment
// -----------------   ---------------       --------------    -----------------------
  {cmd_FullAwake      ,cmd_None}          // UP              - Full Awake
 ,{cmd_FullAwake2     ,cmd_None}          // RIGHT           - Full Awake+
 ,{cmd_QuietMode      ,cmd_None}          // DOWN            - Quiet Mode
 ,{cmd_MidAwake       ,cmd_None}          // LEFT            - Mid Awake
 ,{cmd_None           ,cmd_None}          // TRIANGLE        - Available for mapping
 ,{cmd_None           ,cmd_None}          // CIRCLE          - Available for mapping
 ,{cmd_None           ,cmd_None}          // CROSS           - Available for mapping
 ,{cmd_None           ,cmd_None}          // SQUARE          - Available for mapping
 
 ,{cmd_CantinaDance   ,cmd_None}          // L1+UP           - Cantina Dance
 ,{cmd_Wave2          ,cmd_None}          // L1+RIGHT        - Wave 2
 ,{cmd_LeiaMsg        ,cmd_None}          // L1+DOWN         - Leia Message
 ,{cmd_Wave           ,cmd_None}          // L1+LEFT         - Wave
 ,{cmd_OpenPanel1     ,cmd_None}          // L1+TRIANGLE     - Open Panel #1 (dome)
 ,{cmd_ClosePanel2    ,cmd_None}          // L1+CIRCLE       - Close Panel #2 (dome)
 ,{cmd_ClosePanel1    ,cmd_None}          // L1+CROSS        - Close Panel #1 (dome)
 ,{cmd_OpenPanel2     ,cmd_None}          // L1+SQUARE       - Open Panel #2 (dome)

 ,{cmd_VolumeUp       ,cmd_None}          // SELECT+UP       - Volume Up
 ,{cmd_HPLightsOff    ,cmd_None}          // SELECT+RIGHT    - HP Lights Off
 ,{cmd_VolumeDown     ,cmd_None}          // SELECT+DOWN     - Volume Down
 ,{cmd_HPLightsOn     ,cmd_None}          // SELECT+LEFT     - HP Lights On
 ,{cmd_VolumeMax      ,cmd_None}          // SELECT+TRIANGLE - Volume Max
 ,{cmd_OpenPanelAll   ,cmd_None}          // SELECT+CIRCLE   - Open Panel All (dome)
 ,{cmd_VolumeMid      ,cmd_None}          // SELECT+CROSS    - Volume Mid
 ,{cmd_ClosePanelAll  ,cmd_None}          // SELECT+SQUARE   - Close Panel All (dome)

 ,{cmd_Scream         ,cmd_None}          // START+UP        - Scream
 ,{cmd_Faint          ,cmd_None}          // START+RIGHT     - Faint/Short Circuit
 ,{cmd_None           ,cmd_None}          // START+DOWN      - Disco
 ,{cmd_FastWave       ,cmd_None}          // START+LEFT      - Fast Wave
 ,{cmd_HPRandomOn     ,cmd_None}          // START+TRIANGLE  - HP Random Movement
 ,{cmd_HPLightsOff    ,cmd_None}          // START+CIRCLE    - HP Lights Off
 ,{cmd_HPReset        ,cmd_None}          // START+CROSS     - HP Reset
 ,{cmd_HPLightsOn     ,cmd_None}          // START+SQUARE    - HP Lights On

 ,{cmd_Custom1        ,cmd_None}          // PS+UP           - Custom 1 (Play bank 8 track 8; Logics display "Star Wars")
 ,{cmd_Custom2        ,cmd_None}          // PS+RIGHT        - Custom 2 (Play bank 8 track 9; Random logics display)
 ,{cmd_Custom3        ,cmd_None}          // PS+DOWN         - Custom 3 (Play bank 8 track 10; Random logics display)
 ,{cmd_Custom4        ,cmd_None}          // PS+LEFT         - Custom 4 (Play bank 8 track 11; Random logics display)
 ,{cmd_OpenPanel3     ,cmd_None}          // PS+TRIANGLE     - Open Panel #3 (dome)
 ,{cmd_ClosePanel4    ,cmd_None}          // PS+CIRCLE       - Close Panel #4 (dome)
 ,{cmd_ClosePanel3    ,cmd_None}          // PS+CROSS        - Close Panel #3 (dome)
 ,{cmd_OpenPanel4     ,cmd_None}          // PS+SQUARE       - Open Panel #4 (dome)
};
#endif

#endif
