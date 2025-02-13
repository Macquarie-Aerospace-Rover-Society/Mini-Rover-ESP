// https://youtu.be/uYnBKLXCG6A?t=9
/* Pin Definitions */
#include <Arduino.h>
#include <ESP32Servo.h> // Install Library

// Servo signals
#define SERVO_A_SIGNAL 3
#define SERVO_B_SIGNAL 42
#define SERVO_C_SIGNAL 14

#define ANGLE_DELTA 5

#define A_MOVE_POS 't'
#define A_MOVE_NEG 'g'
#define B_MOVE_POS 'y'
#define B_MOVE_NEG 'h'
#define C_MOVE_POS 'u'
#define C_MOVE_NEG 'j'

Servo ServeArm[3];

void init_arm() {
    ServeArm[0].attach(SERVO_A_SIGNAL);
    ServeArm[1].attach(SERVO_B_SIGNAL);
    ServeArm[2].attach(SERVO_C_SIGNAL);
    // ServeArm[0].write(0); // Smack a user when ever power on
    // ServeArm[1].write(0);
    // ServeArm[2].write(0);
}

void arm_shift(Servo &p_servo, bool dir) {
    int angle = p_servo.read();
    if(dir){
        angle = angle + ANGLE_DELTA;
    } else {
        angle = angle - ANGLE_DELTA;
    }
    p_servo.write(angle);
}

void arm_drive(char c){
    if(A_MOVE_POS == c){
        arm_shift(ServeArm[0], true);
    }
    if(A_MOVE_NEG == c){
        arm_shift(ServeArm[0], false);
    }
    if(B_MOVE_POS == c){
        arm_shift(ServeArm[1], true);
    }
    if(B_MOVE_NEG == c){
        arm_shift(ServeArm[1], false);
    }
    if(C_MOVE_POS == c){
        arm_shift(ServeArm[2], true);
    }
    if(C_MOVE_NEG == c){
        arm_shift(ServeArm[2], false);
    }
}
