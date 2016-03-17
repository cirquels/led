// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

int redPin = 3;
int greenPin = 5;
int bluePin = 6;
int whitePin = 9;

int dipPins[] = {A0,A1,A2};

// the setup routine runs once when you press reset:
void setup() {

  Serial.begin(9600);
  
  // initialize the feedback digital pin as an output.
  // We don't need to initialise the PWM pins, analogWrite() does that for us
  pinMode(led, OUTPUT);
  // Initialise the inputs as inputs.
  int i;
  for(i = 0; i<=2; i++){
    pinMode(dipPins[i], INPUT);
   }
}

void setPinValue(int pin, int value) {
  analogWrite(pin, 255-value);
}

int programSelect(){
 int i,j=0;
 for(i=0; i<=2; i++){
   j = (j << 1) | digitalRead(dipPins[i]);
 }
 return (int) j;
}

// the loop routine runs over and over again forever:
void loop() {

Serial.println("PROGRAM SELECTION:");
Serial.println(programSelect());
  
  switch (programSelect()) {
    
    case 0:
      // RGB switch input mode
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
      
    case 1:
    
      // RGB fading
      Serial.println("RGB Cycle Mode (Program 1)");
      
      unsigned int rgbColour[3];

      // Start off with red.
      rgbColour[0] = 255;
      rgbColour[1] = 0;
      rgbColour[2] = 0;

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
            delay(5);
          }
        }
      }
      
      break;
      
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
