#ifndef __INC_TOY_PATTERNS_H
#define __INC_TOY_PATTERNS_H

extern CRGBPalette16 gCurrentPalette;
extern uint8_t gIndex;

// Patterns
// --------

void alternating(CRGB *pixels, unsigned long frame)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (i % 2 == 0)
        {
            pixels[i] = ColorFromPalette(gCurrentPalette, gIndex - i/8);
        }
        else
        {
            pixels[i] = ColorFromPalette(gCurrentPalette, gIndex + 128 - i/8);
        }
    }
}

void paletteSweep(CRGB *pixels, unsigned long frame)
{
    // basic pattern. cycle through the palette with a slight spread of colors.
    CRGBPalette16 palette = gCurrentPalette;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        pixels[i] = ColorFromPalette(palette, gIndex + (i * 7));
    }
}

void addGlitter(CRGB* pixels, fract8 chanceOfGlitter)
{
    // adds a sparkle every chanceOfGlitter/255 frames.
    const uint8_t maxBright = 90;  // brightness of sparkle
    const uint8_t fadeSteps = 18;  // frames to fade
    static int glitter[fadeSteps]; // glitter led index history

    for (int i = fadeSteps - 1; i > 0; i--)
    {
        uint8_t level = scale8(ease8InOutCubic((fadeSteps - i) * (256 / fadeSteps)), maxBright);
        pixels[glitter[i]] += CRGB(level, level, level);
        glitter[i] = glitter[i - 1];
    }

    if (random8() < chanceOfGlitter)
    {
        int led = random16(NUM_LEDS);
        pixels[led] += CRGB(maxBright, maxBright, maxBright);
        glitter[0] = led;
    }
    else
    {
        glitter[0] = NUM_LEDS;
    }
}

void paletteSweepWithGlitter(CRGB *pixels, unsigned long frame)
{
    // basic pattern with added glitter fading in.
    paletteSweep(pixels, frame);
    addGlitter(pixels, min(frame / 20, 100));
}

void leaderSpread(CRGB* pixels, unsigned long frame)
{
    // basic pattern with the center hue a few frames ahead of the rest.

    // randomize converging/diverging
    static int direction;
    if (frame == 1) direction = random8(2) == 1 ? 1 : -1;

    for (int i = 0; i < NUM_LEDS; i++)
    {
        pixels[(i + NUM_LEDS / 2) % NUM_LEDS] =
            ColorFromPalette(gCurrentPalette, gIndex + sin8((128 / NUM_LEDS) * (i + 1) * direction));
    }
}

void leaderSpreadWithGlitter(CRGB *pixels, unsigned long frame)
{
    // basic pattern with added glitter fading in.
    leaderSpread(pixels, frame);
    addGlitter(pixels, min(frame / 20, 100));
}

void distributed(CRGB* pixels, unsigned long frame)
{
    // palette hues evenly distributed among leds.
    const int spread = -255 / NUM_LEDS;
    //pixels[0] = ColorFromPalette(gCurrentPalette, sin8(gIndex));
    for (int i = 0; i < NUM_LEDS; i++)
    {
        pixels[i] = ColorFromPalette(gCurrentPalette, gIndex + spread * i);
    }
}

void distributedWithGlitter(CRGB* pixels, unsigned long frame)
{
    distributed(pixels, frame);
    addGlitter(pixels, min(frame / 20, 100));
}

void confetti(CRGB* pixels, unsigned long frame)
{
    // random colored speckles that blink in and fade smoothly.
    fadeToBlackBy(pixels, NUM_LEDS, 10);
    for (uint8_t i = 0; i < beatsin8(10, 1, 3); i++)
    {
        int pos = random16(NUM_LEDS);
        pixels[pos] += ColorFromPalette(gCurrentPalette, gIndex + i * 16);
    }
}

void confetti2(CRGB* pixels, unsigned long frame)
{
    // random colored speckles that blink in and fade smoothly.
    fadeToBlackBy(pixels, NUM_LEDS, beatsin8(8, 4, 12));
    for (uint8_t i = 0; i < beatsin8(10, 1, 3); i++)
    {
        int pos = random16(NUM_LEDS);
        pixels[pos] += ColorFromPalette(gCurrentPalette, gIndex + random8(beatsin8(6, 20, 160)));
    }
    addGlitter(pixels, min(frame / 20, 100));
}

void juggle(CRGB* pixels, unsigned long frame)
{
    // colored dots, weaving in and out of sync with each other.
    fadeToBlackBy(pixels, NUM_LEDS, 20);
    for (int i = 1; i <= 7; i++)
    {
        pixels[beatsin16(i, 0, NUM_LEDS)] |= ColorFromPalette(gCurrentPalette, gIndex + i * 3);
    }
}

