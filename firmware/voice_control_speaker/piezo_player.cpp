/******************************************************************************
 * piezo_player.cpp
 * 
 * Module phat nhac qua Piezo Speaker su dung LEDC PWM tren ESP32-S3
 * 
 * Cach hoat dong:
 *   - Dung LEDC timer de tao song vuong voi tan so = not nhac
 *   - Non-blocking: update() kiem tra thoi gian va chuyen not tiep theo
 *   - Melody tu dong lap lai (loop) cho den khi goi stop()
 *   - Beep dung blocking delay (ngan, cho feedback nhanh)
 * 
 *****************************************************************************/

#include "piezo_player.h"
#include "config.h"
#include "melodies.h"

// ============================================================================
// ======  BIEN TRANG THAI  ===================================================
// ============================================================================

static bool _is_playing = false;          // Dang phat nhac?
static int  _current_track = 0;           // Index bai hien tai
static int  _current_note_index = 0;      // Index not hien tai trong bai
static unsigned long _note_start_time = 0; // Thoi diem bat dau not hien tai
static bool _is_beeping = false;          // Dang phat beep? (block melody)

// ============================================================================
// ======  HELPER NOI BO  =====================================================
// ============================================================================

/**
 * Phat mot tan so qua LEDC PWM
 * @param freq Tan so Hz (0 = tat)
 */
static void _tone(int freq) {
    if (freq <= 0) {
        // Tat am thanh (REST)
        ledcWrite(PIEZO_PIN, 0);
    } else {
        // Thay doi tan so LEDC
        ledcChangeFrequency(PIEZO_PIN, freq, PIEZO_LEDC_RESOLUTION);
        ledcWrite(PIEZO_PIN, PIEZO_DEFAULT_VOLUME);
    }
}

/**
 * Tat am thanh hoan toan
 */
static void _no_tone() {
    ledcWrite(PIEZO_PIN, 0);
}

/**
 * Doc not nhac tu PROGMEM
 */
static Note _read_note(int track_index, int note_index) {
    Note note;
    memcpy_P(&note, &MELODY_LIST[track_index].notes[note_index], sizeof(Note));
    return note;
}

// ============================================================================
// ======  KHOI TAO  =========================================================
// ============================================================================

void piezo_init() {
    // Cau hinh LEDC cho GPIO18
    // ESP32-S3 Arduino Core 3.x: dung ledcAttach thay vi ledcSetup + ledcAttachPin
    ledcAttach(PIEZO_PIN, 1000, PIEZO_LEDC_RESOLUTION);  // Default 1kHz, 8-bit
    _no_tone();

    _is_playing = false;
    _current_track = 0;
    _current_note_index = 0;
    _is_beeping = false;

    Serial.println("[PIEZO] Khoi tao thanh cong");
    Serial.printf("[PIEZO] Pin: GPIO%d, So bai nhac: %d\n", PIEZO_PIN, MELODY_COUNT);
    
    for (int i = 0; i < MELODY_COUNT; i++) {
        Serial.printf("[PIEZO]   [%d] %s (%d not)\n", 
                      i, MELODY_LIST[i].name, MELODY_LIST[i].length);
    }
}

// ============================================================================
// ======  DIEU KHIEN PHAT NHAC  ==============================================
// ============================================================================

void piezo_play(int track_index) {
    // Gioi han index hop le
    if (track_index < 0 || track_index >= MELODY_COUNT) {
        track_index = 0;
    }

    _current_track = track_index;
    _current_note_index = 0;
    _is_playing = true;

    // Phat not dau tien
    Note note = _read_note(_current_track, 0);
    _tone(note.frequency);
    _note_start_time = millis();

    Serial.printf("[PIEZO] [>] Phat bai: \"%s\" (track %d)\n", 
                  MELODY_LIST[_current_track].name, _current_track);
}

void piezo_stop() {
    _is_playing = false;
    _no_tone();
    Serial.println("[PIEZO] [x] Dung phat nhac");
}

void piezo_next_track() {
    // Chuyen sang bai tiep theo (quay vong)
    int next = (_current_track + 1) % MELODY_COUNT;
    Serial.printf("[PIEZO] [>>] Chuyen bai: %d -> %d\n", _current_track, next);
    piezo_play(next);
}

// ============================================================================
// ======  CAP NHAT NON-BLOCKING  =============================================
// ============================================================================

void piezo_update() {
    // Khong lam gi neu dang beep hoac khong phat nhac
    if (_is_beeping || !_is_playing) {
        return;
    }

    // Kiem tra not hien tai da het chua
    Note current_note = _read_note(_current_track, _current_note_index);
    unsigned long elapsed = millis() - _note_start_time;

    if (elapsed >= current_note.duration) {
        // Chuyen sang not tiep theo
        _current_note_index++;

        // Kiem tra het bai -> lap lai tu dau
        if (_current_note_index >= MELODY_LIST[_current_track].length) {
            _current_note_index = 0;  // Loop bai nhac
            Serial.printf("[PIEZO] [~] Lap lai: \"%s\"\n", 
                          MELODY_LIST[_current_track].name);
        }

        // Phat not moi
        Note next_note = _read_note(_current_track, _current_note_index);
        _tone(next_note.frequency);
        _note_start_time = millis();
    }
}

// ============================================================================
// ======  BEEP PHAN HOI  =====================================================
// ============================================================================

void piezo_beep(int frequency, int duration_ms) {
    _is_beeping = true;

    // Tam dung melody neu dang phat
    bool was_playing = _is_playing;

    // Phat beep (blocking)
    _tone(frequency);
    delay(duration_ms);
    _no_tone();
    delay(50);  // Khoang lang ngan sau beep

    _is_beeping = false;

    // Khoi phuc melody neu dang phat
    if (was_playing && _is_playing) {
        Note note = _read_note(_current_track, _current_note_index);
        _tone(note.frequency);
        _note_start_time = millis();
    }
}

// ============================================================================
// ======  GETTERS  ============================================================
// ============================================================================

bool piezo_is_playing() {
    return _is_playing;
}

int piezo_get_current_track() {
    return _current_track;
}

const char* piezo_get_current_track_name() {
    if (_current_track >= 0 && _current_track < MELODY_COUNT) {
        return MELODY_LIST[_current_track].name;
    }
    return "Unknown";
}
