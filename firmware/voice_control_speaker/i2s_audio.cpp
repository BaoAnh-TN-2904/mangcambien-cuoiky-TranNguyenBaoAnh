/******************************************************************************
 * i2s_audio.cpp
 * 
 * Driver I2S cho microphone INMP441 tren ESP32-S3
 * 
 * INMP441 output 32-bit data nhung chi 24-bit co y nghia (MSB-first).
 * Ta doc 32-bit roi shift de lay 16-bit cao nhat.
 * 
 * Su dung ESP-IDF I2S driver (legacy) tuong thich voi Arduino ESP32 core.
 * 
 *****************************************************************************/

#include "i2s_audio.h"
#include "config.h"
#include <driver/i2s.h>

// ============================================================================
// ======  KHOI TAO I2S  =====================================================
// ============================================================================

bool i2s_audio_init() {
    // Cau hinh I2S driver
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),  // Master, chi nhan
        .sample_rate = I2S_SAMPLE_RATE,                         // 16000 Hz
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,           // INMP441 gui 32-bit
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,            // Mono - kenh trai (L/R = GND)
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,      // Standard I2S
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,               // Interrupt level 1
        .dma_buf_count = I2S_DMA_BUF_COUNT,                     // 8 DMA buffers
        .dma_buf_len = I2S_DMA_BUF_LEN,                         // 512 samples moi buffer
        .use_apll = false,                                       // Khong dung APLL
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    // Cau hinh pin I2S
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_MIC_SCK_PIN,     // GPIO5 -- Serial Clock
        .ws_io_num = I2S_MIC_WS_PIN,       // GPIO4 -- Word Select
        .data_out_num = I2S_PIN_NO_CHANGE,  // Khong dung TX (speaker dung PWM)
        .data_in_num = I2S_MIC_SD_PIN       // GPIO6 -- Serial Data from mic
    };

    // Cai dat driver I2S
    esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("[I2S] Loi cai dat driver: %d\n", err);
        return false;
    }

    // Cai dat pin I2S
    err = i2s_set_pin(I2S_PORT, &pin_config);
    if (err != ESP_OK) {
        Serial.printf("[I2S] Loi cau hinh pin: %d\n", err);
        return false;
    }

    // Xoa DMA buffer cu
    i2s_zero_dma_buffer(I2S_PORT);

    Serial.println("[I2S] Khoi tao thanh cong");
    Serial.printf("[I2S] Sample Rate: %d Hz, Bits: 32 (->16), Mono\n", I2S_SAMPLE_RATE);

    return true;
}

// ============================================================================
// ======  DOC AUDIO SAMPLES  ================================================
// ============================================================================

int i2s_audio_read(int16_t* buffer, int num_samples) {
    // Buffer tam de doc 32-bit samples tu INMP441
    int32_t raw_samples[I2S_DMA_BUF_LEN];
    int samples_read = 0;

    while (samples_read < num_samples) {
        // Tinh so samples can doc trong lan nay
        int to_read = min((int)I2S_DMA_BUF_LEN, num_samples - samples_read);
        size_t bytes_read = 0;

        // Doc tu I2S DMA buffer (blocking voi timeout 100ms)
        esp_err_t err = i2s_read(I2S_PORT, 
                                  (void*)raw_samples, 
                                  to_read * sizeof(int32_t), 
                                  &bytes_read, 
                                  pdMS_TO_TICKS(100));

        if (err != ESP_OK) {
            #if DEBUG_I2S
            Serial.printf("[I2S] Loi doc: %d\n", err);
            #endif
            break;
        }

        int got_samples = bytes_read / sizeof(int32_t);

        // Chuyen 32-bit -> 16-bit (lay 16 bit cao nhat)
        // INMP441 tra data o MSB-aligned, 24-bit huu ich trong 32-bit word
        for (int i = 0; i < got_samples; i++) {
            buffer[samples_read + i] = (int16_t)(raw_samples[i] >> 16);
        }

        samples_read += got_samples;

        #if DEBUG_I2S
        if (got_samples > 0) {
            Serial.printf("[I2S] Doc %d samples (tong: %d/%d)\n", 
                          got_samples, samples_read, num_samples);
        }
        #endif
    }

    return samples_read;
}

// ============================================================================
// ======  DOC VAO BUFFER FLOAT CHO EDGE IMPULSE  ===========================
// ============================================================================

bool i2s_audio_read_to_float_buffer(float* buffer, int num_samples) {
    // Buffer tam 16-bit
    const int CHUNK_SIZE = 512;
    int16_t raw_chunk[CHUNK_SIZE];
    int total_read = 0;

    while (total_read < num_samples) {
        int to_read = min(CHUNK_SIZE, num_samples - total_read);
        int got = i2s_audio_read(raw_chunk, to_read);

        if (got <= 0) {
            Serial.println("[I2S] Khong doc duoc du lieu!");
            return false;
        }

        // Chuyen int16_t -> float (chuan hoa ve [-1.0, 1.0] cho Edge Impulse)
        for (int i = 0; i < got; i++) {
            buffer[total_read + i] = (float)raw_chunk[i] / 32768.0f;
        }

        total_read += got;
    }

    return true;
}

// ============================================================================
// ======  DUNG I2S  ==========================================================
// ============================================================================

void i2s_audio_deinit() {
    i2s_driver_uninstall(I2S_PORT);
    Serial.println("[I2S] Driver da duoc go");
}
