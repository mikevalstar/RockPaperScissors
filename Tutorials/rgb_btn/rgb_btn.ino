#define BTN_ROCK 10
#define BTN_PAPER 11
#define BTN_SCISSORS 12

#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7

void setup() {
  // put your setup code here, to run once:
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(BTN_ROCK, INPUT);
  pinMode(BTN_PAPER, INPUT);
  pinMode(BTN_SCISSORS, INPUT);
}

void loop() {
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
  
  // put your main code here, to run repeatedly:
  if(digitalRead(BTN_ROCK) == HIGH){
    digitalWrite(RED_PIN, LOW);
  }

  if(digitalRead(BTN_PAPER) == HIGH){
    digitalWrite(GREEN_PIN, LOW);
  }

  if(digitalRead(BTN_SCISSORS) == HIGH){
    digitalWrite(BLUE_PIN, LOW);
  }

  delay(25);
}
