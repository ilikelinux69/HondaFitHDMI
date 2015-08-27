

// -------------------------------------------------------------
// can_loopback for Teensy 3.1
//
// MCP2515 interface sends frame out and internal CAN interface receives it.
// Received data is passed to the serial port.

#include <FlexCAN.h>
#include <teensy_mcp_can.h>
//#include <SPI.h>
#include <spi4teensy3.h>

// LED for receive indication.  Can't use onboard one because it shares the SPI SCLK pin
int led = 14;

//FlexCAN
FlexCAN CANIn(125000);
static CAN_message_t rxmsg;

//MCP2515 CAN
//Interrupt is D2, from schematic
//Default SPI Pins on Teensy:
// CS:        10
// DOUT/MOSI: 11
// DIN/MISO   12
// SCLK       13 (also the LED!)

//MCP library requires CS pin as argument.  Use default
const int csPin = 10;
MCP_CAN CANOut(csPin);
unsigned char buf[8] = {0, 1, 2, 3, 4, 5, 6, 7};

// -------------------------------------------------------------
static void printCanFrame(CAN_message_t f)
{
      Serial.print(millis());
      Serial.print("ms");
      Serial.print("\t0x");
      Serial.print(f.id, HEX);
      Serial.print("\t[");
      Serial.print(f.len);
      Serial.print("]\t");
      for (int i = 0; i < f.len; i++) 
      {
        Serial.print(f.buf[i], HEX);
        Serial.print("  ");
      }
      Serial.write('\r');
      Serial.write('\n');
}

// -------------------------------------------------------------
void setup(void)
{
  
  CANIn.begin();
  Serial.begin(115200);
  
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);

  delay(1000);
  
  // F() stores string in flash rather than RAM
  Serial.println(F("Teensy CAN Bus Loopback Test"));

  Serial.print(F("Initializing MCP2515 interface..."));

  // Attempt to initialize module
  while (CAN_OK != CANOut.begin(CAN_125KBPS))
  {
    Serial.print(".");
    delay(100);
  }
  
  Serial.println("success\r\n");

}


// -------------------------------------------------------------
void loop(void)
{
    while ( CANIn.read(rxmsg) ) {

      // Toggle our LED to show bus activity
      digitalWrite(led, 1);
      printCanFrame(rxmsg);
      
    }
    delay(10);
    digitalWrite(led, 0);

    delay(250);

    CANOut.sendMsgBuf(0x1ABC1234, 1, 8, buf);

    delay(250);

}
