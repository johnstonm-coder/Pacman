#include <RTClib.h>
RTC_DS3231 rtc;
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#define START_BUTTON 24
bool gameStarted = false;
bool mapDrawn = false;
int xPin = A0;
int yPin = A1;
int green = 23;
int red = 22;
int h;
const char* timeformat = "AM";

/*******************************************************************************
   Arduino_GFX setting
 ******************************************************************************/
#define GFX_BL DF_GFX_BL  // default backlight pin

#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else
Arduino_DataBus *bus = create_default_Arduino_DataBus();

#define DF_GFX_CS        9
#define DF_GFX_RST        7 // Or set to -1 and connect to Arduino RESET pin
#define DF_GFX_DC         8

Arduino_GFX *gfx = new Arduino_ST7796(bus, 7 /* RST */, -135 /* rotation */);
#endif

/*******************************************************************************
   SPRITE STRUCT
 ******************************************************************************/
struct Sprite {
  int x, y;
  int w, h;
  const uint16_t *bitmap;
};

/*******************************************************************************
   PACMAN SPRITES 10x12 OPEN HALF AND CLOSED
 ******************************************************************************/
const uint16_t pacmanopen[] PROGMEM = {
  0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000,
  0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000,
  0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000,
  0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
  0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000,
  0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000
};
const uint16_t pacmanclosed[] PROGMEM = {
  0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000,
  0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000,
  0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
  0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
  0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
  0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000,
  0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000
};

/*******************************************************************************
   CREATE SPRITE
  ******************************************************************************/
Sprite playero = { 100, 100, 10, 12, pacmanopen };
/******************************************************************************/

/*******************************************************************************
  DRAW SPRITE WITH TRANSPARENCY
  ******************************************************************************/
void drawSprite(Sprite s, uint16_t transparent) {
  for (int y = 0; y < s.h; y++) {
    for (int x = 0; x < s.w; x++) {
      uint16_t color = pgm_read_word(&(s.bitmap[y * s.w + x]));
      if (color != transparent) {
        gfx->drawPixel(s.x + x, s.y + y, color);
      }
    }
  }
}

/*******************************************************************************
  ANIMATION SETTINGS (movement synced)
 ******************************************************************************/
const uint16_t *pacmanFrames[] = { pacmanopen,  pacmanclosed};
int currentFrame = 0;

/*******************************************************************************

  /*******************************************************************************
   MAP
 *******************************************************************************/
