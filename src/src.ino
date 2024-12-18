#include <Arduino.h>

// Motor pin stubs - Replace with actual pin numbers
#define MOTOR_A_PWM 39  // Replace this
#define MOTOR_A_DIR 37  // Replace this
#define MOTOR_B_PWM 38  // Replace this
#define MOTOR_B_DIR 36  // Replace this
#define MOTOR_C_PWM 40  // Replace this
#define MOTOR_C_DIR 41  // Replace this
#define MOTOR_D_PWM 45  // Replace this
#define MOTOR_D_DIR 35  // Replace this

// Control keys
#define LEFT 'a'
#define RIGHT 'd'
#define FORWARD 'w'
#define BACKWARD 's'
#define INFO 'i'

#define MTRINC 100
#define MTRDEC -100

#define MOTOR_CONTROL(x, y) \
  setSpeed(motor1, motor1.velocity + x); \
  setSpeed(motor2, motor2.velocity + y); \
  setSpeed(motor3, motor3.velocity + x); \
  setSpeed(motor4, motor4.velocity + y)

// 255 max 0 min
#define POWER_LVL 125

//##############################
/* Type defs */

typedef struct mah_motor {
  int velocity;
  uint8_t phase, enable;  // Phase is the direction and enable pwm
} MOTOR;

//##############################
/* Function definitions */

int setSpeed(MOTOR &p_motor, int velocity);
void motor_init(MOTOR &p_motor, uint8_t direction_pin, uint8_t pwm_pin);

//##############################
/* Global vars */

MOTOR motor1;
MOTOR motor2;
MOTOR motor3;
MOTOR motor4;

//##############################
/* Main program entry point */

void setup() {
  Serial.begin(115200);

  // Initialize motors - Update pin values as needed
  motor_init(motor1, MOTOR_A_DIR, MOTOR_A_PWM);
  motor_init(motor2, MOTOR_B_DIR, MOTOR_B_PWM);
  motor_init(motor3, MOTOR_C_DIR, MOTOR_C_PWM);
  motor_init(motor4, MOTOR_D_DIR, MOTOR_D_PWM);

  Serial.println("HELLO TERMINAL");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (LEFT == c) {
      Serial.println("LEFT");
      setSpeed(motor1, motor1.velocity - MTRINC);
      setSpeed(motor2, motor2.velocity + MTRINC);
      setSpeed(motor3, motor3.velocity - MTRINC);
      setSpeed(motor4, motor4.velocity + MTRINC);
    } else if (RIGHT == c) {
      Serial.println("RIGHT");
      setSpeed(motor1, motor1.velocity + MTRINC);
      setSpeed(motor2, motor2.velocity - MTRINC);
      setSpeed(motor3, motor3.velocity + MTRINC);
      setSpeed(motor4, motor4.velocity - MTRINC);
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
    } else if (INFO == c) {
      Serial.printf("Motor values: M1: %d, M2: %d, M3: %d, M4: %d\n", motor1.velocity, motor2.velocity, motor3.velocity, motor4.velocity);
    } else {
      Serial.printf("Controls: %c%c%c%c\n", FORWARD, LEFT, BACKWARD, RIGHT);
    }
  }
}

//##############################
/* Function implementations */

void motor_init(MOTOR &p_motor, uint8_t direction_pin, uint8_t pwm_pin) {
  p_motor.enable = pwm_pin;
  p_motor.phase = direction_pin;

  pinMode(direction_pin, OUTPUT);
  setSpeed(p_motor, 0);
}

int setSpeed(MOTOR &p_motor, int velocity) {
  int dir = LOW;
  p_motor.velocity = velocity;
  int speed;
  if (velocity < 0) {
    speed = velocity * -1;
    dir = HIGH;
  } else {
    speed = velocity;
  }

  // Special handling for Motor D to reverse direction
  if (&p_motor == &motor4) {
    dir = !dir;  // Invert the direction
  }

  digitalWrite(p_motor.phase, dir);
  analogWrite(p_motor.enable, speed);
  return 0;
}