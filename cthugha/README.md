# cthugha

## description

ESP32にDHT22を載せた温湿度センサ

* HiLetgo ESP32 ESP-32 NodeMCU
* `/` へのGETで現在の温度と湿度をJSONで返すWebAPIを提供する

|sensor|pin|
|-|-|
|DHT22|32|

### response type

```json
{
  "data": {
    "temperature": 0.0,
    "humidity": 0.0
  }
}
```

## Issue

- [ ] 開発中、センサの値がNaNを返してくる事があった
  - SurfaceのUSBから電力を取っていたので電力不足説（DHT22はそこそこ電力を食う）
  - 値の読み取りのdelayが足りていない説（DHT22はそこそこリードが遅く、最大2秒程度かかるという噂）
  - しばらく様子を見る
- [ ] 常駐している間にWiFiトラブルがあったときの再接続処理の実装
  - loop()で定期的にWiFiの接続状況を見て、異常ならハードリセットとかが手軽そう