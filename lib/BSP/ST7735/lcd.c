#include "lcd.h"
#include "font.h"

void lcd_rs_set(void);
void lcd_rs_reset(void);
void lcd_cs_set(void);
void lcd_cs_reset(void);
void lcd_bl_on(void);
void lcd_bl_off(void);
uint32_t lcd_get_time(void);
uint32_t lcd_get_date(void);

int32_t lcd_get_tick(void);
int32_t lcd_spi_transmit(uint8_t* pdata, uint32_t length);
int32_t lcd_spi_receive(uint8_t* pdata, uint32_t length);
uint32_t get_dev_idc(void);

// LCD_RS//dc
#define LCD_RS_SET lcd_rs_set()
#define LCD_RS_RESET lcd_rs_reset()
// LCD_CS
#define LCD_CS_SET lcd_cs_set()
#define LCD_CS_RESET lcd_cs_reset()
// LCD_BL
#define LCD_BL_SET lcd_bl_on()
#define LCD_BL_RESET lcd_bl_off()

static int32_t lcd_init(void);
static int32_t lcd_writereg(uint8_t reg, uint8_t* pdata, uint32_t length);
static int32_t lcd_readreg(uint8_t reg, uint8_t* pdata);
static int32_t lcd_senddata(uint8_t* pdata, uint32_t length);
static int32_t lcd_recvdata(uint8_t* pdata, uint32_t length);

ST7735_IO_t st7735_pIO = {lcd_init,
                          NULL,
                          0,
                          lcd_writereg,
                          lcd_readreg,
                          lcd_senddata,
                          lcd_recvdata,
                          lcd_get_tick};

ST7735_Object_t st7735_pObj;
uint32_t st7735_id;

uint16_t POINT_COLOR = WHITE;
uint16_t BACK_COLOR = BLACK;

extern const char* app_info;
extern const char* build_date;
extern const char* version;

void LCD_Test(void) {
    uint8_t text[35];

    ST7735Ctx.Orientation = ST7735_ORIENTATION_LANDSCAPE_ROT180;
    ST7735Ctx.Panel = HannStar_Panel;
    ST7735Ctx.Type = ST7735_0_9_inch_screen;

    ST7735_RegisterBusIO(&st7735_pObj, &st7735_pIO);
    ST7735_LCD_Driver.Init(&st7735_pObj, ST7735_FORMAT_RBG565, &ST7735Ctx);
    ST7735_LCD_Driver.ReadID(&st7735_pObj, &st7735_id);

    ST7735_LCD_Driver.FillRect(
            &st7735_pObj, 0, 0, ST7735Ctx.Width, ST7735Ctx.Height, BACK_COLOR);

    uint32_t time = lcd_get_time();
    uint32_t date = lcd_get_date();

    snprintf((char*)&text, sizeof(text), "%s %s", app_info, version);
    LCD_ShowString(0, 4, (uint16_t)ST7735Ctx.Width, 12, 12, text);
    snprintf((char*)&text, sizeof(text), "%s", build_date);
    LCD_ShowString(0, 16, (uint16_t)ST7735Ctx.Width, 12, 12, text);
    snprintf((char*)&text, sizeof(text), "CPU IDC: 0x%lX", get_dev_idc());
    LCD_ShowString(0, 28, (uint16_t)ST7735Ctx.Width, 12, 12, text);
    snprintf((char*)&text, sizeof(text), "LCD ID: 0x%lX", st7735_id);
    LCD_ShowString(0, 40, (uint16_t)ST7735Ctx.Width, 12, 12, text);
    snprintf((char*)&text,
             sizeof(text),
             "Start: %02lX-%02lX-20%02lX %02lX:%02lX:%02lX",
             (date >> 16) & 0xFF,
             (date >> 8) & 0xFF,
             date & 0xFF,
             (time >> 16) & 0xFF,
             (time >> 8) & 0xFF,
             time & 0xFF);
    LCD_ShowString(0, 52, (uint16_t)ST7735Ctx.Width, 12, 12, text);

    LCD_BL_SET;
}

void LCD_ShowSysTime(const char* sys_time) {
    uint8_t text[35];

    snprintf((char*)&text, sizeof(text), "Time:  %s", sys_time);
    LCD_ShowString(0, 64, (uint16_t)ST7735Ctx.Width, 12, 12, text);
}

