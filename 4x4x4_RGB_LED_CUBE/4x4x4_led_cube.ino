#include <SPI.h>
#define latch_pin 2
#define interrupt_pin 3
#define blank_pin 4
#define data_pin 11// used by SPI, must be pin 11
#define clock_pin 13// used by SPI, must be 13


int shift_out;
byte anode[4];

//This is how the brightness for every LED is stored,

byte  red0[8], red1[8], red2[8], red3[8];
byte  blue0[8], blue1[8], blue2[8], blue3[8];
byte  green0[8], green1[8], green2[8], green3[8];


int level = 0; //keeps track of which level we are shifting data to
int anodelevel = 0; //this increments through the anode levels
int BAM_Bit, BAM_Counter = 0; // Bit Angle Modulation variables to keep track of things
unsigned int data[4][4][4];

volatile boolean jump=false;

void setup() {
  Serial.begin(9600);
  SPI.setBitOrder(MSBFIRST);//Most Significant Bit First
  SPI.setDataMode(SPI_MODE0);// Mode 0 Rising edge of data, keep clock low
  SPI.setClockDivider(SPI_CLOCK_DIV2);//Run the data in at 16MHz/2 - 8MHz
  
  noInterrupts();// kill interrupts until everybody is set up

  //Timer 1 to refresh the cube
  TCCR1A = B00000000;
  TCCR1B = B00001011;

  TIMSK1 = B00000010;
  OCR1A = 30;

  anode[0] = B00000001;
  anode[1] = B00000010;
  anode[2] = B00000100;
  anode[3] = B00001000;

  pinMode(latch_pin, OUTPUT);//Latch
  pinMode(data_pin, OUTPUT);//MOSI DATA
  pinMode(clock_pin, OUTPUT);//SPI Clock
  pinMode(blank_pin, OUTPUT);//Output Enable  important to do this last, so LEDs do not flash on boot up
  SPI.begin();//start up the SPI library
  attachInterrupt(digitalPinToInterrupt(interrupt_pin), next, RISING);
  interrupts();//let the show begin, this lets the multiplexing start
}

void loop() {
  //test(100,15,15,15);
  breathing(200);
  planeSwipe(100);
  moveSqure(10000);
  movePlane(10000,100);
  randomLeds(10000,10);
  rain(10000,100,15,10,0);
}

void next(){
    jump=true;
}
void breathing(int d_time){
  //red
  for(int b=0;b<16;++b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
          if(jump){
            jump=false;
            clean();  
            return ;
          }
          LED(i,j,k,0,0,b);
        }
      }
    }
    delay(d_time);
  }
  //blue + red
  for(int b=0;b<16;++b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
          if(jump){
            jump=false;
            clean();
            return;  
          }
          LED(i,j,k,b,0,15);
        }
      }
    }
    delay(d_time);
  }
  //blue
  for(int b=15;b>=0;--b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
          if(jump){
            jump=false;
            clean(); 
            return; 
          }
          LED(i,j,k,15,0,b);
        }
      }
    }
    delay(d_time);
  }
  //blue + green
  for(int b=0;b<16;++b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
          if(jump){
            jump=false;
            clean();  
            return;
          }
          LED(i,j,k,15,b,0);
        }
      }
    }
    delay(d_time);
  }
  //green
  for(int b=15;b>=0;--b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
          if(jump){
            jump=false;
            clean();  
            return;
          }
          LED(i,j,k,b,15,0);
        }
      }
    }
    delay(d_time);
  }
  //green + red
  for(int b=0;b<16;++b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
          if(jump){
            jump=false;
            clean();  
            return;
          }
          LED(i,j,k,0,15,b);
        }
      }
    }
    delay(d_time);
  }
  //red + green + blue
  for(int b=0;b<16;++b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
          if(jump){
            jump=false;
            clean();  
            return;
          }
          LED(i,j,k,b,15,15);
        }
      }
    }
    delay(d_time);
  }
  clean();
 }
 
void randomLeds(unsigned long _time,int d_time) {
  int x, y, z, red, green, blue;
  unsigned long start = millis();

  while (millis() - start < _time&&!jump) {
    x = random(4);
    y = random(4);
    z = random(4);
    red = random(16);
    green = random(16);
    blue = random(16);
    LED(x, y, z, red, green, blue);
    delay(d_time);
  }
  jump=false;
  clean();
}

//actually BGR because I weld the wrong line.
void LED(int row, int column, int level, byte red, byte green, byte blue) { //****LED Routine****LED Routine****LED Routine****LED Routine

  if(row>=4||column>=4||level>=4)
    return;
  
  int whichbyte = int(((level * 16) + (row * 4) + column) / 8);


  int wholebyte = (level * 16) + (row * 4) + column;

  bitWrite(red0[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 0));
  bitWrite(red1[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 1));
  bitWrite(red2[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 2));
  bitWrite(red3[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 3));

  bitWrite(green0[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 0));
  bitWrite(green1[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 1));
  bitWrite(green2[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 2));
  bitWrite(green3[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 3));

  bitWrite(blue0[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 0));
  bitWrite(blue1[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 1));
  bitWrite(blue2[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 2));
  bitWrite(blue3[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 3));


}

