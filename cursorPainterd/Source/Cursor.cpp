#include "Cursor.h"

// All image colors, as an array of RGBA color components.
static const constexpr unsigned char colors [8][4] =
{
    {0, 0, 0, 255},
    {255, 0, 0, 0},
    {201, 191, 191, 255},
    {212, 145, 145, 255},
    {201, 158, 158, 255},
    {255, 24, 24, 46},
    {186, 172, 172, 255},
    {0, 0, 0, 0}
};

// All image data, stored in a string of color indices starting at index 'a'.
static const constexpr char* imageData = "aabbbbbbbbbbb"
                                         "aaabbbbbbbbbb"
                                         "acaabbbbbbbbb"
                                         "accaabbbbbbbb"
                                         "acccaabbbbbbb"
                                         "acdccaabbbbbb"
                                         "aceeccaabbbbb"
                                         "acefeccaabbbb"
                                         "aceffeccaabbb"
                                         "acefffeccaabb"
                                         "aceffffeccaab"
                                         "acefffffdccaa"
                                         "acefffeeggcca"
                                         "acefeegggaaaa"
                                         "acdegggcaabbb"
                                         "acgggaggcaabb"
                                         "acaaaaaggcabb"
                                         "aaabbbaagaabb"
                                         "bbbbbbhaaahbb";

// Gets the color of an image pixel.
FBPainter::RGBAPixel FBPainter::Cursor::getColor
(const size_t x, const size_t y)
{
    if (x >= width || y >= height)
    {
        return RGBAPixel();
    }
    size_t charIdx = y * width + x;
    size_t idx = imageData[charIdx] - 'a';
    return RGBAPixel(colors[idx][0], colors[idx][1], colors[idx][2],
            colors[idx][3]);
}