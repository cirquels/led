// These are our lovely output pins
#define outputPinLedFeedback 13
#define outputPinLedRed 3
#define outputPinLedGreen 5
#define outputPinLedBlue 6
#define outputPinLedWhite 9

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

// This function takes an array of RGBW arrays, and a dwell time, then cycles
void staticColourCycle(byte colours[][4], int dwellTime = 500) {

  Serial.println("SCC");

  int numColours = sizeof(colours);

  Serial.println(numColours);

  byte i = 0;
  for (i = 0; i < numColours; i++) {

    Serial.print("Colour Step: ");
    Serial.println(i);

    byte output[4] = { colours[i][0], colours[i][1], colours[i][2], colours[i][3] };
    setOutput(output);

    delay(dwellTime);
  }

}

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

        // Choose the colours to increment and decrement.
        for (byte decColour = 0; decColour < 3; decColour += 1) {
          byte incColour = decColour == 2 ? 0 : decColour + 1;

          // cross-fade the two colours.
          for(byte i = 0; i < 255; i += 1) {
            rgbColour[decColour] -= 1;
            rgbColour[incColour] += 1;

            byte output[4] = { rgbColour[0], rgbColour[1], rgbColour[2], 0 };

            setOutput(output);

            delay(cycleTime);
          }
        }
      }

      break;

    }

    case 2:

    {

      // Rule Britannia (or any othe nation with a red, white and blue flag)
      // Cycles red, white and blue with a delay on each colour.
      Serial.println("Rule Britannia (Program 2)");

      byte colours[][4] = {
        {*COLOUR_RED},
        { 0, 255, 0, 0 },
        { 0, 0, 255, 0 },
        { 0, 0, 0, 255 }
      };

      while(true) {

        staticColourCycle(colours);

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
