#include <NewTone.h>
#include <Wire.h>
#include <RTCHelper.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <Timer.h>
#include "constants.h"

#define DS3231_I2C_ADDRESS  0x68
#define LCD_I2C_ADDRESS     0x3F
#define respAlarm1             5

RTCHelper rtc(DS3231_I2C_ADDRESS);
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
IRrecv irrecv(11);

decode_results results;

boolean stateDate = false;
boolean stateEdition = false;

Timer timer;
int timerHour;
int timerDate;
int timerDisplay;
int timerTrateInput;

String input = "";

char ss = 'A';
boolean ts = false;
boolean aa = false;
int qa = 0;
byte alarm = LOW;

void showDateOrTemp()
{
  stateDate = !stateDate;
  if(stateDate)
  {
    lcd.setCursor(3,1);
    lcd.print(rtc.getDate());
  }
  else
  {
    lcd.setCursor(3,1);
    lcd.print(" ");
    lcd.write(byte(0));
    lcd.print(rtc.readTemperature(), 2);
    lcd.write(byte(2));
    lcd.print("C  ");
  }
}

void showHour()
{
  String hour = rtc.getTime();
  if (hour.length() == 8)
    lcd.setCursor(3,0);
  else
    lcd.setCursor(2,0);
  lcd.write(byte(1));
  lcd.print(hour);
}

void applyInput(String value, int n)
{
  int a1, a2, a3, f;
  char ap = ' ';
  char string[20];
  value.toCharArray(string, 20);
  char* format24 = "%d:%d:%d";
  char* format12 = "%cM %d:%d:%d";
  char* formatDate = "%d/%d/%d";
  if(n == 1){
    if (value.length() == 8 && value.charAt(2) == ':')
    {
      sscanf(string, format24, &a1, &a2, &a3);
      rtc.setHour(a1, a2, a3, -1);
    }
    else if (value.length() == 11)
    {
      sscanf(string, format12, &ap, &a1, &a2, &a3);
      if (ap == 'A')
        f = 0;
      else
        f = 1;
      rtc.setHour(a1, a2, a3, f);
    }
    else
    {
       if (value.length() == 8 && value.charAt(2) == '/')
      {
        sscanf(string, formatDate, &a1, &a2, &a3);
        rtc.setDate(a1, a2, a3);
      }
    }
  }
  else{
    if(qa == 1){
      if (value.length() == 8 && value.charAt(2) == ':')
      {
        sscanf(string, format24, &a1, &a2, &a3);
        rtc.setAlarm(a1, a2, a3, -1, 1);
      }
      else if (value.length() == 11)
      {
        sscanf(string, format12, &ap, &a1, &a2, &a3);
        if (ap == 'A')
          f = 0;
        else
          f = 1;
        rtc.setAlarm(a1, a2, a3, f, 1);
      }
    }
    else if(qa == 2){
      if (value.length() == 8 && value.charAt(2) == ':')
      {
        sscanf(string, format24, &a1, &a2, &a3);
        rtc.setAlarm(a1, a2, a3, -1, 2);
      }
      else if (value.length() == 11)
      {
        sscanf(string, format12, &ap, &a1, &a2, &a3);
        if (ap == 'A')
          f = 0;
        else
          f = 1;
        rtc.setAlarm(a1, a2, a3, f, 2);
      }
    }
    qa = 0;
  }
}

String configure(int type)
 {
   String result;
   switch(type)
   {
     case 0: // AM/PM
       result = "[A|P]M XX:XX:XX ";
       break;
     case 1: // 24H
       result = "XX:XX:XX        ";
       break;
     case 2: // DATE
       result = "XX/XX/XX        ";
       break;
     default:
       result = "ERROR           ";
       break;
   }
   return result;
}

void startLcd()
{
  showHour();
  showDateOrTemp();
  timerHour = timer.every(900, showHour);
  timerDate = timer.every(4500, showDateOrTemp);
}

