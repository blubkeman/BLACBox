/*
 * Settings_MD_Functions.h - Marcduino function configuration for the B.L.A.C.Box system
 * Created by Brian Lubkeman, 21 May 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
*/

// ======================================================================================
// DISCLAIMER
// --------------------------------------------------------------------------------------
// The following settings are intended to reproduce the Marcduino functionality exactly
// as it is found in the SHADOW_MD code.  It is designed to permit some modification, but
// do not attempt this with little thought or studying the code.  The author of this
// sketch is not responsible for correcting your mistakes.
// ======================================================================================

#ifndef _SETTINGS_MD_FUNCTIONS_H_
#define _SETTINGS_MD_FUNCTIONS_H_

/* ===============================================
   *  ORIGINAL MarcDuino control settings
   *  
   *  PS3 Navigation (single or dual)
   *  
   *    FOOT CONTROLLER
   *    ===============
   *    Up           = Std function 12 (Full Awake mode reset (panel close, rnd sound, holo move,holo lights off))
   *    Right        = Std function 14 (Full Awake+ reset (panel close, rnd sound, holo move, holo lights on))
   *    Down         = Std function 11 (Quite mode reset (panel close, stop holos, stop sounds))
   *    Left         = Std function 13 (Mid Awake mode reset (panel close, rnd sound, stop holos))
   *    Cross+Up     = Std function 26 (Volume Up)
   *    Cross+Right  = Std function 24 (Holo Lights Off (All))
   *    Cross+Down   = Std function 27 (Volume Down)
   *    Cross+Left   = Std function 23 (Holo Lights On (All))
   *    Circle+Up    = Std function 2  (Scream - all panels open)
   *    Circle+Right = Std function 7  (Faint / Short Circuit)
   *    Circle+Down  = Std function 10 (Disco)
   *    Circle+Left  = Std function 4  (Fast (smirk) back and forth wave)
   *    PS+Up        = Custom function; play track 183, logic display "Star Wars", no panels
   *    PS+Right     = Custom function; play track 185, logic display random, no panels
   *    PS+Down      = Custom function; play track 184, logic display random, no panels
   *    PS+Left      = Custom function; play track 186, logic display random, no panels 
   *    L1+Up        = Std function 8  (Cantina Dance - orchestral, rhythmic panel dance)
   *    L1+Right     = Std function 5  (Wave 2, Open progressively all panels, then close one by one)
   *    L1+Down      = Std function 9  (Leia message)
   *    L1+Left      = Std function 3  (Wave, One Panel at a time)
   *    
   *    DOME CONTROLLER
   *    ===============
   *    Up           = Std function 58 (Open Body Panel #2)
   *    Right        = Std function 57 (Close Body Panel #1)
   *    Down         = Std function 59 (Close Body Panel #2)
   *    Left         = Std function 56 (Open Body Panel #1)
   *    Cross+Up     = Std function 28 (Volume Max)
   *    Cross+Right  = Std function 30 (Open All Dome Panels)
   *    Cross+Down   = Std function 29 (Volume Mid)
   *    Cross+Left   = Std function 33 (Close All Dome Panels)
   *    Circle+Up    = Std function 22 (Random Holo Movement On (All) - No other actions)
   *    Circle+Right = Std function 24 (Holo Lights Off (All))
   *    Circle+Down  = Std function 25 (Holo reset (motion off, lights off))
   *    Circle+Left  = Std function 23 (Holo Lights On (All))
   *    PS+Up        = Std function 38 (Open Dome Panel #3)
   *    PS+Right     = Std function 41 (Close Dome Panel #4)
   *    PS+Down      = Std function 39 (Close Dome Panel #3)
   *    PS+Left      = Std function 40 (Open Dome Panel #4)
   *    L1+Up        = Std function 34 (Open Dome Panel #1)
   *    L1+Right     = Std function 37 (Close Dome Panel #2)
   *    L1+Down      = Std function 35 (Close Dome Panel #1)
   *    L1+Left      = Std function 36 (Open Dome Panel #2)
   * =============================================== */

