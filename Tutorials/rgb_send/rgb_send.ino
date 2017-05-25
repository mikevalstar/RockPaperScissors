#define BTN_ROCK 10
#define BTN_PAPER 11
#define BTN_SCISSORS 12

#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7

#define IR_READ_POWER 9
#define IR_READ A7
#define IR_WRITE 8

void setup() {
  while (!Serial);
  Serial.begin(9600);
  
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(BTN_ROCK, INPUT);
  pinMode(BTN_PAPER, INPUT);
  pinMode(BTN_SCISSORS, INPUT);

  pinMode(IR_READ_POWER, OUTPUT);
  pinMode(IR_READ, INPUT);
  pinMode(IR_WRITE, OUTPUT);

  Serial.println("SETUP Complete");
}

void loop() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(IR_WRITE, LOW);
  digitalWrite(IR_READ_POWER, HIGH);
  
  // put your main code here, to run repeatedly:
  if(digitalRead(BTN_ROCK) == HIGH){
    digitalWrite(IR_WRITE, HIGH);
    Serial.println("Rock");
  }

  if(digitalRead(BTN_PAPER) == HIGH){
    digitalWrite(IR_WRITE, HIGH);
    Serial.println("Paper");
  }

  if(digitalRead(BTN_SCISSORS) == HIGH){
    digitalWrite(IR_WRITE, HIGH);
    Serial.println("Scissors");
  }

  int val = analogRead(IR_READ);
  Serial.println(val);

  delay(250);
}
