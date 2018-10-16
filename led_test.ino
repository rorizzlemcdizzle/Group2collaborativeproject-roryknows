int pin = 0;

void setup() {
  // put your setup code here, to run once:

  
  
 Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
pinMode(pin, OUTPUT);

 if(pin >= 16) {
  pin = 0;
  }
  
  digitalWrite(pin, HIGH);

  delay(300);

  digitalWrite(pin, HIGH);

  Serial.println(pin);

    delay(300);
    
  pin = pin + 1;
}
