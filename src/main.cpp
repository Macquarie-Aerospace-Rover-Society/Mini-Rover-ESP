#include <Arduino.h>

#include "simpleWIFI.hpp"

#include "arm.hpp"

// Motor pin stubs - Replace with actual pin numbers
/* 
 * https://github.com/Freenove/Freenove_Basic_Starter_Kit_for_ESP32_S3
 * PWM pins capable pins are all pins apparently
 * STAR pins (suspected PSRAM / FLASH pins) are 35, 36,37
 * 
 * PWM pins chosen 41,42,43,44 : PIN 41 on silkscreen 
 * DIR pins chosen 4 ,5 ,6 ,7  : PIN 4  on silkscreen
 */

#define MOTOR_A_PWM 41  // Replace this
#define MOTOR_A_DIR 4   // Replace this

#define MOTOR_B_PWM 42  // Replace this
#define MOTOR_B_DIR 5   // Replace this

#define MOTOR_C_PWM 2  // Replace this
#define MOTOR_C_DIR 6   // Replace this

#define MOTOR_D_PWM 1  // Replace this
#define MOTOR_D_DIR 7   // Replace this

// M1A > M1B === FORWARD
#define MOTOR_ARM_SPIN_DIFF_A 9  // M1A
#define MOTOR_ARM_SPIN_DIFF_B 10 // M1B

// Control keys
#define LEFT 'a'
#define RIGHT 'd'
#define FORWARD 'w'
#define BACKWARD 's'
#define INFO 'i'
#define HALT 'c'

#define CLOCKWISE   'e'
#define A_CLOCKWISE 'q'

// 5 Speed Levels - 255/5 => 51
#define MTRINC 51
#define MTRDEC -51

// Use Webserver
#define WEB_INTERFACE

// PWM max value
#define PWMMAX 255

// Low or HIGH
#define DEFAULT_ORIENTATION HIGH

#define MOTOR_CONTROL(x, y) \
  setSpeed(motor1, motor1.velocity + x); \
  setSpeed(motor2, motor2.velocity + y); \
  setSpeed(motor3, motor3.velocity + x); \
  setSpeed(motor4, motor4.velocity + y)

// 255 max 0 min
#define POWER_LVL 255

//##############################
/* Type defs */

typedef struct mah_motor {
  int velocity;
  uint8_t phase, enable;  // Phase is the direction and enable pwm
  uint8_t orientation;
} MOTOR;

typedef struct nah_motor {
  int velocity;
  uint8_t dir_A, pwm_B;
  uint8_t orientation;
} MOTOR_DIFF;

// typedef struct mah_funcs {
//   void (drive)(char);
// } FUN_FUNC;



//##############################
/* Function definitions */

void motor_init(MOTOR &p_motor, uint8_t direction_pin, uint8_t pwm_pin);
int setSpeed(MOTOR &p_motor, int velocity);
int motor_flip_orientation(MOTOR &p_motor);

int setSpeed(MOTOR_DIFF &p_motor, int velocity);
void motor_init(MOTOR_DIFF &p_motor, uint8_t direction_pin, uint8_t pwm_pin);

void drive(char c);
void drive_arm(char c);

void MARS_WIFI_simple_simple_handle(){
    if (server.hasArg("plain")) {
        String payload = server.arg("plain");
        char str[100];
        snprintf(str,10, "%s",payload);
        int i = 0;
        while(str[i]!= 0){
          drive(str[i]);
          drive_arm(str[i]);
          arm_drive(str[i]);
          i++;
        }

        snprintf(str, sizeof(str), "Successful trigger of function: %s\n", payload);
        Serial.print(str);
        server.send(200, "text/plain", str);
    }
}

//##############################
/* Global vars */

MOTOR motor1;
MOTOR motor2;
MOTOR motor3;
MOTOR motor4;

/* Arm stuff */
MOTOR_DIFF motor_base;


//##############################
/* Main program entry point */

void setup() {
  Serial.begin(115200);

  // Initialize motors - Update pin values as needed
  motor_init(motor1, MOTOR_A_DIR, MOTOR_A_PWM);
  motor_init(motor2, MOTOR_B_DIR, MOTOR_B_PWM);
  motor_init(motor3, MOTOR_C_DIR, MOTOR_C_PWM);
  motor_init(motor4, MOTOR_D_DIR, MOTOR_D_PWM);
  motor_flip_orientation(motor3);

  motor_init(motor_base, MOTOR_ARM_SPIN_DIFF_A, MOTOR_ARM_SPIN_DIFF_B);

  Serial.println("HELLO TERMINAL");
#ifdef WEB_INTERFACE
  MARS_WIFI_init(MARS_WIFI_simple_simple_handle);
#endif

  init_arm();
}

void loop() {

#ifdef WEB_INTERFACE
  server.handleClient();
#endif

  if (Serial.available()) {
    char c = Serial.read();
    drive(c);
    drive_arm(c);
    arm_drive(c); // This is cursed....
  }
}

//##############################
/* Function implementations */

void motor_init(MOTOR &p_motor, uint8_t direction_pin, uint8_t pwm_pin) {
  p_motor.enable = pwm_pin;
  p_motor.phase = direction_pin;

  p_motor.orientation = DEFAULT_ORIENTATION;

  pinMode(direction_pin, OUTPUT);
  setSpeed(p_motor, 0);
}