// -------------------------
// Function Types Definition
// -------------------------
// Assigning controller button combinations begins with deciding if it will run a
//  standard Marcduino function or a custom function.  This array is used to set
//  the function type (standard or custom) for each button combo.  Please do not
//  modify the sequence of the button combos.  Doing so will really mess this up.

const uint8_t functionTypes[40] = {
  
  // 0 = standard Marcduino function
  // 1 = custom Marcduino function
  
  0, 0, 0, 0,  //          UP, RIGHT, DOWN, LEFT
  0, 0, 0, 0,  // SELECT + UP, RIGHT, DOWN, LEFT
  0, 0, 0, 0,  //  START + UP, RIGHT, DOWN, LEFT
  1, 1, 1, 1,  //     PS + UP, RIGHT, DOWN, LEFT
  0, 0, 0, 0,  //     L1 + UP, RIGHT, DOWN, LEFT
  0, 0, 0, 0,  //          TRIANGLE (Nav2 UP), CIRCLE (Nav2 RIGHT), CROSS(Nav2 DOWN), SQUARE (Nav2 LEFT)
  0, 0, 0, 0,  // SELECT + TRIANGLE (Nav2 UP), CIRCLE (Nav2 RIGHT), CROSS(Nav2 DOWN), SQUARE (Nav2 LEFT)
  0, 0, 0, 0,  //  START + TRIANGLE (Nav2 UP), CIRCLE (Nav2 RIGHT), CROSS(Nav2 DOWN), SQUARE (Nav2 LEFT)
  0, 0, 0, 0,  //     PS + TRIANGLE (Nav2 UP), CIRCLE (Nav2 RIGHT), CROSS(Nav2 DOWN), SQUARE (Nav2 LEFT)
  0, 0, 0, 0   //     L1 + TRIANGLE (Nav2 UP), CIRCLE (Nav2 RIGHT), CROSS(Nav2 DOWN), SQUARE (Nav2 LEFT)
};

// -----------------------------
// Standard Functions Definition
// -----------------------------
// Most button combos are using the standard functions.  This array will specify
//  which standard function is assigned to each button combo.  Buttons that were
//  set to custom in the above function type array will still have an entry in
//  this array, but they will be set to 0.
// If you wish to modify these, doing so is fairly easy.  Please only modify the
//  assigned value.  Do not change the sequence of the button combinations.
// To assign a different standard function, look through the descr_xx constants
//  defined further down in this setting file.  When you locate the desired
//  function, note the xx value in the constant name.  Assign this value (without
//  leading zeros) to the desired button combination.
// To change it to a custom function, set it to 0 and be sure to change the
//  function type above to 1.

