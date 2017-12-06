/***********************************************
1. SPI Interface Inatruction
      clockPin --> SCK(EN)
      latchPin --> CS(RS)
      dataPin --> SID(RW)
 2. Connection:
    1)Turn the BL_ON Switch to the "ON" side;
    2)Turn the PBS_ON Switch to the "SPI" side

Method1:
      LCD                   Arduino
      EN                 Digital Pin 2
      RS                 Digital Pin 7
      RW                 Digital Pin 10
      VCC                     5V
      GND                     GND;

Method2:
      LCD                          Arduino
      SCK       clockPin(defined in the "initDriverPin" function)
      CS        latchPin(defined in the "initDriverPin" function)
      SID       dataPin (defined in the "initDriverPin" function)
      VCC                            5V
      GND                           GND
***********************************************/

#include "LCD12864RSPI.h"
#include "DFrobot_bmp.h"
#include "DFrobot_char.h"

#define AR_SIZE( a ) sizeof( a ) / sizeof( a[0] )

//#define OUTPUT_ARRAY_SIZE 8
//uint8_t outputArray[OUTPUT_ARRAY_SIZE];

unsigned char DI_pincount = 8;  // digital pin count
int DI_Raw[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };    // actual digital output pins
int DI_Val[8];                                         // digital output vars
String dato;

boolean toggle = true;
boolean toggleAgitate = true;
boolean toggleSpin = true;
boolean toggleStart = true;

int lcd_key     = 0;
int adc_key_in  = 0;
#define btnMENU  0
#define btnOP1   1
#define btnOP2   2
#define btnOP3   3
#define btnSELECT 4
#define btnNONE 5


// set input pin numbers:
const int MenuButton = A0;    // the number of the pushbutton pin
const int OptionButtonA = A1;    // the number of the pushbutton pin
const int OptionButtonB = A2;    // the number of the pushbutton pin
const int OptionButtonC = A3;    // the number of the pushbutton pin

// set output pin numbers:
const int RinseWash2Relays = 2;    // the number of the Rinse and Wash Relays pin (0=Rinse, 1= Wash)
const int HiMotor2Relays = 3;      // the number of the Hi Motor Speed Relays pin
const int LoMotorRelay = 4;        // the number of the Low Motor Speed Relay pin
const int Agitate2Relays = 5;      // the number of the Agitate function Relays pin
const int Spin2Relays = 6;         // the number of the Spin function Relays pin
const int Bypass2Relays = 7;       // the number of the Bypass Relays pin
const int FillRelay = 8;           // the number of the Fill function Relay pin

// Variables will change:
int Rinse = LOW;         // the current state of the output pin
int Wash = HIGH;             // the current reading from the input pin
int HiMotorOn = HIGH;   // the previous reading from the input pin
int HiMotorOff = LOW;   // the previous reading from the input pin
int LowMotorHIGH = HIGH;   // the previous reading from the input pin
int LowMotorOff = LOW;   // the previous reading from the input pin
int AgitateOn = HIGH;   // the previous reading from the input pin
int AgitateOff = LOW;   // the previous reading from the input pin
int SpinOn = HIGH;   // the previous reading from the input pin
int SpinOff = LOW;   // the previous reading from the input pin
int BypassOn = HIGH;   // the previous reading from the input pin
int BypassOff = LOW;   // the previous reading from the input pin
int FillOn = HIGH;   // the previous reading from the input pin
int FillOff = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

//unsigned char wangzhi[]=" www.DFRobot.cn ";//
//unsigned char en_char1[]="ST7920 LCD12864 ";// 
//unsigned char en_char2[]="Test, Copyright ";// 
//unsigned char en_char3[]="by DFRobot ---> ";// 

String en_char1=" MENU PRINCIPAL ";// 
String en_char2="SOY  NANY     ->";// 
String en_char3="SOY  NEFTALI  ->";// 
String en_char4="SOY  MONTSE   ->";//

//unsigned char en_char1[]=" LUIS SAUL MENU ";// 
//unsigned char en_char2[]="agitate setup ->";// 
//unsigned char en_char3[]="spin setup    ->";// 
//unsigned char en_char4[]="start now     ->";//

//String en_char1=" LUIS SAUL MENU ";// 
//String en_char2="agitate setup ->";// 
//String en_char3="spin setup    ->";// 
//String en_char4="start now     ->";//

