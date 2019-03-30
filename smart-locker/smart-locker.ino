#include <Keypad.h>  //795ADBC
#include<LiquidCrystal_PCF8574.h>
#include<EEPROM.h>
#include "Ticker.h"

#define DEBUG

int relay1=10;
int relay2=11;
int buzzer=13;
bool r1;
bool r2;
bool z1;
bool z2;

char outpass1[4];
char inpass1[4];
char inpass1x[4];
int i1=0;
char key_pressed1=0;
bool cpass1=0;
bool hours11=0;
bool hours12=0;

char outpass2[4];
char inpass2[4];
char inpass2x[4];
int i2=0;
char key_pressed2=0;
bool cpass2=0;
bool hours21=0;
bool hours22=0;

int passCount=0;

LiquidCrystal_PCF8574 lcd(0x27);
const byte rows=4;
const byte columns=4;

char hexaKeys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte row_pins[rows]={9, 8, 7, 6};
byte column_pins[columns]={5, 4, 3, 2};

enum state_t {
  sNone,
  sHomeDisplay, sHomeWaitKeyPress,
  sMainMenuDisplay, sMainMenuWaitKeyPress,
  sDelay,
  sSelectSlotDisplay, sSelectSlotWaitKeyPress,
  sInbox1, sInbox2,
  sHr1Display, sHr2Display,
  sHr1, sHr2,
  sPay1, sPay2,
  sEnterPassword1, sEnterPassword2, 
  sEnterPassword1Again, sEnterPassword2Again,
  sInpass1, sInpass2,
  sInpass1Again, sInpass2Again,
  sInpass1cmp, sInpass2cmp,
  sIntoTheBox1, sIntoTheBox2,
  sLock1, sLock2,
  sLogoutDis, sLogout,
  sOutBox1, sOutBox2,
  sOutPass1Dis, sOutPass2Dis,
  sOutPass1, sOutPass2,
  sUnlock1, sUnlock2,
  
};

state_t pState=sHomeDisplay;
state_t nextState;

Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

Ticker timer1(printCounter1, 1000);
Ticker timer2(printCounter2, 1000);
Ticker timer3(delayTime, 500);

void setup(){
  Serial.begin(9600);
  Serial.println("setup..");
  
  lcd.begin(20,4);
  pinMode(buzzer,OUTPUT);
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  digitalWrite(relay1,HIGH);
  digitalWrite(relay2,HIGH);

  #ifdef DEBUG
    EEPROM.write(3, 0);
    EEPROM.write(4, 0);
  #endif
      
  z1=0;
  z2=0;

  r1=EEPROM.read(3);
  if(r1==1){
    digitalWrite(relay1,LOW);
    z1=1;
  }
  r2=EEPROM.read(4);
  if(r2==1){
    digitalWrite(relay2,LOW);
    z2=1;
  }

  cpass1 = EEPROM.read(1);
  if (cpass1==1) {
    for (int a=0; a<4; a++){
    int a1=10;
      inpass1[a] = EEPROM.read(a1);
      a1++;
    }
  }
  cpass2 = EEPROM.read(2);
  if (cpass2==1) {
    for (int b=0; b<4; b++){
    int a1=20;
      inpass2[b] = EEPROM.read(a1);
      a1++;
    }
  }
//  START1(); 
}

void loop(){
//  Serial.println("loop");
  timer1.update();
  timer2.update();
  timer3.update();

  processTask();  
  
}