/* Direction pin isn't really a direction choice, it just allows for pwm to appear negative for reverse */
void motor_init(MOTOR_DIFF &p_motor, uint8_t direction_pin, uint8_t pwm_pin) {
  p_motor.dir_A = direction_pin;
  p_motor.pwm_B = pwm_pin;

  p_motor.orientation = DEFAULT_ORIENTATION;

  pinMode(direction_pin, OUTPUT);
  setSpeed(p_motor, 0);
}

/* Values for speeds
 * 0  , 0  => stop ==
 * 0  , 1  => max  <<
 * 1  , 0  => max  >>
 * 0.5, 1  => 50%  <
 * 0.5, 0  => 50%  >
 * .25, 1  => 75%  <
 * .75, 0  => 75%  >
 * 
 */
int setSpeed(MOTOR_DIFF &p_motor, int velocity) {
  if(velocity >  PWMMAX){
    velocity = PWMMAX;
  }
  if(velocity < -(PWMMAX)){
    velocity = -(PWMMAX);
  }
  p_motor.velocity = velocity;
  if(p_motor.orientation){
    velocity = velocity * -1;
  }
  if(velocity == 0){
    p_motor.velocity = velocity;
    digitalWrite(p_motor.dir_A, LOW);
    analogWrite(p_motor.pwm_B, 0);
    return 0;
  }

  if(velocity > 0){
    int speed = map(velocity, 0, PWMMAX, 0,PWMMAX);
    digitalWrite(p_motor.dir_A, LOW);
    analogWrite(p_motor.pwm_B, speed);
  } else {
    int speed = map(velocity, -PWMMAX, 0, 0,PWMMAX);
    digitalWrite(p_motor.dir_A, HIGH);
    analogWrite(p_motor.pwm_B, speed);
  }
  return 0;
}

int setSpeed(MOTOR &p_motor, int velocity) {
  if(velocity >  PWMMAX){
    velocity = PWMMAX;
  }
  if(velocity < -(PWMMAX)){
    velocity = -(PWMMAX);
  }
  uint8_t dir = p_motor.orientation;
  p_motor.velocity = velocity;
  uint8_t speed;
  if (velocity < 0) {
    speed = velocity * -1;
    dir = !dir;
  } else {
    speed = velocity;
  }

  digitalWrite(p_motor.phase, dir);
  analogWrite(p_motor.enable, speed);
  return 0;
}

int motor_flip_orientation(MOTOR &p_motor){
  p_motor.orientation = !p_motor.orientation;
  return 0;
}


void drive(char c){
  if (LEFT == c) {
    Serial.println("LEFT");
    setSpeed(motor1, motor1.velocity + MTRINC);
    setSpeed(motor2, motor2.velocity + MTRINC);
    setSpeed(motor3, motor3.velocity - MTRINC);
    setSpeed(motor4, motor4.velocity - MTRINC);
  } else if (RIGHT == c) {
    Serial.println("RIGHT");
    setSpeed(motor1, motor1.velocity - MTRINC);
    setSpeed(motor2, motor2.velocity - MTRINC);
    setSpeed(motor3, motor3.velocity + MTRINC);
    setSpeed(motor4, motor4.velocity + MTRINC);
  } else if (FORWARD == c) {
    Serial.println("FORWARD");
    setSpeed(motor1, motor1.velocity + MTRINC);
    setSpeed(motor2, motor2.velocity + MTRINC);
    setSpeed(motor3, motor3.velocity + MTRINC);
    setSpeed(motor4, motor4.velocity + MTRINC);
  } else if (BACKWARD == c) {
    Serial.println("BACKWARD");
    setSpeed(motor1, motor1.velocity - MTRINC);
    setSpeed(motor2, motor2.velocity - MTRINC);
    setSpeed(motor3, motor3.velocity - MTRINC);
    setSpeed(motor4, motor4.velocity - MTRINC);
  } else if (HALT == c) {
    Serial.println("HALTING");
    setSpeed(motor1, 0);
    setSpeed(motor2, 0);
    setSpeed(motor3, 0);
    setSpeed(motor4, 0);
  } else if (INFO == c) {
    Serial.printf("Motor values: M1: %d, M2: %d, M3: %d, M4: %d\n", motor1.velocity, motor2.velocity, motor3.velocity, motor4.velocity);
  } else {
    Serial.printf("Controls: %c%c%c%c\n", FORWARD, LEFT, BACKWARD, RIGHT);
  }
}


void drive_arm(char c){

  if (CLOCKWISE == c) {
    Serial.println("Clockwise");
    setSpeed(motor_base,  (MTRINC*3/4));
  } else if (A_CLOCKWISE == c) {
    Serial.println("Anti-Clockwise");
    setSpeed(motor_base, - (MTRINC*3/4));
  } else {
    if (motor_base.velocity > 0){
      setSpeed(motor_base, motor_base.velocity*3/4);
    } else if (motor_base.velocity < 0) {
      setSpeed(motor_base, motor_base.velocity*3/4);
    }
  }

}

/*
if (CLW == c) {
  Serial.println("Clockwise");
  setSpeed(motor_base, motor_base.velocity + MTRINC);
} else if (ACLW == c) {
  Serial.println("Anti-Clockwise");
  setSpeed(motor_base, motor_base.velocity - MTRINC);
} else {
    if (motor_base.velocity > 0){
      setSpeed(motor_base, motor_base.velocity - MTRINC);
    } else if (motor_base.velocity < 0) {
      setSpeed(motor_base, motor_base.velocity + MTRINC);
    }
  }
*/