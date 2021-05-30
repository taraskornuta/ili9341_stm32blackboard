#ifndef COLORS_H_
#define COLORS_H_

// Basic Color definitions
#define  COLOR_BLACK                         (unsigned short)(0x0000)
#define  COLOR_BLUE                          (unsigned short)(0x001F)
#define  COLOR_RED                           (unsigned short)(0xF800)
#define  COLOR_GREEN                         (unsigned short)(0x07E0)
#define COLOR_CYAN                          (unsigned short)(0x07FF)
#define COLOR_MAGENTA                       (unsigned short)(0xF81F)
#define COLOR_YELLOW                        (unsigned short)(0xFFE0)
#define COLOR_WHITE                         (unsigned short)(0xFFFF)

#define COLOR_NAVY              (unsigned short)(0x000F)
#define COLOR_DARKGREEN             (unsigned short)(0x03E0)
#define COLOR_DARKCYAN            (unsigned short)(0x03EF)
#define COLOR_MAROON            (unsigned short)(0x7800)
#define COLOR_PURPLE            (unsigned short)(0x780F)
#define COLOR_OLIVE              (unsigned short)(0x7BE0)
#define COLOR_LIGHTGREY            (unsigned short)(0xC618)
#define COLOR_DARKGREY            (unsigned short)(0x7BEF)
#define COLOR_ORANGE            (unsigned short)(0xFD20)
#define COLOR_GREENYELLOW          (unsigned short)(0xAFE5)

// Grayscale Values
#define COLOR_GRAY_15                       (unsigned short)(0x0861)    //  15  15  15
#define COLOR_GRAY_30                       (unsigned short)(0x18E3)    //  30  30  30
#define COLOR_GRAY_50                       (unsigned short)(0x3186)    //  50  50  50
#define COLOR_GRAY_80                       (unsigned short)(0x528A)    //  80  80  80
#define COLOR_GRAY_128                      (unsigned short)(0x8410)    // 128 128 128
#define COLOR_GRAY_200                      (unsigned short)(0xCE59)    // 200 200 200
#define COLOR_GRAY_225                      (unsigned short)(0xE71C)    // 225 225 225

// Color Palettes
#define COLOR_THEME_LIMEGREEN_BASE          (unsigned short)(0xD7F0)    // 211 255 130
#define COLOR_THEME_LIMEGREEN_DARKER        (unsigned short)(0x8DE8)    // 137 188  69
#define COLOR_THEME_LIMEGREEN_LIGHTER       (unsigned short)(0xEFF9)    // 238 255 207
#define COLOR_THEME_LIMEGREEN_SHADOW        (unsigned short)(0x73EC)    // 119 127 103
#define COLOR_THEME_LIMEGREEN_ACCENT        (unsigned short)(0xAE6D)    // 169 204 104

#define COLOR_THEME_VIOLET_BASE             (unsigned short)(0x8AEF)    // 143  94 124
#define COLOR_THEME_VIOLET_DARKER           (unsigned short)(0x4187)    //  66  49  59
#define COLOR_THEME_VIOLET_LIGHTER          (unsigned short)(0xC475)    // 194 142 174
#define COLOR_THEME_VIOLET_SHADOW           (unsigned short)(0x40E6)    //  66  29  52
#define COLOR_THEME_VIOLET_ACCENT           (unsigned short)(0xC992)    // 204  50 144

#define COLOR_THEME_EARTHY_BASE             (unsigned short)(0x6269)    //  97  79  73
#define COLOR_THEME_EARTHY_DARKER           (unsigned short)(0x3103)    //  48  35  31
#define COLOR_THEME_EARTHY_LIGHTER          (unsigned short)(0x8C30)    // 140 135 129
#define COLOR_THEME_EARTHY_SHADOW           (unsigned short)(0xAB29)    // 173 102  79
#define COLOR_THEME_EARTHY_ACCENT           (unsigned short)(0xFE77)    // 250 204 188

#define COLOR_THEME_SKYBLUE_BASE            (unsigned short)(0x95BF)    // 150 180 255
#define COLOR_THEME_SKYBLUE_DARKER          (unsigned short)(0x73B0)    // 113 118 131
#define COLOR_THEME_SKYBLUE_LIGHTER         (unsigned short)(0xE75F)    // 227 235 255
#define COLOR_THEME_SKYBLUE_SHADOW          (unsigned short)(0x4ACF)    //  75  90 127
#define COLOR_THEME_SKYBLUE_ACCENT          (unsigned short)(0xB5F9)    // 182 188 204

// Using these values allows you to update the entire UI color scheme in one location
#define COLOR_THEME_DEFAULT_BASE            COLOR_THEME_LIMEGREEN_BASE
#define COLOR_THEME_DEFAULT_DARKER          COLOR_THEME_LIMEGREEN_DARKER
#define COLOR_THEME_DEFAULT_LIGHTER         COLOR_THEME_LIMEGREEN_LIGHTER
#define COLOR_THEME_DEFAULT_SHADOW          COLOR_THEME_LIMEGREEN_SHADOW
#define COLOR_THEME_DEFAULT_ACCENT          COLOR_THEME_LIMEGREEN_ACCENT

#endif /* COLORS_H_ */
