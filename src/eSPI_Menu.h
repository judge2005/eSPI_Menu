#ifndef _ESPI_MENU_H
#define _ESPI_MENU_H

#include <TFT_eSPI.h>

#ifndef ESPI_MENU_MAX_TITLE_CHARS
#define ESPI_MENU_MAX_TITLE_CHARS 15
#endif

#ifndef ESPI_MENU_MAX_ITEM_CHARS
#define ESPI_MENU_MAX_ITEM_CHARS 15
#endif

#ifndef ESPI_MENU_MAX_ITEMS
#define ESPI_MENU_MAX_ITEMS 15
#endif

namespace eSPIMenu {
enum State { none=0, selected=1, disabled=2 };

struct Margins {
    int top, left, bottom, right;
};

struct Rectangle {
    int x, y, width, height;
};

struct Point {
    int x, y;
};

struct Size {
    int width, height;
};

class Spec {
public:
    Spec(TFT_eSPI *display);

    void setFont(uint8_t font);
    void setMargins(int top, int left, int bottom, int right);
    void setBorder(int top, int left, int bottom, int right);
    void setTextViewport(int top, int left, int bottom, int right);
    void setItemColors(uint16_t background, uint16_t foreground, uint16_t selectedBackground, uint16_t selectedForeground, uint16_t disabledBackground, uint16_t disabledForeground);
    void setBorderColors(uint16_t borderColor, uint16_t selectedBorderColor, uint16_t disabledBorderColor);

protected:
    void drawAt(int32_t x, int32_t y, const char* text, State state);
    void init(TFT_eSPI *display);
    void reset();
    void clear(int x, int y, uint16_t color);
    void drawDownArrow(int32_t x, int32_t y, State state);
    void drawUpArrow(int32_t x, int32_t y, State state);

    int getHeight();

    TFT_eSPI *display = 0;

    uint8_t font = 2;
    uint16_t backgroundColors[3] = { TFT_BLACK, TFT_WHITE, TFT_BLACK };
    uint16_t foregroundColors[3] = { TFT_WHITE, TFT_BLACK, TFT_DARKGREY };
    uint16_t borderColors[3] = { TFT_BLACK, TFT_WHITE, TFT_BLACK };
    uint16_t height = 0;
    uint16_t width = 0;
    
    Margins margins = {0, 0, 0, 0};
    Margins border = {0, 0, 0, 0};
    Rectangle textViewport = {-1, -1, -1, -1};

    friend class Menu;
};

class Menu {
public:
    Menu(TFT_eSPI *display);

    void reset();
    int addItem(const char*, State state = none);
    void setTitle(const char *);

    void show();
    void clear(uint16_t color);
    void up();
    void down();
    const char* getSelectedText();
    int getSelectedItem();

    Spec& getTitleSpec() { return titleSpec; }
    Spec& getItemSpec() { return itemSpec; }
protected:
    void drawItem(int index);
    void drawAllVisibleItems();

    bool specsInited = false;

    TFT_eSPI *display = 0;

    char title[ESPI_MENU_MAX_TITLE_CHARS+1] = "";
    char items[ESPI_MENU_MAX_ITEMS][ESPI_MENU_MAX_ITEM_CHARS+1];
    State itemState[ESPI_MENU_MAX_ITEMS];

    int selectedIndex = -1;
    int itemCount = 0;
    int itemHeight = 0;
    int titleHeight = 0;
    uint8_t maxDisplayRows = 0;
    uint8_t startDisplayRow = 0;

    Spec titleSpec;
    Spec itemSpec;
};
};
#endif