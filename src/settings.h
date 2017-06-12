#ifndef __INC_TOY_SETTINGS_H
#define __INC_TOY_SETTINGS_H

#define DATA_PIN    6
#define CLOCK_PIN   5
#define LED_TYPE    APA102
#define COLOR_ORDER EOrder::BGR
#define NUM_LEDS    64
#define WIDTH       8
#define HEIGHT      8
#define BOUSTROPHEDON false

#define CUR 0 // index to current buffer
#define PREV 1 // index to previous buffer

//#define BUTTON1_PIN 5
//#define BUTTON2_PIN 6
#define BUTTON_POWER DAC

#define BRIGHTNESS         96
#define FRAMES_PER_SECOND  120

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#endif