void sinelon(CRGB* pixels, unsigned long frame)
{
    // a colored dot sweeping back and forth, with fading trails
    static uint8_t bpm = 6, loops = 5;
    if (frame == 1)
    {
        bpm = random8(2, 18);
        loops = random8(1, 8);
    }

    int beat = beatsin16(bpm, 0, NUM_LEDS * loops);
    int pos = beat % NUM_LEDS;
    static int prev_beat;
    if (frame == 1) prev_beat = beat;

    fadeToBlackBy(pixels, NUM_LEDS, 20);
    pixels[pos] = ColorFromPalette(gCurrentPalette, gIndex + pos);

    if (beat > prev_beat + 1)
    {
        for (int i = prev_beat + 1; i < beat; i++)
        {
            pixels[i % NUM_LEDS] = ColorFromPalette(gCurrentPalette, gIndex + pos);
        }
    }

    if (beat < prev_beat - 1)
    {
        for (int i = prev_beat - 1; i > beat; i--)
        {
            pixels[i % NUM_LEDS] = ColorFromPalette(gCurrentPalette, gIndex + pos);
        }
    }

    prev_beat = beat;
}

void candle(CRGB* pixels, unsigned long frame)
{
    // slow flicker with some slight noise
    uint8_t flickerBpm = 16;
    CRGBPalette16 palette = gCurrentPalette;
    uint8_t beat = beatsin8(flickerBpm, 150, 220) + beatsin8(flickerBpm * 8 / 3, 0, 30);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        pixels[i] = ColorFromPalette(palette, gIndex + (i * 3), beat - pow(random8(10), 1.2)); // , beat-gIndex+(i*3));
    }
}

void pulseTracer(CRGB* pixels, unsigned long frame)
{
    uint8_t bpm = 10;
    int offset = beatsin8(bpm, 0, 32) - 16;
    int pos = beatsin16(bpm * 5 / 2, 0, NUM_LEDS - 1);

    for (int i = 1; i < NUM_LEDS; i++)
    {
        pixels[(i + pos) % (NUM_LEDS - 1) + 1] =
            ColorFromPalette(gCurrentPalette, gIndex + (i * offset), 255 - beatsin8(bpm, 100, 255));
    }

    pixels[0] = ColorFromPalette(gCurrentPalette, gIndex, beatsin8(bpm, 60, 255));
}

void beatPhaser(CRGB* pixels, unsigned long frame)
{
    fadeToBlackBy(pixels, NUM_LEDS, beatsin8(8, 2, 8));
    int pos = scale8(beat8(beatsin8(10, 0, 60)), NUM_LEDS);
    pixels[pos] = ColorFromPalette(gCurrentPalette, gIndex, beatsin8(10, 100, 255));
}

// matrix code -----------------------------------------------------------------

uint16_t XY(uint8_t x, uint8_t y)
{
    if (!BOUSTROPHEDON) return (y * WIDTH) + x;
    else
    {
        if (y & 0x01)
            return (y * WIDTH) + (WIDTH - 1) - x;
        else
            return (y * WIDTH) + x;
    }
}

void square(CRGB* pixels, CRGB color, uint8_t radius)
{
    // top / bottom
    for (uint8_t x = WIDTH / 2 - radius; x < WIDTH / 2 + radius; x++)
    {
        pixels[XY(x, WIDTH / 2 - radius)] = color;
        pixels[XY(x, WIDTH / 2 + radius - 1)] = color;
    }

    // left / right
    for (uint8_t y = HEIGHT / 2 - radius; y < HEIGHT / 2 + radius; y++)
    {
        pixels[XY(HEIGHT / 2 - radius, y)] = color;
        pixels[XY(HEIGHT / 2 + radius - 1, y)] = color;
    }
}

void squares(CRGB* pixels, unsigned long frame)
{
    static uint8_t warp, direction;
    if (frame == 1)
    {
        warp = random8(20, 60);
        direction = random8(2) == 1 ? 1 : -1;
    }

    for (uint8_t radius = 1; radius < WIDTH / 2 + 1; radius++)
    {
        square(
            pixels,
            ColorFromPalette(
                gCurrentPalette,
                gIndex + (radius - 1) * warp * direction),
                //255 - (radius - 1) * warp / 2),
            radius);
    }
}

void row(CRGB* pixels, CRGB color, uint8_t row)
{
    for (uint8_t x = 0; x < WIDTH; x++)
    {
        pixels[XY(x, row)] = color;
    }
}

void column(CRGB* pixels, CRGB color, uint8_t column)
{
    for (uint8_t y = 0; y < HEIGHT; y++)
    {
        pixels[XY(column, y)] = color;
    }
}

void descent(CRGB* pixels, unsigned long frame)
{
    for (uint8_t r = 0; r < FACES * HEIGHT; r++)
    {
        row(pixels, ColorFromPalette(gCurrentPalette, gIndex + pow(r + 1, 2)), r);
    }
}