void configureLCD()
{
  byte tempChar[] = { B00100, B01010, B01010, B01010, B01110, B11111, B11111, B01110 };
  byte relChar[] = { B00000, B00000, B01110, B10001, B10111, B10101, B01110, B00000 };
  byte grausChar[] = { B00000, B01110, B01010, B01110, B00000, B00000, B00000, B00000 };
  lcd.begin(16, 2);
  lcd.createChar(0, tempChar);
  lcd.createChar(1, relChar);
  lcd.createChar(2, grausChar);
  lcd.setBacklight(HIGH);
  lcd.clear();
}

void soa()
{
  Serial.println("STILL DRE");

  int pin_tone = 4;
  int melody[] = { NOTE_C5, NOTE_E5, NOTE_A5, NOTE_B4, NOTE_G5 };
  int noteDurations[] = { 8,8,8 };

  for (int songLoop = 0; songLoop < 3; songLoop++){
    for (int loop1 = 0; loop1 < 8; loop1++){
      for (int thisNote = 0; thisNote < 3; thisNote++) {
        int noteDuration = 1000 / noteDurations[thisNote];
        NewTone(pin_tone, melody[thisNote], noteDuration);

        int pauseBetweenNotes = noteDuration * 0.5;
        delay(pauseBetweenNotes);
        noNewTone(pin_tone);
      }
      delay(150);
    }
    for (int loop2 = 0; loop2 < 3; loop2++){

      NewTone(pin_tone, NOTE_B4, 125);
      delay(62.5);
      noNewTone(pin_tone);

      NewTone(pin_tone, NOTE_E5, 125);
      delay(62.5);
      noNewTone(pin_tone);

      NewTone(pin_tone, NOTE_A5, 125);
      delay(62.5);
      noNewTone(pin_tone);

      delay(150);
    }
    for (int loop3 = 0; loop3 < 5; loop3++){
      NewTone(pin_tone, NOTE_B4, 125);
      delay(62.5);
      noNewTone(pin_tone);

      NewTone(pin_tone, NOTE_E5, 125);
      delay(62.5);
      noNewTone(pin_tone);

      NewTone(pin_tone, NOTE_G5, 125);
      delay(62.5);
      noNewTone(pin_tone);
      delay(150);
    }
  }
}

void showFormat12h()
{
  lcd.home();
  lcd.print(configure(0));
}

void showFormat24h()
{
  lcd.home();
  lcd.print(configure(1));
}

void showFormatDate()
{
  lcd.home();
  lcd.print(configure(2));
}

void readHourFromRemote()
{
  timer.stop(timerHour);
  timer.stop(timerDate);
  lcd.clear();
  lcd.home();
  lcd.print("Hora no formato:");
  if (rtc.getTime().charAt(1) == 'M')
  {
    timer.after(2500, showFormat12h);
    input = "AM ";
    ts = true;
    timerTrateInput = timer.every(500, tratehour12);
  }
  else
  {
    timer.after(2500, showFormat24h);
    timerTrateInput = timer.every(500, tratehour24);
  }
  timerDisplay = timer.every(500, showInput);
}

void readDateFromRemote()
{
  timer.stop(timerHour);
  timer.stop(timerDate);
  lcd.clear();
  lcd.home();
  lcd.print("Data no formato:");
  timer.after(2500, showFormatDate);
  timerTrateInput = timer.every(500, tratedate);
  timerDisplay = timer.every(500, showInput);
}

void showInput()
{
  lcd.setCursor(0,1);
  lcd.print(input);
  lcd.cursor();
  lcd.blink();
}

void trateInput()
{
  if(input.length() == 2 || input.length() == 5)
    input += ":";
}

void tratedate()
{
  if (input.endsWith("/<"))
  {
    lcd.clear();
    input = input.substring(0, input.length()-3);
  }
  if (input.endsWith("<"))
  {
    lcd.clear();
    input = input.substring(0, input.length()-2);
  }
  if(input.length() == 2 || input.length() == 5)
    input += "/";
  if(input.length() > 8)
    input = input.substring(0, 8);
}

