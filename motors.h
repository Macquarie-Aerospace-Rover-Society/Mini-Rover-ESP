#include <Arduino.h>
#include "esp_http_server.h" // standard esp library

#include "config.h"
#include "arm.hpp"

/* Mini Robotic Arm Pin Allocations */
// M1A > M1B === FORWARD
#define MOTOR_ARM_SPIN_DIFF_A 3  // M1A
#define MOTOR_ARM_SPIN_DIFF_B 46 // M1B

/* 5 Speed Levels - 255/5 => 51 */
#define MTRINC 51
#define MTRDEC -51

// PWM max value
#define PWMMAX 255

// Low or HIGH
#define DEFAULT_ORIENTATION HIGH

// #define MOTOR_CONTROL(x, y) \
//   setSpeed(motor_LL, motor_LL.velocity + x); \
//   setSpeed(motor_RL, motor_RL.velocity + y); \
//   setSpeed(motor_LR, motor_LR.velocity + x); \
//   setSpeed(motor_RR, motor_RR.velocity + y)


typedef struct mah_motor {
  int velocity;
  uint8_t phase, enable;  // Phase is the direction and enable pwm
  uint8_t orientation;
} MOTOR;

typedef struct nah_motor {
  int velocity;
  uint8_t dir_A, pwm_B;
  uint8_t orientation; // Unused, just flip the pins
} MOTOR_DIFF;

//##############################
/* Function definitions */

void motor_setup(); // TODO: add comment

// Functions for PWM Motors

void motor_init(MOTOR &p_motor, uint8_t direction_pin, uint8_t pwm_pin); // TODO: add comment
int setSpeed(MOTOR &p_motor, int velocity); // TODO: add comment
int motor_flip_orientation(MOTOR &p_motor); // TODO: add comment

// Functions for DIFF motors.

int setSpeed(MOTOR_DIFF &p_motor, int velocity); // TODO: add comment
void motor_init(MOTOR_DIFF &p_motor, uint8_t direction_pin, uint8_t pwm_pin); // TODO: add comment

// Functions to drive motors - "Takes in command from world to move"
void drive(char c); // TODO: add comment
void drive_arm(char c); // TODO: add comment

/* ??? */
esp_err_t MARS_WIFI_simple_simple_handle(httpd_req_t *req) {
    // Buffer to store received data
    char payload[100] = {0};

    // Read request content
    int ret = httpd_req_recv(req, payload, sizeof(payload) - 1);
    if (ret <= 0) { 
        // If ret == 0, it means the client closed the connection before sending data
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req); // Send timeout response
        }
        return ESP_FAIL;
    }

    // Process the payload (assuming ASCII characters)
    int i = 0;
    while (payload[i] != '\0') {
        drive(payload[i]);
        drive_arm(payload[i]);
        arm_drive(payload[i]);
        i++;
    }

    // Prepare response message
    char response[100];
    snprintf(response, sizeof(response), "Successful trigger of function: %s\n", payload);
    
    // Send response back to client
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, response, strlen(response));

    return ESP_OK;
}

// Types are declared in config
// L_LEAD motor_LL;
// R_LEAD motor_RL;
// #ifdef L_REAR
// L_REAR motor_LR;
// #endif
// #ifdef R_REAR
// R_REAR motor_RR;
// #endif

#ifndef L_REAR
  L_LEAD motor_LL;
  #define L_INIT() \
    motor_init(motor_LL, MOTOR_LEFT_LEAD_DIR, MOTOR_LEFT_LEAD_PWM);
  #define L_TANK(x) \
    setSpeed(motor_LL, motor_LL.velocity + x);
  #define L_STOP() \
    setSpeed(motor_LL, 0);
