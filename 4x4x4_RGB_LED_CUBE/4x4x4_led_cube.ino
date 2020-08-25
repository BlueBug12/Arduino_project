#include <SPI.h>
#define latch_pin 2
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
unsigned long start;//for a millis timer to cycle through the animations


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
  interrupts();//let the show begin, this lets the multiplexing start

}

void loop() {
  game();
  breathing(200);
  /*
  movePlane();
  moveSingle();
  moveSqure();
  moveOnePixel();
  allLeds();
  planeSwipe();
  randomLeds();
*/

}
void refresh(){
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      for(int k=0;k<4;++k){
        if(data[i][j][k]==1)
          LED(k,i,j,15,0,0);
        else if(data[i][j][k]==2)
          LED(k,i,j,0,0,15);
        else
          LED(k,i,j,0,0,0);
     }
    }
   }
}

bool game_over(int x, int y, int z, int player){
    int counter=0;
    //x axis
    for(int i=x+1;i<x+5;++i){
      if(data[i%4][y][z]==player)
        ++counter;  
    }
    if(counter==4){
      if(player==1){
        for(int a=0;a<4;++a){
          for(int i=x+1;i<x+5;++i)
            LED(z,i%4,y,0,0,0);
          delay(300);
          for(int i=x+1;i<x+5;++i)
            LED(z,i%4,y,15,0,0);
          delay(300);
        }
      }
        
      if(player==2){
        for(int a=0;a<4;++a){
          for(int i=x+1;i<x+5;++i)
            LED(z,i%4,y,0,0,0);
          delay(300);
          for(int i=x+1;i<x+5;++i)
            LED(z,i%4,y,0,0,15);
          delay(300);
        }
      }
      return true;
    }
      
    counter=0;
    
    //y axis
    for(int i=y+1;i<y+5;++i){
      if(data[x][i%4][z]==player)
        ++counter;  
    }
    if(counter==4){
      if(player==1){
        for(int a=0;a<4;++a){
          for(int i=y+1;i<y+5;++i)
            LED(z,x,i%4,0,0,0);
          delay(300);
          for(int i=y+1;i<y+5;++i)
            LED(z,x,i%4,15,0,0);
          delay(300);
        }
      }
        
      if(player==2){
        for(int a=0;a<4;++a){
          for(int i=y+1;i<y+5;++i)
            LED(z,x,i%4,0,0,0);
          delay(300);
          for(int i=y+1;i<y+5;++i)
            LED(z,x,i%4,0,0,15);
          delay(300);
        }
      }  
      return true;
    }
      
    counter=0;

    //z axis
    for(int i=z+1;i<z+5;++i){
      if(data[x][y][i%4]==player)
        ++counter;  
    }
    if(counter==4){
      if(player==1){
        for(int a=0;a<4;++a){
          for(int i=z+1;i<z+5;++i)
            LED(i%4,x,y,0,0,0);
          delay(300);
          for(int i=z+1;i<z+5;++i)
            LED(i%4,x,y,15,0,0);
          delay(300);
        }
      }
        
      if(player==2){
        for(int a=0;a<4;++a){
          for(int i=z+1;i<z+5;++i)
            LED(i%4,x,y,0,0,0);
          delay(300);
          for(int i=z+1;i<z+5;++i)
            LED(i%4,x,y,0,0,15);
          delay(300);
        }
      }  
      return true;
    }
      
    //counter=0;

    return false;
    
}

void game(){
  
  unsigned long int start_time = millis();
  bool down=false;
  bool turn=false;
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      for(int k=0;k<4;++k){
        data[i][j][k]=0;
     }
    }
   }
   int bx=1;
   int by=1;
   int bz=3;
   
   while(1){
    int tmpx=bx;
    int tmpy=by;

    while(millis()-start_time<300){
      refresh();
      //clean();
      if(analogRead(A1)<=10){
        tmpx=(bx==0)? 3:bx-1;
        while(data[tmpx][tmpy][3]){
          tmpx = (tmpx==0)?3:tmpx-1;
        }
        
      }
      if(analogRead(A1)>=1013){
        tmpx=(bx==3)?0:bx+1;
        while(data[tmpx][tmpy][3]){
          tmpx = (tmpx==3)?0:tmpx+1;
        }
        
      }
      if(analogRead(A2)<=10){
        tmpy=(by==0)?3:by-1;
        while(data[tmpx][tmpy][3]){
          tmpy = (tmpy==0)?0:tmpy-1;
        }
        
      }
      if(analogRead(A2)>=1013){
        tmpy=(by==3)?0:by+1;
        while(data[tmpx][tmpy][3]){
          tmpy=(tmpy==3)?0:tmpy+1;
        }
        
      }
      if(analogRead(A3)==0){
        down=true;
      }
    }
    //bx=tmpx;
    //by=tmpy;

    start_time=millis();
    if(turn)
      LED(3,tmpx,tmpy,15,0,0);
    else
      LED(3,tmpx,tmpy,0,0,15);
    
    if(down){
      down=false;
      int index=2;
      
      while(index>=0&&!data[tmpx][tmpy][index]){
        if(turn){
          LED(index,tmpx,tmpy,15,0,0);
          LED(index+1,tmpx,tmpy,0,0,0); 
        }
        else{
          LED(index,tmpx,tmpy,0,0,15);
          LED(index+1,tmpx,tmpy,0,0,0);
        }
        
        delay(50);
        --index;
        
      }
      turn=!turn;
      data[tmpx][tmpy][index+1]=(int)turn+1;
      if(game_over(tmpx,tmpy,index+1,(int)turn+1)){
        break;
      }
      for(int i=tmpx*4+tmpy;i<tmpx*4+tmpy+16;++i){
        i%=16;
        if(!data[i/4][i%4][3]){
          bx=i/4;
          by=i%4;
          break;  
        }
      }
      
    }
    else{
      bx = tmpx;
      by = tmpy; 
    }
    
    while(millis()-start_time<300);
    start_time=millis();
    
    
   }
   
    
    
    
}

