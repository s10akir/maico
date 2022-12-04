# cthugha

ESP32 に DHT22 を載せた温湿度センサ

## description

- HiLetgo ESP32 ESP-32 NodeMCU
- DHT22から取得した値を定期的にinfluxDBへPOSTする

| sensor | pin |
| ------ | --- |
| DHT22  | 32  |

## Issue

- [ ] 開発中、センサの値が NaN を返してくる事があった
  - Surface の USB から電力を取っていたので電力不足説（DHT22 はそこそこ電力を食う）
  - 値の読み取りの delay が足りていない説（DHT22 はそこそこリードが遅く、最大 2 秒程度かかるという噂）
  - しばらく様子を見る
- [ ] 常駐している間に WiFi トラブルがあったときの再接続処理の実装
  - loop()で定期的に WiFi の接続状況を見て、異常ならハードリセットとかが手軽そう
