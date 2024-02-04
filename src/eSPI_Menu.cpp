#include <eSPI_Menu.h>

using namespace eSPIMenu;

Spec::Spec(TFT_eSPI *display) : display(display) {
}

void Spec::reset() {
    textViewport.x = textViewport.y = textViewport.width = textViewport.height = -1;
    border.top = border.left = border.bottom = border.right = 0;
    margins.top = margins.left = margins.bottom = margins.right = 0;

    backgroundColors[none] = TFT_BLACK;
    backgroundColors[selected] = TFT_WHITE;
    backgroundColors[disabled] = TFT_BLACK;
    
    foregroundColors[none] = TFT_WHITE;
    foregroundColors[selected] = TFT_BLACK;
    foregroundColors[disabled] = TFT_DARKGREY;
    
    borderColors[none] = TFT_BLACK;
    borderColors[selected] = TFT_WHITE;
    borderColors[disabled] = TFT_BLACK;
    
    font = 2;
}

void Spec::init(TFT_eSPI *display) {
    Serial.println("Initializing spec");

    display->setTextFont(font);
    int textWidthInset = margins.left + margins.right + border.left + border.right;
    int textHeightInset = margins.top + margins.bottom + border.top + border.bottom;

    int16_t maxHeight = display->fontHeight();
    if (textViewport.height == -1) {
        textViewport.height = maxHeight;
    }
    if (textViewport.width == -1) {
        textViewport.width = display->width() - textWidthInset;
    }
    if (textViewport.x == -1) {
        textViewport.x = margins.left + border.left;
    }
    if (textViewport.y == -1) {
        textViewport.y = margins.top + border.top;
    }
    maxHeight = margins.top + margins.bottom + border.top + border.bottom + textViewport.height;

    height = maxHeight;
    width = display->width();
}

void Spec::clear(int x, int y, uint16_t color) {
    display->fillRect(x, y, width, height, color);
}

void Spec::setFont(uint8_t font) {
    this->font = font;
}

void Spec::setMargins(int top, int left, int bottom, int right) {
    margins.top = top;
    margins.left = left;
    margins.bottom = bottom;
    margins.right = right;
}

void Spec::setBorder(int top, int left, int bottom, int right) {
    border.top = top;
    border.left = left;
    border.bottom = bottom;
    border.right = right;
}

void Spec::setTextViewport(int x, int y, int width, int height) {
    textViewport.x = x;
    textViewport.y = y;
    textViewport.width = width;
    textViewport.height = height;
}

void Spec::setItemColors(uint16_t background, uint16_t foreground, uint16_t selectedBackground, uint16_t selectedForeground, uint16_t disabledBackground, uint16_t disabledForeground) {
    backgroundColors[none] = background;
    backgroundColors[selected] = selectedBackground;
    backgroundColors[disabled] = disabledBackground;
    foregroundColors[none] = foreground;
    foregroundColors[selected] = selectedForeground;
    foregroundColors[disabled] = disabledForeground;
}

void Spec::setBorderColors(uint16_t borderColor, uint16_t selectedBorderColor, uint16_t disabledBorderColor) {
    borderColors[none] = borderColor;
    borderColors[selected] = selectedBorderColor;
    borderColors[disabled] = disabledBorderColor;
}

int Spec::getHeight() {
    return height;
}

void Spec::drawUpArrow(int32_t x, int32_t y, State state) {
    // Draw in rectangle
    int wh = textViewport.height * 3 / 4;
    int dh = (textViewport.height - wh) / 2;
    int bottomY = y + textViewport.height - dh - margins.bottom - border.bottom;
    int topY = y + dh + margins.top + border.top;
    int rightX = x + textViewport.width - margins.right - border.right;
    int leftX = rightX - wh;
    int centerX = x + (leftX + rightX) / 2;

    display->fillRect(leftX - 2, topY, wh + 2, wh, backgroundColors[state]);
    display->fillTriangle(leftX, bottomY,   // bottom left
                rightX, bottomY,            // bottom right
                centerX, topY,              // center
                foregroundColors[state]);
}

void Spec::drawDownArrow(int32_t x, int32_t y, State state) {
    int wh = textViewport.height * 3 / 4;
    int dh = (textViewport.height - wh) / 2;
    int bottomY = y + textViewport.height - dh - margins.bottom - border.bottom;
    int topY = y + dh + margins.top + border.top;
    int rightX = x + textViewport.width - margins.right - border.right;
    int leftX = rightX - wh;
    int centerX = x + (leftX + rightX) / 2;

    display->fillRect(leftX - 2, topY, wh + 2, wh, backgroundColors[state]);
    display->fillTriangle(leftX, topY,      // top left
                rightX, topY,               // top right
                centerX, bottomY,           // center
                foregroundColors[state]);
}