const uint8_t standardFunctions[40] = {
  12, //          UP    - Full Awake mode reset (panel close, rnd sound, holo move,holo lights off)
  14, //          RIGHT - Full Awake+ reset (panel close, rnd sound, holo move, holo lights on)
  11, //          DOWN  - Quite mode reset (panel close, stop holos, stop sounds)
  13, //          LEFT  - Mid Awake mode reset (panel close, rnd sound, stop holos)
  26, // SELECT + UP    - Volume Up
  24, // SELECT + RIGHT - Holo Lights Off (All)
  27, // SELECT + DOWN  - Volume Down
  23, // SELECT + LEFT  - Holo Lights On (All)
   2, //  START + UP    - Scream - all panels open
   7, //  START + RIGHT - Faint / Short Circuit
  10, //  START + DOWN  - Disco
   4, //  START + LEFT  - Fast (smirk) back and forth wave
   0, //     PS + UP    - See custom config
   0, //     PS + RIGHT - See custom config
   0, //     PS + DOWN  - See custom config
   0, //     PS + LEFT  - See custom config
   8, //     L1 + UP    - Cantina Dance - orchestral, rhythmic panel dance
   5, //     L1 + RIGHT - Wave 2, Open progressively all panels, then close one by one
   9, //     L1 + DOWN  - Leia message
   3, //     L1 + LEFT  - Wave, One Panel at a time
  58, //          TRIANGLE (Nav2 UP)  - Open Body Panel #2
  57, //          CIRCLE (Nav2 RIGHT) - Close Body Panel #1
  59, //          CROSS(Nav2 DOWN)    - Close Body Panel #2
  56, //          SQUARE (Nav2 LEFT)  - Open Body Panel #1
  28, // SELECT + TRIANGLE (Nav2 UP)  - Volume Max
  30, // SELECT + CIRCLE (Nav2 RIGHT) - Open All Dome Panels
  29, // SELECT + CROSS(Nav2 DOWN)    - Volume Mid
  33, // SELECT + SQUARE (Nav2 LEFT)  - Close All Dome Panels
  22, //  START + TRIANGLE (Nav2 UP)  - Random Holo Movement On (All) - No other actions
  24, //  START + CIRCLE (Nav2 RIGHT) - Holo Lights Off (All)
  25, //  START + CROSS(Nav2 DOWN)    - Holo reset (motion off, lights off)
  23, //  START + SQUARE (Nav2 LEFT)  - Holo Lights On (All)
  38, //     PS + TRIANGLE (Nav2 UP)  - Open Dome Panel #3
  41, //     PS + CIRCLE (Nav2 RIGHT) - Close Dome Panel #4
  39, //     PS + CROSS(Nav2 DOWN)    - Close Dome Panel #3
  40, //     PS + SQUARE (Nav2 LEFT)  - Open Dome Panel #4
  34, //     L1 + TRIANGLE (Nav2 UP)  - Open Dome Panel #1
  37, //     L1 + CIRCLE (Nav2 RIGHT) - Close Dome Panel #2
  35, //     L1 + CROSS(Nav2 DOWN)    - Close Dome Panel #1
  36, //     L1 + SQUARE (Nav2 LEFT)  - Open Dome Panel #2
};

// --------------------------
// Custom Function Definition
// --------------------------
// This is where we'll define a custom function including all of its parts:
//  sound, logic displays, and panels.
// The first data element (mp3Number) specifies a track number to be played.
//  This is limited to tracks 182 through 200.  Any other value will not be played.
// The second data element (logicFunction) specifies which preprogrammed display
//  to use, or when set to 8, you can specify your own custom text (the third
//  data element).
// The fourth data element (panelFunction) specifies which preprogrammed panel routine
//  to use, or when set to 9, you can specify your own routine (the fifth data element
//  which is defined in the above section "Custom Panel Routine Definition").

typedef struct {
  uint8_t mp3Number;
  uint8_t logicFunction;
  String  customText;
  uint8_t panelFunction;
} CustomFunctions_Struct;

// The following sets up the custom functions for PS+UP, PS+RIGHT, PS+DOWN, and PS+LEFT.
// Each is playing a custom sound, running a preprogrammed logic display, and no panel
//   routine.

const CustomFunctions_Struct customFunctions[] = {
  {0,   0, "", 0}  // This is an initialization setting.
 ,{183, 5, "", 0}  // PS+UP;    play track #183, display "Star Wars", no custom text, no panel display
 ,{185, 1, "", 0}  // PS+RIGHT; play track #185, random logic display, no custom text, no panel display
 ,{184, 1, "", 0}  // PS+DOWN;  play track #184, random logic display, no custom text, no panel display
 ,{186, 1, "", 0}  // PS+LEFT;  play track #186, random logic display, no custom text, no panel display

// This is how you would include a real custom panel routine.  Note the value 9
//   in the fourth data element and a pointer to the custom panel routine array.
// ,{187, 1, "", 9, &panelRoutine_PSUP[NUMBER_OF_DOME_PANELS]}

// This is how you would include custom text for the logic display.  Note the
//   value 8 in the second data element and the text in the third.
// ,{0, 8, "My custom text", 0, &panelRoutine_NONE[1]}

};

// -------------------------------
// Custom Panel Routine Definition
// -------------------------------
// If you are not defining any custom panel routines, please skip to the
//  "Custom Function Definition" section below.

// This is where things can get real messy.  We can define a custom panel routine.
// To start, we need to know how each panel should behave in the routine.
// For that, we define a data structure with the following data elements.
// The first element (Use) indicates if the panel is used in the routine.
// The second element (Start_Delay) indicates in seconds how much time must
//  pass after the routine starts and before this panel is opened.
// The third element (Open_Time) indicates in seconds how long the panel stays
//  open. After that time passes, the panel will close.

