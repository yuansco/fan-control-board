#include <stdint.h>

#ifndef __SSD1306_FONTS_H__
#define __SSD1306_FONTS_H__

#include "board.h"

typedef struct {
        const uint8_t FontWidth;    /*!< Font width in pixels */
        uint8_t FontHeight;   /*!< Font height in pixels */
        const uint16_t *data; /*!< Pointer to data font data array */
} FontDef;

extern FontDef Font_data;

#endif // __SSD1306_FONTS_H__
