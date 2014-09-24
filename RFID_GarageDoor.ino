/*
 * Dump block 0 of a MIFARE RFID card using a RFID-RC522 reader
 * Uses MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT. 
 ----------------------------------------------------------------------------- 
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               52                MOSI
 * SPI MISO   12               51                MISO
 * SPI SCK    13               50                SCK
 *
 * Hardware required:
 * Arduino
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>

#define RELAY_PIN 9

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

// RFID Reader settings
#define SS_PIN 10    //Arduino Nano
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); //ensure the switch is OPEN
  
  Serial.begin(115200);        // Initialize serial communications with the PC
  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card

  Serial.println("Print block 0 of a MIFARE PICC ");
}

void loop() {

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())    return;

  Serial.print("Card UID:");    //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i]);
    Serial.print(" ");
    //          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //          Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 

  Serial.print(" PICC type: ");   // Dump PICC type
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte buffer[18];  
  byte block  = 0;
  byte status;
  //Serial.println("Authenticating using key A...");
 
 delay(1000);  // pause before authenticating, make sure you want to open it
 
 status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    
    return;
  } else {
    Serial.print("PCD_Authenticate() SUCCESS: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  
        digitalWrite(RELAY_PIN, LOW);
    delay(1000);
    digitalWrite(RELAY_PIN, HIGH);
  }

  // Read block
  byte byteCount = sizeof(buffer);
  status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  else  // Dump data
  for (byte index = 0; index < 16; index++) {
    Serial.print(buffer[index] < 0x10 ? " 0" : " ");
    Serial.print(buffer[index], HEX);
    if ((index % 4) == 3) Serial.print(" ");
  }
  Serial.println(" ");
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

}