struct PanelSettings_Struct {
  bool Use;
  int Start_Delay;
  int Open_Time;
};

// We need at least one defined array to get the code to compile.
// This one array sets all panels to not be used. It effectively does nothing.
PanelSettings_Struct panelRoutine_NONE[NUMBER_OF_DOME_PANELS]= {
  {false, 0, 0}
 ,{false, 0, 0}
 ,{false, 0, 0}
 ,{false, 0, 0}
 ,{false, 0, 0}
 ,{false, 0, 0}
 ,{false, 0, 0}
 ,{false, 0, 0}
 ,{false, 0, 0}
 ,{false, 0, 0}
};

/* // This example opens dome panels 1-6 after 1 second
 // then closes them all after 5 seconds.
const PanelSettings_Struct panelRoutine_PSUP[NUMBER_OF_DOME_PANELS] = {
  {true, 1, 5},  // dome panel #1
  {true, 1, 5},  // dome panel #2
  {true, 1, 5},  // dome panel #3
  {true, 1, 5},  // dome panel #4
  {true, 1, 5},  // dome panel #5
  {true, 1, 5},  // dome panel #6
  {false, 1, 5},  // dome panel #7
  {false, 1, 5},  // dome panel #8
  {false, 1, 5},  // dome panel #9
  {false, 1, 5}   // dome panel #10
};
*/

// Set up a table (array) to reference the panel routines.

const PanelSettings_Struct *const table_panelRoutines[] = {
  panelRoutine_NONE
// ,panelRoutine_PSUP
};

// =================================================================================
// =================================================================================
// PLEASE, DO NOT MODIFY ANYTHING BELOW THIS LINE.
// =================================================================================
// =================================================================================

// Marcduino has 78 pre-defined "standard" functions.
// We're going to turn those into constants stored in program memory instead of RAM.
// Not all of these will be tied to controller buttons, but all are defined here.
// The xx value in the constant name represents the standard function number that
//  is associated with the command.

