/*
       --------------------------------------------------------------------------------------------------------------------
       Example to change UID of changeable MIFARE card.
       --------------------------------------------------------------------------------------------------------------------
       This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
       This sample shows how to set the UID on a UID changeable MIFARE card.
       @author Tom Clement
       @license Released into the public domain.
       Typical pin layout used:
       -----------------------------------------------------------------------------------------
                   MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
                   Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
       Signal      Pin          Pin           Pin       Pin        Pin              Pin
       -----------------------------------------------------------------------------------------
       RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
       SPI SS      SDA(SS)      10            53        D10        10               10
       SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
       SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
       SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
    */
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define SS_PIN 5
#define RST_PIN 0

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

MFRC522::MIFARE_Key key;

MFRC522::MIFARE_Key F_key;

// But of course this is a more proper approach
byte blockAddr      = 0;  //第0块
byte trailerBlock   = 3;
byte buffer[18];
byte test[18];
byte check_buffer[18];
byte nuidPICC[4];
byte test_size = sizeof(test);
byte size = sizeof(buffer);
byte check_size = sizeof(check_buffer);
MFRC522::StatusCode status;
int mode;

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  Wire.begin (41, 42);
  lcd.init();
  lcd.backlight();
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
    F_key.keyByte[i] = 0xFF;
  }
  Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
  Serial.print(F("Using key (for A and B):"));
  dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
  Serial.println();
  Serial.println(F("BEWARE: Data will be written to the PICC, in sector #0"));
  Serial.println();
  mode = 0;
}

void read(){
  Serial.println("read scan");
  // status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &F_key, &(mfrc522.uid));
  // if (status != MFRC522::STATUS_OK) {
  //   Serial.print(F("PCD_Authenticate() failed: "));
  //   Serial.println(mfrc522.GetStatusCodeName(status));
  //   return;
  // }
  // status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, test, &test_size);
  // if (status != MFRC522::STATUS_OK) {
  //   Serial.print(F("MIFARE_Read() failed: "));
  //   Serial.println(mfrc522.GetStatusCodeName(status));
  // }
  // dump_byte_array(test, (byte)16);
  // Serial.println();
  // mfrc522.PCD_StopCrypto1();
  
  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));  

  digitalWrite(7, HIGH);
  digitalWrite(6, LOW);

  char data[16] = "";
  data[0] = '0';
  data[1] = 'x';
  data[2] = ' ';
  data[3] = ' ';
  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = mfrc522.uid.uidByte[i];
    int num = (int)nuidPICC[i];
    int low = num & 0xF;
    int high = (num >> 4) & 0xF;
    data[4+i*3] = high >= 10 ? high + 55 : high + 48;;
    data[4+i*3+1] = low >= 10 ? low + 55 : low + 48;
    data[4+i*3+2] = ' ';
  }
  lcd.setCursor(0,0);
  lcd.print("UID : ");
  lcd.setCursor(0,1);
  lcd.print(data);

  Serial.println("please remove the card...");
  delay(2000);
  mode = 1;
}

void write(){
  Serial.println("write scan");

  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  dump_byte_array(buffer, (byte)16);
  Serial.println();
  buffer[0] = nuidPICC[0];
  buffer[1] = nuidPICC[1];
  buffer[2] = nuidPICC[2];
  buffer[3] = nuidPICC[3];
  buffer[4] = nuidPICC[0] ^ nuidPICC[1] ^ nuidPICC[2] ^ nuidPICC[3];
  dump_byte_array(buffer, (byte)16);
  Serial.println();
  Serial.println((int)buffer[4]);

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, check_buffer, &check_size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  dump_byte_array(check_buffer, (byte)16);
  Serial.println();
  Serial.println(F("Checking result..."));
  byte count = 0;
  for (byte i = 0; i < 4; i++) {
    // Compare buffer (= what we've read) with dataBlock (= what we've written)
    if (buffer[i] == check_buffer[i])
      count++;
  }
  // Serial.print(F("Number of bytes that match = ")); Serial.println(count);
  if (count == 4) {
    Serial.println(F("Success :-)"));
  } 
  else {
    Serial.println(F("Failure, no match :-("));
    Serial.println(F("  perhaps the write didn't work properly..."));
  }
  Serial.println("please remove the card...");
  digitalWrite(7, LOW);
  digitalWrite(6, HIGH);
  mfrc522.PCD_StopCrypto1();
  delay(2000);
  digitalWrite(6, LOW);
  mode = 0;
}

void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  if(mode) {
    write();
  }
  else read();
  
}
/**
    Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}