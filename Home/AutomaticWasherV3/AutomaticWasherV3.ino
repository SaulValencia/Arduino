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
      EN                 Digital Pin 11
      RS                 Digital Pin 12
      RW                 Digital Pin 13
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
boolean toggleAgitate = false;
boolean toggleSpin = false;
boolean toggleStart = false;

boolean toggleLavadoRapido = false;
boolean toggleLavadoLento = false;
boolean toggleInicio = false;

int indexCursor = 1;



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
const int WashRelay = 2;           // the number of the Wash Relay pin
const int RinseRelay = 3;          // the number of the Rinse Relay pin
const int HiMotor2Relays = 4;      // the number of the Hi Motor Speed Relays pin
const int LoMotorRelay = 5;        // the number of the Low Motor Speed Relay pin
const int Agitate2Relays = 6;      // the number of the Agitate function Relays pin
const int Spin2Relays = 7;         // the number of the Spin function Relays pin
const int Bypass2Relays = 8;       // the number of the Bypass Relays pin
const int FillRelay = 9;           // the number of the Fill function Relay pin
//const int ExtraRelay = 10;           // the number of the Fill function Relay pin

// Variables will change:
int WashOn = HIGH;             // the current reading from the input pin
int WashOff = LOW;             // the current reading from the input pin
int RinseOn = HIGH;         // the current state of the output pin
int RinseOff = LOW;         // the current state of the output pin
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
//int ExtraOn = HIGH;   // the previous reading from the input pin
//int ExtraOff = LOW;   // the previous reading from the input pin


// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

String en_char1="  MENU PRINCIPAL";// 
String en_char2="        SOY NANY";// 
String en_char3="     SOY NEFTALI";// 
String en_char4="      SOY MONTSE";//

String en_cursor="->";//
String off_cursor="  ";//



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
  delay(50);
  //LCDA.CLEAR();//����

  LCDA.DisplayStr(indexCursor,0,en_cursor,2);//
  delay(10);

  menuActual = mainMenu;
  menuAnterior = btnNONE;

  // Setup Pins ******************

  pinMode(MenuButton, INPUT);
  pinMode(OptionButtonA, INPUT);
  pinMode(OptionButtonB, INPUT);
  pinMode(OptionButtonC, INPUT);
  
  pinMode(WashRelay, OUTPUT);
  pinMode(RinseRelay, OUTPUT);
  pinMode(HiMotor2Relays, OUTPUT);
  pinMode(LoMotorRelay, OUTPUT);
  pinMode(Agitate2Relays, OUTPUT);
  pinMode(Spin2Relays, OUTPUT);
  pinMode(Bypass2Relays, OUTPUT);
  pinMode(FillRelay, OUTPUT);
  //pinMode(ExtraRelay, OUTPUT);

  // set initial output state

  digitalWrite(WashRelay, WashOff);
  digitalWrite(RinseRelay, RinseOff);
  digitalWrite(HiMotor2Relays, HiMotorOff);
  digitalWrite(LoMotorRelay, LowMotorOff);
  digitalWrite(Agitate2Relays, AgitateOff);
  digitalWrite(Spin2Relays, SpinOff);
  digitalWrite(Bypass2Relays, BypassOff);
  digitalWrite(FillRelay, FillOff);
  //digitalWrite(ExtraRelay, ExtraOff);
  
}

