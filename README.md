# Wireless Swiss Army Knife
Flipper Zero inspired project using the LilyGo T-Display S3 board and some modules for RF, IR, and RFID/NFC capabilities.

## Hardware Requirements
- LilyGo T-Display-S3
- RC522
- nRF24L01
- Rotary Encoder with built-in Switch
- Pushbutton Switch
- IR Transmitter LED
- IR Receiver TSOP4838
- Slider Switch for the Battery
- 3V3 Battery of choice (I've got a 500mAh 802035)
- Enclosure of choice (Mine is a 42x82x29 height, width and depth respectively in mm)
- Optional external antenna for the ESP32, although it doesn't seem to improve the signal.

## Dependencies
- Platform: espressif32@6.6.0
- Framework: Arduino
- Libraries:
  - Wire
  - FS
  - TFT_eSPI (^2.5.43)
  - MFRC522 (^1.4.10)
  - LVGL (^8.3.9)

## Setup
1. Clone this repository
2. Open with PlatformIO
3. Build and upload to your T-Display-S3

## License
MIT License