const char cmd_00[] PROGMEM = "";       
const char cmd_01[] PROGMEM = ":SE00\r";
const char cmd_02[] PROGMEM = ":SE01\r";
const char cmd_03[] PROGMEM = ":SE02\r";
const char cmd_04[] PROGMEM = ":SE03\r";
const char cmd_05[] PROGMEM = ":SE04\r";
const char cmd_06[] PROGMEM = ":SE05\r";
const char cmd_07[] PROGMEM = ":SE06\r";
const char cmd_08[] PROGMEM = ":SE07\r";
const char cmd_09[] PROGMEM = ":SE08\r";
const char cmd_10[] PROGMEM = ":SE09\r";
const char cmd_11[] PROGMEM = ":SE10\r";
const char cmd_12[] PROGMEM = ":SE11\r";
const char cmd_13[] PROGMEM = ":SE13\r";
const char cmd_14[] PROGMEM = ":SE14\r";
// Panel movement and light display only, no sound
const char cmd_15[] PROGMEM = ":SE51\r";
const char cmd_16[] PROGMEM = ":SE52\r";
const char cmd_17[] PROGMEM = ":SE53\r";
const char cmd_18[] PROGMEM = ":SE54\r";
const char cmd_19[] PROGMEM = ":SE55\r";
const char cmd_20[] PROGMEM = ":SE56\r";
const char cmd_21[] PROGMEM = ":SE57\r";
// Holoprojector
const char cmd_22[] PROGMEM = "*RD00\r";
const char cmd_23[] PROGMEM = "*ON00\r";
const char cmd_24[] PROGMEM = "*OF00\r";
const char cmd_25[] PROGMEM = "*ST00\r";
// Volume control
const char cmd_26[] PROGMEM = "$+\r";
const char cmd_27[] PROGMEM = "$-\r";
const char cmd_28[] PROGMEM = "$f\r";
const char cmd_29[] PROGMEM = "$m\r";
// Dome panel movement
const char cmd_30[] PROGMEM = ":OP00\r";
const char cmd_31[] PROGMEM = ":OP11\r";
const char cmd_32[] PROGMEM = ":OP12\r";
const char cmd_33[] PROGMEM = ":CL00\r";
const char cmd_34[] PROGMEM = ":OP01\r";
const char cmd_35[] PROGMEM = ":CL01\r";
const char cmd_36[] PROGMEM = ":OP02\r";
const char cmd_37[] PROGMEM = ":CL02\r";
const char cmd_38[] PROGMEM = ":OP03\r";
const char cmd_39[] PROGMEM = ":CL03\r";
const char cmd_40[] PROGMEM = ":OP04\r";
const char cmd_41[] PROGMEM = ":CL04\r";
const char cmd_42[] PROGMEM = ":OP05\r";
const char cmd_43[] PROGMEM = ":CL05\r";
const char cmd_44[] PROGMEM = ":OP06\r";
const char cmd_45[] PROGMEM = ":CL06\r";
const char cmd_46[] PROGMEM = ":OP07\r";
const char cmd_47[] PROGMEM = ":CL07\r";
const char cmd_48[] PROGMEM = ":OP08\r";
const char cmd_49[] PROGMEM = ":CL08\r";
const char cmd_50[] PROGMEM = ":OP09\r";
const char cmd_51[] PROGMEM = ":CL09\r";
const char cmd_52[] PROGMEM = ":OP10\r";
const char cmd_53[] PROGMEM = ":CL10\r";
// Body panel movement *** assumes second Marcduino master in the body using Serial3 from Arduino ***
const char cmd_54[] PROGMEM = ":OP00\r";
const char cmd_55[] PROGMEM = ":CL00\r";
const char cmd_56[] PROGMEM = ":OP01\r";
const char cmd_57[] PROGMEM = ":CL01\r";
const char cmd_58[] PROGMEM = ":OP02\r";
const char cmd_59[] PROGMEM = ":CL02\r";
const char cmd_60[] PROGMEM = ":OP03\r";
const char cmd_61[] PROGMEM = ":CL03\r";
const char cmd_62[] PROGMEM = ":OP04\r";
const char cmd_63[] PROGMEM = ":CL04\r";
const char cmd_64[] PROGMEM = ":OP05\r";
const char cmd_65[] PROGMEM = ":CL05\r";
const char cmd_66[] PROGMEM = ":OP06\r";
const char cmd_67[] PROGMEM = ":CL06\r";
const char cmd_68[] PROGMEM = ":OP07\r";
const char cmd_69[] PROGMEM = ":CL07\r";
const char cmd_70[] PROGMEM = ":OP08\r";
const char cmd_71[] PROGMEM = ":CL08\r";
const char cmd_72[] PROGMEM = ":OP09\r";
const char cmd_73[] PROGMEM = ":CL09\r";
const char cmd_74[] PROGMEM = ":OP10\r";
const char cmd_75[] PROGMEM = ":CL10\r";
// Magic panel lighting
const char cmd_76[] PROGMEM = "*MO99\r";
const char cmd_77[] PROGMEM = "*MO00\r";
const char cmd_78[] PROGMEM = "*MF10\r";

// Set up a table (array) to reference the commands.

const char *const table_stdCmd[] = {
  cmd_00, cmd_01, cmd_02, cmd_03, cmd_04, cmd_05, cmd_06, cmd_07, cmd_08, cmd_09,
  cmd_10, cmd_11, cmd_12, cmd_13, cmd_14, cmd_15, cmd_16, cmd_17, cmd_18, cmd_19,
  cmd_20, cmd_21, cmd_22, cmd_23, cmd_24, cmd_25, cmd_26, cmd_27, cmd_28, cmd_29,
  cmd_30, cmd_31, cmd_32, cmd_33, cmd_34, cmd_35, cmd_36, cmd_37, cmd_38, cmd_39,
  cmd_40, cmd_41, cmd_42, cmd_43, cmd_44, cmd_45, cmd_46, cmd_47, cmd_48, cmd_49,
  cmd_50, cmd_51, cmd_52, cmd_53, cmd_54, cmd_55, cmd_56, cmd_57, cmd_58, cmd_59,
  cmd_60, cmd_61, cmd_62, cmd_63, cmd_64, cmd_65, cmd_66, cmd_67, cmd_68, cmd_69,
  cmd_70, cmd_71, cmd_72, cmd_73, cmd_74, cmd_75, cmd_76, cmd_77, cmd_78
};

