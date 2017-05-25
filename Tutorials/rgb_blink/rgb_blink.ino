#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7

void setup() {
  // put your setup code here, to run once:
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(RED_PIN, HIGH);
  delay(500);
  digitalWrite(RED_PIN, LOW);

  digitalWrite(GREEN_PIN, HIGH);
  delay(500);
  digitalWrite(GREEN_PIN, LOW);

  digitalWrite(BLUE_PIN, HIGH);
  delay(500);
  digitalWrite(BLUE_PIN, LOW);
}
