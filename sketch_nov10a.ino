#include <MeMCore.h>
MeBuzzer buzzer;

// initialize ports
MeLineFollower lineFinder(PORT_1);
MeUltrasonicSensor ultraSensor(PORT_2);
MeRGBLed led(PORT_7);
MeLightSensor ldr(PORT_6);

// initialize notes
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319

// define pins
#define IR_LEFT A0
#define IR_RIGHT A1

// initialize motors
MeDCMotor motor_left(M1);
MeDCMotor motor_right(M2);


// motor speed
uint8_t MaxLeftSpeed = 150;
uint8_t MaxRightSpeed = 150;

// delay time turn
uint32_t delayturn = 750; // turn in 780ms
uint32_t delay180turn = 1500; // turn in 1560ms

// set point IR sensors
long setpointRight = 400;
long setpointLeft = 930;

// victory tune
void victory() {
 int melody[] = {
    NOTE_C6, NOTE_C6, NOTE_C6,
    NOTE_C6,
    NOTE_GS5, NOTE_AS5,
    NOTE_C6,   0,   NOTE_AS5,
    NOTE_C6,
    0,
 };
 int noteDurations[] = {
    15, 10, 12,
    4,
    4, 4,
    12, 12, 12,
    1,
    2,
 };
 for (int thisNote = 0; thisNote < 11; thisNote++) {
    
// to calculate the note duration, take one second
    int noteDuration = 1000/noteDurations[thisNote];
    buzzer.tone(8, melody[thisNote],noteDuration);   
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
// stop the tone playing:
    buzzer.noTone(8);
 }
}

// Detects a black line
// returns 1 if there is black line or 0 if there is no black line
int is_black_line(){
 int sensorState = lineFinder.readSensors();
 if (sensorState == S1_IN_S2_IN ||sensorState == S1_IN_S2_OUT||sensorState == S1_OUT_S2_IN){
    return 1;
 } 
 else{
    return 0;
 }
}

void turn_right() {
    motor_left.run(-MaxLeftSpeed);
    motor_right.run(-100);
    delay(delayturn);
    //motor_left.run(-MaxLeftSpeed);
    //motor_right.run(MaxRightSpeed); 
}

void turn_left() {
    motor_right.run(MaxRightSpeed);
    motor_left.run(100);
    delay(delayturn);
    //motor_left.run(-MaxLeftSpeed);
    //motor_right.run(MaxRightSpeed); 
}

void turn180() {
    motor_left.run(-MaxLeftSpeed);
    motor_right.run(-120);
    delay(delay180turn);
}

/**
 * Detect the color
 * @return 1 -> Red -> left-turn
 *         2 -> Green -> right-turn
 *         3 -> Yellow -> 180 turn within the same grid
 *         4 -> Black -> victory
 */
float colour[3] = {0,0,0};


int ledvalues[3][3]=
{
  {255,0,0},
  {0,255,0},
  {0,0,255}
};

long color() {
  for (int i=0; i<3; i++)
  {
    //flash led colours
    led.setColor(ledvalues[i][0],ledvalues[i][1], ledvalues[i][2]);
    led.show();
    delay(500);

    for (int j=0;j<5;j++) {
      //ldr reading specific colour
      colour[i] += ldr.read();
      delay(50);
    }
  }
  led.setColor(0,0,0);
  led.show();
  //print values
  for (int i=0;i<3;i++)
  {
    colour[i] /= 5;
    Serial.println(colour[i]); 
  }
  //red is max value
  if (colour[0] > colour[1] && colour[0] > colour[2]) {
    if (colour[0] + colour[1] + colour[2] < 1500) {
      Serial.println("black");
      return 4;
    }
    else if (colour[0]-colour[1] > 300) {
      Serial.println("red");
      return 1;
    }
    else if (colour[0]-colour[1] < 300) {
      Serial.println("yellow");
      return 3; 
    }
  } else if (colour[1] > colour[0] && colour[1] > colour[2]) {
    Serial.println("green");
    return 2;
  }
}

int avg(int pin)
{
  int sum = 0;
 for (int i = 0; i < 30; i += 1)
 {
  sum += analogRead(pin);
 }

 return sum / 30;
}

void setup()
{
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);
  Serial.begin(9600);
  setpointLeft = avg(IR_LEFT);
  setpointRight = avg(IR_RIGHT);
}

void loop()
{
  uint32_t distance = ultraSensor.distanceCm();
  /**
  int output_right = analogRead(IR_RIGHT);
  int output_left = analogRead(IR_LEFT);
  while ((output_right > setpointRight) && (output_left < setpointLeft)) {
    motor_left.run(-MaxLeftSpeed);
    motor_right.run(MaxRightSpeed);
    delay(100);
    output_right = analogRead(IR_RIGHT);
    output_left = analogRead(IR_LEFT);
  }
  if (output_left > setpointLeft) {
   motor_left.run(-MaxLeftSpeed);
   motor_right.run(MaxRightSpeed - 50);
   delay(100);} else if (output_right < setpointRight) {
   motor_left.run(-MaxLeftSpeed + 50);
   motor_right.run(MaxRightSpeed);  
   delay(100);  
  }
  */


  //stop if a black strip is identified
  if (is_black_line() == 1) {
    motor_left.stop();
    motor_right.stop();
    long detected_color = color();
    if (detected_color == 1) {
      turn_left();
    } else if (detected_color == 2) {
      turn_right();
    } else if (detected_color == 3) {
      turn180();
    } else if (detected_color == 4) {
      victory();
      delay(10000);
    }
  } else {
    int output_right = analogRead(IR_RIGHT);
    int output_left = analogRead(IR_LEFT);
    while ((output_right > setpointRight) && (output_left < setpointLeft)) {
      motor_left.run(-MaxLeftSpeed);
      motor_right.run(MaxRightSpeed);
      delay(100);
      output_right = analogRead(IR_RIGHT);
      output_left = analogRead(IR_LEFT);
    }
    if (output_left > setpointLeft) {
     motor_left.run(-MaxLeftSpeed);
     motor_right.run(MaxRightSpeed - 50);
     delay(100);
    } else if (output_right < setpointRight) {
     motor_left.run(-MaxLeftSpeed + 50);
     motor_right.run(MaxRightSpeed);  
     delay(100);  
    }    
  }
  /**
  uint32_t distance = ultraSensor.distanceCm();
  delay(100);
  while (distance < 😍 {
    motor_left.run(0);
    motor_right.run(0);
    turn_right();
    distance = ultraSensor.distanceCm();
    delay(100);
  }
  motor_left.run(-MaxLeftSpeed);
  motor_right.run(MaxRightSpeed);
  delay(100);
  */
}
