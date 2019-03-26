#include <Keypad.h>  //795ADBC
#include<LiquidCrystal_PCF8574.h>
#include<EEPROM.h>

int relay1=10;
int relay2=11;
int buzzer=13;
bool r1;
bool r2;

char outpass1[4];
char inpass1[4];
int i1=0;
char key_pressed1=0;
bool cpass1=0;
bool hours11=0;
bool hours12=0;

char outpass2[4];
char inpass2[4];
int i2=0;
char key_pressed2=0;
bool cpass2=0;
bool hours21=0;
bool hours22=0;

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

Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

void setup(){
  lcd.begin(20,4);
  pinMode(buzzer,OUTPUT);
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);

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
  START1(); 
}

void INPASS1(){
  char inpass1x[4]; 
  lcd.clear();
  lcd.setCursor(3,1);
  lcd.print("ENTER PASSWORD");
  lcd.setCursor(8,2);
  int a=0;
  int a1=10;
  while (a<4){
      char key1=keypad_key.getKey();
      if (key1){
        inpass1[a]=key1;
        lcd.print("*");
        EEPROM.write(a1,key1);
        a++;
        a1++;
      }
    }
    delay(500);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("ENTER PASSWORD AGAIN");
  lcd.setCursor(8,2);
  int b=0;
  while(b<4){
    char key2=keypad_key.getKey();
    if (key2){ 
    inpass1x[b]=key2;
    lcd.print("*");
    b++;
    }
  }
  delay(500);
       if(!(strncmp(inpass1, inpass1x, 4))){
        lcd.clear();
        lcd.setCursor(6,1);
        lcd.print("SUCCESS");
        delay(1000);
        }
       else {
        lcd.clear();
        lcd.setCursor(2,1);
        lcd.print("PLEASE TRY AGAIN");
        delay(1000);
        INPASS1();
      }
   cpass1 = 1;
   EEPROM.write(1, cpass1);
   lcd.setCursor(0,0);
   lcd.print("PLEASE PUT YOUR ITEM");
   lcd.setCursor(4,1);
   lcd.print("INTO THE BOX");
   lcd.setCursor(2,2);
   lcd.print("WITHIN 30 SECOND");
   delay(2000);
   LOCK1();
}


void INPASS2(){
  char inpass2x[4]; 
  lcd.clear();
  lcd.setCursor(3,1);
  lcd.print("ENTER PASSWORD");
  lcd.setCursor(8,2);
  int a=0;
  int a1=20;
  while (a<4){
      char key1=keypad_key.getKey();
      if (key1){
        inpass2[a]=key1;
        lcd.print("*");
        EEPROM.write(a1,key1);
        a++;
        a1++;
      }
    }
    delay(500);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("ENTER PASSWORD AGAIN");
  lcd.setCursor(8,2);
  int b=0;
  while(b<4){
    char key2=keypad_key.getKey();
    if (key2){
    inpass2x[b]=key2;
    lcd.print("*");
    b++;
    }
  }
  delay(500);
       if(!(strncmp(inpass2, inpass2x, 4))){
        lcd.clear();
        lcd.setCursor(6,1);
        lcd.print("SUCCESS");
        delay(1000);
        }
       else {
        lcd.clear();
        lcd.setCursor(2,1);
        lcd.print("PLEASE TRY AGAIN");
        delay(1000);
        INPASS2();
      }
   cpass2 = 1;
   EEPROM.write(2, cpass2);
   lcd.setCursor(0,0);
   lcd.print("PLEASE PUT YOUR ITEM");
   lcd.setCursor(4,1);
   lcd.print("INTO THE BOX");
   lcd.setCursor(2,2);
   lcd.print("WITHIN 30 SECOND");
   delay(2000);
   LOCK2();
}



void OUTPASS1(){
 lcd.clear();
 lcd.setCursor(3,1);
 lcd.print("ENTER PASSWORD");
 lcd.setCursor(8,2);
 do{
 key_pressed1=keypad_key.getKey();
  if(key_pressed1){
    outpass1[i1++]=key_pressed1;
    lcd.print("*");
    if (i1==4) {
      delay(200);
      for (int a=0; a<4; a++){
       int a1=10;
        inpass1[a] = EEPROM.read(a1);
        a1++;
      }
      if (!(strncmp(outpass1, inpass1, 4)))
      {
        lcd.clear();
        lcd.setCursor(6,1);
        lcd.print("SUCCESS");
        i1 = 0;
        UNLOCK1();
      }
      else {
        lcd.clear();
        lcd.setCursor(3,1);
        lcd.print("WRONG PASSWORD");
        lcd.setCursor(0,1);
        delay(1000);
        i1 = 0;
        LOGOUT();
      }
    }
  }
 }
        while(i1<4);
      
    }