//unsigned char se_char1[]=" LUIS SAUL MENU ";// 
//unsigned char se_char2[]="AGITATE SETUP ->";// 
//unsigned char se_char3[]="SPIN SETUP    ->";// 
//unsigned char se_char4[]="START NOW     ->";//


  int mainMenu = 0;//  " MENU PRINCIPAL ";
    int nanyMenu = 1;//  "  NANY  LAVADO  ";
    int taliMenu = 2;//  "  TALI  LAVADO  ";
    int montseMenu = 3;//" MONTSE  LAVADO ";


  int setupMenu = 4;// "  SETUP  MENU   ";  
    int luisMenu = 5;//  " LUIS SAUL MENU ";
    int outputMenu = 6;//"     OUTPUT     ";
    int settingsMenu = 7;//"    SETINGS     ";
      int washMenu = 8;//"   WASH  TIME   ";
      int rinseMenu = 9;//"   RINSE TIME   ";
      int cyclingMenu = 10;//"    CYCLING     ";
  
  int menuActual = 0;
  int menuAnterior = 0;


void setup()
{
  //LCDA.initDriverPin(2,7,10);
  LCDA.initDriverPin(11,12,13); 
  LCDA.Initialise(); // INIT SCREEN  
  delay(100);
  LCDA.DrawFullScreen(logo);//LOGO
  delay(1000);
  LCDA.CLEAR();//����
  delay(100);

  LCDA.DisplayStr(0,0,en_char1,16);//
  delay(10);
  LCDA.DisplayStr(1,0,en_char2,16);//
  delay(10);
  LCDA.DisplayStr(2,0,en_char3,16);//
  delay(10);
  LCDA.DisplayStr(3,0,en_char4,16);//
  delay(500);
  //LCDA.CLEAR();//����

  menuActual = mainMenu;
  menuAnterior = btnNONE;


  //Setup Pins

  pinMode(MenuButton, INPUT);
  pinMode(OptionButtonA, INPUT);
  pinMode(OptionButtonB, INPUT);
  pinMode(OptionButtonC, INPUT);
  
  pinMode(RinseWash2Relays, OUTPUT);
  pinMode(HiMotor2Relays, OUTPUT);
  pinMode(LoMotorRelay, OUTPUT);
  pinMode(Agitate2Relays, OUTPUT);
  pinMode(Spin2Relays, OUTPUT);
  pinMode(Bypass2Relays, OUTPUT);
  pinMode(FillRelay, OUTPUT);

  // set initial output state

  digitalWrite(RinseWash2Relays, Wash);
  digitalWrite(HiMotor2Relays, HiMotorOff);
  digitalWrite(LoMotorRelay, LowMotorOff);
  digitalWrite(Agitate2Relays, AgitateOff);
  digitalWrite(Spin2Relays, SpinOff);
  digitalWrite(Bypass2Relays, BypassOff);
  digitalWrite(FillRelay, FillOff);

  
}