void mirror(CRGB* pixels, unsigned long frame)
{
    for (uint8_t r = 0; r < HEIGHT / 2; r++)
    {
        row(pixels, blend(ColorFromPalette(gCurrentPalette, gIndex), CRGB::Black, r * 75), HEIGHT / 2 + r);
        row(pixels, blend(ColorFromPalette(gCurrentPalette, gIndex + 128), CRGB::Black, r * 70), HEIGHT / 2 - 1 - r);
    }
}

void hatchflash(CRGB* pixels, unsigned long frame)
{
    fadeToBlackBy(pixels, NUM_LEDS, beatsin8(8, 3, 8));

    if (random8(100) < beatsin8(10, 10, 30))
    {
        uint8_t direction = random8(2);
        uint8_t row = random8(HEIGHT);
        uint8_t col = random8(WIDTH);

        for (uint8_t r = 0; r < (direction == 1 ? WIDTH : HEIGHT); r++)
        {
            pixels[direction == 1 ? XY(r, row) : XY(col, r)] += ColorFromPalette(gCurrentPalette, gIndex + direction * 128);
        }

        // if (random8(2) == 1) row(pixels, ColorFromPalette(gCurrentPalette, gIndex), random8(HEIGHT));
        // else column(pixels,ColorFromPalette(gCurrentPalette, gIndex + 128), random8(WIDTH));
    }
}

void fade(CRGB* pixels, unsigned long frame)
{
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            pixels[XY(x, y)] = blend(ColorFromPalette(gCurrentPalette, gIndex), CRGB::Black, 256 * y / HEIGHT);
        }
    }
}

void scanner(CRGB* pixels, unsigned long frame)
{

    // sinelon by rows/columns
    // a colored line sweeping back and forth, with fading trails
    static uint8_t bpm = 6, loops = 5, direction = 0;
    if (frame == 1)
    {
        bpm = random8(5, 18);
        loops = random8(3, 8);
        direction = random8(2);
    }

    int beat = beatsin16(bpm, 0, (direction == 0 ? WIDTH : HEIGHT) * loops);
    int pos = beat % (direction == 0 ? WIDTH : HEIGHT);

    fadeToBlackBy(pixels, NUM_LEDS, 15);

    if (direction == 0)
    {
        column(pixels, ColorFromPalette(gCurrentPalette, gIndex + beat), pos);
    }
    else
    {
        row(pixels, ColorFromPalette(gCurrentPalette, gIndex + beat), pos);
    }

}

// cube code -------------------------------------------------------------------

uint8_t faceOffset(uint8_t face) { return face * WIDTH * HEIGHT; }
uint16_t cubeXY(uint8_t face, uint8_t x, uint8_t y)
{
    return faceOffset(face) + XY(x, y);
}

void cube1(CRGB* pixels, unsigned long frame)
{

    for (uint8_t face = 0; face < FACES; face++)
    {
        for (uint8_t x = 0; x < WIDTH; x++)
        {
            for (uint8_t y = 0; y < HEIGHT; y++)
            {
                pixels[cubeXY(face, x, y)] = ColorFromPalette(gCurrentPalette, gIndex + face * 48);
            }
        }
    }

}

void cube2(CRGB* pixels, unsigned long frame)
{

    for (uint8_t face = 0; face < FACES; face++)
    {
        for (uint8_t x = 0; x < WIDTH; x++)
        {
            for (uint8_t y = 0; y < HEIGHT; y++)
            {
                pixels[cubeXY(face, x, y)] = ColorFromPalette(gCurrentPalette, gIndex + face * 48);
                if ((x == 1 || x == 2) && (y == 1 || y == 2))
                {
                    pixels[cubeXY(face, x, y)] += blend(CRGB::Black, CRGB::White, ease8InOutCubic(beatsin8(10, 0, 255)));
                }
                else if ((x == 0 || x == 3) && (y == 0 || y == 3))
                {
                    pixels[cubeXY(face, x, y)] = blend(ColorFromPalette(gCurrentPalette, gIndex + (face + 2) * 48), ColorFromPalette(gCurrentPalette, gIndex + face * 48), ease8InOutCubic(beatsin8(10, 0, 255)));
                }
            }
        }
    }

}

void hatchflashCube(CRGB* pixels, unsigned long frame)
{
    fadeToBlackBy(pixels, NUM_LEDS, beatsin8(8, 3, 8));

    if (random8(100) < beatsin8(10, 70, 90))
    {
        uint8_t direction = random8(2);
        uint8_t row = random8(HEIGHT);
        uint8_t col = random8(WIDTH);
        uint8_t face = random8(FACES);

        for (uint8_t r = 0; r < (direction == 1 ? WIDTH : HEIGHT); r++)
        {
            pixels[direction == 1 ? cubeXY(face, r, row) : cubeXY(face, col, r)] += ColorFromPalette(gCurrentPalette, gIndex + face * 32);
        }

        // if (random8(2) == 1) row(pixels, ColorFromPalette(gCurrentPalette, gIndex), random8(HEIGHT));
        // else column(pixels,ColorFromPalette(gCurrentPalette, gIndex + 128), random8(WIDTH));
    }
}

#endif