//  ************************************************************** MAIN LOOP *********************************************************************
void loop()
  {

  // ******************************* READ DIGITAL OUTPUTS *************************************
  // Read input pins into var array
  for(char i=0; i<DI_pincount; i++) {  
  DI_Val[i] = digitalRead(DI_Raw[i]);}
 
  dato = String(DI_Val[7]) + String(DI_Val[6]) + String(DI_Val[5]) + String(DI_Val[4]) + String(DI_Val[3]) + String(DI_Val[2]) + String(DI_Val[1]) + String(DI_Val[0]);

  // ************************** READ BUTTONS AND DO SPECIFIC MENU ACTIONS *********************
  //lcd_key = read_LCD_buttons();
  read_LCD_buttons();
  if(menuAnterior != lcd_key)
  {
  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
   // ********************** BUTTON UP ***************************
   case btnMENU:
     {
     minusCursorUp();
     break;
     }
   // ********************** BUTTON DOWNN ************************
   case btnOP1:
     {
     plusCursorDown();
     break;
     }
   // ********************** BUTTON SELECT ***********************
   case btnOP2:
     {
       // ********** INDEX CURSOR ROW 0 ***********
       if (indexCursor == 0)
       {
         if (menuActual == mainMenu || menuActual == setupMenu)
         {if (menuActual == mainMenu && toggle == true)  {menuActual = setupMenu; show_menu(menuActual);}
         if (menuActual == setupMenu && toggle == false) {menuActual = mainMenu; show_menu(menuActual);}
         toggle = !toggle;}
         if (menuActual == luisMenu)  {menuActual = setupMenu; show_menu(menuActual);}
         if (menuActual == nanyMenu)  {menuActual = mainMenu; show_menu(menuActual);}
       }
       // ********** INDEX CURSOR ROW 1 ***********
       if (indexCursor == 1)
       {
         if (menuActual == setupMenu)  {menuActual = luisMenu; show_menu(menuActual);}
         if (menuActual == mainMenu)  {menuActual = nanyMenu; show_menu(menuActual);}
         if (menuActual == nanyMenu)  {lavado_rapido();}
         if (menuActual == luisMenu) {agitate_setup();}
       }
       // ********** INDEX CURSOR ROW 2 ***********
       if (indexCursor == 2)
       {
         if (menuActual == luisMenu) {spin_setup();}
       }
       // ********** INDEX CURSOR ROW 3 ***********
       if (indexCursor == 3)
       {
         if (menuActual == luisMenu) {start();}
         if (menuActual == nanyMenu) {inicio();}
       }
     break;
     }

   case btnOP3:
     {
     break;
     }

   case btnSELECT:
     {
     break;
     }

   case btnNONE:
     {
     break;
     }
  }
  
  menuAnterior = lcd_key;
  }
  
  //LCDA.DisplayStr(0,0, String(lcd_key),8);
  //Dato mostrado en la V2, se deshabilita en V3
  //LCDA.DisplayStr(0,0, dato,8);
  }
//  *************************************************************** END LOOP *********************************************************************



//  ********************************* SHOW MENU ************************
void show_menu(int menu)
{
  // ********************* MAIN MENU ******************
  if (menu == mainMenu)
  {
    en_char1="  MENU PRINCIPAL";// 
    en_char2="        SOY NANY";// 
    en_char3="     SOY NEFTALI";// 
    en_char4="      SOY MONTSE";//
    LCDA.DisplayStr(0,0,en_char1,16);//
    delay(10);
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(50);
    showCursor();
   }
  // ********************* SETUP MENU ******************
  if (menu == setupMenu)
  {
    en_char1 = "      SETUP MENU";// 
    en_char2 = "       SAUL MENU";// 
    en_char3 = "          OUTPUT";// 
    en_char4 = "         SETINGS";//
    LCDA.DisplayStr(0,0,en_char1,16);//
    delay(10);
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(50);
    showCursor();
  }
  // ********************* LUIS MENU ******************
  if (menu == luisMenu)
  {
    en_char1 = "  LUIS SAUL MENU";// 
    en_char2 = "   agitate setup";// 
    en_char3 = "      spin setup";// 
    en_char4 = "       start now";//
    LCDA.DisplayStr(0,0,en_char1,16);//
    delay(10);
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(50);
    //Asegurar que no caiga en la opcion Agitate setup
    //para no volver su estado en true
    indexCursor=0;
    showCursor();
    //delay(500);
  }

  // ********************* NANY MENU ******************
  if (menu == nanyMenu)
  {
    en_char1 = "       NANY MENU";// 
    en_char2 = "   lavado rapido";// 
    en_char3 = "    lavado lento";// 
    en_char4 = "     inicio/paro";//
    LCDA.DisplayStr(0,0,en_char1,16);//
    delay(10);
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(50);
    //Asegurar que no caiga en la opcion lavado rapido
    //para no volver su estado en true
    indexCursor=0;
    showCursor();
    //delay(500);
  }


}

