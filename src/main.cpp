#include <Arduino.h>

#define MOTOR_A_PWM  39
#define MOTOR_A_DIR  37

#define MOTOR_B_PWM  38
#define MOTOR_B_DIR  36

// 255 max 0 min
#define POWER_LVL 125

//##############################
/* Type defs */

typedef struct mah_motor{
  int velocity;
  uint8_t phase, enable; // Phase is the direction and enable pwm
} MOTOR;

//##############################
/* Function defintions */

int setSpeed(MOTOR &p_motor, int velocity);
void motor_init(MOTOR &p_motor, uint8_t direction_pin, uint8_t pwm_pin);

//##############################
/* Global vars */

MOTOR shit_motor;
MOTOR zuck_motor;

//##############################
/* Main program entry point */

void setup()
{
  Serial.begin(115200);
  motor_init(shit_motor, MOTOR_A_DIR, MOTOR_A_PWM);
  motor_init(zuck_motor, MOTOR_B_DIR, MOTOR_B_PWM);
  Serial.println("HELLO TERMINAL");

}

void loop()
{
//  int x = random(-255,255);
int x, y;
if(shit_motor.velocity == 0) {
  x = POWER_LVL;
} else {
  x = shit_motor.velocity * -1;
}
if(zuck_motor.velocity == 0) {
  y = -POWER_LVL;
} else {
  y = zuck_motor.velocity * -1;
}

 setSpeed(shit_motor, x);
 setSpeed(zuck_motor, y);
 Serial.printf("set the speed to %d\n", x);
 delay(3000);
}



//##############################
/* Function implementations */


void motor_init(MOTOR &p_motor, uint8_t direction_pin, uint8_t pwm_pin){
  p_motor.enable = pwm_pin;
  p_motor.phase = direction_pin;

  pinMode(direction_pin, OUTPUT);

  setSpeed(p_motor, 0);
}

int setSpeed(MOTOR &p_motor, int velocity){
  int dir = LOW;
  p_motor.velocity = velocity;
  int speed;
  if(velocity < 0){
    speed = velocity * -1;
    dir = HIGH;
  } else {
    speed = velocity;
  }
  digitalWrite(p_motor.phase,dir);
  analogWrite(p_motor.enable, velocity);
  return 0;
}