#ifndef __INC_TOY_UTILS_H
#define __INC_TOY_UTILS_H

// Utilities
// ---------

uint8_t random8Except(uint8_t max, uint8_t except)
{
    // returns a random number 0 <= r < max, but won't return r == except.
    // useful for picking a random array value different from the previous one.
    uint8_t r = random8(max);
    while (r == except && max > 1)
    {
        r = random8(max);
    }
    return r;
}

String colorToString(CRGB color)
{
    return String::format("(%d, %d, %d)", color.r, color.g, color.b);
}

String paletteToString(CRGBPalette16 palette)
{
    String paletteString = "[";
    for (uint8_t i = 0; i < 16; i++)
    {
        paletteString += colorToString(palette[i]) + (i < 15 ? ", " : "]");
    }
    return paletteString;
}

#endif
