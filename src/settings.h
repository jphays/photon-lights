#ifndef __INC_TOY_SETTINGS_H
#define __INC_TOY_SETTINGS_H

#define DATA_PIN    6
#define CLOCK_PIN   5
#define LED_TYPE    APA102
#define COLOR_ORDER EOrder::BGR
#define NUM_LEDS    96
#define WIDTH       4
#define HEIGHT      4
#define FACES       6
#define BOUSTROPHEDON true

#define CUR 0 // index to current buffer
#define PREV 1 // index to previous buffer

//#define BUTTON1_PIN 5
//#define BUTTON2_PIN 6
#define BUTTON_POWER DAC

#define BRIGHTNESS         64
#define FRAMES_PER_SECOND  80

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#endif
