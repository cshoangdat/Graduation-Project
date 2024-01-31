#ifndef __WS2812B_H__
#define __WS2812B_H__
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void ws2812_init();

typedef struct {
  uint8_t green;
  uint8_t red;
  uint8_t blue;
} ws2812_pixel_t;

void ws2812_update(ws2812_pixel_t *pixels);

#ifdef __cplusplus
}
#endif
#endif