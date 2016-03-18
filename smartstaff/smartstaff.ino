// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

int redPin = 3;
int greenPin = 5;
int bluePin = 6;
int whitePin = 9;

int dipPins[] = {A0,A1,A2};

// Let's set this show up!
void setup() {

  Serial.begin(9600);
  
  // initialize the feedback pin as an output.
  // We don't need to initialise the PWM pins, analogWrite() does that for us
  pinMode(led, OUTPUT);
  
  // Initialise the inputs as inputs.
  int i;
  for(i = 0; i<=2; i++){
    pinMode(dipPins[i], INPUT);
  }

  // Set all the outputs to off
  setPinValue(redPin, 0);
  setPinValue(greenPin, 0);
  setPinValue(bluePin, 0);
  setPinValue(whitePin, 0);
}

// This sets pin values via the necessary maths to account for the strip being a common anode
void setPinValue(int pin, int value) {
  analogWrite(pin, 255-value);
}

// This handy function reads the program select code from the DIP switches
int programSelect(){
  int i,j=0;
  for(i=0; i<=2; i++){
    j = (j << 1) | digitalRead(dipPins[i]);
  }
  return (int) j;
}

// Enter the loop!
void loop() {

  Serial.println("PROGRAM SELECTION:");
  Serial.println(programSelect());

  // This switch statement performs the program selection
  switch (programSelect()) {
    
    case 0:
      {
    
      // RGB switch input mode
      // Reads DIPs 2, 3 and 4 corresponding to red, green and blue. Switches to white LEDs when all are high.
      Serial.println("RGB Switch Mode (Program 0)");
      
      while(true) {
        
        if (digitalRead(dipPins[0]) == HIGH && digitalRead(dipPins[1]) == HIGH && digitalRead(dipPins[2]) == HIGH) {
          setPinValue(redPin, 0);
          setPinValue(greenPin, 0);
          setPinValue(bluePin, 0);
          setPinValue(whitePin, 255);
        } else {
          setPinValue(whitePin, 0);
          if (digitalRead(dipPins[0]) == HIGH) {
            setPinValue(redPin, 255);
          } else {
            setPinValue(redPin, 0);
          }
          if (digitalRead(dipPins[1]) == HIGH) {
            setPinValue(greenPin, 255);
          } else {
            setPinValue(greenPin, 0);
          }
          if (digitalRead(dipPins[2]) == HIGH) {
            setPinValue(bluePin, 255);
          } else {
            setPinValue(bluePin, 0);
          }
        }
        
      }
       
      break;

      }
      
    case 1:

      {
    
      // RGB Fading
      // Cycles smoothly between the red, green and blue LEDs.
      Serial.println("RGB Cycle Mode (Program 1)");
      
      unsigned int rgbColour[3];

      // Start off with red.
      rgbColour[0] = 255;
      rgbColour[1] = 0;
      rgbColour[2] = 0;

      // How long should we wait between shifting to the next step (milliseconds).
      int cycleTime = 5;

      while(true) {
    
        // Choose the colours to increment and decrement.
        for (int decColour = 0; decColour < 3; decColour += 1) {
          int incColour = decColour == 2 ? 0 : decColour + 1;
      
          // cross-fade the two colours.
          for(int i = 0; i < 255; i += 1) {
            rgbColour[decColour] -= 1;
            rgbColour[incColour] += 1;
            
            setPinValue(redPin, rgbColour[0]);
            setPinValue(greenPin, rgbColour[1]);
            setPinValue(bluePin, rgbColour[2]);
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

      int dwellTime = 600;
      
      while(true) {
        
        setPinValue(redPin, 255);
        setPinValue(bluePin, 0);
        delay(dwellTime);
        setPinValue(whitePin, 255);
        setPinValue(redPin, 0);
        delay(dwellTime);
        setPinValue(bluePin, 255);
        setPinValue(whitePin, 0);
        delay(dwellTime);
        
      }
       
      break;

      }
      
    default: 
    
      // No available program for this ID
      // Flash the feedback LED
      Serial.println("No program available!");
      
      while(true) {
        digitalWrite(led, HIGH);
        delay(1000);
        digitalWrite(led, LOW);
        delay(1000);
      }
    break;
  }
}