void tratehour24()
{
  if (input.endsWith(":<"))
  {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    input = input.substring(0, input.length()-3);
  }
  if (input.endsWith("<"))
  {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    input = input.substring(0, input.length()-2);
  }
  if(input.length() == 2 || input.length() == 5)
    input += ":";
  if(input.length() > 8)
    input = input.substring(0, 8);
}

void tratehour12()
{
  input.setCharAt(0, ss);
  if (input.endsWith(" <"))
    input = input.substring(0, input.length()-1);
  else if (input.endsWith(":<"))
  {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    input = input.substring(0, input.length() -3);
  }
  else if (input.endsWith("<"))
  {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    input = input.substring(0, input.length() -2);
  }
  if (input.length() == 5 || input.length() == 8)
    input += ":";
  if (input.length() > 11)
    input = input.substring(0,11);
}

void restartNormal()
{
  ts = false;
  //verifica se a hora foi alterada
  if(stateEdition == true){
    applyInput(input, 1);
    stateEdition = false;
  }
  //verifica se o alarme foi alterado
  else if(aa == true){
    applyInput(input, 2);
    aa = false;
  }
  input = "";
  timer.stop(timerTrateInput);
  timer.stop(timerDisplay);
  lcd.clear();
  lcd.noCursor();
  lcd.noBlink();
  startLcd();
}

void vaiSoar() 
{
  timer.after(10, soa);
}
void setup() {
  Wire.begin();
  Serial.begin(9600);
  irrecv.enableIRIn();

  configureLCD();
  startLcd();

  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  //interrupção quando pino 3 vai de 0 para 1
  attachInterrupt(digitalPinToInterrupt(3), vaiSoar, FALLING);


  rtc.write(STATUS_REG, (rtc.read(STATUS_REG) & 0xFC));
}

void loop()
{
  timer.update();
  if (irrecv.decode(&results))
  {
    switch(results.value)
    {
      case IR_OK: irOkAction(); break;
      case IR_UP: irUpAction(); break; // troca a visualizaçao entre 12h e 24h
      case IR_DOWN: irDownAction(); break; // troca a visualizaçao entre data e temperatura
      case IR_RIGHT: irRightAction(); break; //botão setar alarm
      case IR_LEFT: input += "<"; break;
      case IR_0: input += "0"; break;
      case IR_1: input += "1"; break;
      case IR_2: input += "2"; break;
      case IR_3: input += "3"; break;
      case IR_4: input += "4"; break;
      case IR_5: input += "5"; break;
      case IR_6: input += "6"; break;
      case IR_7: input += "7"; break;
      case IR_8: input += "8"; break;
      case IR_9: input += "9"; break;
      case IR_ASTERISCO: irAsteriscoAction(); break; // alterar hora
      case IR_TRALHA: irTralhaAction(); break; // alterar data
      default: break;
    }
    delay(200);
    irrecv.resume();
  }
}

void irUpAction()
{
  if (ts)
    if (ss == 'A')
      ss = 'P';
    else
      ss = 'A';
  else
  {
    rtc.changeFormatHour();
    lcd.setCursor(0,0);
    lcd.print("                ");
  }
}

void irRightAction()
{
  if(qa == 1)
    qa = 2;
  else
    qa = 1;
  aa = true;
  readHourFromRemote();
}

void irDownAction()
{
  showDateOrTemp();
  timer.stop(timerDate);
  timerDate = timer.every(5000, showDateOrTemp);
}

void irOkAction()
{
  rtc.write(STATUS_REG, (rtc.read(STATUS_REG) & 0xFC));
  rtc.write(STATUS_REG, (rtc.read(STATUS_REG) | 0x03));
  restartNormal();
}

void irAsteriscoAction()
{
  stateEdition = true;
  readHourFromRemote();
}

void irTralhaAction()
{
  stateEdition = true;
  readDateFromRemote();
}
