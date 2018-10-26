#include <SR04.h>

#include <MIDI.h>

#define TRIG_PIN 1
#define ECHO_PIN 3
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long distance;

boolean testScale = false;
boolean majorScale = true;
boolean minorScale = false;
boolean controlParameter = false;
int midiValue = 0;
int previousMidiValue = 0;
int instrumentType = 4;
int velocity = 0;
int buttonSelected = 0;

int buttonPin = 5;        // 1st button connected to digital pin 56
int state = LOW;          // This variable holds the 1st button state
int reading;              // variable to store the read value from the 1st button
int previous = HIGH;      // This varaiable is the prvious state of the 1st button
long time = 0;            // the last time the output pin was toggled for the 1st button
long debounce = 200;      // 200ms delay

MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);      // Launch MIDI and listen to all channels
  pinMode (buttonPin, INPUT_PULLUP);  // This sets the buttonPin as a PULLUP input
  Serial.begin(115200);               // Open serial monitor at 115200 baud to see ping results.
}

/*
 * The main loop
 */

void loop() {

  // The distance
  distance = sr04.Distance();

  // Maps the distance of 0 - 127 (cm?) into Midi data 0 - 127. This means that we have 1 semitone per cm in theory.
  midiValue = map(distance, 0, 127, 0, 127);

  //handleTestButton();

  // If the distance is higher or equal to 127, cap it at 127.
  if (midiValue >= 127) {
    midiValue = 127;
  }

  // If nothing is blocking the sensor, stop sending a note.
  if (midiValue == 127) {
    MIDI.sendNoteOff(midiValue, 0, 1);
    return;
  }

  // If the boolean to control a parameter is true
  if (controlParameter) {
    handleControlParameter(midiValue);
  }

  // If the notes are to be played in increments
  if (testScale) {
    calculateTestScale(midiValue);
  }

  // If the notes are to be played in a major scale
  else if (majorScale) {
    calculateMajorScale(midiValue);
  }

  // If the notes are to be played in a minor scale
  else if (minorScale) {
    calculateMinorScale(midiValue);
  }
  else {
    sendNote(midiValue);
  }
 }

/*
 * Sends a control change based on the midiValue (currently not working)
 */

void handleControlParameter(int midiValue) {
  //if (midiValue != previousMidiValue) { // if the midi value doesnt equal the previous midi value
    digitalWrite(13, HIGH);
    MIDI.sendControlChange(01, midiValue, 1);
    //digitalWrite(13, LOW);
  //}
  //previousMidiValue = midiValue; // the previous midi value is now the midi value
  delay (20); // 20 millisecond delay
}

/* Instrument type will be selected via a button on the cube, will need to create a method that changes the instrument (sends a control function to Ableton to change channel)
   based upon which button is toggled.
*/

void selectInstrument(int buttonSelected) {
  switch(buttonSelected) {
    case 1: // Pad
           instrumentType = 1;
           break;
    case 2: // Grand piano
          instrumentType = 2;
          break;
    case 3: // Strings
          instrumentType = 3;
          break;
     case 4: // French horn
          instrumentType = 4;
          break;
    }
}

/*
 * This grabs the velocity integer based on the instrument that is selected.
 */

static int getVelocity(int instrumentType) {
  switch(instrumentType) {
    case 1: // Pad
           return 80;
    case 2: // Grand piano
          return 64;
    case 3: // Strings
          return 70;
    case 4: // French horn
          return 45;
    }
}

/*
 * Used to calculate the midi value at fixed points (increments). Only active if the increment button is toggled to on.
 */

void calculateTestScale(int midiValue) {
  int maximum = 50; // The maximum value

  // If the value is greater than the maximum value, stop sending a note.
  if (midiValue > maximum) {
    MIDI.sendNoteOff(midiValue, 0, 1);
    return;
  }
    sendNote(
    midiValue <= 10 ? 60 // c3
  : midiValue <= 20 ? 64 // e3
  : midiValue <= 30 ? 67 // g3
  : midiValue <= 40 ? 71 // b3
  :                   74 // d4
  );
}

/*
 * Used to calculate a c major scale. Only active if the boolean 'majorScale' is true.
 */

void calculateMajorScale(int midiValue) {
  int maximum = 50; // The maximum value

  // If the value is greater than the maximum value, stop sending a note.
  if (midiValue > maximum) {
    MIDI.sendNoteOff(midiValue, 0, 1);
    return;
  }
  sendNote(
    midiValue <= 10 ? 60 // c3
  : midiValue <= 20 ? 64 // e3
  : midiValue <= 30 ? 67 // g3
  : midiValue <= 40 ? 72 // a4
  :                   76 // b4
  );
}

/*
 * Used to calculate a c minor scale. Only active if the boolean 'minorScale' is true.
 */

void calculateMinorScale(int midiValue) {
  int maximum = 50; // The maximum value

  // If the value is greater than the maximum value, stop sending a note.
  if (midiValue > maximum) {
    MIDI.sendNoteOff(midiValue, 0, 1);
    return;
  }
    sendNote(
    midiValue <= 10 ? 60 // c3
  : midiValue <= 20 ? 63 // d#3
  : midiValue <= 30 ? 67 // g3
  : midiValue <= 40 ? 72 // c4
  :                   75 // d#4
  );
}

/*
 * Sends the midi note data. Having a method dedicated to this saves repeated code and helps on memory usage.
 */

void sendNote(int midiValue) {
  digitalWrite(13, HIGH);
  MIDI.sendNoteOn(midiValue, getVelocity(instrumentType), 1);   // Send a note
  delay(500);                                                   // Wait for a half a second
  MIDI.sendNoteOff(midiValue, 0, 1);                            // Stop the note message
  digitalWrite(13, LOW);
}

void handleTestButton() {
  reading = digitalRead(buttonPin); // read the input pin
  if (reading == HIGH && previous == LOW && millis() - time > debounce) { // if the button is toggled on, it was toggled off previously and at least 200ms has passed
    boolean buttonIsOn = state == HIGH; // simplified boolean used instead of clunky if / else statements
    state = buttonIsOn ? LOW : HIGH; // if button is on, state is low, else is high
    MIDI.sendControlChange(01, buttonIsOn ? 0 : 127, 2); // amp plugin and internal led toggled off/on depending on button state
    time = millis();
  }
  //digitalWrite(ledPin, state); // internal led comes on when the button is toggled on, in the case of ableton, when the amp plugin is on
  previous = reading;
}

