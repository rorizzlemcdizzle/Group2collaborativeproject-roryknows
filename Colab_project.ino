#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

/*
   Owain's Arduino Project
*/


/*
   The list of ints used during the sketch.
*/
int potPin = 5; //1st potentiometer connected to analog pin 5
int value = 0; // variable to store the value read from the 1st potentiometer
int midiValue = 0;
int previousMidiValue = 0; // the previous reading from the midi value for the 1st potentiometer
int ledPin = 13; // the pin for the internal led
int buttonPin = 5; // 1st button connected to digital pin 56
int state = LOW; // This variable holds the 1st button state
int reading; // variable to store the read value from the 1st button
int previous = HIGH; // This varaiable is the prvious state of the 1st button

/*
   The list of longs used during the sketch.
*/
long time = 0; // the last time the output pin was toggled for the 1st button
long time2 = 0; // the last time the output pin was toggled for the 2nd button
long debounce = 200; // the debounce time, increase if the output flickers

/*
   INFORMATION:
   1st button: Toggles amp plugin with distortion
   2nd button: Toggles reverb and chorus. If the button is toggled on, reverb is off and chorus is on. If it's off, then reverb is on and chorus is off.
   1st potentiometer: If button 2 is toggled on, controls reverb wet/dry, else it controls frequency cutoff.
   2nd potentiometer: Controls reverb decay time
   3rd potentiometer: Controls frequency band 1
*/

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library. OMNI sets it to listen to all channels.
  pinMode (buttonPin, INPUT_PULLUP); // This sets the buttonPin as a PULLUP input
  pinMode (buttonPin2, INPUT_PULLUP); // This sets the buttonPin2 as a PULLUP input
  pinMode (ledPin, OUTPUT); // This sets the ledpin as an output
  Serial.begin(115200);
}

void loop() {
  //1st button
  reading = digitalRead(buttonPin); // read the input pin
  if (reading == HIGH && previous == LOW && millis() - time > debounce) { // if the button is toggled on, it was toggled off previously and at least 200ms has passed
    boolean buttonIsOn = state == HIGH; // simplified boolean used instead of clunky if / else statements
    state = buttonIsOn ? LOW : HIGH; // if button is on, state is low, else is high
    MIDI.sendControlChange(01, buttonIsOn ? 0 : 127, 2); // amp plugin and internal led toggled off/on depending on button state
    time = millis();
  }
  digitalWrite(ledPin, state); // internal led comes on when the button is toggled on, in the case of ableton, when the amp plugin is on
  previous = reading;

  // This is placeholder code for our project
  value = analogRead(potPin); // read the input pin
  midiValue = map(value, 0, 1023, 0, 127); // maps the potentiometer min/max values to midi mix/max values
  boolean incrementButtonToggled = buttonToggle = state == LOW; // if the increment button is toggled on, send midi value by increments.
  if (midiValue != previousMidiValue) { // if the midi value doesnt equal the previous midi value
    if (incrementButtonToggled){ // if the increment button is toggled on, send midi value by increments.
      calculateIncrement(midiValue);
    }
    MIDI.sendControlChange(01, midiValue, 1);
  }
  previousMidiValue = midiValue; // the previous midi value is now the midi value
  delay (20); // 20 millisecond delay
}



/*
 * Used to calculate the midi value at fixed points (increments). Only active if the increment button is toggled to on.
 */

void calculateIncrement(int midiValue) {
    if (midiValue >= 0 && midiValue <= 20) {
      midiValue = 10;
    }
    if (midiValue >= 20 && midiValue <= 40) {
      midiValue = 30;
    }
    if (midiValue >= 40 && midiValue <= 60) {
      midiValue = 50;
    }
    if (midiValue >= 60 && midiValue <= 80) {
      midiValue = 70;
    }
    if (midiValue >= 80 && midiValue <= 100) {
      midiValue = 90;
    }
    if (midiValue >= 100 && midiValue <= 127) {
      midiValue = 113;
    }
}

