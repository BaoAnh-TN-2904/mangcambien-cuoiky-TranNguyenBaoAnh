/******************************************************************************
 * piezo_player.h
 * 
 * Module phat nhac qua Piezo Speaker su dung LEDC PWM
 * Ho tro phat melody non-blocking va beep phan hoi
 * 
 *****************************************************************************/

#ifndef PIEZO_PLAYER_H
#define PIEZO_PLAYER_H

#include <Arduino.h>

/**
 * Khoi tao Piezo Player
 * Cau hinh LEDC PWM channel cho GPIO18
 */
void piezo_init();

/**
 * Phat bai nhac theo index
 * Non-blocking -- can goi piezo_update() trong loop()
 * 
 * @param track_index Chi so bai nhac (0, 1, 2, ...)
 */
void piezo_play(int track_index);

/**
 * Dung phat nhac
 */
void piezo_stop();

/**
 * Chuyen sang bai ke tiep va bat dau phat
 */
void piezo_next_track();

/**
 * Cap nhat trang thai phat nhac (non-blocking)
 * PHAI goi ham nay trong loop() moi iteration
 */
void piezo_update();

/**
 * Phat beep ngan (blocking)
 * Dung cho feedback khi nhan lenh
 * 
 * @param frequency Tan so Hz
 * @param duration_ms Thoi gian phat (ms)
 */
void piezo_beep(int frequency, int duration_ms);

/**
 * Kiem tra piezo dang phat nhac khong
 * @return true neu dang phat melody
 */
bool piezo_is_playing();

/**
 * Lay index bai nhac hien tai
 * @return Index bai nhac (0-based)
 */
int piezo_get_current_track();

/**
 * Lay ten bai nhac hien tai
 * @return Chuoi ten bai
 */
const char* piezo_get_current_track_name();

#endif // PIEZO_PLAYER_H
