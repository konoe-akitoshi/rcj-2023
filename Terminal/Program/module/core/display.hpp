#ifndef MODULE_CORE_DISPLAY_HPP
#define MODULE_CORE_DISPLAY_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/TFT_eSPI.hpp"
#else
#include "TFT_eSPI.h"
#endif

struct ScreenPosition {
    int32_t x;
    int32_t y;
    constexpr ScreenPosition() : x(0), y(0) {}
    constexpr ScreenPosition(int32_t _x, int32_t _y) : x(_x), y(_y) {}
};

namespace module
{
class Display
{
  public:
    static constexpr int32_t WIDTH = 320;
    static constexpr int32_t HEIGHT = 240;
    static constexpr uint16_t BACKGROUND_COLOR = TFT_BLACK;
    static constexpr uint16_t TEXT_COLOR_FG = TFT_WHITE;
    static constexpr uint16_t TEXT_COLOR_BG = BACKGROUND_COLOR;

    explicit constexpr Display(TFT_eSPI& tft) : tft_(tft) {}

    void setup(void) const {
        tft_.begin();
        tft_.setRotation(3);
        reset();
    }

    void reset(void) const {
        tft_.fillScreen(BACKGROUND_COLOR);
        tft_.setTextSize(TEXT_HEIGHT_ / 10);
        tft_.setTextColor(TEXT_COLOR_FG, TEXT_COLOR_BG);
    }

    void drawPixel(ScreenPosition pos, uint16_t color) const {
        const auto p = fixScreenPosition(pos);
        tft_.drawPixel(p.x, p.y, color);
    }

    void drawFlame(const uint16_t color) const {
        tft_.fillRect(0, 0, WIDTH, PADDING_, color);
        tft_.fillRect(0, 0, PADDING_, HEIGHT, color);
        tft_.fillRect(0, HEIGHT - PADDING_, WIDTH, PADDING_, color);
        tft_.fillRect(WIDTH - PADDING_, 0, PADDING_, HEIGHT, color);
    }

    void drawStringLine(const char* string, uint8_t line_number, bool fill_background = false) const {
        drawStringLine(String(string), line_number, fill_background);
    }

    void drawStringLine(const String string, uint8_t line_number, bool fill_background = false) const {
        if (fill_background) {
            tft_.setTextColor(TEXT_COLOR_BG, TEXT_COLOR_FG);
        }
        tft_.drawString(string, PADDING_, TEXT_HEIGHT_ * line_number + PADDING_);
        if (fill_background) {
            tft_.setTextColor(TEXT_COLOR_FG, TEXT_COLOR_BG);
        }
    }

    void drawLine(const ScreenPosition pos0, const ScreenPosition pos1, const uint16_t color = TEXT_COLOR_FG) const {
        const auto p0 = fixScreenPosition(pos0);
        const auto p1 = fixScreenPosition(pos1);
        tft_.drawLine(p0.x, p0.y, p1.x, p1.y, color);
    }

    void drawCircle(const ScreenPosition pos, const int32_t r, const uint32_t color = TEXT_COLOR_FG) const {
        const auto p = fixScreenPosition(pos);
        tft_.drawCircle(p.x, p.y, r, color);
    }

    void fillCircle(const ScreenPosition pos, const int32_t r, const uint32_t color = TEXT_COLOR_FG) const {
        const auto p = fixScreenPosition(pos);
        tft_.fillCircle(p.x, p.y, r, color);
    }

    void eraseCircle(const ScreenPosition pos, const int32_t r) const {
        const auto p = fixScreenPosition(pos);
        tft_.fillCircle(p.x, p.y, r, BACKGROUND_COLOR);
    }

  private:
    TFT_eSPI& tft_;
    static constexpr int32_t PADDING_ = 10;
    static constexpr uint8_t TEXT_HEIGHT_ = 20;

    constexpr inline ScreenPosition fixScreenPosition(const ScreenPosition p) const {
        return {p.x + (WIDTH / 2), (HEIGHT / 2) - p.y};
    }
};
}  // namespace module

#endif