void loop()
  {

    // Read input pins into var array
    for(char i=0; i<DI_pincount; i++) {  
     DI_Val[i] = digitalRead(DI_Raw[i]);
     }  

  dato = String(DI_Val[7]) + String(DI_Val[6]) + String(DI_Val[5]) + String(DI_Val[4]) + String(DI_Val[3]) + String(DI_Val[2]) + String(DI_Val[1]) + String(DI_Val[0]);

  //input_cr=(char*) &outputArray[1];

  //LCDA.DisplayString(3,0,en_char4,16);//
  //LCDA.DisplayString(3,0, input_cr,5);//
  
  //menuActual = luisMenu;
  //show_menu(menuActual);

  //delay(100);
  //LCDA.DisplayString(0,0,show1,16);//
  //delay(10);
  //LCDA.DisplayString(1,0,show2,16);//
  //delay(10);
  //LCDA.DisplayString(2,0,show3,16);//
  //delay(10);
  //LCDA.DisplayString(3,0,wangzhi,16);//LOGO
  //delay(5000);

  //lcd_key = read_LCD_buttons();
  read_LCD_buttons();
  if(menuAnterior != lcd_key)
  {
  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
   case btnMENU:
     {
     if (menuActual == mainMenu || menuActual == setupMenu)
     {if (menuActual == mainMenu && toggle == true)  {menuActual = setupMenu; show_menu(menuActual);}
     if (menuActual == setupMenu && toggle == false) {menuActual = mainMenu; show_menu(menuActual);}
     toggle = !toggle;}

     if (menuActual == luisMenu)  {menuActual = setupMenu; show_menu(menuActual);}
     
     //if (menuActual == setupMenu || menuActual == luisMenu)
     //{if (menuActual == setupMenu && toggle == true)  menuActual = luisMenu; show_menu(menuActual);
     //if (menuActual == luisMenu && toggle == false)  menuActual = setupMenu; show_menu(menuActual);
     //toggle = !toggle;}
     
     //if (menuActual == setupMenu || menuActual == luisMenu)
     //if (menuActual == setupMenu && toggle == true)  menuActual = luisMenu; show_menu(menuActual);

     
     //show_menu(luisMenu);
     //menuAnterior=btnMENU;
     //lcd.print("RIGHT ");
     break;
     }
   case btnOP1:
     {
     //if (menuActual == setupMenu || menuActual == luisMenu)
     if (menuActual == setupMenu)  {menuActual = luisMenu; show_menu(menuActual);}
     
     if (menuActual == luisMenu) {agitate_setup();}
     //if (menuActual == luisMenu && toggleAgitate == false) {LCDA.DisplayStr(1,0, "agitate setup ->",16); toggleAgitate = !toggleAgitate;}
     
     //if (menuActual == luisMenu) //  && toggleAgitate == true
     //{LCDA.DisplayStr(1,0, "agitate setup ->",16);} // toggleAgitate = !toggleAgitate;
     //if (menuActual == luisMenu && toggle == false)  menuActual = setupMenu; show_menu(menuActual);
     //toggle = !toggle;}


     //lcd.print("LEFT   ");
     break;
     }
   case btnOP2:
     {

     if (menuActual == luisMenu) {spin_setup();}
     
     //lcd.print("UP    ");
     break;
     }
   case btnOP3:
     {
     
     if (menuActual == luisMenu) {start();}
     
     //lcd.print("DOWN  ");
     break;
     }
   case btnSELECT:
     {
     //lcd.print("SELECT");
     break;
     }
     case btnNONE:
     {
     //lcd.print("NONE  ");
     //menuAnterior=btnNONE;
     break;
     }

        }   //se cierra switch
  
  //if (menuActual == mainMenu && lcd_key == btnMENU) menuActual = luisMenu; show_menu(menuActual);
  //if (menuActual == luisMenu && lcd_key == btnMENU) menuActual = mainMenu; show_menu(menuActual);
  
  menuAnterior = lcd_key;
     }    //se cierra if

  //if (menuActual == mainMenu && lcd_key == btnMENU) menuActual = luisMenu; show_menu(menuActual);
    //if(lcd_key == btnMENU) show_menu(luisMenu);

    //show_menu(luisMenu);
    //LCDA.DisplayStr(3,0, String(lcd_key),8);//

  }  //Finaliza loop principal



void show_menu(int menu)
{
    
  //String hi=menu;
  //LCDA.DisplayStr(2,0, menu,16);//
  //LCDA.DisplayStr(3,0, "QUE TAL",16);//
  

  if (menu == mainMenu)
  {

    en_char1=" MENU PRINCIPAL ";// 
    en_char2="SOY  NANY     ->";// 
    en_char3="SOY  NEFTALI  ->";// 
    en_char4="SOY  MONTSE   ->";//


    LCDA.DisplayStr(0,0,en_char1,16);//
    delay(10);
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(50);

    //LCDA.DisplayStr(3,0, dato,8);//
    //delay(100);
   }

  if (menu == setupMenu)
  {
  
    en_char1 = "  SETUP  MENU   ";// 
    en_char2 = "SAUL MENU     ->";// 
    en_char3 = "OUTPUT        ->";// 
    en_char4 = "SETINGS       ->";//

    LCDA.DisplayStr(0,0,en_char1,16);//
    delay(10);
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(50);

    //LCDA.DisplayStr(3,0, dato,8);//
    //delay(100);
    
  }


  if (menu == luisMenu)
  {
  
    en_char1 = " LUIS SAUL MENU ";// 
    en_char2 = "agitate setup ->";// 
    en_char3 = "spin setup    ->";// 
    en_char4 = "start now     ->";//

    LCDA.DisplayStr(0,0,en_char1,16);//
    delay(10);
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(50);

    //LCDA.DisplayStr(3,0, dato,8);//
    //delay(100);
    
  }



  //if (tipo == "     OUTPUT     ")
  //{
  //LCDA.WriteByte(3,0, input_cr,16);//
  //LCDA.DisplayStr(3,0, dato,8);//
  //delay(100);
  //}

  //unsigned char en_char1[]=" LUIS SAUL MENU ";// 
  //unsigned char en_char2[]="agitate setup ->";// 
  //unsigned char en_char3[]="spin setup    ->";// 
  //unsigned char en_char4[]="start now     ->";//

  

}



