#include <TimerOne.h>

int segundos=0;
int minutos=0;
int horas=0;

void setup() 
{
  // Initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards
  pinMode(13, OUTPUT);    
  
  Timer1.initialize(1000000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will be on 10 secs, then will be off 10 secs
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
}
 
void loop()
{
  // Main code loop
  // TODO: Put your regular (non-ISR) logic here
}
 
/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
    // Toggle LED
    digitalWrite( 13, digitalRead( 13 ) ^ 1 );

    if(segundos >= 60)
      {
        //Toggle LED
        //digitalWrite( 13, digitalRead( 13 ) ^ 1 );
        minutos++;
        segundos = 0;
      }
    
    if(minutos >= 60)
      {
        horas++;
        minutos = 0;
      }

    if(horas >= 6)
      {
        horas = 0;
      }

   
    segundos++;

}
