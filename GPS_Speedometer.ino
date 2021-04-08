#include <TinyGPS.h>
#include <SoftwareSerial.h>

TinyGPS gps;
SoftwareSerial ss(A0,A1);

int GPSBaudrate = 9600;

bool *digits[3];

int anodePins[3] = {4,3,2};

// cathode pins       A  B C D E F G 
int cathodePins[7] = {11,9,7,6,5,10,8};

bool segments[10][7] = {
  {1,1,1,1,1,1,0},
  {0,1,1,0,0,0,0},
  {1,1,0,1,1,0,1},
  {1,1,1,1,0,0,1},
  {0,1,1,0,0,1,1},
  {1,0,1,1,0,1,1},
  {1,0,1,1,1,1,1},
  {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1},
};

long timer1 = 0;
long timer2 = 0;
int count = 0;
int currentAnode = 0;
bool newData = false;

void setup() {
  //set pins to OUTPUT
  for(int i = 0; i < 7; i++){
    pinMode(cathodePins[i], OUTPUT);
    digitalWrite(cathodePins[i], HIGH);
  }
  for(int i = 0; i < 3; i++){
    pinMode(anodePins[i], OUTPUT);
    digitalWrite(anodePins[i], LOW);
  }
  ss.begin(GPSBaudrate);
  Serial.begin(9600);
}

//172
void displayNumber(int num){
  digits[0] = segments[int(num % 10)];
  digits[1] = segments[int(num / 10) % 10];
  digits[2] = segments[int(num / 100) % 10];
}


//Show one of the digits
void showDigit(int anode){
  digitalWrite(anodePins[((anode+1) % 3)], LOW);
  digitalWrite(anodePins[((anode+2) % 3)], LOW);
  for(int i = 0; i < 7; i++){
    digitalWrite(cathodePins[i], !digits[anode][i]);
  }
  digitalWrite(anodePins[anode], HIGH);
}



void loop() {

  while(ss.available()){
    char c = ss.read();
    Serial.print(c);
    if(gps.encode(c)){
      newData = true;
    }
  }

  //change the anode pin every 1 milisecond
  if(abs(micros() - timer1) > 1000){
    currentAnode = (currentAnode + 1)% 3;
    showDigit(currentAnode);
    timer1 = micros();
  }

  //increment every 1.0s
  if(abs(millis() - timer2) > 1000){
    if(!newData){
      count++;
      if(count > 3){
        displayNumber(404);
      }
    }else{
      count = 0;
      displayNumber(int(gps.f_speed_kmph()));
    }
    timer2 = millis();
  }
}