// ***************************** CURSOR ***************************
void showCursor()
{
  LCDA.DisplayStr(indexCursor,0,en_cursor,2);//
  delay(10);
}

void minusCursorUp()
{
  LCDA.DisplayStr(indexCursor,0,off_cursor,2);//
  delay(10);
  
  indexCursor = indexCursor - 1;
  if(indexCursor < 0) {indexCursor = 3;}
  LCDA.DisplayStr(indexCursor,0,en_cursor,2);//
  delay(10);
}

void plusCursorDown()
{
  LCDA.DisplayStr(indexCursor,0,off_cursor,2);//
  delay(10);

  indexCursor = indexCursor + 1;
  if(indexCursor > 3) {indexCursor = 0;}
  LCDA.DisplayStr(indexCursor,0,en_cursor,2);//
  delay(10);
}

// ************************* READ BUTTONS ***************************
void read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) lcd_key = btnNONE; //return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V3 use this threshold
 if (adc_key_in < 15)  lcd_key = btnMENU; //return btnMENU;  
 if (adc_key_in < 150 && adc_key_in > 135) lcd_key = btnOP1; //return btnOP1; 
 if (adc_key_in < 300 && adc_key_in > 285) lcd_key = btnOP2; // return btnOP2; 
 if (adc_key_in < 450 && adc_key_in > 440) lcd_key = btnOP3; // return btnOP3; 
 if (adc_key_in < 600 && adc_key_in > 585) lcd_key = btnSELECT; // return btnSELECT;  

 //if (adc_key_in < 10)  lcd_key = btnMENU; //return btnMENU;  
 //if (adc_key_in < 145 && adc_key_in > 140) lcd_key = btnOP1; //return btnOP1; 
 //if (adc_key_in < 295 && adc_key_in > 290) lcd_key = btnOP2; // return btnOP2; 
 //if (adc_key_in < 446 && adc_key_in > 444) lcd_key = btnOP3; // return btnOP3; 
 //if (adc_key_in < 595 && adc_key_in > 590) lcd_key = btnSELECT; // return btnSELECT;  

}

// ******************* WASHING MACHINE FUNCTIONS *********************
void agitate_setup()
{
    if(toggleAgitate == false && toggleSpin == false && indexCursor == 1)
    {en_char2 = "   AGITATE SETUP";// 
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    en_char4 = "       START NOW";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);
    toggleAgitate=true;
    relayAgitate();}
    else
    {if(toggleAgitate == true)
    {en_char2 = "   agitate setup";// 
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    en_char4 = "       start now";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);
    toggleAgitate=false;
    relayOut();}}
    //else

    showCursor();
    //toggleAgitate = !toggleAgitate;
}

void spin_setup()
{
    if(toggleAgitate == false && toggleSpin == false)
    {en_char3 = "      SPIN SETUP";// 
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    en_char4 = "       START NOW";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);
    toggleSpin = true;
    relaySpin();}
    else
    {if(toggleSpin == true)
    {en_char3 = "      spin setup";// 
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    en_char4 = "       start now";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);
    toggleSpin=false;
    relayOut();}}
    //else

    showCursor();
    //toggleSpin = !toggleSpin;
}

void start()
{
    if(toggleAgitate == true || toggleSpin == true)
    {
      if(toggleStart == false)
        {en_char4 = "  RUN.. STOP NOW";// 
        LCDA.DisplayStr(3,0,en_char4,16);//
        delay(10);
        toggleStart=true;
        relayAgitateStart();
        relaySpinStart();}
      else
        {en_char2 = "   agitate setup";// 
        LCDA.DisplayStr(1,0,en_char2,16);//
        delay(10);
        toggleAgitate = false;
    
        en_char3 = "      spin setup";// 
        LCDA.DisplayStr(2,0,en_char3,16);//
        delay(10);
        toggleSpin=false;

        en_char4 = "       start now";// 
        LCDA.DisplayStr(3,0,en_char4,16);//
        delay(10);
        toggleStart=false;    
        relayOut();}
    }
    
    showCursor();
}


