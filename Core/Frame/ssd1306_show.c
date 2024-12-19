
#include "board.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_show.h"

/******************************************************************************/
/* OLED SSD1306 */

#ifdef SSD1306_OLED

/* 128x64 OLED with 7x10 FONT = 18x6 */
// init null page
char page_data[BUFF_SIZE_COLS][BUFF_SIZE_ROWS] = {
        "                 ",
        "                 ",
        "   Loading...    ",
        "                 ",
        "                 ",
        "                 ",
};

void ssd1306_show(void) {

        int y = 0;

        ssd1306_Fill(Black);
        ssd1306_SetCursor(2, y);
        ssd1306_WriteString(&page_data[0][0], Font_data, White);
        y += Y_OFFSET;

        ssd1306_SetCursor(2, y);
        ssd1306_WriteString(&page_data[1][0], Font_data, White);
        y += Y_OFFSET;

        ssd1306_SetCursor(2, y);
        ssd1306_WriteString(&page_data[2][0], Font_data, White);
        y += Y_OFFSET;

#if defined(SSD1306_INCLUDE_FONT_7x10)
        ssd1306_SetCursor(2, y);
        ssd1306_WriteString(&page_data[3][0], Font_data, White);
        y += Y_OFFSET;

        ssd1306_SetCursor(2, y);
        ssd1306_WriteString(&page_data[4][0], Font_data, White);
        y += Y_OFFSET;

        ssd1306_SetCursor(2, y);
        ssd1306_WriteString(&page_data[5][0], Font_data, White);
#endif
        ssd1306_UpdateScreen();
}
#else
void ssd1306_show(void) {return;}
#endif /* SSD1306_OLED */



