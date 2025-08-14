#pragma once

#include "lvgl.h"

// Function type for square actions
typedef void (*square_action_t)(void);

// Structure to hold square data
typedef struct {
    const char* label;           // Text label for the square
    square_action_t action;      // Function to call when square is activated
    const char* description;     // Optional description/tooltip
    const void* image;          // Pointer to the image descriptor
} square_config_t;

// Function declarations
void square_1_action(void);
void square_2_action(void);
void square_3_action(void);
void square_4_action(void);
void square_5_action(void);
void square_6_action(void);
void square_7_action(void);
void square_8_action(void);
void square_9_action(void);

// Forward declarations of image descriptors
extern const lv_img_dsc_t wifi;
extern const lv_img_dsc_t bluetooth;
extern const lv_img_dsc_t folder;
extern const lv_img_dsc_t settings;
extern const lv_img_dsc_t power;
extern const lv_img_dsc_t server;
extern const lv_img_dsc_t radio;
extern const lv_img_dsc_t tv;
extern const lv_img_dsc_t sun;

// Array of square configurations
static const square_config_t square_configs[] = {
    {"WiFi", square_1_action, "WiFi Settings", &wifi},
    {"BT", square_2_action, "Bluetooth Settings", &bluetooth}, 
    {"IR", square_3_action, "IR Control", &tv},
    {"NFC", square_4_action, "NFC", &radio},
    {"Web Server", square_5_action, "Web Server", &server},
    {"Files", square_6_action, "File Manager", &folder},
    {"Display", square_7_action, "Brightness Settings", &sun},
    {"System", square_8_action, "System Settings", &settings},
    {"Power", square_9_action, "Power Options", &power},
    {NULL, NULL, NULL, NULL}  // Terminator
};