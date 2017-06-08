// Inputs
#define BTN_ROCK 10
#define BTN_PAPER 11
#define BTN_SCISSORS 12

#define RED_PIN 7
#define GREEN_PIN 6
#define BLUE_PIN 5

#define IR_READ_POWER 9
#define IR_READ A7
#define IR_WRITE 8

// Constants
#define LOOP_SPEED 25
#define SEND_WINDOW 3000
#define IR_THRESHOLD 300

// Reading vars
#define READ_BUFFER_LENGTH 50
bool read_buffer[READ_BUFFER_LENGTH];
int my_choice = 0;
int loop_delay = LOOP_SPEED;

void setup() {
  //while (!Serial);
  Serial.begin(9600);
  
  pinMode(BTN_ROCK, INPUT);
  pinMode(BTN_PAPER, INPUT);
  pinMode(BTN_SCISSORS, INPUT);
  
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);

  pinMode(IR_READ_POWER, OUTPUT);
  pinMode(IR_READ, INPUT);
  pinMode(IR_WRITE, OUTPUT);

  buffer_clear();

  Serial.println("SETUP Complete");
}

void loop() {
  // Buttons
  if(digitalRead(BTN_ROCK) == HIGH){
    Serial.println("ROCK");
    set_led(BLUE_PIN, 250);
    set_irled(1, SEND_WINDOW);
    my_choice = 1;
  }

  if(digitalRead(BTN_PAPER) == HIGH){
    Serial.println("PAPER");
    set_led(BLUE_PIN, 250);
    set_irled(2, SEND_WINDOW);
    my_choice = 2;
  }

  if(digitalRead(BTN_SCISSORS) == HIGH){
    Serial.println("SCISSORS");
    set_led(BLUE_PIN, 250);
    set_irled(3, SEND_WINDOW);
    my_choice = 3;
  }

  loop_irled();
  loop_led();
  delay(loop_delay);
}


// IR Vars
int irled_timer = -1;
int irled_stop = 0;
int irled_count = 0;
int irled_type = 0;
int opponent_result = -1;

// IR Reader
void irread(){
  if(opponent_result != -1){
    return;
  }
  int val = analogRead(IR_READ);
  buffer_shift(val > IR_THRESHOLD);
  Serial.println(val);

  // figure out count in each end of the buffer equel
  float cnt1 = 0;
  float cnt2 = 0;
  float half_buff = READ_BUFFER_LENGTH / 2;
  for(int i = 0; i < half_buff; i++){
    if(read_buffer[i] == true){
      cnt1++;
    }
  }
  for(int x = half_buff + 1; x < READ_BUFFER_LENGTH; x++){
    if(read_buffer[x] == true){
      cnt2++;
    }
  }

  // check for known opponent value
  if(cnt1 / half_buff > .9 && cnt2 / half_buff > .9){ // 90%
    opponent_result = 1;
    Serial.println("opponent rock");
  }else if(cnt1 / half_buff > .45 && cnt2 / half_buff > .45
          && cnt1 / half_buff < .55 && cnt2 / half_buff < .55){ // 45 - 55%
    opponent_result = 2;
    Serial.println("opponent paper");
  }else if(cnt1 / half_buff > .28 && cnt2 / half_buff > .28
          && cnt1 / half_buff < .38 && cnt2 / half_buff < .38){ // 28 - 38%
    opponent_result = 3;
    Serial.println("opponent scissors");
  }
}

// Set the IR LED to be on for timer duration
void loop_irled(){
  if(irled_timer == -1){
    return;
  }else if(irled_timer < irled_stop){
    if(irled_count % irled_type == 0){
      digitalWrite(IR_WRITE, HIGH);
    }else{
      digitalWrite(IR_WRITE, LOW);
    }
    irread();
    irled_timer += LOOP_SPEED;
    irled_count++;
  }else{
    digitalWrite(IR_WRITE, LOW);
    digitalWrite(IR_READ_POWER, LOW);
    irled_timer = -1;

    print_result();
  }
}

void set_irled(int modtype, int timer){
  digitalWrite(IR_READ_POWER, HIGH);
  digitalWrite(IR_WRITE, LOW);

  irled_type = modtype;
  irled_timer = 0;
  irled_count = 0;
  irled_stop = timer;
  opponent_result = -1;
  buffer_clear();
}

// print out the result
void print_result(){
  if(opponent_result == -1){
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    delay(250);
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    return;  
  }
  
  if(my_choice == opponent_result){
    set_led(BLUE_PIN, 2000);
  }else if(my_choice == 1 && opponent_result == 2){
    set_led(RED_PIN, 2000);
  }else if(my_choice == 2 && opponent_result == 3){
    set_led(RED_PIN, 2000);
  }else if(my_choice == 3 && opponent_result == 1){
    set_led(RED_PIN, 2000);
  }else if(my_choice == 2 && opponent_result == 1){
    set_led(GREEN_PIN, 2000);
  }else if(my_choice == 3 && opponent_result == 2){
    set_led(GREEN_PIN, 2000);
  }else if(my_choice == 1 && opponent_result == 3){
    set_led(GREEN_PIN, 2000);
  }
}

// Set the LED to be on for timer duration
int led_pin = 0;
int led_timer = -1;
int led_stop = 0;

void loop_led(){
  if(led_timer < led_stop){
    if(led_timer == 0){
      digitalWrite(led_pin, LOW);
    }
    led_timer += LOOP_SPEED;
  }else{
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);
  }
}

void set_led(int pin, int timer){
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
    
  led_pin = pin;
  led_timer = 0;
  led_stop = timer;
}

// Helper functions
void buffer_shift(bool val){
  for(int i = 1; i < READ_BUFFER_LENGTH; i++) { 
    read_buffer[i-1] = read_buffer[i]; 
  }
  read_buffer[READ_BUFFER_LENGTH - 1] = val;
  
}

void buffer_clear(){
  for(int i = 0; i < READ_BUFFER_LENGTH; i++) { 
    read_buffer[i] = false; 
  }
}

