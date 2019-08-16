/**
 * @file Cursor.h
 *
 * @brief  Image data from Cursor.png encoded into a static class.
 */

#pragma once
#include "../FBPainter.hpp"
#include <cstddef>
#include <limits>

namespace FBPainter
{
    class Cursor
    {
    public:
        // Represents an invalid index:
        static const constexpr size_t npos = std::numeric_limits<size_t>::max();

        // Number of distinct image colors:
        static const constexpr size_t numColors = 8;

        // Image width in pixels:
        static const constexpr size_t width = 13;

        // Image height in pixels:
        static const constexpr size_t height = 19;

        /**
         * @brief  Gets the color of an image pixel.
         *
         * @param x  The pixel's x-coordinate.
         *
         * @param y  The pixel's y-coordinate.
         *
         * @return   The color of that pixel, or a null RGBAPixel if the
         *           coordinates are invalid.
         */
        static RGBAPixel getColor(const size_t x,const size_t y);
    };
}