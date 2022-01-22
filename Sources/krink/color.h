#pragma once
#include <stdint.h>

/*! \file color.h
    \brief Provides color operations.
*/

/// <summary>
/// Extract the value of a channel from a given color.
/// </summary>
/// <param name="color"></param>
/// <param name="ch">One of 'R', 'G', 'B', 'A'</param>
/// <returns>The value of the selected channel</returns>
uint32_t kr_color_get_channel(uint32_t color, const char ch);

/// <summary>
/// Set the value of a channel from a given color.
/// </summary>
/// <param name="color"></param>
/// <param name="ch">One of 'R', 'G', 'B', 'A'</param>
/// <param name="value">Value in range 0..255</param>
/// <returns>The modified color</returns>
uint32_t kr_color_set_channel(uint32_t color, const char ch, uint32_t value);

/// <summary>
/// Create a color from byte components.
/// </summary>
/// <param name="r"></param>
/// <param name="g"></param>
/// <param name="b"></param>
/// <param name="a"></param>
/// <returns>The color in `uint32_t` format</returns>
uint32_t kr_color_from_rgbai(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/// <summary>
/// Create a color from normalized float components.
/// </summary>
/// <param name="r"></param>
/// <param name="g"></param>
/// <param name="b"></param>
/// <param name="a"></param>
/// <returns>The color in `uint32_t` format</returns>
uint32_t kr_color_from_rgbaf_normalized(float r, float g, float b, float a);
