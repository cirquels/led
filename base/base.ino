#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

#define SERVER_ADDRESS 0x00

// These are nice preset colours

// Black
byte COLOUR_BLACK[] = { 0, 0, 0, 0 };

// The primaries
byte COLOUR_RED[] = { 255, 0, 0, 0 };
byte COLOUR_GREEN[] = { 0, 255, 0, 0 };
byte COLOUR_BLUE[] = { 0, 0, 255, 0 };

// The mixed primaries
byte COLOUR_YELLOW[] = { 255, 255, 0, 0 };
byte COLOUR_MAGENTA[] = { 255, 0, 255, 0 };
byte COLOUR_CYAN[] = { 0, 255, 255, 0 };

// The flavours of white
byte COLOUR_WHITE_MIX[] = { 255, 255, 255, 0 };
byte COLOUR_WHITE_TRUE[] = { 0, 0, 0, 255 };

// Radio control!
RH_ASK driver(6000);
RHDatagram manager(driver, SERVER_ADDRESS);

// This function sends a command sequence
void goToColour(int address, byte colour[4], int dwellTime = 250, int fadeTime = 0) {

  uint8_t data[6];

  data[0] = colour[0];
  data[1] = colour[1];
  data[2] = colour[2];
  data[3] = colour[3];
  data[4] = highByte(fadeTime);
  data[5] = lowByte(fadeTime);
  manager.sendto(data, sizeof(data), address);

  // Wait about for the given dwell time.
  // Adds the fadetime because the remotes will be fading...
  delay(dwellTime + fadeTime);

}

void setup() 
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");
}

void loop()
{

  goToColour(0xff, COLOUR_RED, 500, 2000);
  goToColour(0xff, COLOUR_BLUE, 1000, 500);
  
}