void processTask() {
  switch(pState) {
    case sHomeDisplay:
      START1();
      pState = sHomeWaitKeyPress;
      break;
    case sHomeWaitKeyPress:
      if(!(keypad_key.getKey()==NO_KEY)){
        pState = sMainMenuDisplay;
      }
      #ifdef DEBUG
        if(Serial.available() > 0) { 
          char inChar = Serial.read(); 
          Serial.println(inChar);
          pState = sMainMenuDisplay;
        }
      #endif
      break;
    case sMainMenuDisplay:
      START2();
      pState = sMainMenuWaitKeyPress;
      break;
    case sMainMenuWaitKeyPress:
      START2_WAIT();
      break;
    case sDelay:
      //wait
      break;
    case sSelectSlotDisplay:
      LOGIN();
      pState = sSelectSlotWaitKeyPress;
      break;
    case sSelectSlotWaitKeyPress: LOGIN_WAIT(); break;
    case sInbox1: INBOX1(); break;
    case sInbox2: INBOX2(); break;
    case sHr1Display: HR_DISPLAY(); Serial.println("sHr1Display"); pState = sHr1; break;
    case sHr2Display: HR_DISPLAY(); Serial.println("sHr2Display"); pState = sHr2; break;
    case sHr1: HR1(); break;
    case sHr2: HR2(); break;
    case sPay1: PAY1(); break;  
    case sPay2: PAY2(); break;
    case sEnterPassword1: enterPassword(); passCount=0; pState = sInpass1; Serial.println("sEnterPassword1"); break;
    case sEnterPassword2: enterPassword(); passCount=0; pState = sInpass2; Serial.println("sEnterPassword2"); break;
    case sInpass1: INPASS1(); break;
    case sInpass2: INPASS2(); break;
    case sEnterPassword1Again: enterPasswordAgain(); passCount=0; pState = sInpass1Again; break;
    case sEnterPassword2Again: enterPasswordAgain(); passCount=0; pState = sInpass2Again; break;
    case sInpass1Again: INPASS1Again(); break;
    case sInpass2Again: INPASS2Again(); break;
    case sInpass1cmp: INPASS1cmp(); break;
    case sInpass2cmp: INPASS2cmp(); break;
    case sIntoTheBox1: 
      Serial.println("sIntoTheBox1");
      intoTheBox(); 
      timer3.interval(2000); timer3.start(); pState = sDelay;
      nextState = sLock1;
      break;
    case sIntoTheBox2: 
      Serial.println("sIntoTheBox2");
      intoTheBox(); 
      timer3.interval(2000); timer3.start(); pState = sDelay;
      nextState = sLock2;
      break;
    case sLock1: LOCK1(); break;
    case sLock2: LOCK2(); break; 
    case sLogoutDis: LOGOUT_DIS(); pState = sLogout; break;
    case sLogout: LOGOUT(); break;
    case sOutBox1: OUTBOX1(); break;
    case sOutBox2: OUTBOX2(); break;
    case sOutPass1Dis: OUTPASS_DIS(); pState = sOutPass1; break;
    case sOutPass2Dis: OUTPASS_DIS(); pState = sOutPass2; break;
    case sOutPass1: OUTPASS1(); break;
    case sOutPass2: OUTPASS2(); break;
    case sUnlock1: UNLOCK1(); break;
    case sUnlock2: UNLOCK2(); break;
  }
}

void printCounter1() {
  Serial.print("Counter1 ");
  Serial.println(timer1.counter());
}

void printCounter2() {
  Serial.print("Counter2 ");
  Serial.println(timer2.counter());
}

void delayTime() {
  Serial.print("delayTime ");
  Serial.println(timer3.counter());
  timer3.stop();
  pState = nextState;
}

void OUTPASS_DIS(){
  Serial.println("OUTPASS_DIS: ENTER PASSWORD");
 lcd.clear();
 lcd.setCursor(3,1);
 lcd.print("ENTER PASSWORD");
 lcd.setCursor(8,2);
}

void OUTPASS1(){
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
  if(keypad_key_getKey){
    outpass1[i1++]=keypad_key_getKey;
    lcd.print("*");
    if (i1==4) {
//      delay(200);
      int a1=10;
      for (int a=0; a<4; a++){       
        inpass1[a] = EEPROM.read(a1);
        a1++;
      }
      if (!(strncmp(outpass1, inpass1, 4)))
      {
        lcd.clear();
        lcd.setCursor(6,1);
        lcd.print("SUCCESS");
        i1 = 0;
//        UNLOCK1();
        timer3.interval(1000); timer3.start(); pState = sDelay;
        nextState = sUnlock1;
      }
      else {
        lcd.clear();
        lcd.setCursor(3,1);
        lcd.print("WRONG PASSWORD");
        lcd.setCursor(0,1);
        i1 = 0;
//        delay(1000);        
//        LOGOUT();
        timer3.interval(1000); timer3.start(); pState = sDelay;
        nextState = sLogoutDis;
      }
    }
  }
}