void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode) {
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint16_t x0 = x;
    uint16_t colortemp = POINT_COLOR;
    uint32_t h, w;

    uint16_t write[size][size == 12 ? 6 : 8];
    uint16_t count;

    ST7735_GetXSize(&st7735_pObj, &w);
    ST7735_GetYSize(&st7735_pObj, &h);

    num = num - ' ';
    count = 0;

    if (!mode) {
        for (t = 0; t < size; t++) {
            if (size == 12)
                temp = asc2_1206[num][t];
            else
                temp = asc2_1608[num][t];

            for (t1 = 0; t1 < 8; t1++) {
                if (temp & 0x80)
                    POINT_COLOR = (colortemp & 0xFF) << 8 | colortemp >> 8;
                else
                    POINT_COLOR = (BACK_COLOR & 0xFF) << 8 | BACK_COLOR >> 8;

                write[count][t / 2] = POINT_COLOR;
                count++;
                if (count >= size)
                    count = 0;

                temp <<= 1;
                y++;
                if (y >= h) {
                    POINT_COLOR = colortemp;
                    return;
                }
                if ((y - y0) == size) {
                    y = y0;
                    x++;
                    if (x >= w) {
                        POINT_COLOR = colortemp;
                        return;
                    }
                    break;
                }
            }
        }
    } else {
        for (t = 0; t < size; t++) {
            if (size == 12)
                temp = asc2_1206[num][t];
            else
                temp = asc2_1608[num][t];
            for (t1 = 0; t1 < 8; t1++) {
                if (temp & 0x80)
                    write[count][t / 2] = (POINT_COLOR & 0xFF) << 8 | POINT_COLOR >> 8;
                count++;
                if (count >= size)
                    count = 0;

                temp <<= 1;
                y++;
                if (y >= h) {
                    POINT_COLOR = colortemp;
                    return;
                }
                {
                    y = y0;
                    x++;
                    if (x >= w) {
                        POINT_COLOR = colortemp;
                        return;
                    }
                    break;
                }
            }
        }
    }
    ST7735_FillRGBRect(&st7735_pObj, x0, y0, (uint8_t*)&write, size == 12 ? 6 : 8, size);
    POINT_COLOR = colortemp;
}

void LCD_ShowString(uint16_t x,
                    uint16_t y,
                    uint16_t width,
                    uint16_t height,
                    uint8_t size,
                    uint8_t* p) {
    uint8_t x0 = (uint8_t)x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' ')) {
        if (x >= width) {
            x = x0;
            y += size;
        }
        if (y >= height)
            break;
        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}

void LCD_ShowStringFixFont(uint16_t x, uint16_t y, uint8_t* p) {
    LCD_ShowString(x, y, (uint16_t)ST7735Ctx.Width, 12, 12, p);
}

static int32_t lcd_init(void) {
    int32_t result = ST7735_OK;
    return result;
}

static int32_t lcd_writereg(uint8_t reg, uint8_t* pdata, uint32_t length) {
    int32_t result;

    LCD_CS_RESET;
    LCD_RS_RESET;
    result = lcd_spi_transmit(&reg, 1);
    LCD_RS_SET;
    if (length > 0)
        result += lcd_spi_transmit(pdata, length);
    LCD_CS_SET;
    if (result > 0) {
        result = -1;
    } else {
        result = 0;
    }
    return result;
}

static int32_t lcd_readreg(uint8_t reg, uint8_t* pdata) {
    int32_t result;
    LCD_CS_RESET;
    LCD_RS_RESET;

    result = lcd_spi_transmit(&reg, 1);
    LCD_RS_SET;
    result += lcd_spi_receive(pdata, 1);
    LCD_CS_SET;
    if (result > 0) {
        result = -1;
    } else {
        result = 0;
    }
    return result;
}

static int32_t lcd_senddata(uint8_t* pdata, uint32_t length) {
    (void)pdata;
    (void)length;
    int32_t result;
    LCD_CS_RESET;
    // LCD_RS_SET;
    result = lcd_spi_transmit(pdata, length);
    LCD_CS_SET;
    if (result > 0) {
        result = -1;
    } else {
        result = 0;
    }
    return result;
}

static int32_t lcd_recvdata(uint8_t* pdata, uint32_t length) {
    (void)pdata;
    (void)length;
    int32_t result;
    LCD_CS_RESET;
    // LCD_RS_SET;
    result = lcd_spi_receive(pdata, length);
    LCD_CS_SET;
    if (result > 0) {
        result = -1;
    } else {
        result = 0;
    }
    return result;
}
