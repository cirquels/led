#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

// This is this device's address
#define CLIENT_ADDRESS 0x01

// These are our lovely output pins
#define outputPinLedFeedback 13
#define outputPinLedRed 3
#define outputPinLedGreen 5
#define outputPinLedBlue 6
#define outputPinLedWhite 9

// These pins are the DIP inputs
const byte inputPinsDips[] = {A0,A1,A2};

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

// Variables to hold the current colour, so fades can be calculated
byte currentColourRed;
byte currentColourGreen;
byte currentColourBlue;
byte currentColourWhite;

// Set Output
void setOutput(byte colour[4]) {

  analogWrite(outputPinLedRed, 255 - colour[0]);
  analogWrite(outputPinLedGreen, 255 - colour[1]);
  analogWrite(outputPinLedBlue, 255 - colour[2]);
  analogWrite(outputPinLedWhite, 255 - colour[3]);

}

// Read DIP Switch Values
byte DIPReadValue(){

  byte i,j=0;
  for(i=0; i<=2; i++){
    j = (j << 1) | digitalRead(inputPinsDips[i]);
  }

  return (byte) j;

}

// This function fades to a specific colour from whatever the current state is.
void goToColour(byte colour[4], int dwellTime = 250, int fadeTime = 0) {

  // Somewhere to put the output
  byte output[4];

  if (fadeTime > 0) {

    // We actually have a fade time to consider. Engage maths!

    byte fadeSteps;

    // Decide how many steps we should use
    if (fadeTime > 255) {
      fadeSteps = 255;
    } else {
      fadeSteps = fadeTime;
    }

    // Shift the fadeTime to account for the step time
    int stepTime = fadeTime / fadeSteps;

    // What's the actual delta per step for each colour?
    int changeRedStep = (colour[0] - currentColourRed) / fadeSteps;
    int changeGreenStep = (colour[1] - currentColourGreen) / fadeSteps;
    int changeBlueStep = (colour[2] - currentColourBlue) / fadeSteps;
    int changeWhiteStep = (colour[3] - currentColourWhite) / fadeSteps;

    // Somewhere to put the output colours.
    int colourOutputRed;
    int colourOutputGreen;
    int colourOutputBlue;
    int colourOutputWhite;

    // Do the magic!
    int i;
    for(i=0; i<=fadeSteps; i++){

      colourOutputRed = currentColourRed + ( changeRedStep * i );
      colourOutputGreen = currentColourGreen + ( changeGreenStep * i );
      colourOutputBlue = currentColourBlue + ( changeBlueStep * i );
      colourOutputWhite = currentColourWhite + ( changeWhiteStep * i );

      // Sanity!
      if (colourOutputRed > 255) colourOutputRed = 255;
      if (colourOutputGreen > 255) colourOutputGreen = 255;
      if (colourOutputBlue > 255) colourOutputBlue = 255;
      if (colourOutputWhite > 255) colourOutputWhite = 255;
      if (colourOutputRed < 0) colourOutputRed = 0;
      if (colourOutputGreen < 0) colourOutputGreen = 0;
      if (colourOutputBlue < 0) colourOutputBlue = 0;
      if (colourOutputWhite < 0) colourOutputWhite = 0;

      // Set the output.
      output[0] = (byte) colourOutputRed;
      output[1] = (byte) colourOutputGreen;
      output[2] = (byte) colourOutputBlue;
      output[3] = (byte) colourOutputWhite;
      setOutput(output);

      // Wait for the necessary delay.
      delay(stepTime);
    }

  }

  // Set the output
  // This either snaps if no fade, or finalises if there is a fade.
  setOutput(colour);

  currentColourRed = colour[0];
  currentColourGreen = colour[1];
  currentColourBlue = colour[2];
  currentColourWhite = colour[3];

  // Wait about for the given dwell time.
  delay(dwellTime);

}

// Radio control!
RH_ASK driver(6000);
RHDatagram manager(driver, CLIENT_ADDRESS);

uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