ISR(TIMER1_COMPA_vect) {

  PORTD |= 1 << blank_pin;
  if (BAM_Counter == 8)
    BAM_Bit++;
  else if (BAM_Counter == 24)
    BAM_Bit++;
  else if (BAM_Counter == 56)
    BAM_Bit++;

  BAM_Counter++;

  switch (BAM_Bit) {
    case 0:
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(red0[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(green0[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(blue0[shift_out]);
      break;
    case 1:
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(red1[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(green1[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(blue1[shift_out]);
      break;
    case 2:
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(red2[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(green2[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(blue2[shift_out]);
      break;
    case 3:
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(red3[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(green3[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(blue3[shift_out]);

      if (BAM_Counter == 120) {
        BAM_Counter = 0;
        BAM_Bit = 0;
      }
      break;
  }

  SPI.transfer(anode[anodelevel]);//finally, send out the anode level byte

  PORTD |= 1 << latch_pin; //Latch pin HIGH
  PORTD &= ~(1 << latch_pin); //Latch pin LOW
  PORTD &= ~(1 << blank_pin); //Blank pin LOW to turn on the LEDs with the new data

  anodelevel++;
  level = level + 2;

  if (anodelevel == 4)
    anodelevel = 0;
  if (level == 8)
    level = 0;
  pinMode(blank_pin, OUTPUT);
}



void clean() {
  for (int ii = 0; ii < 4; ii++)
    for (int jj = 0; jj < 4; jj++)
      for (int kk = 0; kk < 4; kk++)
        LED(ii, jj, kk, 0, 0, 0);
}
void test(int d_time,int b,int g,int r) {
  char x[64]={0,1,2,3,3,3,3,2,1,0,0,0,1,2,2,1,1,2,2,1,0,0,0,1,2,3,3,3,3,2,1,0,0,1,2,3,3,3,3,2,1,0,0,0,1,2,2,1,1,2,2,1,0,0,0,1,2,3,3,3,3,2,1,0};
  char y[64]={0,0,0,0,1,2,3,3,3,3,2,1,1,1,2,2,2,2,1,1,1,2,3,3,3,3,2,1,0,0,0,0,0,0,0,0,1,2,3,3,3,3,2,1,1,1,2,2,2,2,1,1,1,2,3,3,3,3,2,1,0,0,0,0};
  for(int i=0;i<64;++i){
    if(jump){
      jump=false;
      clean();
      return;  
    }
    LED(x[i], y[i], i/16, b, g, r);
    delay(d_time);
  }
  clean();
}
void moveSingle(unsigned long _time) {
  unsigned long start = millis();

  while (millis() - start < _time) {
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          
          LED(i, j, k, 15, 0, 15);
          LED(j, i, k, 15, 0, 15);
          LED(i, k, j, 0, 15, 15);
          delay(50);
          clean();
          if(jump){
            jump=false;
            return;  
          }
        }
  }
  clean();
}

void rain(unsigned long int _time,int d_time,int b,int g,int r){
  unsigned long start = millis();
  char x1_array[4]={4,4,4,4};
  char y1_array[4]={4,4,4,4};
  char x2_array[4]={4,4,4,4};
  char y2_array[4]={4,4,4,4};
  while(millis()-start<_time){
      if(jump){
        jump=false;
        clean();
        return;  
       }
      int x1 = random(4);
      int y1 = random(4);
      int x2 = random(4);
      int y2 = random(4);
      
      for(int i=0;i<3;++i){
        LED(x1_array[i], y1_array[i], i, 0, 0, 0);
        LED(x2_array[i], y2_array[i], i, 0, 0, 0);
        x1_array[i]=x1_array[i+1];
        y1_array[i]=y1_array[i+1];
        x2_array[i]=x2_array[i+1];
        y2_array[i]=y2_array[i+1];
      }
      LED(x1_array[3], y1_array[3], 3, 0, 0, 0);
      LED(x2_array[3], y2_array[3], 3, 0, 0, 0);
      
      x1_array[3]=x1;
      y1_array[3]=y1;
      x2_array[3]=x2;
      y2_array[3]=y2;
      
      for(int i=0;i<4;++i){
        LED(x1_array[i], y1_array[i], i, b, g, r);
        LED(x2_array[i], y2_array[i], i, b, g, r);
      }
      delay(d_time);
    }
  clean();
}

void moveSqure(unsigned long _time) {
  unsigned long start = millis();

  while (millis() - start < _time && !jump) {
    int red = random(15);
    int green = random(15);
    int blue = random(15);
    LED(1, 1, 1, red, green, blue);
    LED(1, 1, 2, red, green, blue);
    LED(1, 2, 1,  red, green, blue);
    LED(1, 2, 2,  red, green, blue);
    LED(2, 1, 1,  red, green, blue);
    LED(2, 1, 2,  red, green, blue);
    LED(2, 2, 1,  red, green, blue);
    LED(2, 2, 2,  red, green, blue);
    delay(200);
    clean();
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        LED(i, j, 0,  red, green, blue);
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        LED(i, 0, j,  red, green, blue);
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        LED(0, i, j,  red, green, blue);
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        LED(i, j, 3, red, green, blue);
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        LED(i, 3, j, red, green, blue);
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        LED(3, i, j, red, green, blue);

    delay(200);
    clean();
  }
  jump=false;
  clean();
}
void movePlane(unsigned long _time,int d_time) {
  unsigned long start = millis();

  while (millis() - start < _time) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          LED(i, j, k, 15, 0, 0);
          LED(j, i, k, 0, 10, 0);
          LED(j, k, i, 0, 0, 10);
        }
      if(jump){
        jump=false;
        clean();  
        return;
      }
      delay(d_time);
      clean();
    }
  }
}

void moveOnePixel(unsigned long _time, int d_time) {
  unsigned long start = millis();
  int layer, column, row, red, green, blue;
  while (millis() - start < _time&&!jump) {
    layer = random(4);
    column = random(4);
    row = random(4);
    red = random(16);
    green = random(16);
    blue = random(16);
    LED(layer, column, row, red, green, blue);
    LED(column, layer, row, red, green, blue);
    LED(row , layer, column ,  red, green, blue);
    delay(d_time);
    clean();
  }
  jump=false;
  clean();
}

void planeSwipe(int d_time) {
  for (int i = 0; i < 3; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= (3 - i)) {
        for (int k = 0; k < 4; k++)
          LED(k, j, child, 0, 15, 0);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED(k, j, child, 0, 15, 0);
      }
    }
    delay(d_time);
    clean();
  }
  for (int i = 0; i < 4; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= i) {
        for (int k = 0; k < 4; k++)
          LED(k, child, j , 0, 15, 0);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED(k, child, j, 0, 15, 0);
      }
    }
    delay(d_time);
    clean();
  }
  for (int i = 0; i < 3; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= (3 - i)) {
        for (int k = 0; k < 4; k++)
          LED(k, child, j, 0, 15, 0);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED(k, child, j, 0, 15, 0);
      }
    }
    delay(d_time);
    clean();
  }
  for (int i = 0; i < 4; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= i) {
        for (int k = 0; k < 4; k++)
          LED(k, j, child , 0, 15, 0);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED(k, j, child, 0, 15, 0);
      }
    }
    delay(d_time);
    clean();
  }
  ///////////////
  for (int i = 0; i < 3; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= (3 - i)) {
        for (int k = 0; k < 4; k++)
          LED(j, child, k,  0, 0, 15);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED(j, child, k,   0, 0, 15);
      }
    }
    delay(d_time);
    clean();
  }
  for (int i = 0; i < 4; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= i) {
        for (int k = 0; k < 4; k++)
          LED(child, j , k,  0, 0, 15);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED(child, j, k , 0, 0, 15);
      }
    }
    delay(d_time);
    clean();
  }
  for (int i = 0; i < 3; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= (3 - i)) {
        for (int k = 0; k < 4; k++)
          LED(child, j, k,   0, 0, 15);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED( child, j, k,  0, 0, 15);
      }
    }
    delay(d_time);
    clean();
  }
  for (int i = 0; i < 4; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= i) {
        for (int k = 0; k < 4; k++)
          LED(j, child , k,  0, 0, 15);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED(j, child, k, 0, 0, 15);
      }
    }
    delay(d_time);
    clean();
  }
  //////////////////////
  for (int i = 0; i < 3; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= (3 - i)) {
        for (int k = 0; k < 4; k++)
          LED(j, k, child,  15, 0, 0);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED(j, k, child,   15, 0, 0);
      }
    }
    delay(d_time);
    clean();
  }
  for (int i = 0; i < 4; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= i) {
        for (int k = 0; k < 4; k++)
          LED(child, k, j ,  15, 0, 0);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED(child, k , j, 15, 0, 0);
      }
    }
    delay(d_time);
    clean();
  }
  for (int i = 0; i < 3; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= (3 - i)) {
        for (int k = 0; k < 4; k++)
          LED( child, k, j,  15, 0, 0);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED( child, k, j,  15, 0, 0);
      }
    }
    delay(d_time);
    clean();
  }
  for (int i = 0; i < 4; i++) {
    if(jump){
      jump=false;
      clean();  
      return;
    }
    int child = 0;
    for (int j = 0; j < 4; j++) {
      if (j >= i) {
        for (int k = 0; k < 4; k++)
          LED(j , k, child,  15, 0, 0);
        child++;
      } else {
        for (int k = 0; k < 4; k++)
          LED( j, k, child, 15, 0, 0);
      }
    }
    delay(d_time);
    clean();
  }
}
