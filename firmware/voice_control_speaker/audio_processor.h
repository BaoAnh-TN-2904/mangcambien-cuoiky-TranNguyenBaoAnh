/******************************************************************************
 * audio_processor.h
 * 
 * Wrapper cho Edge Impulse inference engine
 * Xu ly audio buffer -> MFCC features -> Dense NN classification
 * 
 *****************************************************************************/

#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H

#include <Arduino.h>

// Ket qua inference
typedef struct {
    int label_index;        // Chi so label co confidence cao nhat
    float confidence;       // Gia tri confidence (0.0 - 1.0)
    const char* label_name; // Ten label (chuoi)
    bool is_valid;          // true neu confidence >= threshold
    unsigned long inference_time_ms;  // Thoi gian inference (ms)
} InferenceResult;

/**
 * Khoi tao audio processor
 * In thong tin model Edge Impulse ra Serial
 */
void audio_processor_init();

/**
 * Chay inference tren audio buffer
 * 
 * @param audio_buffer  Buffer chua audio samples (float, normalized)
 * @param buffer_size   Kich thuoc buffer (phai = EI_CLASSIFIER_RAW_SAMPLE_COUNT)
 * @return InferenceResult chua label, confidence, va thong tin inference
 */
InferenceResult audio_processor_inference(float* audio_buffer, int buffer_size);

/**
 * Lay kich thuoc buffer can thiet cho inference
 * = EI_CLASSIFIER_RAW_SAMPLE_COUNT
 * 
 * @return So luong float samples can cho 1 lan inference
 */
int audio_processor_get_buffer_size();

/**
 * In thong tin model Edge Impulse
 */
void audio_processor_print_model_info();

#endif // AUDIO_PROCESSOR_H
