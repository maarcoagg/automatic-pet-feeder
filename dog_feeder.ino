#include <Servo.h>
#include <LiquidCrystal.h>

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

/** LCD VARIABLES **/
const int rs = 2;
const int rw = 3;
const int en = 4;
const int d4 = 5;
const int d5 = 6;
const int d6 = 7;
const int d7 = 8;
const int backLight = 13;
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7);
static uint32_t time_stamp = 0;

/** SERVO VARIABLES **/
const int servoPin = 9;
const int closeCan = 130;  // Closed angle in degrees
const int openCan = 10;    // Open angle in degrees
const int feedingTimeOpen = 700;  // Feeding time in ms, MAX = 1000
const int feedingTimeClose = 1000;// Feeding time close in ms
const int detachTime = 4000;
boolean feeded = false;     // Used to correct time after feeding

/** CLOCK VARIABLES - Start with time set to 07:59:50 **/
int h=7; 
int m=59;   
int s=50;   
int hourBtnPin=11;  // pin 11 for Hours Setting Button
int minuteBtnPin=12;  // pin 12 for Minutes Setting Button
static uint32_t last_time, now = 0; // RTC

/** BUZZER PIN **/
const int buzzerPin = 10;   // Buzzer to digital pin 8

/* MUSIC VARIABLES */
int tempo = 108;
const PROGMEM int melody[] = {
  NOTE_A4, 4,
  NOTE_A4, 4,    
  NOTE_A4, 4,
  NOTE_F4, 6,
  NOTE_C5, 16,
  NOTE_A4, 4,
  NOTE_F4, 6,
  NOTE_C5, 16,
  NOTE_A4, 2
};
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;

Servo myservo;

void setup() {
  Serial.begin(9600);
  now=millis();
  // --- Servo ---
  myservo.attach(servoPin);
  myservo.write(closeCan); // Start feeder closed
  delay(1000);
  myservo.detach();
  // --- Buzzer ---
  pinMode(buzzerPin, OUTPUT);
  tone(buzzerPin, 220, 100);
  // --- Buttons ---
  pinMode(hourBtnPin,INPUT_PULLUP);
  pinMode(minuteBtnPin,INPUT_PULLUP);
  // --- LCD ---
  pinMode(backLight, OUTPUT);
  lcd_backlight_on();
  lcd.begin(16,2);
  lcd_intro();
}

void loop() {  
  print_time();
  feed();
  wait(1000);
  count_time();
}

void feed(){
  if (is_time(0,0,0) || is_time(8,0,0) || is_time(16,0,0)) {
    lcd_backlight_on();
    time_stamp = millis();
    play_music();
    feed_dog();
  }
}

void feed_dog(){
  lcd_backlight_on();
  time_stamp = millis();
  Serial.println("Feeding Tarjinha...");
  lcd.setCursor(0,1);           // set cursor to column 0, row 1
  lcd.print("Feeding Tarjinha"); 
  myservo.attach(servoPin);
  myservo.write(openCan);
  delay(feedingTimeOpen);
  myservo.write(closeCan);
  delay(feedingTimeClose);
  myservo.detach();
  delay(detachTime);
  delay(1000-feedingTimeOpen);
  lcd.setCursor(0,1);           // set cursor to column 0, row 1
  lcd.print("                "); 
  feeded = true;
}

void lcd_intro(){
  lcd.clear();                  // start with a blank screen
  lcd.setCursor(0,0);           // set cursor to column 0, row 0 (the first row)
  lcd.print("Tarjinha Feeder:");    // change text to whatever you like. keep it clean!
  
  for(int i = 0; i < 5; i++){
    lcd.setCursor(2,1);           // set cursor to column 0, row 1
    lcd.print("Starting...");  
    delay(500);
    lcd.setCursor(2,1);           // set cursor to column 0, row 1
    lcd.print("           ");
    delay(500);
  }
}

boolean is_time(int hour, int minute, int second){
  if (hour == h && minute == m && second == s)
    return true;
  return false;
}

void count_time(){
    s=s+1; //increment sec. counting
    if(feeded == true) {
      s=s+6;
      feeded = false;
    }
    if(s>=60) {
      s = s - 60;
      m=m+1;
    }
    if(m==60) {
      m=0;
      h=h+1;
    }
    if(h==24) {
      h=0;
  }
}

void wait(int t){
  for(int i = 0; i < 5; i ++) {
    while ((now-last_time)<200) {
      now=millis();
    }
    last_time=now; 
    
    int hourBtn=digitalRead(hourBtnPin);
    int minuteBtn=digitalRead(minuteBtnPin);

    if(hourBtn==0){
      lcd_backlight_on();
      time_stamp = millis();
      h=h+1;
    }
    
    if(minuteBtn==0){
      lcd_backlight_on();
      time_stamp = millis();
      s=0;
      m=m+1;
    }
    
    if(s==60){
      s=0;
      m=m+1;
    }
    if(m==60)
    {
      m=0;
      h=h+1;
    }
    if(h==24)
      h=0;
  }
  // Desliga backlight após 15 segundos
  if(millis()-time_stamp>15000){
    lcd_backlight_off();
  }
  lcd_print_time();
}

void lcd_backlight_on(){
  digitalWrite(backLight, HIGH); // turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.
}

void lcd_backlight_off(){
  digitalWrite(backLight, LOW); // turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.
}

void lcd_print_time(){
  lcd.setCursor(0,1);           // set cursor to column 0, row 1
  lcd.print("    ");
  // Hours
  if(h<10)
    lcd.print("0");// always 2 digits
  lcd.print(h);
  lcd.print(":");
  // Minutes
  if(m<10)
    lcd.print("0");
  lcd.print(m);
  lcd.print(":");
  // Seconds
  if(s<10)
    lcd.print("0");
  lcd.println(s);
  lcd.print("    ");
}

void print_time(){
  lcd_print_time();
  Serial.print("Time ");
  // Hours
  if(h<10)
    Serial.print("0");// always 2 digits
  Serial.print(h);
  Serial.print(":");
  // Minutes
  if(m<10)
    Serial.print("0");
  Serial.print(m);
  Serial.print(":");
  // Seconds
  if(s<10)
    Serial.print("0");
  Serial.println(s);
}

void play_music(){
  Serial.println("Playing music...");
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = pgm_read_word_near(melody+thisNote + 1);
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzerPin, pgm_read_word_near(melody+thisNote ), noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzerPin);
  }
  delay(650);  
}