void OUTPASS2(){
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
  if(keypad_key_getKey){
    outpass2[i2++]=keypad_key_getKey;
    lcd.print("*");
    if (i2==4) {
//      delay(200);
      int a1=20;
      for (int a=0; a<4; a++){        
        inpass2[a] = EEPROM.read(a1);
        a1++;
      }
      if (!(strncmp(outpass2, inpass2, 4)))
      {
        lcd.clear();
        lcd.setCursor(6,1);
        lcd.print("SUCCESS");
        i2 = 0;
//        UNLOCK2();
        timer3.interval(1000); timer3.start(); pState = sDelay;
        nextState = sUnlock2;
      }
      else {
        lcd.clear();
        lcd.setCursor(3,1);
        lcd.print("WRONG PASSWORD");
        lcd.setCursor(0,1);
        i2 = 0;
//        delay(1000);        
//        LOGOUT();
        timer3.interval(1000); timer3.start(); pState = sDelay;
        nextState = sLogoutDis;
      }
    }
  }
}

void START1(){
  Serial.println("START1: PRESS ANY TO START");
  lcd.clear();
  lcd.setCursor(5,0);  
  lcd.print(":WELCOME:");
  lcd.setCursor(4,1);
  lcd.print("SMART LOCKER");
  lcd.setCursor(2,2);
  lcd.print("PRESS ANY TO START");
  lcd.setCursor(0,3);
  lcd.print("____________________");
}

void START2(){
  Serial.println("START2: LOGIN, LOGOUT, RETURN");
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("PRESS 1 TO LOGIN");
  lcd.setCursor(2,1);
  lcd.print("PRESS 2 TO LOGOUT");
  lcd.setCursor(2,2);
  lcd.print("PRESS 0 TO RETURN");
}

void START2_WAIT(){
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
  if(keypad_key_getKey=='1'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("LOGIN");
//      delay(500);       
//      LOGIN();
    timer3.interval(500); timer3.start(); pState = sDelay;
    nextState = sSelectSlotDisplay;
  }
  else if(keypad_key_getKey=='2'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("LOGOUT");
//      delay(500); 
//      LOGOUT();
    timer3.interval(500); timer3.start(); pState = sDelay;
    nextState = sLogoutDis;
  }
  else if(keypad_key_getKey=='0'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("RETURN");
//      delay(500); 
//      START1();
    timer3.interval(500); timer3.start(); pState = sDelay;
    nextState = sHomeDisplay;
  }
}

void LOGIN(){
  Serial.println("LOGIN: CHOOSE EMPTY SLOT");
  lcd.clear();
  lcd.setCursor(1,1);
  lcd.print("CHOOSE EMPTY SLOT");
  lcd.setCursor(1,2);
  lcd.print("PRESS 0 TO RETURN");
}

void LOGIN_WAIT(){
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
  if(keypad_key_getKey=='1'){
    lcd.clear();
    lcd.setCursor(8,1);
    lcd.print("BOX1");
//      delay(500); 
//      INBOX1();
    timer3.interval(500); timer3.start(); pState = sDelay;
    nextState = sInbox1;
  }
  else if(keypad_key_getKey=='2'){
    lcd.clear();
    lcd.setCursor(8,1);
    lcd.print("BOX2");
//      delay(500); 
//      INBOX2();
    timer3.interval(500); timer3.start(); pState = sDelay;
    nextState = sInbox2;
  }
  else if(keypad_key_getKey=='0'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("RETURN");
//      delay(500); 
//      START2();
    timer3.interval(500);
    timer3.start();
    pState = sDelay;
    nextState = sMainMenuDisplay;
  }

}

