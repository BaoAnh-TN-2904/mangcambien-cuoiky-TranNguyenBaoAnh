/******************************************************************************
 * voice_control_speaker.ino
 * 
 * FIRMWARE CHINH -- Voice Control Mini Speaker
 * ESP32-S3 + INMP441 + Piezo Speaker + Edge Impulse
 * 
 * Chuc nang:
 *   - Thu am lien tuc qua INMP441 (I2S, 16kHz, 16-bit, mono)
 *   - Nhan dang lenh thoai bang Edge Impulse (MFCC + Dense NN)
 *   - Dieu khien phat nhac qua Piezo Speaker (LEDC PWM)
 *   - 5 lenh: "bao anh" (wake), "phat nhac", "dung nhac", "bai ke", noise
 * 
 * Phan cung:
 *   - ESP32-S3 DevKitC
 *   - INMP441 I2S MEMS Microphone (WS=4, SCK=5, SD=6)
 *   - Piezo Buzzer (GPIO18)
 * 
 * Yeu cau:
 *   - Arduino IDE voi ESP32-S3 board package
 *   - Edge Impulse Arduino Library (da train voi 5 labels)
 * 
 * Tac gia: Voice Control Speaker Project
 * Ngay: 2026
 * 
 *****************************************************************************/

#include "config.h"       // <- Edge Impulse #include nam trong file nay
#include "i2s_audio.h"
#include "audio_processor.h"
#include "command_handler.h"
#include "piezo_player.h"

// ============================================================================
// ======  AUDIO BUFFER  ======================================================
// ============================================================================

// Buffer chua audio samples cho moi lan inference
// Kich thuoc = EI_CLASSIFIER_RAW_SAMPLE_COUNT (tu dong tu Edge Impulse model)
static float *audio_buffer = nullptr;
static int audio_buffer_size = 0;

// ============================================================================
// ======  SETUP  =============================================================
// ============================================================================

void setup() {
    // ----- Serial -----
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial && millis() < 3000);  // Doi Serial toi da 3 giay
    
    delay(500);
    Serial.println();
    Serial.println("============================================");
    Serial.println("|   VOICE CONTROL MINI SPEAKER - ESP32-S3  |");
    Serial.println("|   Edge Impulse + INMP441 + Piezo         |");
    Serial.println("============================================");
    Serial.println();

    // ----- Piezo Speaker -----
    Serial.println("[BOOT] Khoi tao Piezo Speaker...");
    piezo_init();
    
    // Beep khoi dong: 2 tieng beep ngan = san sang
    piezo_beep(1000, 100);
    delay(100);
    piezo_beep(1500, 100);
    Serial.println("[BOOT] Piezo OK [v]");
    Serial.println();

    // ----- I2S Microphone -----
    Serial.println("[BOOT] Khoi tao I2S Microphone (INMP441)...");
    if (!i2s_audio_init()) {
        Serial.println("[BOOT] [X] LOI: Khong the khoi tao I2S!");
        Serial.println("[BOOT] Kiem tra ket noi INMP441:");
        Serial.printf("[BOOT]   WS  -> GPIO%d\n", I2S_MIC_WS_PIN);
        Serial.printf("[BOOT]   SCK -> GPIO%d\n", I2S_MIC_SCK_PIN);
        Serial.printf("[BOOT]   SD  -> GPIO%d\n", I2S_MIC_SD_PIN);
        // Beep loi lien tuc
        while (true) {
            piezo_beep(BEEP_ERROR_FREQ, 500);
            delay(500);
        }
    }
    Serial.println("[BOOT] I2S OK [v]");
    Serial.println();

    // ----- Edge Impulse Audio Processor -----
    Serial.println("[BOOT] Khoi tao Edge Impulse Processor...");
    audio_processor_init();
    
    // Cap phat buffer cho inference
    audio_buffer_size = audio_processor_get_buffer_size();
    audio_buffer = (float*)malloc(audio_buffer_size * sizeof(float));
    
    if (audio_buffer == nullptr) {
        Serial.printf("[BOOT] [X] LOI: Khong du bo nho cho audio buffer (%d samples = %d bytes)\n",
                      audio_buffer_size, audio_buffer_size * sizeof(float));
        while (true) {
            piezo_beep(BEEP_ERROR_FREQ, 200);
            delay(200);
        }
    }
    Serial.printf("[BOOT] Audio buffer: %d samples (%d bytes) [v]\n", 
                  audio_buffer_size, audio_buffer_size * sizeof(float));
    Serial.println();

    // ----- Command Handler (State Machine) -----
    Serial.println("[BOOT] Khoi tao Command Handler...");
    command_handler_init();
    Serial.println();

    // ----- Hoan tat -----
    Serial.println("============================================");
    Serial.println("|          HE THONG SAN SANG!              |");
    Serial.println("|    Noi \"bao anh\" de kich hoat            |");
    Serial.println("============================================");
    Serial.println();

    // Beep san sang: 3 tieng beep tang dan
    piezo_beep(800, 80);
    delay(50);
    piezo_beep(1200, 80);
    delay(50);
    piezo_beep(1600, 150);
    
    // In thong tin bo nho
    Serial.printf("[SYS] Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("[SYS] PSRAM size: %d bytes\n", ESP.getPsramSize());
    Serial.println();
}

// ============================================================================
// ======  MAIN LOOP  =========================================================
// ============================================================================

void loop() {
    // ------ BUOC 1: Thu am tu INMP441 ------
    // Doc audio samples vao buffer (blocking cho den du)
    bool read_ok = i2s_audio_read_to_float_buffer(audio_buffer, audio_buffer_size);
    
    if (!read_ok) {
        Serial.println("[LOOP] Loi doc audio! Bo qua frame nay.");
        delay(10);
        return;
    }

    // ------ BUOC 2: Chay Edge Impulse Inference ------
    InferenceResult result = audio_processor_inference(audio_buffer, audio_buffer_size);

    // ------ BUOC 3: Xu ly lenh ------
    command_handler_process(result);

    // ------ BUOC 4: Cap nhat he thong ------
    command_handler_update();  // Kiem tra timeout
    piezo_update();            // Cap nhat phat nhac non-blocking

    // ------ Debug: In trang thai dinh ky ------
    static unsigned long last_status_print = 0;
    if (millis() - last_status_print >= 5000) {
        Serial.printf("[STATUS] State: %s | Piezo: %s | Track: %d (%s) | Heap: %d\n",
                      command_handler_get_state_name(),
                      piezo_is_playing() ? "PLAYING" : "STOPPED",
                      piezo_get_current_track(),
                      piezo_get_current_track_name(),
                      ESP.getFreeHeap());
        last_status_print = millis();
    }
}
