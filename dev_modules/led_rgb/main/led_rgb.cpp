#include <stdio.h>

#include "include/ws2812b.h"

extern "C" void app_main(void)
{
  Ws2812b led(GPIO_NUM_48);
  led.write(255, 0, 0);
}