void lavado_rapido()
{
    if(toggleLavadoRapido == false && toggleLavadoLento == false && indexCursor == 1)
    {en_char2 = "   LAVADO RAPIDO";// 
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    en_char4 = "   INICIAR AHORA";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);
    toggleLavadoRapido=true;
    relayAgitate();}
    else
    {if(toggleAgitate == true)
    {en_char2 = "   lavado rapido";// 
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    en_char4 = "     inicio/paro";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);
    toggleLavadoRapido=false;
    relayOut();}}
    //else

    showCursor();
    //toggleAgitate = !toggleAgitate;
}


void inicio()
{
    if(toggleLavadoRapido == true || toggleLavadoLento == true)
    {
      if(toggleInicio == false)
        {en_char4 = "  EN USO.. PARAR";// 
        LCDA.DisplayStr(3,0,en_char4,16);//
        delay(10);
        toggleInicio=true;
        relayLavadoRapidoStart();
        relayLavadoLentoStart();}
      else
        {en_char2 = "   lavado rapido";// 
        LCDA.DisplayStr(1,0,en_char2,16);//
        delay(10);
        toggleLavadoRapido = false;
    
        en_char3 = "    lavado lento";// 
        LCDA.DisplayStr(2,0,en_char3,16);//
        delay(10);
        toggleLavadoLento=false;

        en_char4 = "     inicio/paro";// 
        LCDA.DisplayStr(3,0,en_char4,16);//
        delay(10);
        toggleInicio=false;    
        relayOut();}
    }
    
    showCursor();
}


// ******************* WASHING MACHINE RELAY SETUP *********************

//  digitalWrite(RinseWash2Relays, Wash);
//  digitalWrite(HiMotor2Relays, HiMotorOff);
//  digitalWrite(LoMotorRelay, LowMotorOff);
//  digitalWrite(Agitate2Relays, AgitateOff);
//  digitalWrite(Spin2Relays, SpinOff);
//  digitalWrite(Bypass2Relays, BypassOff);
//  digitalWrite(FillRelay, FillOff);



void relayAgitate()
{
  if(toggleAgitate == true)
    {
    digitalWrite(WashRelay, WashOn);
    delay(100);
    digitalWrite(RinseRelay, RinseOff);
    delay(100);
    digitalWrite(HiMotor2Relays, HiMotorOn);
    delay(100);
    digitalWrite(LoMotorRelay, LowMotorOff);
    delay(100);
    digitalWrite(Agitate2Relays, AgitateOn);
    delay(100);
    digitalWrite(Spin2Relays, SpinOff);
    delay(100);
    digitalWrite(Bypass2Relays, BypassOff);
    delay(100);
    }
}

void relayAgitateStart()
{
  if(toggleAgitate == true)
    {
    digitalWrite(FillRelay, FillOn);
    delay(100);
    }
}



void relaySpin()
{
  if(toggleSpin == true)
    {
    digitalWrite(WashRelay, WashOff);
    delay(100);
    digitalWrite(RinseRelay, RinseOn);
    delay(100);
    digitalWrite(HiMotor2Relays, HiMotorOn);
    delay(100);
    digitalWrite(LoMotorRelay, LowMotorOff);
    delay(100);
    digitalWrite(Agitate2Relays, AgitateOff);
    delay(100);
    digitalWrite(Spin2Relays, SpinOn);
    delay(100);
    digitalWrite(FillRelay, FillOff);
    delay(100);
    }
}


void relaySpinStart()
{
  if(toggleSpin == true)
    {
    digitalWrite(Bypass2Relays, BypassOn);
    delay(100);
    }
}

