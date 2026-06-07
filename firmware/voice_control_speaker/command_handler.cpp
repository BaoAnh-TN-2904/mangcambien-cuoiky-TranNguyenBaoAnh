/******************************************************************************
 * command_handler.cpp
 * 
 * State Machine dieu khien he thong Voice Control Speaker
 * 
 * Bang chuyen trang thai:
 * 
 *  Trang thai hien tai   |  Lenh nhan duoc   |  Trang thai moi   |  Hanh dong
 * -----------------------|-------------------|-------------------|------------------
 *  IDLE                  |  bao_anh          |  LISTENING        |  Beep wake
 *  IDLE                  |  khac             |  IDLE             |  Bo qua
 *  LISTENING             |  phat_nhac        |  PLAYING          |  Beep ack + Play
 *  LISTENING             |  bai_ke           |  PLAYING          |  Beep ack + Next + Play
 *  LISTENING             |  timeout 5s       |  IDLE             |  Beep error
 *  PLAYING               |  dung_nhac        |  PAUSED           |  Beep ack + Pause
 *  PLAYING               |  bai_ke           |  PLAYING          |  Beep ack + Next track
 *  PLAYING               |  bao_anh          |  LISTENING        |  Pause + Beep wake
 *  PAUSED                |  phat_nhac        |  PLAYING          |  Beep ack + Resume
 *  PAUSED                |  bai_ke           |  PLAYING          |  Beep ack + Next + Play
 *  PAUSED                |  bao_anh          |  LISTENING        |  Beep wake
 *  PAUSED                |  timeout 10s      |  IDLE             |  Beep error
 * 
 *****************************************************************************/

#include "command_handler.h"
#include "config.h"
#include "piezo_player.h"

// ============================================================================
// ======  BIEN TRANG THAI  ===================================================
// ============================================================================

static SystemState _current_state = STATE_IDLE;
static unsigned long _state_enter_time = 0;  // Thoi diem vao trang thai hien tai

// Ten trang thai cho debug
static const char* STATE_NAMES[] = {
    "IDLE",
    "LISTENING",
    "PLAYING",
    "PAUSED"
};

// ============================================================================
// ======  HELPER NOI BO  =====================================================
// ============================================================================

/**
 * Chuyen sang trang thai moi + log
 */
static void _transition_to(SystemState new_state) {
    SystemState old_state = _current_state;
    _current_state = new_state;
    _state_enter_time = millis();

    #if DEBUG_STATE_MACHINE
    Serial.println("----------------------------------------");
    Serial.printf("[STATE] %s -> %s\n", STATE_NAMES[old_state], STATE_NAMES[new_state]);
    Serial.println("----------------------------------------");
    #endif
}

// ============================================================================
// ======  KHOI TAO  =========================================================
// ============================================================================

void command_handler_init() {
    _current_state = STATE_IDLE;
    _state_enter_time = millis();
    Serial.println("[CMD] Command Handler khoi tao -> IDLE");
    Serial.println("[CMD] Dang cho wake word \"bao anh\"...");
}

// ============================================================================
// ======  XU LY KET QUA INFERENCE  ==========================================
// ============================================================================

