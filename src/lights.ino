//#pragma SPARK_NO_PREPROCESSOR
#include "FastLED.h"
FASTLED_USING_NAMESPACE;
//#include "Button.h"
#include "math.h"
#include "settings.h"
#include "utils.h"
#include "palettes.h"
#include "patterns.h"

SYSTEM_THREAD(ENABLED);
//SYSTEM_MODE(MANUAL);

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later."
#endif

CRGB leds[NUM_LEDS]; // actual output array
CRGB buffer[2][NUM_LEDS]; // intermediate buffers

//Button gPatternButton(BUTTON1_PIN, true, true, 20);
//Button gPaletteButton(BUTTON2_PIN, true, true, 20);
//Button gPowerButton(BUTTON_POWER, true, true, 20);

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePattern)(CRGB*, unsigned long);
SimplePattern gPatterns[] = {
    paletteSweep,
    paletteSweepWithGlitter,
    //alternating,
    distributed,
    distributedWithGlitter,
    //leaderSpread,
    //leaderSpreadWithGlitter,
    confetti,
    confetti2,
    juggle,
    //sinelon,
    //candle,
    //pulseTracer,
    beatPhaser,
    //squares,
    descent,
    //mirror,
    //hatchflash,
    hatchflashCube,
    //fade,
    //scanner,
    cube1,
    cube2
};

// List of palettes to use
CRGBPalette16 gPalettes[] = {
    CRGBPalette16(RainbowColors_p),
    CRGBPalette16(RainbowStripeColors_p),
    CRGBPalette16(PartyColors_p),
    //CRGBPalette16(CloudColors_p),
    //CRGBPalette16(ForestColors_p),
    //CRGBPalette16(LavaColors_p),
    CRGBPalette16(OceanColors_p),
    modifiedRainbow_p,
    coldFire_p,
    //royal_p,
    //bp_p,
    spring_gp,
    summer_gp,
    autumn_gp,
    winter_gp,
    //hot_gp,
    cool_gp,
    //kelvino_p,
    //achilles_p
};

// Palette generation functions
typedef CRGBPalette16 (*PaletteFunction)();
PaletteFunction gPaletteFuncs[] = {
    //getPulsePalette,
    getPulse2Palette,
    getPulse4Palette,
    //getStrobePalette,
    getStrobe2Palette,
    getRampPalette,
    getCWCBPalette,
    getEvenPaletteR,
    getQuadPalette
};

// current and next palette, for smooth transitions
CRGBPalette16 gCurrentPalette(RainbowColors_p); // intro palette, e.g. CRGB::LightGrey
CRGBPalette16 gTargetPalette(RainbowColors_p);

uint8_t gCurrentPatternNumber = 0; // Index of current pattern
uint8_t gPreviousPatternNumber = 0; // Index of previous pattern
uint8_t gIndex = 0; // rotating index of "current" color

bool gCycle = true; // whether to automatically cycle patterns
bool gRandomize = true; // whether to add some randomness

bool gTransitioning = false; // currently in a transition?
int gTransitionMillis = 3000; // transition length

// time and frame state
unsigned long gFrame = 0;
unsigned long gSceneFrame[2] = { 0, 0 };
unsigned long gCurrentTime = 0;
unsigned long gPreviousTime = 0;
unsigned long gTransitionTime = 0;

// Transitions -----------------------------------------------------------------

void nextPattern()
{
    gPreviousPatternNumber = gCurrentPatternNumber;

    gCurrentPatternNumber = gRandomize ?
        random8Except(ARRAY_SIZE(gPatterns), gCurrentPatternNumber) :
        (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);

    memmove8(&buffer[PREV], &buffer[CUR], NUM_LEDS * sizeof(CRGB));
    memset(buffer[CUR], 0, sizeof(CRGB));

    gTransitioning = true;
    gTransitionTime = gCurrentTime;
    gSceneFrame[PREV] = gSceneFrame[CUR];
    gSceneFrame[CUR] = 0;

    if (Particle.connected())
    {
        Particle.publish("pattern-change", String(gCurrentPatternNumber));
    }
}

void nextPalette()
{
    bool useFunction;
    int index;

    if (gRandomize)
    {
        useFunction = random8(100) < 30;
        index = random8(useFunction ? ARRAY_SIZE(gPaletteFuncs) : ARRAY_SIZE(gPalettes));
    }
    else
    {
        useFunction = false;
        static uint8_t paletteIndex = 0;
        index = paletteIndex++ % (useFunction ? ARRAY_SIZE(gPaletteFuncs) : ARRAY_SIZE(gPalettes));
    }

    selectPalette(useFunction, index);
}