void relayOut()
{

  digitalWrite(FillRelay, FillOff);
  delay(10);
  digitalWrite(Bypass2Relays, BypassOff);
  delay(10);
  
  digitalWrite(HiMotor2Relays, HiMotorOff);
  delay(10);
  digitalWrite(LoMotorRelay, LowMotorOff);
  delay(10);
  digitalWrite(Agitate2Relays, AgitateOff);
  delay(10);
  digitalWrite(Spin2Relays, SpinOff);
  delay(10);

  digitalWrite(WashRelay, WashOff);
  delay(100);
  digitalWrite(RinseRelay, RinseOff);
  delay(100);

}


void relayLavadoRapido()
{
  if(toggleLavadoRapido == true)
    {
    digitalWrite(WashRelay, WashOn);
    delay(100);
    digitalWrite(RinseRelay, RinseOff);
    delay(100);
    digitalWrite(HiMotor2Relays, HiMotorOn);
    delay(100);
    digitalWrite(LoMotorRelay, LowMotorOff);
    delay(100);
    digitalWrite(Agitate2Relays, AgitateOn);
    delay(100);
    digitalWrite(Spin2Relays, SpinOff);
    delay(100);
    digitalWrite(Bypass2Relays, BypassOff);
    delay(100);
    }
}

void relayLavadoRapidoStart()
{
  int longDelayInSeconds = 0;
  int p=0;
  if(toggleLavadoRapido == true)
    {
    digitalWrite(FillRelay, FillOn);
    delay(100);

    //Tiempo de llenado y lavado 8 min
    //delay(480000);
    longDelayInSeconds = 480;
    while (p < longDelayInSeconds) {
        delay(1000);
        p++;
    }
    p=0;

    //Desactivar conexiones
    relayOut();
    // Espera 10 segundos antes de vaciar el agua
    delay(10000);
    //Configurar relays para vaciar y exprimir
    relaySpin();
    //Iniciar vaciado
    relaySpinStart();
    //Tiempo de vaciado 3 min
    //delay(180000);
    longDelayInSeconds = 180;
    while (p < longDelayInSeconds) {
        delay(1000);
        p++;
    }
    p=0;

    //Detener vaciado
    digitalWrite(Bypass2Relays, BypassOff);
    //Esperar 20 seg
    delay(20000);
    //Iniciar exprimir
    digitalWrite(Bypass2Relays, BypassOn);
       
    //Exprimir 3.5 minutos
    //delay(210000);
    longDelayInSeconds = 210;
    while (p < longDelayInSeconds) {
        delay(1000);
        p++;
    }
    p=0;

    
    //Detener exprimir
    digitalWrite(Bypass2Relays, BypassOff);
    delay(100);
    
    //Desactivar conexiones
    relayOut();

    en_char2 = "   lavado rapido";// 
    LCDA.DisplayStr(1,0,en_char2,16);//
    delay(10);
    toggleLavadoRapido = false;
    
    en_char3 = "    lavado lento";// 
    LCDA.DisplayStr(2,0,en_char3,16);//
    delay(10);
    toggleLavadoLento=false;

    en_char4 = "     inicio/paro";// 
    LCDA.DisplayStr(3,0,en_char4,16);//
    delay(10);
    toggleInicio=false;    
  
    }
}


void relayLavadoLento()
{
  if(toggleLavadoLento == true)
    {
    digitalWrite(WashRelay, WashOn);
    delay(100);
    digitalWrite(RinseRelay, RinseOff);
    delay(100);
    digitalWrite(HiMotor2Relays, HiMotorOn);
    delay(100);
    digitalWrite(LoMotorRelay, LowMotorOff);
    delay(100);
    digitalWrite(Agitate2Relays, AgitateOn);
    delay(100);
    digitalWrite(Spin2Relays, SpinOff);
    delay(100);
    digitalWrite(Bypass2Relays, BypassOff);
    delay(100);
    }
}

void relayLavadoLentoStart()
{
  if(toggleLavadoLento == true)
    {
    digitalWrite(FillRelay, FillOn);
    delay(100);
    }
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