void INBOX1(){
  lcd.clear();
  if(z1==1){
    lcd.setCursor(3,1);
    lcd.print("NOT AVAILABLE");
//    delay(1000);
//    LOGIN();
    timer3.interval(1000);
    timer3.start();
    pState = sDelay;
    nextState = sMainMenuDisplay;
  }
  else if(z1==0){
    lcd.setCursor(5,1);
    lcd.print("AVAILABLE");
//    delay(1000);
//    HR1();
    timer3.interval(1000);
    timer3.start();
    pState = sDelay;
    nextState = sHr1Display;
  }
}
  
void INBOX2(){
  lcd.clear();
  if(z2==1){
    lcd.setCursor(3,1);
    lcd.print("NOT AVAILABLE");
//    delay(1000);
//    LOGIN();
    timer3.interval(1000);
    timer3.start();
    pState = sDelay;
    nextState = sMainMenuDisplay;
  }
  else if(z2==0){
    lcd.setCursor(5,1);
    lcd.print("AVAILABLE");
//    delay(1000);
//    HR2();
    timer3.interval(1000);
    timer3.start();
    pState = sDelay;
    nextState = sHr2Display;   
  }
}

void HR_DISPLAY(){
  Serial.println("HR_DISPLAY: THE NUMBER OF HOURS (1 OR 2)");
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("PLEASE CHOOSE");
  lcd.setCursor(0,1);
  lcd.print("THE NUMBER OF HOURS");
  lcd.setCursor(7,2);
  lcd.print("1 OR 2");
  lcd.setCursor(1,3);
  lcd.print("PRESS 0 TO RETURN");
}

void HR1(){
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
  if(keypad_key_getKey=='1'){
    hours11=1;
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("1 HOUR");
//      delay(500); 
//      PAY1();
    timer3.interval(500);
    timer3.start();
    pState = sDelay;
    nextState = sPay1;
  }
  else if(keypad_key_getKey=='2'){
    hours12=1;
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("2 HOUR");
//      delay(500); 
//      PAY1();
    timer3.interval(500);
    timer3.start();
    pState = sDelay;
    nextState = sPay1;
  }
  else if(keypad_key_getKey=='0'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("RETURN");
//      delay(500); 
//      LOGIN();
    timer3.interval(500);
    timer3.start();
    pState = sDelay;
    nextState = sMainMenuDisplay;
  }
}

void HR2(){
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
  if(keypad_key_getKey=='1'){
    hours21=1;
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("1 HOUR");
//      delay(500); 
//      PAY2();
    timer3.interval(500);
    timer3.start();
    pState = sDelay;
    nextState = sPay2;
  }
  else if(keypad_key_getKey=='2'){
    hours22=1;
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("2 HOUR");
//      delay(500); 
//      PAY2();
    timer3.interval(500);
    timer3.start();
    pState = sDelay;
    nextState = sPay2;
  }
  else if(keypad_key_getKey=='0'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("RETURN");
//      delay(500); 
//      LOGIN();
    timer3.interval(500);
    timer3.start();
    pState = sDelay;
    nextState = sMainMenuDisplay;
  } 
}

void PAY1(){
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("TID WAI GON1");
//  delay(500);
  if(hours11==1){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("TIMER RUN 1 HR");
//    delay(500);
  }
  if(hours12==1){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("TIMER RUN 2 HR");
//    delay(500);
  }
//  INPASS1();
  timer3.interval(500);
  timer3.start();
  pState = sDelay;
  nextState = sEnterPassword1;
}

void PAY2(){
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("TID WAI GON2");
//  delay(500);
  if(hours21==1){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("TIMER RUN 1 HR");
//    delay(500);
  }
  if(hours22==1){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("TIMER RUN 2 HR");
//    delay(500);
  }
//  INPASS2();
  timer3.interval(500);
  timer3.start();
  pState = sDelay;
  nextState = sEnterPassword2;
}

void enterPassword(){
  Serial.println("enterPassword");
  lcd.clear();
  lcd.setCursor(3,1);
  lcd.print("ENTER PASSWORD");
  lcd.setCursor(8,2);
}

