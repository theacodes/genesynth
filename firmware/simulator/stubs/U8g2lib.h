#ifndef _U8G2LIB_H
#define _U8G2LIB_H

static void* u8g2_font_amstrad_cpc_extended_8f = nullptr;
static void* u8g2_font_fub14_tf = nullptr;

enum {
    U8G2_R2
};

class U8G2 {
public:
    U8G2(int rotation, int cs, int dc, int reset) {};
    void setPowerSave(int) {};
    void setFontMode(int) {};
    void setFont(void*) {};
    void setDrawColor(int) {};
    void drawBox(int, int, int, int) {};
    void drawFrame(int, int, int, int) {};
    void drawLine(int, int, int, int) {};
    void drawPixel(int, int) {};
    void setCursor(int, int) {};
    void setFontPosTop() {};
    void drawGlyph(char, int, int) {};
    int printf(const char*, ...) { return 0; };
    void begin() {};
    void firstPage() {};
    bool nextPage() { return false; };
    void clearBuffer() {};
    void sendBuffer() {};
private:
};

typedef U8G2 U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI;

#endif
