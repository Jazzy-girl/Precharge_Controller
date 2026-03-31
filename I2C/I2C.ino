// new file!

#include <Wire.h>

byte tXByte = 0x0123456789ABCDEF01234567;
#define address (0x52)

void I2C_TxHandler(void)
{
  Wire.write(TxByte);
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