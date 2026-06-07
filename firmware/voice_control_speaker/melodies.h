/******************************************************************************
 * melodies.h
 * 
 * Dinh nghia not nhac va cac bai melody mau cho Piezo Speaker
 * Moi bai nhac la mang {tan_so_Hz, thoi_gian_ms}
 * Tan so 0 = nghi (rest/pause)
 * 
 *****************************************************************************/

#ifndef MELODIES_H
#define MELODIES_H

// ============================================================================
// ======  DINH NGHIA NOT NHAC (Hz)  ========================================
// ============================================================================

// Nghi (silence)
#define REST     0

// Quang 3 (Octave 3)
#define NOTE_B3  247

// Quang 4 (Octave 4)
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494

// Quang 5 (Octave 5)
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

// Quang 6 (Octave 6)
#define NOTE_C6  1047
#define NOTE_D6  1175
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_G6  1568
#define NOTE_A6  1760

// ============================================================================
// ======  CAU TRUC NOT NHAC  ================================================
// ============================================================================

typedef struct {
    uint16_t frequency;   // Tan so Hz (0 = nghi)
    uint16_t duration;    // Thoi gian phat (ms)
} Note;

// ============================================================================
// ======  BAI 1: Twinkle Twinkle Little Star  ===============================
// ============================================================================

const Note melody_twinkle[] PROGMEM = {
    {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
    {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 800},
    {REST, 200},
    {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 400}, {NOTE_E4, 400},
    {NOTE_D4, 400}, {NOTE_D4, 400}, {NOTE_C4, 800},
    {REST, 200},
    {NOTE_G4, 400}, {NOTE_G4, 400}, {NOTE_F4, 400}, {NOTE_F4, 400},
    {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 800},
    {REST, 200},
    {NOTE_G4, 400}, {NOTE_G4, 400}, {NOTE_F4, 400}, {NOTE_F4, 400},
    {NOTE_E4, 400}, {NOTE_E4, 400}, {NOTE_D4, 800},
    {REST, 200},
    {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
    {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 800},
    {REST, 200},
    {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 400}, {NOTE_E4, 400},
    {NOTE_D4, 400}, {NOTE_D4, 400}, {NOTE_C4, 800},
};
const int melody_twinkle_len = sizeof(melody_twinkle) / sizeof(Note);

// ============================================================================
// ======  BAI 2: Happy Birthday  =============================================
// ============================================================================

const Note melody_happy_birthday[] PROGMEM = {
    {NOTE_C4, 300}, {NOTE_C4, 100}, {NOTE_D4, 400}, {NOTE_C4, 400},
    {NOTE_F4, 400}, {NOTE_E4, 800},
    {REST, 200},
    {NOTE_C4, 300}, {NOTE_C4, 100}, {NOTE_D4, 400}, {NOTE_C4, 400},
    {NOTE_G4, 400}, {NOTE_F4, 800},
    {REST, 200},
    {NOTE_C4, 300}, {NOTE_C4, 100}, {NOTE_C5, 400}, {NOTE_A4, 400},
    {NOTE_F4, 400}, {NOTE_E4, 400}, {NOTE_D4, 800},
    {REST, 200},
    {NOTE_AS4, 300}, {NOTE_AS4, 100}, {NOTE_A4, 400}, {NOTE_F4, 400},
    {NOTE_G4, 400}, {NOTE_F4, 800},
};
const int melody_happy_birthday_len = sizeof(melody_happy_birthday) / sizeof(Note);

// ============================================================================
// ======  BAI 3: Jingle Bells  ===============================================
// ============================================================================

const Note melody_jingle_bells[] PROGMEM = {
    {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 600},
    {REST, 100},
    {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 600},
    {REST, 100},
    {NOTE_E4, 300}, {NOTE_G4, 300}, {NOTE_C4, 400}, {NOTE_D4, 200},
    {NOTE_E4, 800},
    {REST, 200},
    {NOTE_F4, 300}, {NOTE_F4, 300}, {NOTE_F4, 400}, {NOTE_F4, 200},
    {NOTE_F4, 300}, {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 100},
    {NOTE_E4, 300}, {NOTE_D4, 300}, {NOTE_D4, 300}, {NOTE_E4, 300},
    {NOTE_D4, 600}, {NOTE_G4, 600},
    {REST, 200},
    {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 600},
    {REST, 100},
    {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 600},
    {REST, 100},
    {NOTE_E4, 300}, {NOTE_G4, 300}, {NOTE_C4, 400}, {NOTE_D4, 200},
    {NOTE_E4, 800},
    {REST, 200},
    {NOTE_F4, 300}, {NOTE_F4, 300}, {NOTE_F4, 400}, {NOTE_F4, 200},
    {NOTE_F4, 300}, {NOTE_E4, 300}, {NOTE_E4, 300}, {NOTE_E4, 100},
    {NOTE_G4, 300}, {NOTE_G4, 300}, {NOTE_F4, 300}, {NOTE_D4, 300},
    {NOTE_C4, 800},
};
const int melody_jingle_bells_len = sizeof(melody_jingle_bells) / sizeof(Note);

// ============================================================================
// ======  BANG MELODY  =======================================================
// ============================================================================

// Struct quan ly danh sach bai nhac
typedef struct {
    const Note* notes;
    int length;
    const char* name;
} MelodyInfo;

const MelodyInfo MELODY_LIST[] = {
    { melody_twinkle,        melody_twinkle_len,        "Twinkle Twinkle" },
    { melody_happy_birthday, melody_happy_birthday_len, "Happy Birthday"  },
    { melody_jingle_bells,   melody_jingle_bells_len,   "Jingle Bells"    },
};

const int MELODY_COUNT = sizeof(MELODY_LIST) / sizeof(MelodyInfo);

#endif // MELODIES_H
