/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Settings_Marcduino_Custom.h - Library for Marcduino command settings
 * Created by Brian Lubkeman, 22 March 2021
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */

#ifndef _SETTINGS_MARCDUINO_CUSTOM_H_
#define _SETTINGS_MARCDUINO_CUSTOM_H_

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


/* ===============================================================================
 * There are 40 button combinations used by the original SHADOW_MD code to control
 * the Marcduino.  The following define the mappings for the 40 combinations based
 * on several settings found in Settings.h.  These mappings can be changed to suit 
 * your own designs.  Just make certain to edit the right set.
 * 
 * Do not change the order of the button combinations.  The programming for this
 * controller system assumes this specific order.  
 * 
 * Always provide a mapping for  every combination.  If no command should be sent 
 * then map the command, cmd_None, in its place.
 * =============================================================================== */

#if defined(MD_BODY_MASTER) && defined(MD_BODY_SOUND)
// ----------------------------------------------
// These mappings are used when the optional body
// master is in use and controls the sound.
// ----------------------------------------------
ButtonMapping_Struct buttonMap[] {
//  Dome                Body                 Button Combo      Comment
// -----------------   ---------------       --------------    -----------------------
  {cmd_None           ,cmd_BankXTrackY}   // UP              - Play track #1
 ,{cmd_None           ,cmd_BankXTrackY}   // RIGHT           - Play track #2
 ,{cmd_None           ,cmd_BankXTrackY}   // DOWN            - Play track #3
 ,{cmd_None           ,cmd_BankXTrackY}   // LEFT            - Play track #4
 ,{cmd_None           ,cmd_BankXTrackY}   // TRIANGLE        - Play track #5
 ,{cmd_None           ,cmd_BankXTrackY}   // CIRCLE          - Play track #6
 ,{cmd_None           ,cmd_BankXTrackY}   // CROSS           - Play track #7
 ,{cmd_None           ,cmd_BankXTrackY}   // SQUARE          - Play track #8
 
 ,{cmd_DomeQuiet      ,cmd_BodyQuiet}     // L1+UP           - Quiet Mode
 ,{cmd_DomeQuiet      ,cmd_BodyAwake}     // L1+RIGHT        - Mid Awake
 ,{cmd_DomeAwake2     ,cmd_BodyAwake}     // L1+DOWN         - Full Awake+
 ,{cmd_DomeAwake      ,cmd_BodyAwake}     // L1+LEFT         - Full Awake
 ,{cmd_None           ,cmd_VolumeUp}      // L1+TRIANGLE     - Volume Up
 ,{cmd_None           ,cmd_VolumeMid}     // L1+CIRCLE       - Volume Mid
 ,{cmd_None           ,cmd_VolumeDown}    // L1+CROSS        - Volume Down
 ,{cmd_None           ,cmd_VolumeMax}     // L1+SQUARE       - Volume Max

 ,{cmd_None           ,cmd_None}          // SELECT+UP       - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+RIGHT    - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+DOWN     - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+LEFT     - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+TRIANGLE - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+CIRCLE   - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+CROSS    - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+SQUARE   - Available for mapping

 ,{cmd_None           ,cmd_None}          // START+UP        - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+RIGHT     - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+DOWN      - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+LEFT      - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+TRIANGLE  - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+CIRCLE    - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+CROSS     - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+SQUARE    - Available for mapping

 ,{cmd_None           ,cmd_OpenPanel1}    // PS+UP           - Open Panel #1 (body)
 ,{cmd_None           ,cmd_ClosePanel2}   // PS+RIGHT        - Close Panel #2 (body)
 ,{cmd_None           ,cmd_ClosePanel1}   // PS+DOWN         - Close Panel #1 (body)
 ,{cmd_None           ,cmd_OpenPanel2}    // PS+LEFT         - Open Panel #2 (body)
 ,{cmd_HPReset        ,cmd_None}          // PS+TRIANGLE     - HP Reset
 ,{cmd_HPLightsOn     ,cmd_None}          // PS+CIRCLE       - HP Lights On
 ,{cmd_HPRandomOn     ,cmd_None}          // PS+CROSS        - HP Random Movement
 ,{cmd_HPLightsOff    ,cmd_None}          // PS+SQUARE       - HP Lights Off
};

