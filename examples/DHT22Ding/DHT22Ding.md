# DHT22Ding

some text

| ESP8266  | DHT22 | Description   |
| -------- |:-----:| ------------- |
| 3.3v     | 1     | Power Supply  |
| D4       | 2     | Data |
| GND      | 4     | Ground |



  10K VCC - Data

```
  "dht": {
    "on": {
      "pin": 0,
      "type": "DHT22",
      "readtime": "30s",
      "resendtime": "2m",
      "onTemperature": "device/0?log=temp:$v,displaytext/temp?show=$v,remote/display-t?show=$v",
      "onHumidity": "device/0?log=hum:$v%,displaytext/hum?show=$v,remote/display-h?show=$v",
      "description": "Local DHT sensor"
    }
  },
```

More: 
Shut down DHT22 Power by output pin

