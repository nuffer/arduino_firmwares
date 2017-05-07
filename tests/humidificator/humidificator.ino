void setup() {
  // put your setup code here, to run once:
  DDRD = B01100000;

}

/* 1.767MHz */
void loop() {
  PORTD = B00100000;
  __asm__("nop\n\t""nop\n\t""nop\n\t"); 
  PORTD = B01000000;
  /* aucune nop nécaissaire car loop prend du temps */
}

/* 1.36MHz
void loop() {
  PORTD = B00100000;
  __asm__("nop\n\t");
  __asm__("nop\n\t"); 
  __asm__("nop\n\t"); 
  __asm__("nop\n\t");   
  PORTD = B01000000;
  __asm__("nop\n\t"); 
  __asm__("nop\n\t"); 
  __asm__("nop\n\t"); 
  __asm__("nop\n\t"); 
}*/