#elif defined(MD_BODY_MASTER) && !defined(MD_BODY_SOUND)
// -------------------------------------------------------------------------
// These mappings are used when the optional body master is in use but sound
// is sent to the dome master.  Basically, it only runs body panels.
// -------------------------------------------------------------------------
ButtonMapping_Struct buttonMap[] {
//  Dome                Body                 Button Combo      Comment
// -----------------   ---------------       --------------    -----------------------
  {cmd_BankXTrackY    ,cmd_None}          // UP              - Play track #1
 ,{cmd_BankXTrackY    ,cmd_None}          // RIGHT           - Play track #2
 ,{cmd_BankXTrackY    ,cmd_None}          // DOWN            - Play track #3
 ,{cmd_BankXTrackY    ,cmd_None}          // LEFT            - Play track #4
 ,{cmd_BankXTrackY    ,cmd_None}          // TRIANGLE        - Play track #5
 ,{cmd_BankXTrackY    ,cmd_None}          // CIRCLE          - Play track #6
 ,{cmd_BankXTrackY    ,cmd_None}          // CROSS           - Play track #7
 ,{cmd_BankXTrackY    ,cmd_None}          // SQUARE          - Play track #8
 
 ,{cmd_QuietMode      ,cmd_ClosePanelAll} // L1+UP           - Quiet Mode
 ,{cmd_MidAwake       ,cmd_ClosePanelAll} // L1+RIGHT        - Mid Awake
 ,{cmd_FullAwake2     ,cmd_ClosePanelAll} // L1+DOWN         - Full Awake+
 ,{cmd_FullAwake      ,cmd_ClosePanelAll} // L1+LEFT         - Full Awake
 ,{cmd_VolumeUp       ,cmd_None}          // L1+TRIANGLE     - Volume Up
 ,{cmd_VolumeMid      ,cmd_None}          // L1+CIRCLE       - Volume Mid
 ,{cmd_VolumeDown     ,cmd_None}          // L1+CROSS        - Volume Down
 ,{cmd_VolumeMax      ,cmd_None}          // L1+SQUARE       - Volume Max

 ,{cmd_None           ,cmd_None}          // SELECT+UP       - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+RIGHT    - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+DOWN     - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+LEFT     - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+TRIANGLE - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+CIRCLE   - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+CROSS    - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+SQUARE   - Available for mapping

 ,{cmd_None           ,cmd_None}          // START+UP        - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+RIGHT     - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+DOWN      - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+LEFT      - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+TRIANGLE  - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+CIRCLE    - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+CROSS     - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+SQUARE    - Available for mapping

 ,{cmd_None           ,cmd_OpenPanel1}    // PS+UP           - Open Panel #1 (body)
 ,{cmd_None           ,cmd_ClosePanel2}   // PS+RIGHT        - Close Panel #2 (body)
 ,{cmd_None           ,cmd_ClosePanel1}   // PS+DOWN         - Close Panel #1 (body)
 ,{cmd_None           ,cmd_OpenPanel2}    // PS+LEFT         - Open Panel #2 (body)
 ,{cmd_HPReset        ,cmd_None}          // PS+TRIANGLE     - HP Reset
 ,{cmd_HPLightsOn     ,cmd_None}          // PS+CIRCLE       - HP Lights On
 ,{cmd_HPRandomOn     ,cmd_None}          // PS+CROSS        - HP Random Movement
 ,{cmd_HPLightsOff    ,cmd_None}          // PS+SQUARE       - HP Lights Off
};

#elif !defined(MD_BODY_MASTER)
// ------------------------------------------------------------
// These mappings are used when only the dome master is in use.
// ------------------------------------------------------------
ButtonMapping_Struct buttonMap[] {
//  Dome                Body                 Button Combo      Comment
// -----------------   ---------------       --------------    -----------------------
  {cmd_BankXTrackY    ,cmd_None}          // UP              - Play track #1
 ,{cmd_BankXTrackY    ,cmd_None}          // RIGHT           - Play track #2
 ,{cmd_BankXTrackY    ,cmd_None}          // DOWN            - Play track #3
 ,{cmd_BankXTrackY    ,cmd_None}          // LEFT            - Play track #4
 ,{cmd_BankXTrackY    ,cmd_None}          // TRIANGLE        - Play track #5
 ,{cmd_BankXTrackY    ,cmd_None}          // CIRCLE          - Play track #6
 ,{cmd_BankXTrackY    ,cmd_None}          // CROSS           - Play track #7
 ,{cmd_BankXTrackY    ,cmd_None}          // SQUARE          - Play track #8
 
 ,{cmd_QuietMode      ,cmd_None}          // L1+UP           - Quiet Mode
 ,{cmd_MidAwake       ,cmd_None}          // L1+RIGHT        - Mid Awake
 ,{cmd_FullAwake2     ,cmd_None}          // L1+DOWN         - Full Awake+
 ,{cmd_FullAwake      ,cmd_None}          // L1+LEFT         - Full Awake
 ,{cmd_VolumeUp       ,cmd_None}          // L1+TRIANGLE     - Volume Up
 ,{cmd_VolumeMid      ,cmd_None}          // L1+CIRCLE       - Volume Mid
 ,{cmd_VolumeDown     ,cmd_None}          // L1+CROSS        - Volume Down
 ,{cmd_VolumeMax      ,cmd_None}          // L1+SQUARE       - Volume Max

 ,{cmd_None           ,cmd_None}          // SELECT+UP       - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+RIGHT    - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+DOWN     - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+LEFT     - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+TRIANGLE - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+CIRCLE   - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+CROSS    - Available for mapping
 ,{cmd_None           ,cmd_None}          // SELECT+SQUARE   - Available for mapping

 ,{cmd_None           ,cmd_None}          // START+UP        - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+RIGHT     - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+DOWN      - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+LEFT      - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+TRIANGLE  - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+CIRCLE    - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+CROSS     - Available for mapping
 ,{cmd_None           ,cmd_None}          // START+SQUARE    - Available for mapping

 ,{cmd_None           ,cmd_None}          // PS+UP           - Open Panel #1 (body)
 ,{cmd_None           ,cmd_None}          // PS+RIGHT        - Close Panel #2 (body)
 ,{cmd_None           ,cmd_None}          // PS+DOWN         - Close Panel #1 (body)
 ,{cmd_None           ,cmd_None}          // PS+LEFT         - Open Panel #2 (body)
 ,{cmd_HPReset        ,cmd_None}          // PS+TRIANGLE     - HP Reset
 ,{cmd_HPLightsOn     ,cmd_None}          // PS+CIRCLE       - HP Lights On
 ,{cmd_HPRandomOn     ,cmd_None}          // PS+CROSS        - HP Random Movement
 ,{cmd_HPLightsOff    ,cmd_None}          // PS+SQUARE       - HP Lights Off
};
#endif

#endif