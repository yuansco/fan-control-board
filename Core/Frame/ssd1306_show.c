
#include "board.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_show.h"

/******************************************************************************/
/* OLED SSD1306 */

#ifdef SSD1306_OLED

/* 128x64 OLED with 7x10 FONT = 18x6 */
// init page data
char page_data[BUFF_SIZE_COLS][BUFF_SIZE_ROWS] = {
        "                 ",
        "                 ",
        "   Loading...    ",
        "                 ",
        "                 ",
        "                 ",
};

void ssd1306_show(void) {
    ssd1306_Fill(Black);

#if defined(SSD1306_INCLUDE_FONT_7x10)
    const int num_lines = 6;
#else /* SSD1306_INCLUDE_FONT_11x18 */
    const int num_lines = 3;
#endif

    int y = 0;
    for (int i = 0; i < num_lines; i++) {
        ssd1306_SetCursor(2, y);
        ssd1306_WriteString(&page_data[i][0], Font_data, White);
        y += Y_OFFSET;
    }

    ssd1306_UpdateScreen();
}

#else
void ssd1306_show(void) {return;}
#endif /* SSD1306_OLED */



