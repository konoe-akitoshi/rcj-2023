#ifndef MODULE_CORE_DEPS_TFT_E_SPI_HPP
#define MODULE_CORE_DEPS_TFT_E_SPI_HPP

#define TFT_ESPI_VERSION "1.6.1"

#include "arduino.hpp"

#ifndef TAB_COLOUR
#define TAB_COLOUR 0
#endif

#define SPI_32(H, L) ((H) << 16 | (L))
#define COL_32(H, L) ((H) << 16 | (L))

#define LCD_BACKLIGHT (72Ul)

#define TL_DATUM 0     // Top left (default)
#define TC_DATUM 1     // Top centre
#define TR_DATUM 2     // Top right
#define ML_DATUM 3     // Middle left
#define CL_DATUM 3     // Centre left, same as above
#define MC_DATUM 4     // Middle centre
#define CC_DATUM 4     // Centre centre, same as above
#define MR_DATUM 5     // Middle right
#define CR_DATUM 5     // Centre right, same as above
#define BL_DATUM 6     // Bottom left
#define BC_DATUM 7     // Bottom centre
#define BR_DATUM 8     // Bottom right
#define L_BASELINE 9   // Left character baseline (Line the 'A' character would sit on)
#define C_BASELINE 10  // Centre character baseline
#define R_BASELINE 11  // Right character baseline

#define TFT_BLACK 0x0000       /*   0,   0,   0 */
#define TFT_NAVY 0x000F        /*   0,   0, 128 */
#define TFT_DARKGREEN 0x03E0   /*   0, 128,   0 */
#define TFT_DARKCYAN 0x03EF    /*   0, 128, 128 */
#define TFT_MAROON 0x7800      /* 128,   0,   0 */
#define TFT_PURPLE 0x780F      /* 128,   0, 128 */
#define TFT_OLIVE 0x7BE0       /* 128, 128,   0 */
#define TFT_LIGHTGREY 0xC618   /* 192, 192, 192 */
#define TFT_DARKGREY 0x7BEF    /* 128, 128, 128 */
#define TFT_BLUE 0x001F        /*   0,   0, 255 */
#define TFT_GREEN 0x07E0       /*   0, 255,   0 */
#define TFT_CYAN 0x07FF        /*   0, 255, 255 */
#define TFT_RED 0xF800         /* 255,   0,   0 */
#define TFT_MAGENTA 0xF81F     /* 255,   0, 255 */
#define TFT_YELLOW 0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE 0xFFFF       /* 255, 255, 255 */
#define TFT_ORANGE 0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0 /* 180, 255,   0 */
#define TFT_PINK 0xFC9F

#define TFT_TRANSPARENT 0x0120

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

const uint8_t widtbl_null[1] = {0};

typedef struct
{
    const uint8_t *chartbl;
    const uint8_t *widthtbl;
    uint8_t height;
    uint8_t baseline;
} fontinfo;