void drawMap() {
  gfx->setTextColor(RGB565_YELLOW);
  gfx->setTextSize(4);
  gfx->setCursor(132, 10);
  gfx->print(" PAC-MAN");
  gfx->setTextColor(0x31B3);
  gfx->setTextSize(4);
  gfx->setCursor(132, 290);
  gfx->print(" GAMEBOY");

  gfx->drawLine(132, 48, 347, 48, RGB565_PURPLE);  // Left Top
  gfx->drawLine(130, 49, 131, 49, RGB565_PURPLE);
  gfx->drawLine(129, 50, 129, 51, RGB565_PURPLE);
  gfx->drawLine(128, 52, 128, 123, RGB565_PURPLE);
  gfx->drawLine(129, 124, 129, 125, RGB565_PURPLE);
  gfx->drawLine(130, 126, 130, 127, RGB565_PURPLE);
  gfx->drawLine(132, 127, 167, 127, RGB565_PURPLE);
  gfx->drawLine(168, 128, 168, 151, RGB565_PURPLE);
  gfx->drawLine(167, 152, 128, 152, RGB565_PURPLE);
  gfx->drawLine(348, 49, 349, 49, RGB565_PURPLE);
  gfx->drawLine(350, 50, 350, 51, RGB565_PURPLE);
  gfx->drawLine(351, 52, 351, 123, RGB565_PURPLE);
  gfx->drawLine(350, 124, 350, 125, RGB565_PURPLE);
  gfx->drawLine(349, 126, 348, 126, RGB565_PURPLE);
  gfx->drawLine(347, 127, 312, 127, RGB565_PURPLE);
  gfx->drawLine(311, 128, 311, 151, RGB565_PURPLE);
  gfx->drawLine(312, 152, 351, 152, RGB565_PURPLE);
  gfx->drawLine(132, 283, 347, 283, RGB565_PURPLE);
  gfx->drawLine(130, 282, 131, 282, RGB565_PURPLE);
  gfx->drawLine(129, 281, 129, 280, RGB565_PURPLE);
  gfx->drawLine(128, 279, 128, 208, RGB565_PURPLE);
  gfx->drawLine(129, 207, 129, 206, RGB565_PURPLE);
  gfx->drawLine(130, 205, 130, 204, RGB565_PURPLE);
  gfx->drawLine(132, 204, 167, 204, RGB565_PURPLE);
  gfx->drawLine(168, 203, 168, 180, RGB565_PURPLE);
  gfx->drawLine(167, 179, 128, 179, RGB565_PURPLE);
  gfx->drawLine(348, 282, 349, 282, RGB565_PURPLE);
  gfx->drawLine(350, 281, 350, 280, RGB565_PURPLE);
  gfx->drawLine(351, 279, 351, 208, RGB565_PURPLE);
  gfx->drawLine(350, 207, 350, 206, RGB565_PURPLE);
  gfx->drawLine(349, 205, 348, 205, RGB565_PURPLE);
  gfx->drawLine(347, 204, 312, 204, RGB565_PURPLE);
  gfx->drawLine(311, 203, 311, 180, RGB565_PURPLE);
  gfx->drawLine(312, 179, 351, 179, RGB565_PURPLE);
  gfx->drawLine(133, 51, 234, 51, RGB565_PURPLE);
  gfx->drawLine(245, 51, 346, 51, RGB565_PURPLE);
  gfx->drawPixel(235, 52, RGB565_PURPLE);
  gfx->drawPixel(244, 52, RGB565_PURPLE);
  gfx->drawLine(236, 52, 236, 81, RGB565_PURPLE);
  gfx->drawLine(243, 52, 243, 81, RGB565_PURPLE);
  gfx->drawPixel(237, 82, RGB565_PURPLE);
  gfx->drawPixel(242, 82, RGB565_PURPLE);
  gfx->drawLine(238, 83, 241, 83, RGB565_PURPLE);
  gfx->drawPixel(132, 52, RGB565_PURPLE);
  gfx->drawLine(131, 53, 131, 122, RGB565_PURPLE);
  gfx->drawPixel(132, 123, RGB565_PURPLE);
  gfx->drawLine(133, 124, 169, 124, RGB565_PURPLE);
  gfx->drawPixel(170, 125, RGB565_PURPLE);
  gfx->drawLine(171, 126, 171, 153, RGB565_PURPLE);
  gfx->drawPixel(170, 154, RGB565_PURPLE);
  gfx->drawLine(169, 155, 128, 155, RGB565_PURPLE);
  gfx->drawPixel(347, 52, RGB565_PURPLE);
  gfx->drawLine(348, 53, 348, 122, RGB565_PURPLE);
  gfx->drawPixel(347, 123, RGB565_PURPLE);
  gfx->drawLine(346, 124, 310, 124, RGB565_PURPLE);
  gfx->drawPixel(309, 125, RGB565_PURPLE);
  gfx->drawLine(308, 126, 308, 153, RGB565_PURPLE);
  gfx->drawPixel(309, 154, RGB565_PURPLE);
  gfx->drawLine(310, 155, 351, 155, RGB565_PURPLE);
  gfx->drawLine(150, 68, 169, 68, RGB565_PURPLE);
  gfx->drawPixel(170, 69, RGB565_PURPLE);
  gfx->drawLine(171, 70, 171, 81, RGB565_PURPLE);
  gfx->drawPixel(170, 82, RGB565_PURPLE);
  gfx->drawLine(169, 83, 150, 83, RGB565_PURPLE);
  gfx->drawPixel(149, 82, RGB565_PURPLE);
  gfx->drawLine(148, 81, 148, 70, RGB565_PURPLE);
  gfx->drawPixel(149, 69, RGB565_PURPLE);
  gfx->drawLine(150, 100, 169, 100, RGB565_PURPLE);
  gfx->drawPixel(170, 101, RGB565_PURPLE);
  gfx->drawLine(171, 102, 171, 105, RGB565_PURPLE);
  gfx->drawPixel(170, 106, RGB565_PURPLE);
  gfx->drawLine(169, 107, 150, 107, RGB565_PURPLE);
  gfx->drawPixel(149, 106, RGB565_PURPLE);
  gfx->drawLine(148, 105, 148, 102, RGB565_PURPLE);
  gfx->drawPixel(149, 101, RGB565_PURPLE);
  gfx->drawLine(190, 68, 217, 68, RGB565_PURPLE);
  gfx->drawPixel(218, 69, RGB565_PURPLE);
  gfx->drawLine(219, 70, 219, 81, RGB565_PURPLE);
  gfx->drawPixel(218, 82, RGB565_PURPLE);
  gfx->drawLine(217, 83, 190, 83, RGB565_PURPLE);
  gfx->drawPixel(189, 82, RGB565_PURPLE);
  gfx->drawLine(188, 81, 188, 70, RGB565_PURPLE);
  gfx->drawPixel(189, 69, RGB565_PURPLE);
  gfx->drawLine(262, 68, 289, 68, RGB565_PURPLE);
  gfx->drawPixel(290, 69, RGB565_PURPLE);
  gfx->drawLine(291, 70, 291, 81, RGB565_PURPLE);
  gfx->drawPixel(290, 82, RGB565_PURPLE);
  gfx->drawLine(289, 83, 262, 83, RGB565_PURPLE);
  gfx->drawPixel(261, 82, RGB565_PURPLE);
  gfx->drawLine(260, 81, 260, 70, RGB565_PURPLE);
  gfx->drawPixel(261, 69, RGB565_PURPLE);
  gfx->drawLine(310, 68, 329, 68, RGB565_PURPLE);
  gfx->drawPixel(330, 69, RGB565_PURPLE);
  gfx->drawLine(331, 70, 331, 81, RGB565_PURPLE);
  gfx->drawPixel(330, 82, RGB565_PURPLE);
  gfx->drawLine(329, 83, 310, 83, RGB565_PURPLE);
  gfx->drawPixel(309, 82, RGB565_PURPLE);
  gfx->drawLine(308, 81, 308, 70, RGB565_PURPLE);
  gfx->drawPixel(309, 69, RGB565_PURPLE);
  gfx->drawLine(310, 100, 329, 100, RGB565_PURPLE);
  gfx->drawPixel(330, 101, RGB565_PURPLE);
  gfx->drawLine(331, 102, 331, 105, RGB565_PURPLE);
  gfx->drawPixel(330, 106, RGB565_PURPLE);
  gfx->drawLine(329, 107, 310, 107, RGB565_PURPLE);
  gfx->drawPixel(309, 106, RGB565_PURPLE);
  gfx->drawLine(308, 105, 308, 102, RGB565_PURPLE);
  gfx->drawPixel(309, 101, RGB565_PURPLE);
  gfx->drawLine(286, 100, 289, 100, RGB565_PURPLE);
  gfx->drawPixel(290, 101, RGB565_PURPLE);
  gfx->drawLine(291, 102, 291, 153, RGB565_PURPLE);
  gfx->drawPixel(290, 154, RGB565_PURPLE);
  gfx->drawLine(289, 155, 286, 155, RGB565_PURPLE);
  gfx->drawPixel(285, 154, RGB565_PURPLE);
  gfx->drawLine(284, 153, 284, 133, RGB565_PURPLE);
  gfx->drawPixel(283, 132, RGB565_PURPLE);
  gfx->drawLine(282, 131, 262, 131, RGB565_PURPLE);
  gfx->drawPixel(261, 130, RGB565_PURPLE);
  gfx->drawLine(260, 129, 260, 126, RGB565_PURPLE);
  gfx->drawPixel(261, 125, RGB565_PURPLE);
  gfx->drawLine(262, 124, 282, 124, RGB565_PURPLE);
  gfx->drawPixel(283, 123, RGB565_PURPLE);
  gfx->drawLine(284, 122, 284, 102, RGB565_PURPLE);
  gfx->drawPixel(285, 101, RGB565_PURPLE);
  gfx->drawLine(190, 100, 193, 100, RGB565_PURPLE);
  gfx->drawPixel(194, 101, RGB565_PURPLE);
  gfx->drawLine(195, 102, 195, 122, RGB565_PURPLE);
  gfx->drawPixel(196, 123, RGB565_PURPLE);
  gfx->drawLine(197, 124, 217, 124, RGB565_PURPLE);
  gfx->drawPixel(218, 125, RGB565_PURPLE);
  gfx->drawLine(219, 126, 219, 129, RGB565_PURPLE);
  gfx->drawPixel(218, 130, RGB565_PURPLE);
  gfx->drawLine(217, 131, 197, 131, RGB565_PURPLE);
  gfx->drawPixel(196, 132, RGB565_PURPLE);
  gfx->drawLine(195, 133, 195, 153, RGB565_PURPLE);
  gfx->drawPixel(194, 154, RGB565_PURPLE);
  gfx->drawLine(193, 155, 190, 155, RGB565_PURPLE);
  gfx->drawPixel(189, 154, RGB565_PURPLE);
  gfx->drawLine(188, 153, 188, 102, RGB565_PURPLE);
  gfx->drawPixel(189, 101, RGB565_PURPLE);
  gfx->drawLine(214, 100, 265, 100, RGB565_PURPLE);
  gfx->drawPixel(266, 101, RGB565_PURPLE);
  gfx->drawLine(267, 102, 267, 105, RGB565_PURPLE);
  gfx->drawPixel(266, 106, RGB565_PURPLE);
  gfx->drawLine(265, 107, 245, 107, RGB565_PURPLE);
  gfx->drawPixel(244, 108, RGB565_PURPLE);
  gfx->drawLine(243, 109, 243, 129, RGB565_PURPLE);
  gfx->drawPixel(242, 130, RGB565_PURPLE);
  gfx->drawLine(241, 131, 238, 131, RGB565_PURPLE);
  gfx->drawPixel(237, 130, RGB565_PURPLE);
  gfx->drawLine(236, 129, 236, 109, RGB565_PURPLE);
  gfx->drawPixel(235, 108, RGB565_PURPLE);
  gfx->drawLine(234, 107, 214, 107, RGB565_PURPLE);
  gfx->drawPixel(213, 106, RGB565_PURPLE);
  gfx->drawLine(212, 105, 212, 102, RGB565_PURPLE);
  gfx->drawPixel(213, 101, RGB565_PURPLE);
  // ****Inside lines reflected vertically around y=160, moved down 12 then up 1 (net +11)****

  // top line
  gfx->drawLine(133, 280, 234, 280, RGB565_PURPLE);
  gfx->drawLine(245, 280, 346, 280, RGB565_PURPLE);
  gfx->drawPixel(235, 279, RGB565_PURPLE);
  gfx->drawPixel(244, 279, RGB565_PURPLE);
  gfx->drawLine(236, 279, 236, 250, RGB565_PURPLE);
  gfx->drawLine(243, 279, 243, 250, RGB565_PURPLE);
  gfx->drawPixel(237, 249, RGB565_PURPLE);
  gfx->drawPixel(242, 249, RGB565_PURPLE);
  gfx->drawLine(238, 248, 241, 248, RGB565_PURPLE);

  // top left
  gfx->drawPixel(132, 279, RGB565_PURPLE);
  gfx->drawLine(131, 278, 131, 209, RGB565_PURPLE);
  gfx->drawPixel(132, 208, RGB565_PURPLE);
  gfx->drawLine(133, 207, 169, 207, RGB565_PURPLE);
  gfx->drawPixel(170, 206, RGB565_PURPLE);
  gfx->drawLine(171, 205, 171, 178, RGB565_PURPLE);
  gfx->drawPixel(170, 177, RGB565_PURPLE);
  gfx->drawLine(169, 176, 128, 176, RGB565_PURPLE);

  // top right
  gfx->drawPixel(347, 279, RGB565_PURPLE);
  gfx->drawLine(348, 278, 348, 209, RGB565_PURPLE);
  gfx->drawPixel(347, 208, RGB565_PURPLE);
  gfx->drawLine(346, 207, 310, 207, RGB565_PURPLE);
  gfx->drawPixel(309, 206, RGB565_PURPLE);
  gfx->drawLine(308, 205, 308, 178, RGB565_PURPLE);
  gfx->drawPixel(309, 177, RGB565_PURPLE);
  gfx->drawLine(310, 176, 351, 176, RGB565_PURPLE);

  // 1
  gfx->drawLine(150, 263, 169, 263, RGB565_PURPLE);
  gfx->drawPixel(170, 262, RGB565_PURPLE);
  gfx->drawLine(171, 261, 171, 250, RGB565_PURPLE);
  gfx->drawPixel(170, 249, RGB565_PURPLE);
  gfx->drawLine(169, 248, 150, 248, RGB565_PURPLE);
  gfx->drawPixel(149, 249, RGB565_PURPLE);
  gfx->drawLine(148, 250, 148, 261, RGB565_PURPLE);
  gfx->drawPixel(149, 262, RGB565_PURPLE);

  // 2
  gfx->drawLine(150, 231, 169, 231, RGB565_PURPLE);
  gfx->drawPixel(170, 230, RGB565_PURPLE);
  gfx->drawLine(171, 229, 171, 226, RGB565_PURPLE);
  gfx->drawPixel(170, 225, RGB565_PURPLE);
  gfx->drawLine(169, 224, 150, 224, RGB565_PURPLE);
  gfx->drawPixel(149, 225, RGB565_PURPLE);
  gfx->drawLine(148, 226, 148, 229, RGB565_PURPLE);
  gfx->drawPixel(149, 230, RGB565_PURPLE);

  // 3
  gfx->drawLine(190, 263, 217, 263, RGB565_PURPLE);
  gfx->drawPixel(218, 262, RGB565_PURPLE);
  gfx->drawLine(219, 261, 219, 250, RGB565_PURPLE);
  gfx->drawPixel(218, 249, RGB565_PURPLE);
  gfx->drawLine(217, 248, 190, 248, RGB565_PURPLE);
  gfx->drawPixel(189, 249, RGB565_PURPLE);
  gfx->drawLine(188, 250, 188, 261, RGB565_PURPLE);
  gfx->drawPixel(189, 262, RGB565_PURPLE);

  // 4
  gfx->drawLine(262, 263, 289, 263, RGB565_PURPLE);
  gfx->drawPixel(290, 262, RGB565_PURPLE);
  gfx->drawLine(291, 261, 291, 250, RGB565_PURPLE);
  gfx->drawPixel(290, 249, RGB565_PURPLE);
  gfx->drawLine(289, 248, 262, 248, RGB565_PURPLE);
  gfx->drawPixel(261, 249, RGB565_PURPLE);
  gfx->drawLine(260, 250, 260, 261, RGB565_PURPLE);
  gfx->drawPixel(261, 262, RGB565_PURPLE);

  // 5
  gfx->drawLine(310, 263, 329, 263, RGB565_PURPLE);
  gfx->drawPixel(330, 262, RGB565_PURPLE);
  gfx->drawLine(331, 261, 331, 250, RGB565_PURPLE);
  gfx->drawPixel(330, 249, RGB565_PURPLE);
  gfx->drawLine(329, 248, 310, 248, RGB565_PURPLE);
  gfx->drawPixel(309, 249, RGB565_PURPLE);
  gfx->drawLine(308, 250, 308, 261, RGB565_PURPLE);
  gfx->drawPixel(309, 262, RGB565_PURPLE);

  // 6
  gfx->drawLine(310, 231, 329, 231, RGB565_PURPLE);
  gfx->drawPixel(330, 230, RGB565_PURPLE);
  gfx->drawLine(331, 229, 331, 226, RGB565_PURPLE);
  gfx->drawPixel(330, 225, RGB565_PURPLE);
  gfx->drawLine(329, 224, 310, 224, RGB565_PURPLE);
  gfx->drawPixel(309, 225, RGB565_PURPLE);
  gfx->drawLine(308, 226, 308, 229, RGB565_PURPLE);
  gfx->drawPixel(309, 230, RGB565_PURPLE);

  // 7 (correctly reflected and shifted +11)
  gfx->drawLine(286, 231, 289, 231, RGB565_PURPLE);
  gfx->drawPixel(290, 230, RGB565_PURPLE);
  gfx->drawLine(291, 229, 291, 178, RGB565_PURPLE);
  gfx->drawPixel(290, 177, RGB565_PURPLE);
  gfx->drawLine(289, 176, 286, 176, RGB565_PURPLE);
  gfx->drawPixel(285, 177, RGB565_PURPLE);
  gfx->drawLine(284, 178, 284, 198, RGB565_PURPLE);
  gfx->drawPixel(283, 199, RGB565_PURPLE);
  gfx->drawLine(282, 200, 262, 200, RGB565_PURPLE);
  gfx->drawPixel(261, 201, RGB565_PURPLE);
  gfx->drawLine(260, 202, 260, 205, RGB565_PURPLE);
  gfx->drawPixel(261, 206, RGB565_PURPLE);
  gfx->drawLine(262, 207, 282, 207, RGB565_PURPLE);
  gfx->drawPixel(283, 208, RGB565_PURPLE);
  gfx->drawLine(284, 209, 284, 229, RGB565_PURPLE);
  gfx->drawPixel(285, 230, RGB565_PURPLE);

  // 8
  gfx->drawLine(190, 231, 193, 231, RGB565_PURPLE);
  gfx->drawPixel(194, 230, RGB565_PURPLE);
  gfx->drawLine(195, 229, 195, 209, RGB565_PURPLE);
  gfx->drawPixel(196, 208, RGB565_PURPLE);
  gfx->drawLine(197, 207, 217, 207, RGB565_PURPLE);
  gfx->drawPixel(218, 206, RGB565_PURPLE);
  gfx->drawLine(219, 205, 219, 202, RGB565_PURPLE);
  gfx->drawPixel(218, 201, RGB565_PURPLE);
  gfx->drawLine(217, 200, 197, 200, RGB565_PURPLE);
  gfx->drawPixel(196, 199, RGB565_PURPLE);
  gfx->drawLine(195, 198, 195, 178, RGB565_PURPLE);
  gfx->drawPixel(194, 177, RGB565_PURPLE);
  gfx->drawLine(193, 176, 190, 176, RGB565_PURPLE);
  gfx->drawPixel(189, 177, RGB565_PURPLE);
  gfx->drawLine(188, 178, 188, 229, RGB565_PURPLE);
  gfx->drawPixel(189, 230, RGB565_PURPLE);

  // 9
  gfx->drawLine(214, 231, 265, 231, RGB565_PURPLE);
  gfx->drawPixel(266, 230, RGB565_PURPLE);
  gfx->drawLine(267, 229, 267, 226, RGB565_PURPLE);
  gfx->drawPixel(266, 225, RGB565_PURPLE);
  gfx->drawLine(265, 224, 245, 224, RGB565_PURPLE);
  gfx->drawPixel(244, 223, RGB565_PURPLE);
  gfx->drawLine(243, 222, 243, 202, RGB565_PURPLE);
  gfx->drawPixel(242, 201, RGB565_PURPLE);
  gfx->drawLine(241, 200, 238, 200, RGB565_PURPLE);
  gfx->drawPixel(237, 201, RGB565_PURPLE);
  gfx->drawLine(236, 202, 236, 222, RGB565_PURPLE);
  gfx->drawPixel(235, 223, RGB565_PURPLE);
  gfx->drawLine(234, 224, 214, 224, RGB565_PURPLE);
  gfx->drawPixel(213, 225, RGB565_PURPLE);
  gfx->drawLine(212, 226, 212, 229, RGB565_PURPLE);
}

