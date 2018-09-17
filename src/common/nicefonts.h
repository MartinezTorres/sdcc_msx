#include <msx1hal.h>

#include <tmp/fonts.h>

typedef uint8_t TFont[128][8];

void initRenderedText(uint8_t *slim, uint8_t *bold, uint8_t freeTiles[256], const uint8_t *str, const TFont font, uint8_t padding);

//void renderText(uint8_t *slim, int8_t x, int8_t y, uint16_t addr, uint16_t drawBorder);

//void freeRenderedText(TRenderedText *rm, uint8_t freeTiles[256]);

