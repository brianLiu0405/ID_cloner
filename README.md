# ID_cloner

Board : ESP32-S3
using RC522 sensor clone the ic card UID to another new card

## RFID
- install library
    - LiquidCrystal
    - MFRC522
- setup sensor
    - I2C LCD display
    - rc522
    ``` cpp
      Serial.println(MOSI);
      Serial.print("MISO: ");
      Serial.println(MISO);
      Serial.print("SCK: ");
      Serial.println(SCK);
      Serial.print("SS: ");
      Serial.println(SS);  
  ```

