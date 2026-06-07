/******************************************************************************
 * i2s_audio.h
 * 
 * Driver I2S cho microphone INMP441
 * Thu am thanh PCM 16-bit, mono, 16kHz qua giao tiep I2S
 * 
 *****************************************************************************/

#ifndef I2S_AUDIO_H
#define I2S_AUDIO_H

#include <Arduino.h>

/**
 * Khoi tao I2S peripheral cho INMP441 microphone
 * Cau hinh: RX mode, 16kHz, 16-bit, mono, DMA buffers
 * 
 * @return true neu khoi tao thanh cong
 */
bool i2s_audio_init();

/**
 * Doc audio samples tu INMP441 vao buffer
 * Blocking call -- doi cho den khi du samples hoac timeout
 * 
 * @param buffer     Con tro toi buffer int16_t de luu samples
 * @param num_samples So luong samples can doc
 * @return So samples thuc te doc duoc
 */
int i2s_audio_read(int16_t* buffer, int num_samples);

/**
 * Doc day buffer cho Edge Impulse inference
 * Buffer size phai bang EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE
 * 
 * @param buffer     Con tro toi buffer float de luu normalized samples
 * @param num_samples So luong samples can doc (= EI_CLASSIFIER_RAW_SAMPLE_COUNT)
 * @return true neu doc du samples
 */
bool i2s_audio_read_to_float_buffer(float* buffer, int num_samples);

/**
 * Dung I2S peripheral
 */
void i2s_audio_deinit();

#endif // I2S_AUDIO_H