/*******************************************************************************
   Starting Screen
 ******************************************************************************/
void drawStartScreen() {
  gfx->fillScreen(RGB565_PURPLE);
  gfx->setTextColor(RGB565_BLACK);
  gfx->setTextSize(7);
  gfx->setCursor(80, 100);
  gfx->print("GAMEBOY");
  gfx->setTextSize(3);
  gfx->setCursor(120, 225);
  gfx->print("Press Start");
}

/*******************************************************************************
   pacmantheme
 ******************************************************************************/
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0
int pacmantheme[] = {
  // Pacman Start Up
  NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16, //1
  NOTE_B5, 32, NOTE_FS5, -16, NOTE_DS5, 8, NOTE_C5, 16,
  NOTE_C6, 16, NOTE_G6, 16, NOTE_E6, 16, NOTE_C6, 32, NOTE_G6, -16, NOTE_E6, 8,

  NOTE_B4, 16,  NOTE_B5, 16,  NOTE_FS5, 16,   NOTE_DS5, 16,  NOTE_B5, 32,  //2
  NOTE_FS5, -16, NOTE_DS5, 8,  NOTE_DS5, 32, NOTE_E5, 32,  NOTE_F5, 32,
  NOTE_F5, 32,  NOTE_FS5, 32,  NOTE_G5, 32,  NOTE_G5, 32, NOTE_GS5, 32,  NOTE_A5, 16, NOTE_B5, 8
};
int buzzer = 5;
int theme = 0;
int tempo1 = 105;
int notes1 = sizeof(pacmantheme) / sizeof(pacmantheme[0]) / 2; //
int wholenote1 = (60000 * 4) / tempo1;
int divider1 = 0, noteDuration1 = 0;
/*******************************************************************************
   SETUP
 ******************************************************************************/