#ifdef BLACBOX_VERBOSE
const char descr_00[] PROGMEM = "Invalid function";
const char descr_01[] PROGMEM = "Close All Panels, Servo Off";
const char descr_02[] PROGMEM = "Scream";
const char descr_03[] PROGMEM = "Wave";
const char descr_04[] PROGMEM = "Fast Wave (Smirk)";
const char descr_05[] PROGMEM = "Wave 2";
const char descr_06[] PROGMEM = "Beep Cantina";
const char descr_07[] PROGMEM = "Faint/Short Circuit";
const char descr_08[] PROGMEM = "Cantina Dance";
const char descr_09[] PROGMEM = "Leia Message";
const char descr_10[] PROGMEM = "Disco";
const char descr_11[] PROGMEM = "Quite Mode";
const char descr_12[] PROGMEM = "Full Awake";
const char descr_13[] PROGMEM = "Mid Awake";
const char descr_14[] PROGMEM = "Full Awake+";
// Panel movement and light display only, no sound
const char descr_15[] PROGMEM = "Scream, (NO SOUND)";
const char descr_16[] PROGMEM = "Wave (NO SOUND)";
const char descr_17[] PROGMEM = "Fast Wave (Smirk) (NO SOUND)";
const char descr_18[] PROGMEM = "Wave 2  (NO SOUND)";
const char descr_19[] PROGMEM = "Marching Ants (NO SOUND)";
const char descr_20[] PROGMEM = "Faint/Short Circuit (NO SOUND)";
const char descr_21[] PROGMEM = "Cantina Dance (NO SOUND)";
// Holoprojector
const char descr_22[] PROGMEM = "Random Holo Movement On (All)";
const char descr_23[] PROGMEM = "Holo Lights On (All)";
const char descr_24[] PROGMEM = "Holo Lights Off (All)";
const char descr_25[] PROGMEM = "Holo reset";
// Volume control
const char descr_26[] PROGMEM = "Volume Up";
const char descr_27[] PROGMEM = "Volume Down";
const char descr_28[] PROGMEM = "Volume Max";
const char descr_29[] PROGMEM = "Volume Mid";
// Dome panel movement
const char descr_30[] PROGMEM = "Open All Dome Panels";
const char descr_31[] PROGMEM = "Open Top Dome Panels";
const char descr_32[] PROGMEM = "Open Bottom Dome Panels";
const char descr_33[] PROGMEM = "Close All Dome Panels";
const char descr_34[] PROGMEM = "Open Dome Panel #1";
const char descr_35[] PROGMEM = "Close Dome Panel #1";
const char descr_36[] PROGMEM = "Open Dome Panel #2";
const char descr_37[] PROGMEM = "Close Dome Panel #2";
const char descr_38[] PROGMEM = "Open Dome Panel #3";
const char descr_39[] PROGMEM = "Close Dome Panel #3";
const char descr_40[] PROGMEM = "Open Dome Panel #4";
const char descr_41[] PROGMEM = "Close Dome Panel #4";
const char descr_42[] PROGMEM = "Open Dome Panel #5";
const char descr_43[] PROGMEM = "Close Dome Panel #5";
const char descr_44[] PROGMEM = "Open Dome Panel #6";
const char descr_45[] PROGMEM = "Close Dome Panel #6";
const char descr_46[] PROGMEM = "Open Dome Panel #7";
const char descr_47[] PROGMEM = "Close Dome Panel #7";
const char descr_48[] PROGMEM = "Open Dome Panel #8";
const char descr_49[] PROGMEM = "Close Dome Panel #8";
const char descr_50[] PROGMEM = "Open Dome Panel #9";
const char descr_51[] PROGMEM = "Close Dome Panel #9";
const char descr_52[] PROGMEM = "Open Dome Panel #10";
const char descr_53[] PROGMEM = "Close Dome Panel #10";
// Body panel movement *** assumes second Marcduino master in the body using Serial3 from Arduino ***
const char descr_54[] PROGMEM = "Open All Body Panels";
const char descr_55[] PROGMEM = "Close All Body Panels";
const char descr_56[] PROGMEM = "Open Body Panel #1";
const char descr_57[] PROGMEM = "Close Body Panel #1";
const char descr_58[] PROGMEM = "Open Body Panel #2";
const char descr_59[] PROGMEM = "Close Body Panel #2";
const char descr_60[] PROGMEM = "Open Body Panel #3";
const char descr_61[] PROGMEM = "Close Body Panel #3";
const char descr_62[] PROGMEM = "Open Body Panel #4";
const char descr_63[] PROGMEM = "Close Body Panel #4";
const char descr_64[] PROGMEM = "Open Body Panel #5";
const char descr_65[] PROGMEM = "Close Body Panel #5";
const char descr_66[] PROGMEM = "Open Body Panel #6";
const char descr_67[] PROGMEM = "Close Body Panel #6";
const char descr_68[] PROGMEM = "Open Body Panel #7";
const char descr_69[] PROGMEM = "Close Body Panel #7";
const char descr_70[] PROGMEM = "Open Body Panel #8";
const char descr_71[] PROGMEM = "Close Body Panel #8";
const char descr_72[] PROGMEM = "Open Body Panel #9";
const char descr_73[] PROGMEM = "Close Body Panel #9";
const char descr_74[] PROGMEM = "Open Body Panel #10";
const char descr_75[] PROGMEM = "Close Body Panel #10";
// Magic panel lighting
const char descr_76[] PROGMEM = "Magic Panel ON";
const char descr_77[] PROGMEM = "Magic Panel OFF";
const char descr_78[] PROGMEM = "Magic Panel Flicker (10 sec)";