void enterPasswordAgain(){
  Serial.println("enterPasswordAgain");
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("ENTER PASSWORD AGAIN");
  lcd.setCursor(8,2);
}

void intoTheBox(){
  Serial.println("intoTheBox: PLEASE PUT YOUR ITEM");
  lcd.setCursor(0,0);
  lcd.print("PLEASE PUT YOUR ITEM");
  lcd.setCursor(4,1);
  lcd.print("INTO THE BOX");
  lcd.setCursor(2,2);
  lcd.print("WITHIN 30 SECOND");
//  delay(2000);
//  LOCK1();
}

void INPASS1(){
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
  int a1=passCount+10;
  if (keypad_key_getKey){
    inpass1[passCount]=keypad_key_getKey;
    lcd.print("*");
    EEPROM.write(a1,keypad_key_getKey);
    if(++passCount >= 4) {
      timer3.interval(500); timer3.start(); pState = sDelay;
      nextState = sEnterPassword1Again;
    }
  }
}

void INPASS1Again(){
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
   if (keypad_key_getKey){ 
    inpass1x[passCount]=keypad_key_getKey;
    lcd.print("*");
    if(++passCount >= 4) {
      timer3.interval(500); timer3.start(); pState = sDelay;
      nextState = sInpass1cmp;
    }
  }
}

void INPASS1cmp(){
  if(!(strncmp(inpass1, inpass1x, 4))){
    lcd.clear();
    lcd.setCursor(6,1);
    lcd.print("SUCCESS");
//    delay(1000);
    cpass1 = 1;
    EEPROM.write(1, cpass1);
    timer3.interval(1000); timer3.start(); pState = sDelay;
    nextState = sIntoTheBox1;
  }
  else {
    lcd.clear();
    lcd.setCursor(2,1);
    lcd.print("PLEASE TRY AGAIN");
//    delay(1000);
//    INPASS1();
    timer3.interval(1000); timer3.start(); pState = sDelay;
    nextState = sEnterPassword1;
  }
}

void INPASS2(){
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
  int a1=passCount+20;
  if (keypad_key_getKey){
    inpass2[passCount]=keypad_key_getKey;
    lcd.print("*");
    EEPROM.write(a1,keypad_key_getKey);
    if(++passCount >= 4) {
      timer3.interval(500); timer3.start(); pState = sDelay;
      nextState = sEnterPassword2Again;
    }
  }
}

void INPASS2Again(){
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
  if (keypad_key_getKey){ 
    inpass2x[passCount]=keypad_key_getKey;
    lcd.print("*");
    if(++passCount >= 4) {
      timer3.interval(500); timer3.start(); pState = sDelay;
      nextState = sInpass2cmp;
    }
  }
}

void INPASS2cmp(){
  if(!(strncmp(inpass2, inpass2x, 4))){
    lcd.clear();
    lcd.setCursor(6,1);
    lcd.print("SUCCESS");
//    delay(1000);
    cpass2 = 1;
    EEPROM.write(2, cpass2);
    timer3.interval(1000); timer3.start(); pState = sDelay;
    nextState = sIntoTheBox2;
  }
  else {
    lcd.clear();
    lcd.setCursor(2,1);
    lcd.print("PLEASE TRY AGAIN");
//    delay(1000);
//    INPASS1();
    timer3.interval(1000); timer3.start(); pState = sDelay;
    nextState = sEnterPassword2;
  }
}

void LOGOUT_DIS(){
  Serial.println("LOGOUT_DIS: CHOOSE YOUR SLOT");
  lcd.clear();
  lcd.setCursor(2,1);
  lcd.print("CHOOSE YOUR SLOT");
  lcd.setCursor(2,2);
  lcd.print("PRESS 0 TO RETURN");
}

