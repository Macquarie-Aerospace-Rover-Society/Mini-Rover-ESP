/**
 * Config file, place all changes here
 * 
 */

// Crude top wrap
#ifndef MARS_CTRL_CONFIG
#define MARS_CTRL_CONFIG

// Default WiFi AP details to connect to, use config.env.h for private AP
#define MARS_WIFI_SSID_DEFAULT "MARS_BaseStation"
#define MARS_WIFI_PASS_DEFAULT "marsbasestation"

#define MOTOR_LEFT_LEAD_PWM 21   // White  - M2B (left reverse)
#define MOTOR_LEFT_LEAD_DIR 47  // Brown  - M2A (left forward)
// #define MOTOR_LEFT_REAR_PWM
// #define MOTOR_LEFT_REAR_DIR


#define MOTOR_RIGHT_LEAD_PWM 45  // Yellow - M1A (right reverse)
#define MOTOR_RIGHT_LEAD_DIR 48  // Orange - M1B (right forward)
// #define MOTOR_RIGHT_REAR_PWM
// #define MOTOR_RIGHT_REAR_DIR







// Driver type - Motor
// types: MOTOR, MOTOR_DIFF

#define L_LEAD MOTOR_DIFF
#define R_LEAD MOTOR_DIFF
// #define L_REAR
// #define R_REAR

// Pins for controls

// Control keys
// #define LEFT 'a'
// #define RIGHT 'd'
// #define FORWARD 'w'
// #define BACKWARD 's'
// #define INFO 'i'
// #define HALT 'c'

// #define CLOCKWISE   'e'
// #define A_CLOCKWISE 'q'

// Other defaults
// #define DEFAULT_ORIENTATION HIGH
















































/* !!!!!!!!!! Only Edit above this line !!!!!!!!!! */

#if __has_include("config.env.h")
    #include "config.env.h"
    #if !defined(MARS_WIFI_SSID) || !defined(MARS_WIFI_PASS)
        #error "*** Please define MARS_WIFI_SSID and MARS_WIFI_PASS in config.env.h ***"
    #endif
#else
    #pragma message("\n*** Warning: config.env.h' not found, using defaults. ***\n")
    #define MARS_WIFI_SSID MARS_WIFI_SSID_DEFAULT
    #define MARS_WIFI_PASS MARS_WIFI_PASS_DEFAULT
#endif

// Control keys
#ifndef LEFT
#define LEFT 'a'
#endif
#ifndef RIGHT
#define RIGHT 'd'
#endif
#ifndef FORWARD
#define FORWARD 'w'
#endif
#ifndef BACKWARD
#define BACKWARD 's'
#endif
#ifndef INFO
#define INFO 'i'
#endif
#ifndef HALT
#define HALT 'c'
#endif
#ifndef CLOCKWISE
#define CLOCKWISE   'e'
#endif
#ifndef A_CLOCKWISE
#define A_CLOCKWISE 'q'
#endif
// 5 Speed Levels - 255/5 => 51
#define MTRINC 51
#define MTRDEC -51

// PWM max value
#define PWMMAX 255

// Low or HIGH
#define DEFAULT_ORIENTATION HIGH

#endif