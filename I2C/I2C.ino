// new file!

#include <Wire.h>

byte tXByte = 0xAA;

void I2C_TxHandler(void){
  Wire.write(TxByte);
}

void setup() {
  Wire.begin(0x52); // Init I2C (Slave Mode: addr = 0x52)
  Wire.onRequest(I2C_TxHandler);
}
// New!
void loop() {
  // Nothing to be done here
  // Test: Generate multiple CAN messages
}