void Spec::drawAt(int32_t x, int32_t y, const char* text, State state) {
    Serial.printf("Drawing %s at %d,%d with state %d\n", text, x, y, state);
    // Fill with border color if we have a border
    if (border.left > 0 || border.right > 0 || border.top > 0 || border.bottom > 0) {
        display->fillRect(x, y, width, height, borderColors[state]);
    }

    // Fill with background color
    display->fillRect(x + border.left, y + border.top, width - border.left - border.right, height - border.top - border.bottom, backgroundColors[state]);

    // Draw the text
    display->setTextDatum(TL_DATUM);
    display->setTextColor(foregroundColors[state], backgroundColors[state], true);
    display->setTextFont(font);
    display->setViewport(x + textViewport.x, y + textViewport.y, textViewport.width, textViewport.height, false);
    display->drawString(text, x + textViewport.x, y + textViewport.y);
    display->setViewport(0, 0, display->width(), display->height());
}

Menu::Menu(TFT_eSPI *display) : titleSpec(display), itemSpec(display), display(display) {        
}

void Menu::setTitle(const char *title) {
    strncpy(this->title, title, ESPI_MENU_MAX_TITLE_CHARS);
}

int Menu::addItem(const char* label, State state) {
    if (itemCount < ESPI_MENU_MAX_ITEMS) {
        strncpy(items[itemCount], label, ESPI_MENU_MAX_ITEM_CHARS);
        itemState[itemCount] = state;

        if (state == selected) {
            Serial.printf("Selecting %s\n", label);
            selectedIndex = itemCount;
        }

        return itemCount++;
    }

    return -1;
}

void Menu::reset() {
    itemCount = 0;
    selectedIndex = -1;
    title[0] = 0;
    specsInited = false;
    titleSpec.reset();
    itemSpec.reset();
}

void Menu::show() {
    if (!specsInited) {
        specsInited = true;

        titleSpec.init(display);
        titleHeight = titleSpec.getHeight();

        itemSpec.init(display);
        itemHeight = itemSpec.getHeight();

        maxDisplayRows = (display->height() - titleHeight)/itemHeight;
        startDisplayRow = 0;
    }

    if (selectedIndex >= maxDisplayRows) {
        startDisplayRow = selectedIndex - maxDisplayRows + 1;
    }
    titleSpec.drawAt(0, 0, title, none);
    drawAllVisibleItems();
}

void Menu::clear(uint16_t color) {
    titleSpec.clear(0, 0, color);
    for (int i=0; i<maxDisplayRows; i++) {
        itemSpec.clear(0, titleHeight + itemHeight * i, color);
    }
}

void Menu::drawAllVisibleItems() {
    for (int i=0; i<maxDisplayRows; i++) {
        int rowIndex = i + startDisplayRow;
        if (selectedIndex < 0 && itemState[rowIndex] != disabled) {
            Serial.printf("Setting selected index %d\n", rowIndex);
            selectedIndex = rowIndex;
            itemState[rowIndex] = selected;
        }
        drawItem(rowIndex);
    }
}

void Menu::drawItem(int index) {
    if (index >= startDisplayRow && index < itemCount && index < startDisplayRow + maxDisplayRows) {
        Serial.printf("Drawing item %d\n", index);
        int y = titleHeight + itemHeight * (index - startDisplayRow);
        itemSpec.drawAt(0, y, items[index], itemState[index]);
        if (index == startDisplayRow && index != 0) {
            itemSpec.drawUpArrow(0, y, itemState[index]);
        }
        if (index == startDisplayRow + maxDisplayRows - 1 && index != itemCount - 1) {
            itemSpec.drawDownArrow(0, y, itemState[index]);
        }
    }
}

void Menu::up() {
    if (selectedIndex > 0) {
        int newIndex = selectedIndex - 1;
        while (itemState[newIndex] == disabled) {
            newIndex--;

            if (newIndex < 0) {
                break;
            }
        }

        if (newIndex >= 0) {
            if (newIndex < startDisplayRow) {
                startDisplayRow -= 1;
            }
            itemState[selectedIndex] = none;
            selectedIndex = newIndex;
            itemState[selectedIndex] = selected;
            drawAllVisibleItems();
        }
    }
}

void Menu::down() {
    if (selectedIndex < itemCount-1) {
        int newIndex = selectedIndex + 1;
        while (itemState[newIndex] == disabled) {
            newIndex++;

            if (newIndex >= itemCount) {
                break;
            }
        }

        if (newIndex < itemCount) {
            if (newIndex >= startDisplayRow + maxDisplayRows) {
                startDisplayRow += 1;
            }
            itemState[selectedIndex] = none;
            selectedIndex = newIndex;
            itemState[selectedIndex] = selected;
            drawAllVisibleItems();
        }
    }
}

const char* Menu::getSelectedText() {
    if (selectedIndex >= 0) {
        return items[selectedIndex];
    } else {
        return 0;
    }
}

int Menu::getSelectedItem() {
    return selectedIndex;
}
