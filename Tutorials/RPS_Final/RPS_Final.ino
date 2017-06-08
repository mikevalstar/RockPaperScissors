#include <EEPROM.h> 

// Inputs
#define BTN_ROCK 3
#define BTN_PAPER 12
#define BTN_SCISSORS 11

#define RED_PIN 7
#define GREEN_PIN 6
#define BLUE_PIN 5

#define IR_READ_POWER 9
#define IR_READ A7
#define IR_WRITE 8

// Constants
#define LOOP_SPEED 25
#define SEND_WINDOW 3000
#define IR_THRESHOLD 250

// Debugging Helpers
//#define DEBUG_IR
#define DEBUG_INPUT
#define DEBUG_RESULTS
#define DEBUG_BOOT

// Reading vars
#define READ_BUFFER_LENGTH 50
bool read_buffer[READ_BUFFER_LENGTH];
int my_choice = 0;
int loop_delay = LOOP_SPEED;
bool boot_flourish = true;

// IR Vars
int irled_timer = -1;
int irled_stop = 0;
int irled_count = 0;
int irled_type = 0;
int opponent_result = -1;

// Data Storage Vars - leave space because we're using 4 bytes
int eeprom_error = 1;
int eeprom_tie = 5;
int eeprom_win = 9;
int eeprom_lose = 13;

int cnt_error = 0;
int cnt_tie = 0;
int cnt_win = 0;
int cnt_lose = 0;

// Boot setup
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

  cnt_error = EEPROMReadlong(eeprom_error);
  cnt_tie = EEPROMReadlong(eeprom_tie);
  cnt_win = EEPROMReadlong(eeprom_win);
  cnt_lose = EEPROMReadlong(eeprom_lose);

  #ifdef DEBUG_BOOT
  Serial.println("SETUP Complete");
  Serial.print("System Errors: ");Serial.print(cnt_error);Serial.print("\n");
  Serial.print("System Ties: ");Serial.print(cnt_tie);Serial.print("\n");
  Serial.print("System Wins: ");Serial.print(cnt_win);Serial.print("\n");
  Serial.print("System Loses: ");Serial.print(cnt_lose);Serial.print("\n");
  #endif
}

void loop() {
  // Show boot up lights
  if(boot_flourish){
    boot_lights();
  }
  
  // Buttons
  if(digitalRead(BTN_ROCK) == HIGH && irled_timer == -1){
    #ifdef DEBUG_INPUT
    Serial.println("ROCK");
    #endif
    set_led(BLUE_PIN, 250);
    set_irled(1, SEND_WINDOW);
    my_choice = 1;
  }

  if(digitalRead(BTN_PAPER) == HIGH && irled_timer == -1){
    #ifdef DEBUG_INPUT
    Serial.println("PAPER");
    #endif
    set_led(BLUE_PIN, 250);
    set_irled(2, SEND_WINDOW);
    my_choice = 2;
  }

  if(digitalRead(BTN_SCISSORS) == HIGH && irled_timer == -1){
    #ifdef DEBUG_INPUT
    Serial.println("SCISSORS");
    #endif
    set_led(BLUE_PIN, 250);
    set_irled(3, SEND_WINDOW);
    my_choice = 3;
  }

  loop_irled();
  loop_led();
  delay(loop_delay);
}

// IR Reader
void irread(){
  if(opponent_result != -1){
    return;
  }
  int val = analogRead(IR_READ);
  buffer_shift(val > IR_THRESHOLD);
  #ifdef DEBUG_IR
  Serial.println(val);
  #endif

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
    Serial.println("Result: No Result");
    cnt_error++;
    EEPROMWritelong(eeprom_error, cnt_error);
    return;  
  }
  
  if(my_choice == opponent_result){
    Serial.println("Result: Tie");
    set_led(BLUE_PIN, 2000);
    cnt_tie++;
    EEPROMWritelong(eeprom_tie, cnt_tie);
  }else if(my_choice == 1 && opponent_result == 2){
    Serial.println("Result: Rock vs. Paper - Lose");
    set_led(RED_PIN, 2000);
    cnt_lose++;
    EEPROMWritelong(eeprom_lose, cnt_lose);
  }else if(my_choice == 2 && opponent_result == 3){
    Serial.println("Result: Paper vs. Scissors - Lose");
    set_led(RED_PIN, 2000);
    cnt_lose++;
    EEPROMWritelong(eeprom_lose, cnt_lose);
  }else if(my_choice == 3 && opponent_result == 1){
    Serial.println("Result: Scissors vs. Rock - Lose");
    set_led(RED_PIN, 2000);
    cnt_lose++;
    EEPROMWritelong(eeprom_lose, cnt_lose);
  }else if(my_choice == 2 && opponent_result == 1){
    Serial.println("Result: Paper vs. Rock - Win");
    set_led(GREEN_PIN, 2000);
    cnt_win++;
    EEPROMWritelong(eeprom_win, cnt_win);
  }else if(my_choice == 3 && opponent_result == 2){
    Serial.println("Result: Scissors vs. Paper - Win");
    set_led(GREEN_PIN, 2000);
    cnt_win++;
    EEPROMWritelong(eeprom_win, cnt_win);
  }else if(my_choice == 1 && opponent_result == 3){
    Serial.println("Result: Rock vs. Scissors - Win");
    set_led(GREEN_PIN, 2000);
    cnt_win++;
    EEPROMWritelong(eeprom_win, cnt_win);
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

// Fun stuff
void boot_lights(){
  // Small boot flourish
  for(int i = 50; i > 0; i--) { 
    analogWrite(BLUE_PIN, 255 - i );
    delay(25);
  }
  digitalWrite(BLUE_PIN, HIGH);
  boot_flourish = false;
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

//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to address + 3.
void EEPROMWritelong(int address, long value){
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address){
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);
  
  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