void OUTPASS2(){
 lcd.clear();
 lcd.setCursor(3,1);
 lcd.print("ENTER PASSWORD");
 lcd.setCursor(8,2);
 do{
 key_pressed2=keypad_key.getKey();
  if(key_pressed2){
    outpass2[i2++]=key_pressed2;
    lcd.print("*");
    if (i2==4) {
      delay(200);
      for (int a=0; a<4; a++){
        int a1=20;
        inpass2[a] = EEPROM.read(a1);
        a1++;
      }
      if (!(strncmp(outpass2, inpass2, 4)))
      {
        lcd.clear();
        lcd.setCursor(6,1);
        lcd.print("SUCCESS");
        i2 = 0;
        UNLOCK2();
      }
      else {
        lcd.clear();
        lcd.setCursor(3,1);
        lcd.print("WRONG PASSWORD");
        lcd.setCursor(0,1);
        delay(1000);
        i2 = 0;
        LOGOUT();
      }
    }
  }
 }
        while(i2<4);
      
    }

    void START1(){
  lcd.clear();
  while(1){
  if(!(keypad_key.getKey()==NO_KEY)){
    START2();
  }
  else{
  lcd.setCursor(5,0);  
  lcd.print(":WELCOME:");
  lcd.setCursor(4,1);
  lcd.print("SMART LOCKER");
  lcd.setCursor(2,2);
  lcd.print("PRESS ANY TO START");
  lcd.setCursor(0,3);
  lcd.print("____________________");
}
}
}

void START2(){
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("PRESS 1 TO LOGIN");
  lcd.setCursor(2,1);
  lcd.print("PRESS 2 TO LOGOUT");
  lcd.setCursor(2,2);
  lcd.print("PRESS 0 TO RETURN");
  while(1){
    if(keypad_key.getKey()=='1'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("LOGIN");
    delay(500); 
    LOGIN();
    }
    else if(keypad_key.getKey()=='2'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("LOGOUT");
    delay(500); 
    LOGOUT();
    }
    else if(keypad_key.getKey()=='0'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("RETURN");
    delay(500); 
    START1();
    }
    }
}

void LOGIN(){
  lcd.clear();
  lcd.setCursor(1,1);
  lcd.print("CHOOSE EMPTY SLOT");
  lcd.setCursor(1,2);
  lcd.print("PRESS 0 TO RETURN");
  while(1){
    if(keypad_key.getKey()=='1'){
    lcd.clear();
    lcd.setCursor(8,1);
    lcd.print("BOX1");
    delay(500); 
    INBOX1();
    }
    else if(keypad_key.getKey()=='2'){
    lcd.clear();
    lcd.setCursor(8,1);
    lcd.print("BOX2");
    delay(500); 
    INBOX2();
    }
    else if(keypad_key.getKey()=='0'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("RETURN");
    delay(500); 
    START2();
}
}
}

void INBOX1(){
  lcd.clear();
  if(r1==0){
    lcd.setCursor(3,1);
    lcd.print("NOT AVAILABLE");
    delay(1000);
    LOGIN();
  }
  else if(r1==1){
    lcd.setCursor(5,1);
    lcd.print("AVAILABLE");
    delay(1000);
    HR1();
  }
  }
  
void INBOX2(){
  lcd.clear();
  if(r2==0){
    lcd.setCursor(3,1);
    lcd.print("NOT AVAILABLE");
    delay(1000);
    LOGIN();
  }
  else if(r2==1){
    lcd.setCursor(5,1);
    lcd.print("AVAILABLE");
    delay(1000);
    HR2();
  }
  }

