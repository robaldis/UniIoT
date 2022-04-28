const int switch_pin = D1;
int switch_value;

const int led_pin = D2;


const int potentiometer_pin = A0;
int pote_value;

int min_temp = -2;
int max_temp = 6;

void setup() {
  // put your setup code here, to run once:
  pinMode(switch_pin, INPUT);
  pinMode(potentiometer_pin, INPUT);
  pinMode(led_pin, OUTPUT);


  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (fridgeOn()) {
      Serial.println(fridgeTemp());
      delay(500);
  } else {
      // Brightness based on potentiometer
    pote_value = analogRead(potentiometer_pin);
    int b = map(pote_value, 0, 1023, 0, 253);
    analogWrite(led_pin, b);
  }

}

int fridgeTemp() {
    pote_value = analogRead(potentiometer_pin);
    pote_value = map(pote_value, 0, 1023, min_temp, max_temp);
    return pote_value;
}

bool fridgeOn() {
    switch_value = digitalRead(switch_pin);
    if (switch_value == 0) {
        return false;
    }
    return true;
}
