// new file!

#include <Wire.h>

// byte TxByte = 0x0123456789ABCDEF01234567; // 24 bytes long
byte TxByte[24] = {
  0x01, 0x23, 0x45, 0x67,
  0x89, 0xAB, 0xCD, 0xEF,
  0x01, 0x23, 0x45, 0x67,
  0x89, 0xAB, 0xCD, 0xEF,
  0x01, 0x23, 0x45, 0x67,
  0x89, 0xAB, 0xCD, 0xEF
};
#define address (0x52)

void I2C_TxHandler(void)
{
  Wire.write(TxByte, 24);
}

void setup()
{
  Wire.begin(address); // Init I2C (Slave Mode: addr = 0x52)
  Wire.onRequest(I2C_TxHandler);
}
// New!
void loop()
{
  // Nothing to be done here
  // Test: Generate multiple CAN messages
}