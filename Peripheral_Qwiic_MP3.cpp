/* =================================================================================
 *    B.L.A.C.Box: Brian Lubkeman's Astromech Controller
 * =================================================================================
 * Peripheral_Qwiic_MP3.cpp - Library for a Sparkfun Qwiic MP3 Trigger
 * Created by Brian Lubkeman, 22 November 2020
 * Inspired by S.H.A.D.O.W. controller code written by KnightShade
 * Released into the public domain.
 */
#include "Arduino.h"
#include "Peripheral_Qwiic_MP3.h"

#if defined(QWIIC)

// =====================
//      Constructor
// =====================
Qwiic_MP3::Qwiic_MP3(Buffer* pBuffer)
{
  _buffer = pBuffer;
  _volume = QWIIC_MP3_INITIAL_VOLUME;  // Initializing to mid-volume on a scale of 1-10.

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_DOME) || defined (DEBUG_ALL)
  _className = F("Qwiic_MP3::");
  #endif
}


// =====================
//        begin()
// =====================
void Qwiic_MP3::begin()
{
  // ----------------------------
  // Start the Qwiic MP3 trigger.
  // ----------------------------

  if ( ! _mp3.begin() ) {
    #if defined(DEBUG_QWIIC) || defined(DEBUG_ALL)
    output = _className+F("begin()");
    output += F(" - Qwiic MP3 failed to respond.");
    while (1);
    #endif
  }

  // ---------------------------
  // Verify a SD card is loaded.
  // ---------------------------

  if ( ! _mp3.hasCard() ) {
    #if defined(DEBUG_QWIIC) || defined(DEBUG_ALL)
    output = _className+F("begin()");
    output += F(" - Qwiic MP3 is missing its SD card.");
    while (1);
    #endif
  }

  // ------------------------------------------
  // Set an initial volume level. 0=off, 31=max
  // Play the startup track.
  // ------------------------------------------

  _mp3.setVolume(_volume);
  _mp3.playTrack(QWIIC_MP3_STARTUP_TRACK);

  // ----------
  // Debugging.
  // ----------

  #if defined(DEBUG_QWIIC) || defined (DEBUG_ALL)
  output = _className+F("begin()");
  output += F(" - Qwiic MP3 Trigger started.");
  printOutput();
  #endif
}


// ==============================
//     interpretController()
// ==============================
void Qwiic_MP3::interpretController()
{
  #if defined(MD_STD_CONTROLS)

  /* =====================================================
   * Based on standard Marcduino commands, these are the
   * button combinations that trigger a sound action.
   *
   * UP               - Full Awake (sound random)
   * RIGHT            - Full Awake+ (sound random)
   * DOWN             - Quiet Mode (sound off)
   * LEFT             - Mid Awake (sound random)
   * SELECT+UP        - Volume up
   * SELECT+DOWN      - Volume down
   * SELECT+TRIANGLE  - Volume mid
   * SELECT+CROSS     - Volume max
   * START+UP         - Scream (play track #)
   * START+RIGHT      - Faint/Short Circuit (play track #)
   * START+DOWN       - Disco (play track #)
   * PS+UP            - Custom 1 (play track #)
   * PS+RIGHT         - Custom 2 (play track #)
   * PS+DOWN          - Custom 3 (play track #)
   * PS+LEFT          - Custom 4 (play track #)
   * L1+DOWN          - Leia Message (play track #)
  /* ===================================================== */

  if ( ! ( _buffer->getButton(SELECT) && _buffer->getButton(START) && _buffer->getButton(PS) && _buffer->getButton(L1) )) {

    // Quiet mode

    if ( _buffer->getButton(DOWN) ) {
      _mp3.stop();

    } else if ( _buffer->getButton(UP) ||
                _buffer->getButton(RIGHT) ||
                _buffer->getButton(LEFT) ) {

    // Full Awake, Full Awake+, or Mid Awake modes
      
      _mp3.playTrack(random(QWIIC_MP3_RANDOM_MIN, QWIIC_MP3_RANDOM_MAX+1));
    }

  } else if ( _buffer->getButton(SELECT) ) {

    if ( _buffer->getButton(UP) ) {

      // Volume up.

      if ( _volume < QWIIC_MP3_MAX_VOLUME )
        _mp3.setVolume(_volume++);
    }

    else if ( _buffer->getButton(DOWN) ) {

      // Volume down.

      if ( _volume > 0 )
        _mp3.setVolume(_volume--);
    }

    else if ( _buffer->getButton(TRIANGLE) ) {

      // Volume mid.

      _volume = QWIIC_MP3_MAX_VOLUME / 2;
      _mp3.setVolume(_volume);
    }

    else if ( _buffer->getButton(CROSS) ) {

      // Volume max.

      _volume = QWIIC_MP3_MAX_VOLUME;
      _mp3.setVolume(_volume);
    }

  } else if ( _buffer->getButton(START) ) {
    if ( _buffer->getButton(UP) )            { _mp3.playTrack(1); } // Scream
    else if ( _buffer->getButton(RIGHT) )    { _mp3.playTrack(2); } // Faint/Short circuit
    else if ( _buffer->getButton(DOWN) )     { _mp3.playTrack(3); } // Disco

  } else if ( _buffer->getButton(PS) ) {
    if ( _buffer->getButton(UP) )            { _mp3.playTrack(4); } // Play bank 8 track 8
    else if ( _buffer->getButton(RIGHT) )    { _mp3.playTrack(5); } // Play bank 8 track 9
    else if ( _buffer->getButton(DOWN) )     { _mp3.playTrack(6); } // Play bank 8 track 10
    else if ( _buffer->getButton(LEFT) )     { _mp3.playTrack(7); } // Play bank 8 track 11

  } else if ( _buffer->getButton(L1) &&
             _buffer->getButton(DOWN) )      { _mp3.playTrack(8); } // Leia message

  #elif defined(MD_CUSTOM_CONTROLS)

  // TO DO: Future addition

  #endif
}
#endif