class TFT_eSPI
{
  public:
    TFT_eSPI();
    void init(uint8_t tc = TAB_COLOUR),
        begin(uint8_t tc = TAB_COLOUR);  // Same - begin included for backwards compatibility
    void setBacklight(uint32_t backlight);
    uint32_t backlight();
    virtual void drawPixel(int32_t x, int32_t y, uint32_t color),
        drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size),
        drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color),
        drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color),
        drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color),
        fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    virtual int16_t drawChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font),
        drawChar(uint16_t uniCode, int32_t x, int32_t y),
        height(void),
        width(void);
    void setWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye),
        pushColor(uint16_t color),
        pushColor(uint16_t color, uint32_t len),
        pushColors(uint16_t *data, uint32_t len, bool swap = true),  // With byte swap option
        pushColors(uint8_t *data, uint32_t len),
        fillScreen(uint32_t color);
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color),
        drawRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color),
        fillRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color),
        setRotation(uint8_t r),
        invertDisplay(boolean i),
        drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color),
        drawCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, uint32_t color),
        fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color),
        fillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint32_t color),
        drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color),
        fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color),
        drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color),
        fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color),
        drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color),
        drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color),
        drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor, uint16_t bgcolor),
        setBitmapColor(uint16_t fgcolor, uint16_t bgcolor),  // For 1bpp sprites
        setPivot(int16_t x, int16_t y),
        setCursor(int16_t x, int16_t y),
        setCursor(int16_t x, int16_t y, uint8_t font),
        setTextColor(uint16_t color),
        setTextColor(uint16_t fgcolor, uint16_t bgcolor),
        setTextSize(uint8_t size),
        setTextWrap(boolean wrapX, boolean wrapY = false),
        setTextDatum(uint8_t datum),
        setTextPadding(uint16_t x_width),
        setFreeFont(uint8_t font),
        setTextFont(uint8_t font),
        spiwrite(uint8_t),
        writecommand(uint8_t c),
        writedata(uint8_t d),
        commandList(const uint8_t *addr);

    uint8_t readcommand8(uint8_t cmd_function, uint8_t index = 0);
    uint16_t readcommand16(uint8_t cmd_function, uint8_t index = 0);
    uint32_t readcommand32(uint8_t cmd_function, uint8_t index = 0);
    uint16_t readPixel(int32_t x0, int32_t y0);
    void readRect(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data);
    void pushRect(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data);
    void pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data);
    void pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data, uint16_t transparent);
    void pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, const uint16_t *data, uint16_t transparent);
    void pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, const uint16_t *data);
    void pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t *data, bool bpp8 = true);
    void pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t *data, uint8_t transparent, bool bpp8 = true);
    void setSwapBytes(bool swap);
    bool getSwapBytes(void);
    void readRectRGB(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t *data);
    uint8_t getRotation(void),
        getTextDatum(void),
        color16to8(uint16_t color565);  // Convert 16 bit colour to 8 bits
    int16_t getCursorX(void),
        getCursorY(void);
    int16_t getPivotX(void),
        getPivotY(void);
    uint16_t fontsLoaded(void),
        color565(uint8_t red, uint8_t green, uint8_t blue),  // Convert 8 bit red, green and blue to 16 bits
        color8to16(uint8_t color332);                        // Convert 8 bit colour to 16 bits
    int16_t drawNumber(long long_num, int32_t poX, int32_t poY, uint8_t font),
        drawNumber(long long_num, int32_t poX, int32_t poY),
        drawFloat(float floatNumber, uint8_t decimal, int32_t poX, int32_t poY, uint8_t font),
        drawFloat(float floatNumber, uint8_t decimal, int32_t poX, int32_t poY),
        drawString(const char *string, int32_t poX, int32_t poY, uint8_t font),
        drawString(const char *string, int32_t poX, int32_t poY),
        drawCentreString(const char *string, int32_t dX, int32_t poY, uint8_t font),  // Deprecated, use setTextDatum() and drawString()
        drawRightString(const char *string, int32_t dX, int32_t poY, uint8_t font),   // Deprecated, use setTextDatum() and drawString()
        drawString(const String &string, int32_t poX, int32_t poY, uint8_t font),
        drawString(const String &string, int32_t poX, int32_t poY),
        drawCentreString(const String &string, int32_t dX, int32_t poY, uint8_t font),  // Deprecated, use setTextDatum() and drawString()
        drawRightString(const String &string, int32_t dX, int32_t poY, uint8_t font);   // Deprecated, use setTextDatum() and drawString()

    int16_t textWidth(const char *string, uint8_t font),
        textWidth(const char *string),
        textWidth(const String &string, uint8_t font),
        textWidth(const String &string),
        fontHeight(int16_t font),
        fontHeight(void);
    void setAddrWindow(int32_t xs, int32_t ys, int32_t w, int32_t h);
    void startWrite(void);                          // Begin SPI transaction
    void writeColor(uint16_t color, uint32_t len);  // Write colours without transaction overhead
    void endWrite(void);                            // End SPI transaction
    uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining);
    uint16_t decodeUTF8(uint8_t c);
    size_t write(uint8_t);
    void setAttribute(uint8_t id = 0, uint8_t a = 0);
    uint8_t getAttribute(uint8_t id = 0);
    int32_t cursor_x, cursor_y, padX;
    uint32_t textcolor, textbgcolor;
    uint32_t bitmap_fg, bitmap_bg;
    uint8_t textfont,          // Current selected font
        textsize,              // Current font size multiplier
        textdatum,             // Text reference datum
        rotation;              // Display rotation (0-3)
    int16_t _xpivot;           // x pivot point coordinate
    int16_t _ypivot;           // x pivot point coordinate
    uint8_t decoderState = 0;  // UTF8 decoder state
    uint16_t decoderBuffer;    // Unicode code-point buffer

  private:
    inline void com_begin() __attribute__((always_inline));
    inline void com_end() __attribute__((always_inline));
    inline void com_begin_read() __attribute__((always_inline));
    inline void com_end_read() __attribute__((always_inline));
    void readAddrWindow(int32_t xs, int32_t ys, int32_t w, int32_t h);
    uint8_t tabcolor, colstart = 0, rowstart = 0;  // some ST7735 displays need this changed
    volatile uint32_t *dcport, *csport;
    uint32_t cspinmask, dcpinmask, wrpinmask, sclkpinmask;
    uint32_t lastColor = 0xFFFF;

  protected:
    int32_t win_xe, win_ye;
    int32_t _init_width, _init_height;  // Display w/h as input, used by setRotation()
    int32_t _width, _height;            // Display w/h as modified by current rotation
    int32_t addr_row, addr_col;
    uint32_t _backlight;
    uint32_t fontsloaded;
    uint8_t glyph_ab,            // glyph delta Y (height) above baseline
        glyph_bb;                // glyph delta Y (height) below baseline
    bool isDigits;               // adjust bounding box for numbers to reduce visual jiggling
    bool textwrapX, textwrapY;   // If set, 'wrap' text at right and optionally bottom edge of display
    bool _swapBytes;             // Swap the byte order for TFT pushImage()
    bool locked, inTransaction;  // Transaction and mutex lock flags for ESP32
    bool _booted;                // init() or begin() has already run once
    bool _cp437;                 // If set, use correct CP437 charset (default is ON)
    bool _utf8;                  // If set, use UTF-8 decoder in print stream 'write()' function (default ON)
    uint32_t _lastColor;         // Buffered value of last colour used
};                               // End of class TFT_eSPI

#endif