void setup() {
  pinMode(START_BUTTON, INPUT_PULLUP);
  digitalWrite(red, HIGH);
  delay(1000);
#ifdef DEV_DEVICE_INIT
  DEV_DEVICE_INIT();
#endif
  Serial.begin(115200);
  Serial.println("Arduino_GFX Pac-Man 10x12 Example");

  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif
  gfx->fillScreen(RGB565_BLACK);
  playero.x = (gfx->width() - playero.w) / 2;
  playero.y = (gfx->height() - playero.h) / 2 + 6;
  gfx->setRotation(1);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    rtc.adjust(DateTime(2026, 2, 26, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

}

/*******************************************************************************
   LOOP
 ******************************************************************************/
void loop() {
  if (!gameStarted) {
    drawStartScreen();
    while (digitalRead(START_BUTTON) == HIGH) {
    }
    delay(200);
    gameStarted = true;
    gfx->fillScreen(RGB565_BLACK);
  }
  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);
  if (gameStarted) {
    if (!mapDrawn) {
      drawMap();
      mapDrawn = true;
      delay(300);
    }
    while (theme != 1) {
      for (int thisNote = 0; thisNote < notes1 * 2; thisNote = thisNote + 2) {
        // calculates the duration of each note
        divider1 = pacmantheme[thisNote + 1];
        if (divider1 > 0) {
          // regular note, just proceed
          noteDuration1 = (wholenote1) / divider1;
        } else if (divider1 < 0) {
          // dotted notes are represented with negative durations!!
          noteDuration1 = (wholenote1) / abs(divider1);
          noteDuration1 *= 1.5; // increases the duration in half for dotted notes
        }
        // we only play the note for 90% of the duration, leaving 10% as a pause
        tone(buzzer, pacmantheme[thisNote], noteDuration1 * 0.9);
        // Wait for the specief duration before playing the next note.
        delay(noteDuration1);
        // stop the waveform generation before the next note.
        noTone(buzzer);
        theme = 1;
      }
    }
    DateTime now = rtc.now();
    //filling the old sprite and also filling the old time
    gfx->fillRect(playero.x, playero.y, playero.w, playero.h, RGB565_BLACK);
    gfx->fillRect(10, 10, 100, 20, RGB565_BLACK);
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    //movement with joystick
    if (xValue > 800) {
      playero.x += 2;
    }
    if (xValue < 200) {
      playero.x -= 2;
    }
    if (yValue > 800) {
      playero.y += 2;
    }
    if (yValue < 200) {
      playero.y -= 2;
    }

    //wrap around
    if (playero.x > 342) {
      playero.x = 128;
      playero.y = 160;
    }
    if (playero.x < 128) {
      playero.x = 342;
      playero.y = 160;
    }

    static int step = 0;
    step++;
    if (step % 2 == 0) {
      currentFrame++;
      if (currentFrame >= 2) currentFrame = 0;
    }
    if (currentFrame >= 2) currentFrame = 0;
    playero.bitmap = pacmanFrames[currentFrame];
    drawSprite(playero, 0x0000);


    // CLOCK STUFF
    gfx->setTextColor(RGB565_WHITE);
    gfx->setTextSize(1);
    gfx->setCursor(10, 10);
    static int lastSecond = -1;
    if (now.second() != lastSecond) {
      lastSecond = now.second();
    }
    int hour24 = now.hour();
    const char* timeformat = "AM";
    if (hour24 >= 12) {
      timeformat = "PM";
    }
    h = now.hour() % 12;
    if (h == 0) {
      h = 12;
      timeformat = "PM";
    }
    gfx->setCursor(10, 10);
    gfx->print(h);
    gfx->print(":");
    if (now.minute() < 10) gfx->print("0");
    gfx->print(now.minute(), DEC);
    gfx->print(":");
    if (now.second() < 10) gfx->print("0");
    gfx->print(now.second(), DEC);
    gfx->print(" ");
    gfx->print(timeformat);
    delay(100);
  }
}
