# Voice Control Mini Speaker — ESP32-S3 Firmware

## 📋 Tổng quan

Firmware điều khiển loa nhạc mini bằng giọng nói tiếng Việt, chạy trên ESP32-S3 với mô hình AI Edge Impulse (MFCC + Dense Neural Network).

### Chức năng

| Lệnh thoại | Chức năng |
|---|---|
| **"bảo anh"** | Wake word — kích hoạt hệ thống |
| **"phát nhạc"** | Phát nhạc qua Piezo Speaker |
| **"dừng nhạc"** | Dừng/tạm dừng nhạc |
| **"bài kế"** | Chuyển sang bài kế tiếp |
| *(tiếng ồn)* | Bỏ qua — tránh điều khiển sai |

### Phần cứng

- ESP32-S3 DevKitC
- INMP441 I2S MEMS Microphone
- Piezo Buzzer/Speaker

---

## 🔌 Sơ đồ kết nối

```
ESP32-S3          INMP441           Piezo Speaker
─────────         ───────           ─────────────
3V3  ──────────── VDD
GND  ──────────── GND
GND  ──────────── L/R
GPIO4 ─────────── WS (LRCK)
GPIO5 ─────────── SCK (BCLK)
GPIO6 ─────────── SD (DOUT)
GND  ──────────────────────────────  PIN1 (-)
GPIO18 ────────────────────────────  PIN2 (+)
```

---

## 📱 Cách sử dụng

1. **Bật nguồn** → Nghe 3 tiếng beep tăng dần = sẵn sàng
2. **Nói "bảo anh"** → Nghe 1 tiếng beep cao = đã kích hoạt
3. **Nói "phát nhạc"** → Nghe beep xác nhận + nhạc phát
4. **Nói "bảo anh"** lại → tạm dừng + chờ lệnh mới
5. **Nói "dừng nhạc"** → Dừng phát
6. **Nói "bài kế"** → Chuyển sang bài tiếp

### Tiếng beep phản hồi

| Beep | Ý nghĩa |
|---|---|
| 1 beep cao (1500Hz) | Wake word nhận được |
| 1 beep rất cao (2000Hz) | Lệnh xác nhận thành công |
| 1 beep thấp (500Hz) | Timeout / lỗi |
| 3 beep tăng dần | Khởi động xong |

---

## 🎵 Danh sách bài nhạc

Firmware có sẵn 3 bài nhạc phát qua Piezo:

1. **Twinkle Twinkle Little Star**
2. **Happy Birthday**
3. **Jingle Bells**

Bài nhạc tự động lặp lại cho đến khi nói "dừng nhạc".

### Thêm bài nhạc mới

---

## 🔧 Debug / Troubleshooting

### Serial Monitor output mẫu

```
╔══════════════════════════════════════════╗
║   VOICE CONTROL MINI SPEAKER - ESP32-S3  ║
║   Edge Impulse + INMP441 + Piezo         ║
╚══════════════════════════════════════════╝

[BOOT] Khởi tạo Piezo Speaker...
[PIEZO] Khởi tạo thành công
[BOOT] Piezo OK ✓

[BOOT] Khởi tạo I2S Microphone (INMP441)...
[I2S] Khởi tạo thành công
[I2S] Sample Rate: 16000 Hz, Bits: 32 (→16), Mono
[BOOT] I2S OK ✓

[EI] Thông tin model:
  - Raw sample count : 16000
  - Frequency        : 16000 Hz
  - Label count      : 5
  - Labels:
      [0] bao_anh
      [1] phat_nhac
      [2] dung_nhac
      [3] bai_ke
      [4] noise

╔══════════════════════════════════════════╗
║          HỆ THỐNG SẴN SÀNG!             ║
╚══════════════════════════════════════════╝

[CMD] Đang chờ wake word "bảo anh"...
[STATUS] State: IDLE | Piezo: STOPPED | Track: 0 (Twinkle Twinkle) | Heap: 245120
```

### Các lỗi thường gặp

| Lỗi | Nguyên nhân | Cách sửa |
|---|---|---|
| `#include not found` | Chưa cài Edge Impulse library | Xem bước 4 |
| `I2S init failed` | Sai pin hoặc conflict | Kiểm tra kết nối dây |
| `Confidence luôn thấp` | Model chưa train tốt | Thu thêm data, retrain |
| `Không nhận wake word` | Micro xa hoặc ồn | Nói gần mic, giảm noise |
| `Heap thấp` | Model quá lớn | Dùng partition "Huge APP" |

### Điều chỉnh ngưỡng

Nếu hệ thống nhận sai lệnh, tăng threshold trong `config.h`:

```cpp
#define EI_CONFIDENCE_THRESHOLD   0.80f  // Tăng lên 80%
```

Nếu hệ thống khó nhận lệnh, giảm threshold:

```cpp
#define EI_CONFIDENCE_THRESHOLD   0.60f  // Giảm xuống 60%
```

---

## 📁 Cấu trúc file

```
voice_control_speaker/
├── voice_control_speaker.ino  ← Main sketch (setup + loop)
├── config.h                    ← Cấu hình pin, threshold, ← SỬA #include TẠI ĐÂY
├── i2s_audio.h / .cpp         ← Driver INMP441 microphone
├── audio_processor.h / .cpp   ← Edge Impulse inference wrapper
├── command_handler.h / .cpp   ← State machine (IDLE/LISTENING/PLAYING/PAUSED)
├── piezo_player.h / .cpp      ← Phát nhạc PWM non-blocking
└── melodies.h                  ← Nốt nhạc + bài nhạc mẫu
```

---