#else
  R_REAR motor_LL;
  R_REAR motor_LR;
  #define L_INIT() \
    motor_init(motor_LL, MOTOR_LEFT_LEAD_DIR, MOTOR_LEFT_LEAD_PWM); \
    motor_init(motor_LR, MOTOR_LEFT_REAR_DIR, MOTOR_LEFT_REAR_PWM);
  #define L_TANK(x) \
    setSpeed(motor_LL, motor_LL.velocity + x); \
    setSpeed(motor_LR, motor_LR.velocity + x);
  #define L_STOP() \
    setSpeed(motor_LL, 0); \
    setSpeed(motor_LR, 0);
#endif

#ifndef R_REAR
  R_LEAD motor_RL;
  #define R_INIT() \
    motor_init(motor_RL, MOTOR_RIGHT_LEAD_DIR, MOTOR_RIGHT_LEAD_PWM);
  #define R_TANK(x) \
    setSpeed(motor_RL, motor_RL.velocity + x);
  #define R_STOP() \
    setSpeed(motor_RL, 0);
#else
  R_LEAD motor_RL;
  L_REAR motor_RR;
  #define R_INIT() \
    motor_init(motor_RL, MOTOR_RIGHT_LEAD_DIR, MOTOR_RIGHT_LEAD_PWM); \
    motor_init(motor_RR, MOTOR_RIGHT_REAR_DIR, MOTOR_RIGHT_REAR_PWM);
  #define R_TANK(x) \
    setSpeed(motor_RL, motor_RL.velocity + x); \
    setSpeed(motor_RR, motor_RR.velocity + x);
  #define R_STOP() \
    setSpeed(motor_RL, 0); \
    setSpeed(motor_RR, 0);
#endif

#define ALL_STOP() \
  L_STOP() \
  R_STOP()


/* Arm stuff */
MOTOR_DIFF motor_base;

void motor_setup(){
  // TODO replace with arrays
  // motor_init(motor_LL, MOTOR_LEFT_LEAD_DIR, MOTOR_LEFT_LEAD_PWM);
  // motor_init(motor_RL, MOTOR_RIGHT_LEAD_DIR, MOTOR_RIGHT_LEAD_PWM);
  // #ifdef L_REAR
  // motor_init(motor_LR, MOTOR_LEFT_REAR_DIR, MOTOR_LEFT_REAR_PWM);
  // #endif
  // #ifdef R_REAR
  // motor_init(motor_RR, MOTOR_RIGHT_REAR_DIR, MOTOR_RIGHT_REAR_PWM);
  // #endif
  L_INIT();
  R_INIT();
  // motor_flip_orientation(motor_LL);
  // motor_flip_orientation(motor_RL);

  // TODO move this to arm section
  motor_init(motor_base, MOTOR_ARM_SPIN_DIFF_A, MOTOR_ARM_SPIN_DIFF_B);
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

/** Values for speeds
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

/**
 * Invert the directtion of drive for positive / negative velocity.
 */
int motor_flip_orientation(MOTOR &p_motor){
  p_motor.orientation = !p_motor.orientation;
  return 0;
}


/**
 * 
 */
void drive(char c){
  // TODO replace with loops and arrays
  if (LEFT == c) {
    Serial.println("LEFT");
    L_TANK( - MTRINC );
    R_TANK( + MTRINC );
  } else if (RIGHT == c) {
    Serial.println("RIGHT");
    L_TANK( + MTRINC );
    R_TANK( - MTRINC );
  } else if (FORWARD == c) {
    Serial.println("FORWARD");
    L_TANK( + MTRINC );
    R_TANK( + MTRINC );
  } else if (BACKWARD == c) {
    Serial.println("BACKWARD");
    L_TANK( - MTRINC );
    R_TANK( - MTRINC );
  } else if (HALT == c) {
    Serial.println("HALTING");
    ALL_STOP();
  } else if (INFO == c) {
    // TODO insert DEBUG call
    // Serial.printf("Motor values: M1: %d, M2: %d, M3: %d, M4: %d\n", motor_LL.velocity, motor_RL.velocity, motor_LR.velocity, motor_RR.velocity);
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