// read the buttons
//int read_LCD_buttons()
void read_LCD_buttons()
{
 
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) lcd_key = btnNONE; //return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 10)  lcd_key = btnMENU; //return btnMENU;  
 if (adc_key_in < 145 && adc_key_in > 140) lcd_key = btnOP1; //return btnOP1; 
 if (adc_key_in < 295 && adc_key_in > 290) lcd_key = btnOP2; // return btnOP2; 
 if (adc_key_in < 446 && adc_key_in > 444) lcd_key = btnOP3; // return btnOP3; 
 if (adc_key_in < 595 && adc_key_in > 590) lcd_key = btnSELECT; // return btnSELECT;  

 // For V1.0 comment the other threshold and use the one below:
 /*
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
 */
 
 //return btnNONE;  // when all others fail, return this...
}


void agitate_setup()
{
    if(toggleAgitate == true)
    {en_char2 = "agitate setup ->";// 
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    en_char4 = "start now     ->";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);}
    else
    {en_char2 = "AGITATE SETUP ->";// 
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    en_char4 = "START NOW     ->";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);}

    toggleAgitate = !toggleAgitate;
}

void spin_setup()
{
    if(toggleSpin == true)
    {en_char3 = "spin setup    ->";// 
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    en_char4 = "start now     ->";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);}
    else
    {en_char3 = "SPIN SETUP    ->";// 
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    en_char4 = "START NOW     ->";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);}

    toggleSpin = !toggleSpin;
}

void start()
{
    if(toggleStart == true)
    {en_char4 = "start now     ->";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);}
    else
    {en_char4 = "STOP NOW      ->";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);}

    toggleStart = !toggleStart;
}

//*****************  Codigo de Prueba   ***********************************
  //byte data = 0;
  // loop through the sensors
  //for (int sensorCount = 0; sensorCount < sizeof(sensors); sensorCount++)
  //{
    // read them and add them to the data byte in the correct position
    //data |= (digitalRead(sensors[sensorCount]) << sensorCount);
  //}

  // Define 
  //String str = "This is my string"; 

  // Length (with one extra character for the null terminator)
  //int str_len = str.length() + 1; 

  // Prepare the character array (the buffer) 
  //char char_array[str_len];

  // Copy it over 
  //str.toCharArray(char_array, str_len);

  //uint8_t outputArray[] = {1,0,1,0,1,0,1,0};

  //String str = (char*)outputArray;
  
  //char charBuf[8];
  //charBuf[1]=(char*)outputArray[1];
  //charBuf= {'0','0','0'};
  //str.toCharArray(outputArray, 8);


  //unsigned char en_char4 = str;//
  //en_char4[]="HOLA";//

  //int reading = digitalRead(buttonPin);
  //for (int i = 2; i < 9; i++){
  //outputArray[(i - 2) / 8] |= (!digitalRead(i)) << ((i - 2) % 8);
  //}


  //boolean digValue = digitalRead(2);
  //unsigned char* unbit='0';
  

  //uint8_t unbit[8];  
  
  //byte unbit;
  //const byte sensors[] = { 2, 3, 4, 5, 6, 7, 8, 9};
  //for (int i = 0; i < 8; i++)
  //{
    // read them and add them to the data byte in the correct position
  //  unbit |= (digitalRead(sensors[i]) << i);
  //}

  //uint8_t unbit[] = "10101011";
  //uint8_t unbit[] = "10101011";


  
  //String str = (char*)unbit;
  //outputArray[0] = "10000001";
  
  //char charBuf[3]="000";
  //char b[8] = {char(DI_Val[0]) , char(DI_Val[1]) , char(DI_Val[2]) , char(DI_Val[3]) , char(DI_Val[4]) , char(DI_Val[5]) , char(DI_Val[6]) , char(DI_Val[7])};
  //String str;
  //str=String(a);
  //str.toCharArray(b,8);

  //int n=186;
  //int segundos = n % 60  ;
  //int minutos =  n / 60  ;
  //String S = String(minutos) + ":" + String(segundos);

  // char charBuf[8]="00000000";
  //charBuf[1]=(char*)outputArray[1];
  //charBuf[]= "00000000";

  //char *input_cr;
  //input_cr=(char*) &unbit;
  //input_cr=(char*) &b;