// Set up the whole shebang.
void setup() {

  Serial.begin(9600);

  // initialize the feedback pin as an output.
  // We don't need to initialise the PWM pins, analogWrite() does that for us
  pinMode(outputPinLedFeedback, OUTPUT);

  // Initialise the inputs as inputs.
  byte i;
  for(i = 0; i<=2; i++){
    pinMode(inputPinsDips[i], INPUT);
  }

  // Set all the outputs to off
  setOutput(COLOUR_BLACK);

}

// Enter the loop!
void loop() {

  byte programId = DIPReadValue();

  Serial.print("PROGRAM SELECTION: ");
  Serial.println(programId);

  // This switch statement performs the program selection
  switch (programId) {

    case 0:

    {

      // Direct Colour Select Mode 1
      // Emulates the behaviour of the classic RGB switches.
      Serial.println("RGB Switch Mode (Program 0)");

      while(true) {

        byte colourSelection = DIPReadValue();

        switch (colourSelection) {

          case 1:
            // 001 - Blue
            setOutput(COLOUR_BLUE);
            break;

          case 2:
            // 010 - Green
            setOutput(COLOUR_GREEN);
            break;

          case 3:
            // 011 - Cyan
            setOutput(COLOUR_CYAN);
            break;

          case 4:
            // 100 - Red
            setOutput(COLOUR_RED);
            break;

          case 5:
            // 101 - Magenta
            setOutput(COLOUR_MAGENTA);
            break;

          case 6:
            // 110 - Yellow
            setOutput(COLOUR_YELLOW);
            break;

          case 7:
            // 111 - White
            setOutput(COLOUR_WHITE_TRUE);
            break;

          default:
            // 000 - Black
            setOutput(COLOUR_BLACK);
            break;

        }

      }

      break;

    }

    case 1:

    {

      // RGB Fading
      // Cycles smoothly between the red, green and blue LEDs.
      // Use DIPs to set the cycle speed
      Serial.println("RGB Cycle Mode (Program 1)");

      byte rgbColour[3];

      // Start off with red.
      rgbColour[0] = 255;
      rgbColour[1] = 0;
      rgbColour[2] = 0;

      // How long should we wait between shifting to the next step (milliseconds).
      byte cycleBaseTime = 5;
      byte cycleMultiplier = 0;
      byte cycleTime = cycleBaseTime * cycleMultiplier;

      while(true) {

        cycleMultiplier = DIPReadValue();

        if (cycleMultiplier == 0) {
          cycleTime = cycleBaseTime;
        } else {
          cycleTime = cycleBaseTime * cycleMultiplier;
        }

        goToColour(COLOUR_RED,cycleTime);
        goToColour(COLOUR_GREEN,cycleTime);
        goToColour(COLOUR_BLUE,cycleTime);

      }

      break;

    }

    case 2:

    {

      // Rule Britannia (or any other nation with a red, white and blue flag)
      // Cycles red, white and blue with a delay on each colour.
      Serial.println("Rule Britannia (Program 2)");

      while(true) {

        goToColour(COLOUR_RED, 1200, 200);
        goToColour(COLOUR_WHITE_TRUE, 1200, 200);
        goToColour(COLOUR_BLUE, 1200, 200);

      }

      break;

    }

    case 7:

    {

      // Remote Raw Stream
      // Listens for raw colour commands from the remote, and blindly obeys
      Serial.println("Remote - Raw Stream (Program 7)");

      // Start listening for things
      if (!manager.init())
        Serial.println("Error with radio init!");

      while(true) {

        if (manager.available())
        {
          // Wait for a message addressed to us from the client
          uint8_t len = sizeof(buf);
          uint8_t from;
          if (manager.recvfrom(buf, &len, &from))
          {
            byte colour[4] = { (byte)buf[0], (byte)buf[1], (byte)buf[2], (byte)buf[3] };
            int fadeTime = (byte)buf[4] * 256 + (byte)buf[5];
            goToColour(colour, 0, fadeTime);
          }
        }

      }

      break;

    }

    default:

      // No available program for this ID
      // Flash the feedback LED
      Serial.println("No program available!");

      while(true) {
        digitalWrite(outputPinLedFeedback, HIGH);
        delay(1000);
        digitalWrite(outputPinLedFeedback, LOW);
        delay(1000);
      }
      break;

  }

}