void command_handler_process(InferenceResult result) {
    // Bo qua neu confidence thap
    if (!result.is_valid) {
        return;
    }

    int label = result.label_index;

    // Bo qua noise o moi trang thai
    if (label == LABEL_NOISE) {
        return;
    }

    // ====== XU LY THEO TRANG THAI HIEN TAI ======

    switch (_current_state) {

        // ----------------------------------------------------------
        // IDLE: Chi chap nhan wake word
        // ----------------------------------------------------------
        case STATE_IDLE:
            if (label == LABEL_BAO_ANH) {
                Serial.println("[CMD] [MIC] Wake word detected! -> LISTENING");
                piezo_beep(BEEP_WAKE_FREQ, BEEP_WAKE_DURATION);
                _transition_to(STATE_LISTENING);
            }
            break;

        // ----------------------------------------------------------
        // LISTENING: Cho lenh dieu khien
        // ----------------------------------------------------------
        case STATE_LISTENING:
            if (label == LABEL_PHAT_NHAC) {
                Serial.println("[CMD] [PLAY] Lenh: PHAT NHAC");
                piezo_beep(BEEP_ACK_FREQ, BEEP_ACK_DURATION);
                piezo_play(piezo_get_current_track());
                _transition_to(STATE_PLAYING);
            }
            else if (label == LABEL_BAI_KE) {
                Serial.println("[CMD] [NEXT] Lenh: BAI KE (chuyen + phat)");
                piezo_beep(BEEP_ACK_FREQ, BEEP_ACK_DURATION);
                piezo_next_track();
                _transition_to(STATE_PLAYING);
            }
            else if (label == LABEL_DUNG_NHAC) {
                Serial.println("[CMD] [STOP] Lenh: DUNG NHAC (khong co gi dang phat)");
                piezo_beep(BEEP_ERROR_FREQ, BEEP_ERROR_DURATION);
                // Giu o LISTENING
            }
            else if (label == LABEL_BAO_ANH) {
                // Re-trigger wake -> reset timeout
                Serial.println("[CMD] [MIC] Wake word lai -> Reset timeout");
                piezo_beep(BEEP_WAKE_FREQ, BEEP_WAKE_DURATION);
                _state_enter_time = millis();  // Reset timeout
            }
            break;

        // ----------------------------------------------------------
        // PLAYING: Dang phat nhac
        // ----------------------------------------------------------
        case STATE_PLAYING:
            if (label == LABEL_DUNG_NHAC) {
                Serial.println("[CMD] [PAUSE] Lenh: DUNG NHAC -> PAUSED");
                piezo_beep(BEEP_ACK_FREQ, BEEP_ACK_DURATION);
                piezo_stop();
                _transition_to(STATE_PAUSED);
            }
            else if (label == LABEL_BAI_KE) {
                Serial.printf("[CMD] [NEXT] Lenh: BAI KE (dang o track %d)\n", 
                              piezo_get_current_track());
                piezo_beep(BEEP_ACK_FREQ, BEEP_ACK_DURATION);
                piezo_next_track();
                // Giu o PLAYING
            }
            else if (label == LABEL_BAO_ANH) {
                Serial.println("[CMD] [MIC] Wake word -> Tam dung, cho lenh moi");
                piezo_stop();
                piezo_beep(BEEP_WAKE_FREQ, BEEP_WAKE_DURATION);
                _transition_to(STATE_LISTENING);
            }
            break;

        // ----------------------------------------------------------
        // PAUSED: Tam dung nhac
        // ----------------------------------------------------------
        case STATE_PAUSED:
            if (label == LABEL_PHAT_NHAC) {
                Serial.println("[CMD] [PLAY] Lenh: PHAT NHAC (resume)");
                piezo_beep(BEEP_ACK_FREQ, BEEP_ACK_DURATION);
                piezo_play(piezo_get_current_track());
                _transition_to(STATE_PLAYING);
            }
            else if (label == LABEL_BAI_KE) {
                Serial.println("[CMD] [NEXT] Lenh: BAI KE (chuyen + phat)");
                piezo_beep(BEEP_ACK_FREQ, BEEP_ACK_DURATION);
                piezo_next_track();
                _transition_to(STATE_PLAYING);
            }
            else if (label == LABEL_BAO_ANH) {
                Serial.println("[CMD] [MIC] Wake word -> LISTENING");
                piezo_beep(BEEP_WAKE_FREQ, BEEP_WAKE_DURATION);
                _transition_to(STATE_LISTENING);
            }
            break;
    }
}

// ============================================================================
// ======  CAP NHAT TIMEOUT  ==================================================
// ============================================================================

void command_handler_update() {
    unsigned long elapsed = millis() - _state_enter_time;

    switch (_current_state) {
        case STATE_LISTENING:
            if (elapsed >= LISTENING_TIMEOUT_MS) {
                Serial.println("[CMD] [TIMEOUT] LISTENING timeout -> IDLE");
                piezo_beep(BEEP_ERROR_FREQ, BEEP_ERROR_DURATION);
                _transition_to(STATE_IDLE);
                Serial.println("[CMD] Dang cho wake word \"bao anh\"...");
            }
            break;

        case STATE_PAUSED:
            if (elapsed >= PAUSED_TIMEOUT_MS) {
                Serial.println("[CMD] [TIMEOUT] PAUSED timeout -> IDLE");
                piezo_beep(BEEP_ERROR_FREQ, BEEP_ERROR_DURATION);
                _transition_to(STATE_IDLE);
                Serial.println("[CMD] Dang cho wake word \"bao anh\"...");
            }
            break;

        default:
            break;
    }
}

// ============================================================================
// ======  GETTERS  ============================================================
// ============================================================================

SystemState command_handler_get_state() {
    return _current_state;
}

const char* command_handler_get_state_name() {
    return STATE_NAMES[_current_state];
}
