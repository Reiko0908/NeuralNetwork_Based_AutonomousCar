#ifndef WS2812B_H
#define WS2812B_H

#include "driver/rmt_tx.h"
#include "hal/rmt_types.h"

#define RMT_RESOLUTION 20000000

const rmt_symbol_word_t
  rmt_word_bit_one = {
    .duration0 = 16,
    .level0 = 1,
    .duration1 = 9,
    .level1 = 0,
  },
  rmt_word_bit_zero = {
    .duration0 = 8,
    .level0 = 1,
    .duration1 = 17,
    .level1 = 0,
  },
  rmt_word_reset = {
    .duration0 = 1050,
    .level0 = 0,
    .duration1 = 0,
    .level1 = 0,
  };

class Ws2812b{
  private:
    rmt_channel_handle_t channel_handler;
    rmt_encoder_handle_t copy_encoder_handler;
    rmt_transmit_config_t transmit_config;

  public:
    Ws2812b(gpio_num_t io_num);
    void write(uint8_t red, uint8_t green, uint8_t blue);
};

#endif
