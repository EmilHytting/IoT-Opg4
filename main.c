#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include "Include/ProjectDefine/ProjectDefines.h"
#include "Include/Timer/Timer.h"

static volatile uint16_t external_count;
static volatile uint8_t external_event;
static volatile uint32_t seconds;

static void U0c(char c){while(!(UCSR0A&(1<<UDRE0))){} UDR0=c;}
static void U1c(char c){while(!(UCSR1A&(1<<UDRE1))){} UDR1=c;}
static void U0s(const char*s){while(*s)U0c(*s++);}
static void U1s(const char*s){while(*s)U1c(*s++);}
static void Number(uint32_t n,void(*put)(char)){char d[10];uint8_t l=0;if(!n){put('0');return;}while(n){d[l++]=(char)('0'+n%10UL);n/=10UL;}while(l)put(d[--l]);}

static void UART_Init(void){
  UBRR0=MyUBRR; UCSR0A=0; UCSR0B=(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0); UCSR0C=(1<<UCSZ01)|(1<<UCSZ00);
  UBRR1=MyUBRR; UCSR1A=0; UCSR1B=(1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1); UCSR1C=(1<<UCSZ11)|(1<<UCSZ10);
}
static void INT0_Init(void){DDRD&=(uint8_t)~(1<<DDD2);PORTD|=(1<<PORTD2);EICRA=(EICRA&(uint8_t)~((1<<ISC01)|(1<<ISC00)))|(1<<ISC01);EIFR=(1<<INTF0);EIMSK|=(1<<INT0);}
static void Display_ShowCount(uint16_t count){(void)count; /* Indsaet display-driver her. */}

ISR(INT0_vect){external_count++;external_event=1;EICRA=(EICRA&(uint8_t)~((1<<ISC01)|(1<<ISC00)))|(1<<ISC01)|(1<<ISC00);}
ISR(USART0_RX_vect){uint8_t c=UDR0;if(c>='a'&&c<='z')c=(uint8_t)(c-('a'-'A'));U0c((char)c);}
ISR(USART1_RX_vect){uint8_t c=UDR1;if(c>='A'&&c<='Z')c=(uint8_t)(c+('a'-'A'));U1c((char)c);}

int main(void){
  DDRB|=(1<<GreenLedBit);UART_Init();INT0_Init();Timer1_Init_ms(1000);sei();
  while(1){
    if(external_event){uint16_t n;cli();external_event=0;n=external_count;sei();U0s("Ekstern interrupt: ");Number(n,U0c);U0s("\r\n");U1s("Ekstern interrupt: ");Number(n,U1c);U1s("\r\n");Display_ShowCount(n);}
    if(timer_second_flag){uint32_t n;cli();timer_second_flag=0;seconds++;n=seconds;sei();U1s("Sekund: ");Number(n,U1c);U1s("\r\n");}
  }
}
