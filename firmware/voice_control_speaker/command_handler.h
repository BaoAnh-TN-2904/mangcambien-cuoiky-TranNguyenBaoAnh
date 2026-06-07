/******************************************************************************
 * command_handler.h
 * 
 * State Machine dieu khien he thong Voice Control Speaker
 * 
 * Trang thai:
 *   IDLE     -> Chi lang nghe wake word "bao anh"
 *   LISTENING -> Da kich hoat, cho lenh (timeout 5s)
 *   PLAYING  -> Dang phat nhac, van nhan lenh
 *   PAUSED   -> Tam dung (timeout 10s -> IDLE)
 * 
 *****************************************************************************/

#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include "audio_processor.h"

// Enum trang thai he thong
typedef enum {
    STATE_IDLE,       // Nghi -- chi nghe wake word
    STATE_LISTENING,  // Da kich hoat -- cho lenh
    STATE_PLAYING,    // Dang phat nhac
    STATE_PAUSED      // Tam dung nhac
} SystemState;

/**
 * Khoi tao Command Handler
 * Set trang thai ban dau = IDLE
 */
void command_handler_init();

/**
 * Xu ly ket qua inference va chuyen trang thai
 * Goi ham nay moi khi co ket qua inference moi
 * 
 * @param result Ket qua tu audio_processor_inference()
 */
void command_handler_process(InferenceResult result);

/**
 * Cap nhat state machine (kiem tra timeout)
 * PHAI goi trong loop() moi iteration
 */
void command_handler_update();

/**
 * Lay trang thai hien tai
 * @return SystemState hien tai
 */
SystemState command_handler_get_state();

/**
 * Lay ten trang thai dang chuoi (cho debug)
 * @return Chuoi ten trang thai
 */
const char* command_handler_get_state_name();

#endif // COMMAND_HANDLER_H
