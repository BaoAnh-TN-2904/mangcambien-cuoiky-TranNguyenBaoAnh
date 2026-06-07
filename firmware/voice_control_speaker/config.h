/******************************************************************************
 * config.h
 * 
 * Cau hinh trung tam cho du an Voice Control Mini Speaker
 * ESP32-S3 + INMP441 + Piezo Speaker + Edge Impulse
 * 
 *****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// ======  EDGE IMPULSE MODEL  ================================================
// ============================================================================
// 
// Ten project Edge Impulse: voice_control_vi
//
#include <voice_control_vi_inferencing.h>

// ============================================================================
// ======  I2S MICROPHONE (INMP441) PINS  =====================================
// ============================================================================
#define I2S_MIC_WS_PIN      4    // Word Select (LRCK)
#define I2S_MIC_SCK_PIN     5    // Serial Clock (BCLK)
#define I2S_MIC_SD_PIN      6    // Serial Data (DOUT from mic)

// ============================================================================
// ======  PIEZO SPEAKER PIN  =================================================
// ============================================================================
#define PIEZO_PIN            18   // PWM output to piezo buzzer

// ============================================================================
// ======  I2S CONFIGURATION  =================================================
// ============================================================================
#define I2S_PORT             I2S_NUM_0
#define I2S_SAMPLE_RATE      16000    // 16 kHz
#define I2S_SAMPLE_BITS      16       // 16-bit audio
#define I2S_DMA_BUF_COUNT    8        // So DMA buffers
#define I2S_DMA_BUF_LEN      512      // So samples moi DMA buffer

// ============================================================================
// ======  EDGE IMPULSE INFERENCE  ============================================
// ============================================================================
// Nguong confidence toi thieu de chap nhan lenh (0.0 - 1.0)
#define EI_CONFIDENCE_THRESHOLD   0.70f

// Chi so label trong model Edge Impulse
// Dieu chinh theo thu tu labels trong Edge Impulse project
// Mo Edge Impulse Studio -> Impulse Design de xem thu tu
#define LABEL_BAO_ANH        0    // "bao_anh" - wake word
#define LABEL_PHAT_NHAC      1    // "phat_nhac" - play
#define LABEL_DUNG_NHAC      2    // "dung_nhac" - stop  
#define LABEL_BAI_KE         3    // "bai_ke" - next track
#define LABEL_NOISE          4    // "noise" - background noise

// ============================================================================
// ======  STATE MACHINE TIMEOUTS  ============================================
// ============================================================================
#define LISTENING_TIMEOUT_MS    5000    // Timeout LISTENING -> IDLE (5 giay)
#define PAUSED_TIMEOUT_MS       10000   // Timeout PAUSED -> IDLE (10 giay)

// ============================================================================
// ======  PIEZO PLAYER  ======================================================
// ============================================================================
#define PIEZO_LEDC_CHANNEL   0        // LEDC channel cho PWM
#define PIEZO_LEDC_RESOLUTION 8       // 8-bit resolution
#define PIEZO_DEFAULT_VOLUME  127     // Duty cycle (0-255), 127 = 50%

// Beep feedback durations (ms)
#define BEEP_WAKE_FREQ       1500     // Tan so beep khi wake (Hz)
#define BEEP_WAKE_DURATION   200      // Thoi gian beep wake (ms)
#define BEEP_ACK_FREQ        2000     // Tan so beep xac nhan lenh (Hz)
#define BEEP_ACK_DURATION    100      // Thoi gian beep xac nhan (ms)
#define BEEP_ERROR_FREQ      500      // Tan so beep loi/timeout (Hz)
#define BEEP_ERROR_DURATION  300      // Thoi gian beep loi (ms)

// ============================================================================
// ======  SERIAL DEBUG  ======================================================
// ============================================================================
#define SERIAL_BAUD_RATE     115200

// Bat/tat debug log chi tiet
#define DEBUG_INFERENCE      1    // In ket qua inference moi frame
#define DEBUG_STATE_MACHINE  1    // In chuyen trang thai
#define DEBUG_I2S            0    // In thong tin I2S (verbose, tat khi ko can)

#endif // CONFIG_H