void selectPalette(bool useFunction, int index)
{
    if (useFunction)
        gTargetPalette = gPaletteFuncs[index]();
    else
        gTargetPalette = gPalettes[index];

    String paletteInfo = useFunction ? "F" : "P";
    paletteInfo += String(index) + ": " + paletteToString(gTargetPalette);

    if (Particle.connected())
    {
        Particle.publish("palette-change", paletteInfo);
    }
}

void renderFrame()
{

    // render the current pattern to the current buffer
    gPatterns[gCurrentPatternNumber](buffer[CUR], gSceneFrame[CUR]);

    // transition if necessary
    if (gTransitioning)
    {
        // render the previous pattern to the previous buffer
        gPatterns[gPreviousPatternNumber](buffer[PREV], gSceneFrame[PREV]);

        // calculate the transition progress and blend the current and previous scenes to the led array
        fract8 transitionPercent = min(((gCurrentTime - gTransitionTime) * 256) / gTransitionMillis, 255);
        blend(buffer[PREV], buffer[CUR], leds, NUM_LEDS, ease8InOutQuad(transitionPercent));

        // transition ending?
        if (transitionPercent >= 255)
        {
            gTransitioning = false;
        }
    }
    else
    {
        // no transition, just copy the current buffer to led array
        memmove8(&leds, &buffer[CUR], NUM_LEDS * sizeof(CRGB));
    }

}

// Cloud functions -------------------------------------------------------------

int setHue(String hue)
{
    gCycle = false;
    gTargetPalette = CRGBPalette16(CHSV(hue.toInt(), 255, 255));
    gCurrentPalette = CRGBPalette16(CHSV(hue.toInt(), 255, 255));
}

int nextPal(String arg)
{
  gCycle = false;
  gRandomize = false;
  nextPalette();
}

int nextPat(String arg)
{
    gCycle = false;
    gRandomize = false;
    nextPattern();
}

int randomize(String arg)
{
    gCycle = true;
    gRandomize = true;
    nextPalette();
}

// Input -----------------------------------------------------------------------

/*void handleInput()
{
    gPatternButton.read();
    gPaletteButton.read();

    if (gPatternButton.wasPressed())
    {
        gCycle = false;
        gRandomize = false;
        gTransitionMillis = 500;
        nextPattern();
    }

    if (gPaletteButton.wasPressed())
    {
        gCycle = false;
        gRandomize = false;
        nextPalette();
    }

    if (gPatternButton.pressedFor(1000) || gPaletteButton.pressedFor(1000))
    {
        gCycle = true;
        gRandomize = true;
        gTransitionMillis = 3000;
    }
}*/

void setupInput()
{
    pinMode(BUTTON_POWER, INPUT_PULLDOWN);
}

void handleInput()
{
    if (digitalRead(BUTTON_POWER) == HIGH)
    {
        sleep();
    }
}

void sleep()
{
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(100);
    System.sleep(SLEEP_MODE_DEEP);
}

// Initialization --------------------------------------------------------------

void setup()
{
    // tell FastLED about the LED strip configuration
    //FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    //FastLED.setCorrection(0xFFA08C);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);

    // set up serial
    Serial.begin(9600);
    // set up input
    setupInput();
    // take control of photon onboard LED
    RGBClass::control(true);

    // cloud functions
    //Particle.function("hue", setHue);
    Particle.function("next-palette", nextPal);
    Particle.function("next-pattern", nextPat);
    Particle.function("randomize", randomize);

    // randomize
    random16_add_entropy(analogRead(A0) + analogRead(A1));

    if (gRandomize)
    {
        gTargetPalette = gPalettes[random8(ARRAY_SIZE(gPalettes))];
        gCurrentPatternNumber = random8(ARRAY_SIZE(gPatterns));
    }
}

// Main Loop -------------------------------------------------------------------

void loop()
{

    // update state
    gFrame++;
    gSceneFrame[CUR]++;
    gSceneFrame[PREV]++;
    gPreviousTime = gCurrentTime;
    gCurrentTime = millis();

    // read buttons
    handleInput();

    // render the current frame to the led buffer, transitioning scenes if necessary
    renderFrame();

    // set the onboard RGB led to the first color of the strip
    RGBClass::color(leds[0].r, leds[0].g, leds[0].b);
    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND);

    // do some periodic updates
    EVERY_N_MILLISECONDS(20) { gIndex++; } // slowly cycle the "base color" through the palette
    EVERY_N_MILLISECONDS(20) { nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette); }
    EVERY_N_SECONDS(20) { if (gCycle) nextPattern(); } // change patterns periodically
    EVERY_N_SECONDS(17) { if (gCycle) nextPalette(); } // change palettes periodically

}
