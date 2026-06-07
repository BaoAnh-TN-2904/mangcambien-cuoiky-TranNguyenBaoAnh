/******************************************************************************
 * audio_processor.cpp
 * 
 * Wrapper cho Edge Impulse inference engine
 * 
 * Luong xu ly:
 *   1. Nhan buffer audio float (normalized -1.0 to 1.0)
 *   2. Tao Edge Impulse signal tu buffer
 *   3. Chay classifier (MFCC -> Dense NN)
 *   4. Tra ve label co confidence cao nhat
 * 
 *****************************************************************************/

#include "audio_processor.h"
#include "config.h"

// ============================================================================
// ======  EDGE IMPULSE SIGNAL CALLBACK  ======================================
// ============================================================================

// Buffer toan cuc de Edge Impulse callback truy cap
static float* _inference_buffer = nullptr;
static int    _inference_buffer_size = 0;

/**
 * Callback cho Edge Impulse signal_t
 * Edge Impulse goi ham nay de lay raw data tu buffer
 */
static int audio_signal_get_data(size_t offset, size_t length, float* out_ptr) {
    // Copy du lieu tu buffer vao output pointer
    for (size_t i = 0; i < length; i++) {
        size_t idx = offset + i;
        if (idx < (size_t)_inference_buffer_size) {
            out_ptr[i] = _inference_buffer[idx];
        } else {
            out_ptr[i] = 0.0f;  // Padding neu vuot qua buffer
        }
    }
    return EIDSP_OK;
}

// ============================================================================
// ======  KHOI TAO  ==========================================================
// ============================================================================

void audio_processor_init() {
    Serial.println("========================================");
    Serial.println("  EDGE IMPULSE AUDIO PROCESSOR");
    Serial.println("========================================");
    audio_processor_print_model_info();
}

// ============================================================================
// ======  IN THONG TIN MODEL  ================================================
// ============================================================================

void audio_processor_print_model_info() {
    Serial.println("[EI] Thong tin model:");
    Serial.printf("  - Raw sample count : %d\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT);
    Serial.printf("  - Frame size       : %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    Serial.printf("  - Frame count      : %d\n", EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME);
    Serial.printf("  - Frequency        : %d Hz\n", (int)EI_CLASSIFIER_FREQUENCY);
    Serial.printf("  - Label count      : %d\n", EI_CLASSIFIER_LABEL_COUNT);
    
    Serial.println("  - Labels:");
    for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        Serial.printf("      [%d] %s\n", i, ei_classifier_inferencing_categories[i]);
    }
    
    Serial.printf("  - Confidence threshold: %.0f%%\n", EI_CONFIDENCE_THRESHOLD * 100);
    Serial.println("========================================");
}

// ============================================================================
// ======  CHAY INFERENCE  ====================================================
// ============================================================================

InferenceResult audio_processor_inference(float* audio_buffer, int buffer_size) {
    InferenceResult result;
    result.label_index = -1;
    result.confidence = 0.0f;
    result.label_name = "unknown";
    result.is_valid = false;
    result.inference_time_ms = 0;

    // Kiem tra kich thuoc buffer
    if (buffer_size != EI_CLASSIFIER_RAW_SAMPLE_COUNT) {
        Serial.printf("[EI] LOI: Buffer size (%d) != Raw sample count (%d)\n",
                      buffer_size, EI_CLASSIFIER_RAW_SAMPLE_COUNT);
        return result;
    }

    // Thiet lap buffer cho callback
    _inference_buffer = audio_buffer;
    _inference_buffer_size = buffer_size;

    // Tao signal tu buffer
    signal_t signal;
    signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
    signal.get_data = &audio_signal_get_data;

    // Cau truc ket qua Edge Impulse
    ei_impulse_result_t ei_result = { 0 };

    // Chay classifier
    unsigned long start_time = millis();
    EI_IMPULSE_ERROR ei_err = run_classifier(&signal, &ei_result, false);
    result.inference_time_ms = millis() - start_time;

    if (ei_err != EI_IMPULSE_OK) {
        Serial.printf("[EI] LOI khi chay classifier: %d\n", ei_err);
        return result;
    }

    // Tim label co confidence cao nhat
    float max_confidence = 0.0f;
    int max_index = -1;

    for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        float conf = ei_result.classification[i].value;
        
        #if DEBUG_INFERENCE
        Serial.printf("  [%d] %-15s: %.2f%%\n", 
                      i, 
                      ei_result.classification[i].label, 
                      conf * 100.0f);
        #endif

        if (conf > max_confidence) {
            max_confidence = conf;
            max_index = i;
        }
    }

    // Ghi ket qua
    result.label_index = max_index;
    result.confidence = max_confidence;
    
    if (max_index >= 0 && max_index < EI_CLASSIFIER_LABEL_COUNT) {
        result.label_name = ei_result.classification[max_index].label;
    }
    
    result.is_valid = (max_confidence >= EI_CONFIDENCE_THRESHOLD);

    #if DEBUG_INFERENCE
    Serial.printf("[EI] Ket qua: \"%s\" (%.1f%%) %s [%lu ms]\n",
                  result.label_name,
                  result.confidence * 100.0f,
                  result.is_valid ? "[OK] VALID" : "[X] thap",
                  result.inference_time_ms);
    #endif

    return result;
}

// ============================================================================
// ======  HELPER  =============================================================
// ============================================================================

int audio_processor_get_buffer_size() {
    return EI_CLASSIFIER_RAW_SAMPLE_COUNT;
}