void LOGOUT(){  
  char keypad_key_getKey = keypad_key.getKey();
  #ifdef DEBUG
    if(Serial.available() > 0) { 
      char inChar = Serial.read(); 
      Serial.println(inChar);
      keypad_key_getKey = inChar;
    }
  #endif
  if(keypad_key_getKey=='1'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("BOX1");
//    delay(500); 
//    OUTBOX1();
    timer3.interval(500); timer3.start(); pState = sDelay;
    nextState = sOutBox1;
  }
  else if(keypad_key_getKey=='2'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("BOX2");
//    delay(500); 
//    OUTBOX2() ;
    timer3.interval(500); timer3.start(); pState = sDelay;
    nextState = sOutBox2;
  }
  else if(keypad_key_getKey=='0'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("RETURN");
//    delay(500); 
//    START2(); 
    timer3.interval(500); timer3.start(); pState = sDelay;
    nextState = sMainMenuDisplay;
  }
}

void OUTBOX1() {
  lcd.clear();
  if(z1==0) {
    lcd.setCursor(3,1);
    lcd.print("NOT AVAILABLE");
//    delay(1000) ;
//    LOGOUT() ;
    timer3.interval(1000); timer3.start(); pState = sDelay;
    nextState = sLogoutDis;
  }
  else if (z1==1) {
    lcd.setCursor(5,1);
    lcd.print("AVAILABLE");
//    delay(1000);
//    OUTPASS1();
    timer3.interval(1000); timer3.start(); pState = sDelay;
    nextState = sOutPass1Dis;
  }
}

void OUTBOX2() {
  lcd.clear();
  if(z2==0) {
    lcd.setCursor(3,1);
    lcd.print("NOT AVAILABLE");
//    delay(1000) ;
//    LOGOUT() ;
    timer3.interval(1000); timer3.start(); pState = sDelay;
    nextState = sLogoutDis;
  }
  else if(z2==1) {
    lcd.setCursor(5,1);
    lcd.print("AVAILABLE");
//    delay(1000);
//    OUTPASS2();
    timer3.interval(1000); timer3.start(); pState = sDelay;
    nextState = sOutPass2Dis;
  }
}
  
void LOCK1(){
  Serial.println("LOCK1");
  digitalWrite(10,LOW);
  r1=1;
  z1=1;
  EEPROM.write(3, r1);
  lcd.clear();
  lcd.setCursor(5,1);
  lcd.print("THANK YOU");
//  delay(1000);
//  START1();
  timer3.interval(1000); timer3.start(); pState = sDelay;
  nextState = sHomeDisplay;
  timer1.start();
}

void LOCK2(){
  Serial.println("LOCK2");
  digitalWrite(11,LOW);
  r2=1;
  z2=1;
  lcd.clear();
  lcd.setCursor(5,1);
  lcd.print("THANK YOU");
//  delay(1000);
//  START1();
  timer3.interval(1000); timer3.start(); pState = sDelay;
  nextState = sHomeDisplay;
  timer2.start();
}

void UNLOCK1(){
  Serial.println("UNLOCK1");
  digitalWrite(10,HIGH);
  r1=0;
  z1=0;
  EEPROM.write(4, r2);
  lcd.clear();
  lcd.setCursor(5,1);
  lcd.print("THANK YOU");
//  delay(1000);
//  START1();
  cpass1=0;
  timer3.interval(1000); timer3.start(); pState = sDelay;
  nextState = sHomeDisplay;

  int secCount = timer1.counter();
//  secCount = 7515; //test
  int hh = secCount/(60*60);
  int mm = (secCount - (hh*60*60))/(60);
  int ss = secCount%60;
  Serial.println(String(hh)+":"+String(mm)+":"+String(ss));
  timer1.stop();
}

void UNLOCK2(){
  Serial.println("UNLOCK2");
  digitalWrite(11,HIGH);
  r2=1;
  z2=1;
  lcd.clear();
  lcd.setCursor(5,1);
  lcd.print("THANK YOU");
//  delay(1000);
//  START1();
  cpass2=0;
  timer3.interval(1000); timer3.start(); pState = sDelay;
  nextState = sHomeDisplay;
  
  int secCount = timer2.counter();
  int hh = secCount/(60*60);
  int mm = (secCount - (hh*60*60))/(60);
  int ss = secCount%60;
  Serial.println(String(hh)+":"+String(mm)+":"+String(ss));
  timer2.stop();
}