void HR1(){
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("PLEASE CHOOSE");
  lcd.setCursor(0,1);
  lcd.print("THE NUMBER OF HOURS");
  lcd.setCursor(7,2);
  lcd.print("1 OR 2");
  lcd.setCursor(1,3);
  lcd.print("PRESS 0 TO RETURN");
while(1){
    if(keypad_key.getKey()=='1'){
    hours11=1;
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("1 HOUR");
    delay(500); 
    PAY1();
    }
    else if(keypad_key.getKey()=='2'){
    hours12=1;
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("2 HOUR");
    delay(500); 
    PAY1();
    }
    else if(keypad_key.getKey()=='0'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("RETURN");
    delay(500); 
    LOGIN();
    }
    }
  }

void HR2(){
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("PLEASE CHOOSE");
  lcd.setCursor(0,1);
  lcd.print("THE NUMBER OF HOURS");
  lcd.setCursor(7,2);
  lcd.print("1 OR 2");
  lcd.setCursor(1,3);
  lcd.print("PRESS 0 TO RETURN");
while(1){
    if(keypad_key.getKey()=='1'){
    hours21=1;
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("1 HOUR");
    delay(500); 
    PAY2();
    }
    else if(keypad_key.getKey()=='2'){
    hours22=1;
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("2 HOUR");
    delay(500); 
    PAY2();
    }
    else if(keypad_key.getKey()=='0'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("RETURN");
    delay(500); 
    LOGIN();
    }
    }
  }

void PAY1(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TID WAI GON1");
  delay(500);
  if(hours11==1){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("TIMER RUN 1 HR");
    delay(500);
  }
  if(hours12==1){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("TIMER RUN 2 HR");
    delay(500);
  }
  INPASS1();
}

void PAY2(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TID WAI GON2");
  delay(500);
  if(hours21==1){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("TIMER RUN 1 HR");
    delay(500);

  }
  if(hours22==1){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("TIMER RUN 2 HR");
    delay(500);
  }
  INPASS2();
}

void LOGOUT(){
  lcd.clear();
  lcd.setCursor(2,1);
  lcd.print("CHOOSE YOUR SLOT");
  lcd.setCursor(2,2);
  lcd.print("PRESS 0 TO RETURN");
while(1){
    if(keypad_key.getKey()=='1'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("BOX1");
    delay(500); 
    OUTBOX1();
    }
    else if(keypad_key.getKey()=='2'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("BOX2");
    delay(500); 
    OUTBOX2() ;
    
    }
    else if(keypad_key.getKey()=='0'){
    lcd.clear();
    lcd.setCursor(7,1);
    lcd.print("RETURN");
    delay(500); 
    START2();
    }
    }
}

void OUTBOX1() {
  lcd.clear();
  if(r1==1) {
  lcd.setCursor(3,1);
  lcd.print("NOT AVAILABLE");
  delay(1000) ;
  LOGOUT() ;
 }

  else if (r1==0) {
    lcd.setCursor(5,1);
    lcd.print("AVAILABLE");
    delay(1000);
    OUTPASS1();
  }
}

void OUTBOX2() {
  lcd.clear();
  if(r2==1) {
  lcd.setCursor(3,1);
  lcd.print("NOT AVAILABLE");
  delay(1000) ;
  LOGOUT() ;
 }
  else if(r2==0) {
    lcd.setCursor(5,1);
    lcd.print("AVAILABLE");
    delay(1000);
    OUTPASS2();
  }
}


void loop(){
  
}
  
void LOCK1(){
  digitalWrite(10,LOW);
  r1=0;
  EEPROM.write(3, r1);
  lcd.clear();
  lcd.setCursor(5,1);
  lcd.print("THANK YOU");
  delay(1000);
  START1();
}
void UNLOCK1(){
  digitalWrite(10,HIGH);
  r1=1;
  EEPROM.write(4, r2);
  lcd.clear();
  lcd.setCursor(5,1);
  lcd.print("THANK YOU");
  delay(1000);
  START1();
  cpass1=0;
}

void LOCK2(){
  digitalWrite(11,LOW);
  r2=0;
  lcd.clear();
  lcd.setCursor(5,1);
  lcd.print("THANK YOU");
  delay(1000);
  START1();
}
void UNLOCK2(){
  digitalWrite(11,HIGH);
  r2=1;
  lcd.clear();
  lcd.setCursor(5,1);
  lcd.print("THANK YOU");
  delay(1000);
  START1();
  cpass2=0;
}