// Set up a table (array) to reference the command descriptions.

const char *const table_stdDescr[] = {
  descr_00, descr_01, descr_02, descr_03, descr_04, descr_05, descr_06, descr_07, descr_08, descr_09,
  descr_10, descr_11, descr_12, descr_13, descr_14, descr_15, descr_16, descr_17, descr_18, descr_19,
  descr_20, descr_21, descr_22, descr_23, descr_24, descr_25, descr_26, descr_27, descr_28, descr_29,
  descr_30, descr_31, descr_32, descr_33, descr_34, descr_35, descr_36, descr_37, descr_38, descr_39,
  descr_40, descr_41, descr_42, descr_43, descr_44, descr_45, descr_46, descr_47, descr_48, descr_49,
  descr_50, descr_51, descr_52, descr_53, descr_54, descr_55, descr_56, descr_57, descr_58, descr_59,
  descr_60, descr_61, descr_62, descr_63, descr_64, descr_65, descr_66, descr_67, descr_68, descr_69,
  descr_70, descr_71, descr_72, descr_73, descr_74, descr_75, descr_76, descr_77, descr_78
};
#endif

// When sound is sent through the body master, some of the standard functions
//  need to split the command sending part to the dome and part to the body.
//  Here, we define constants to help with that.  Again, these are saved in
//  program memory.

const char dome_02[] PROGMEM = ":SE51\r";                      const char body_02[] PROGMEM = "$61\r";
const char dome_03[] PROGMEM = ":SE52\r";                      const char body_03[] PROGMEM = "";
const char dome_04[] PROGMEM = ":SE53\r";                      const char body_04[] PROGMEM = "";
const char dome_05[] PROGMEM = ":SE54\r";                      const char body_05[] PROGMEM = "";
const char dome_06[] PROGMEM = ":SE55\r";                      const char body_06[] PROGMEM = "$81\r";
const char dome_07[] PROGMEM = ":SE56\r@0T4\r";                const char body_07[] PROGMEM = "";
const char dome_08[] PROGMEM = ":SE57\r";                      const char body_08[] PROGMEM = "";
const char dome_09[] PROGMEM = ":SE00\r@@0T6\r*HD01";          const char body_09[] PROGMEM = "$71\r";
const char dome_10[] PROGMEM = "";                             const char body_10[] PROGMEM = "$86\r";
const char dome_11[] PROGMEM = ":SE00\r@0T0\r*ST00\r";         const char body_11[] PROGMEM = "$s\r";
const char dome_12[] PROGMEM = ":SE00\r@0T1\r*RD00\r*OF00\r";  const char body_12[] PROGMEM = "$R\r";
const char dome_13[] PROGMEM = ":SE00\r@0T1\r*ST00\r";         const char body_13[] PROGMEM = "$R\r";
const char dome_14[] PROGMEM = ":SE00\r@0T1\r*RD00\r*ON00\r";  const char body_14[] PROGMEM = "$R\r";

