
#ifndef __SSD1306_SHOW_H__
#define __SSD1306_SHOW_H__

#ifdef SSD1306_OLED

#if defined(SSD1306_INCLUDE_FONT_7x10)

#define BUFF_SIZE_ROWS 18 // number of words in one line (7x18 = 126)
#define BUFF_SIZE_COLS 6  // number of line in oled (6x10 = 60)
#define Y_OFFSET 10

extern char page_data[BUFF_SIZE_COLS][BUFF_SIZE_ROWS];

#elif defined(SSD1306_INCLUDE_FONT_11x18)

#define BUFF_SIZE_ROWS 10 // number of words in one line (11x10 = 110)
#define BUFF_SIZE_COLS 3  // number of line in oled (3x18 = 54)
#define Y_OFFSET 18

extern char page_data[BUFF_SIZE_COLS][BUFF_SIZE_ROWS];

#endif


void ssd1306_show(void);

#endif /* SSD1306_OLED */

#endif /* __SSD1306_SHOW_H__ */