void breathing(int d_time){
  //blue
  for(int b=0;b<16;++b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
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
          LED(i,j,k,b,0,15);
        }  
      }
    }
    delay(d_time);
  }
  //red
  for(int b=15;b>=0;--b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
          LED(i,j,k,15,0,b);
        }  
      }
    }
    delay(d_time);
  }
  //red + green
  for(int b=0;b<16;++b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
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
          LED(i,j,k,b,15,0);
        }  
      }
    }
    delay(d_time);
  }
  //green + blue
  for(int b=0;b<16;++b){
    for(int i=0;i<4;++i){
      for(int j=0;j<4;++j){
        for(int k=0;k<4;++k){
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
          LED(i,j,k,b,15,15);
        }  
      }
    }
    delay(d_time);
  }
  clean();
 }
void randomLeds() {
  int x, y, z, red, green, blue;
  start = millis();

  while (millis() - start < 5000) {
    x = random(4);
    y = random(4);
    z = random(4);
    red = random(16);
    green = random(16);
    blue = random(16);
    LED(x, y, z, red, green, blue);

    delay(20);
  }
  clean();
}
void planeSwipe() {
  for (int i = 0; i < 3; i++) {
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
    delay(100);
    clean();
  }
  for (int i = 0; i < 4; i++) {
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
    delay(100);
    clean();
  }
  for (int i = 0; i < 3; i++) {
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
    delay(100);
    clean();
  }
  for (int i = 0; i < 4; i++) {
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
    delay(100);
    clean();
  }
  ///////////////
  for (int i = 0; i < 3; i++) {
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
    delay(100);
    clean();
  }
  for (int i = 0; i < 4; i++) {
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
    delay(100);
    clean();
  }
  for (int i = 0; i < 3; i++) {
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
    delay(100);
    clean();
  }
  for (int i = 0; i < 4; i++) {
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
    delay(100);
    clean();
  }
  //////////////////////
  for (int i = 0; i < 3; i++) {
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
    delay(100);
    clean();
  }
  for (int i = 0; i < 4; i++) {
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
    delay(100);
    clean();
  }
  for (int i = 0; i < 3; i++) {
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
    delay(100);
    clean();
  }
  for (int i = 0; i < 4; i++) {
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
    delay(100);
    clean();
  }
}
void movePlane() {
  start = millis();

  while (millis() - start < 5000) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          LED(i, j, k, 15, 0, 0);
          LED(j, i, k, 0, 10, 0);
          LED(j, k, i, 0, 0, 10);
        }

      delay(100);
      clean();
    }
  }
}
void moveSingle() {
  start = millis();

  while (millis() - start < 5000) {
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          LED(i, j, k, 15, 0, 15);
          LED(j, i, k, 15, 0, 15);
          LED(i, k, j, 0, 15, 15);
          delay(50);
          clean();
        }
  }
}
void moveOnePixel() {
  start = millis();
  int layer, column, row, red, green, blue;
  while (millis() - start < 5000) {
    layer = random(4);
    column = random(4);
    row = random(4);
    red = random(16);
    green = random(16);
    blue = random(16);
    LED(layer, column, row, red, green, blue);
    LED(column, layer, row, red, green, blue);
    LED(row , layer, column ,  red, green, blue);
    delay(50);
    clean();
  }
}
void moveSqure() {
  start = millis();

  while (millis() - start < 5000) {
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
}
void allLeds() {
  for (int brightness = 0; brightness < 16; brightness++) {
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          LED(i, j, k, brightness, 0, 0);
        }
    delay(30);
  }
  delay(300);
  for (int brightness = 15; brightness >= 0; brightness--) {
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          LED(i, j, k, brightness, 0, 0);
        }
    delay(30);
  }
  for (int brightness = 0; brightness < 16; brightness++) {
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          LED(i, j, k, 0, brightness, 0);
        }
    delay(30);
  }
  delay(300);
  for (int brightness = 15; brightness >= 0; brightness--) {
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          LED(i, j, k, 0, brightness, 0);
        }
    delay(30);
  }
  for (int brightness = 0; brightness < 16; brightness++) {
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          LED(i, j, k, 0, 0, brightness);
        }
    delay(30);
  }
  delay(300);
  for (int brightness = 15; brightness >= 0; brightness--) {
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++) {
          LED(i, j, k, 0, 0, brightness);
        }
    delay(30);
  }

}
void LED(int level, int row, int column, byte red, byte green, byte blue) { //****LED Routine****LED Routine****LED Routine****LED Routine


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
  int ii, jj, kk;
  for (ii = 0; ii < 4; ii++)
    for (jj = 0; jj < 4; jj++)
      for (kk = 0; kk < 4; kk++)
        LED(ii, jj, kk, 0, 0, 0);

}
