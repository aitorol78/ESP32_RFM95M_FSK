

#define RXD1 25 // Serial port connected to LPC1768
#define TXD1 32

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);   
  
  Serial.begin(115200);
  while (!Serial);

  Serial.println("test_ESP32_RFM95W_TXoutput");
}

void loop() {

  Serial1.write(0x55);
  Serial.println("0x55 sent");
  
  delay(10);
  
}
