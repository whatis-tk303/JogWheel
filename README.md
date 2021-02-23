# JogWheel

## これは何？

お絵かきソフト、3D CAD等の操作を簡単にするための、いわゆる「左手デバイス」を作ります。

https://twitter.com/i/status/1363391566933413889

* ESP32を Arduinoとして開発
* BLEキーボードに仕立てる
* ロータリーエンコーダーで回転操作を入力できるようにする


## 依存しているライブラリ

BleKeyboard ( https://github.com/T-vK/ESP32-BLE-Keyboard )


## ハードウェア

* マイコン
  * ESP32： ESP32 Core Board V2 （秋月で売ってる ESP32-DevKit C など、互換ボードがいろいろあり）
* 電源
  * ESP32ボードの USBコネクタから入力（+5V）
* ロータリーエンコーダー
  * A相(P32)、B相(P33)、GND
  * P32 と P33 は Arduino側で INPUT_PULLUP 指定をする。