// Set up a table (array) to reference the split dome commands.

const char *const table_domeCmd[] = {
  dome_02, dome_03, dome_04, dome_05, dome_06, dome_07, dome_08,
  dome_09, dome_10, dome_11, dome_12, dome_13, dome_14
};

// Set up a table (array) to reference the split body commands.

const char *const table_bodyCmd[] = {
  body_02, body_03, body_04, body_05, body_06, body_07, body_08,
  body_09, body_10, body_11, body_12, body_13, body_14
};

// Marcduino supports defining your own custom functions.  These are made up of
//  sound tracks, panel movements, and logic displays.  Sound does not need an
//  array of commands, but panels and logic display do.  We'll build those here.
//  Once more, these constants are saved in program memory.

const char logic_01[] PROGMEM = "@0T1\r";
const char logic_02[] PROGMEM = "@0T4\r";
const char logic_03[] PROGMEM = "@0T5\r";
const char logic_04[] PROGMEM = "@0T6\r";
const char logic_05[] PROGMEM = "@0T10\r";
const char logic_06[] PROGMEM = "@0T11\r";
const char logic_07[] PROGMEM = "@0T92\r";
const char logic_08[] PROGMEM = "@0T100\r";

// Set up a table (array) to reference the logic functions.

const char *const table_logicCmd[] = {
  logic_01, logic_02, logic_03, logic_04,
  logic_05, logic_06, logic_07, logic_08
};

#ifdef BLACBOX_VERBOSE
const char ldDescr_01[] PROGMEM = "Display normal random sequence";
const char ldDescr_02[] PROGMEM = "Short circuit (10 second display sequence)";
const char ldDescr_03[] PROGMEM = "Scream (flashing light display sequence)";
const char ldDescr_04[] PROGMEM = "Leia (34 second light sequence)";
const char ldDescr_05[] PROGMEM = "Display “Star Wars”";
const char ldDescr_06[] PROGMEM = "March light sequence";
const char ldDescr_07[] PROGMEM = "Spectrum, bar graph display sequence";
const char ldDescr_08[] PROGMEM = "Dispaly custom text";

// Set up a table (array) to reference the logic function descriptions.

const char *const table_logicDescr[] = {
  ldDescr_01, ldDescr_02, ldDescr_03, ldDescr_04,
  ldDescr_05, ldDescr_06, ldDescr_07, ldDescr_08
};
#endif

// The custom panel functions have already been defined through the standard
// functions, so we'll use those commands to build the panel function array.

// Set up a table (array) to reference the panel functions.

const char *const table_panelCmd[] = {
  cmd_33, // Panels stay closed (normal position)
  cmd_15, // Scream sequence, all panels open
  cmd_16, // Wave panel sequence
  cmd_17, // Fast (smirk) back and forth panel sequence
  cmd_18, // Wave 2 panel sequence, open progressively all panels, then close one by one)
  cmd_19, // Marching ants panel sequence
  cmd_20, // Faint / short circuit panel sequence
  cmd_21  // Rhythmic cantina panel sequence
};

#ifdef BLACBOX_VERBOSE
// Set up a table (array) to reference the panel function descriptions.

const char *const table_panelDescr[] = {
  descr_33, descr_15, descr_16, descr_17,
  descr_18, descr_19, descr_20, descr_21
};
#endif

#endif
