#include "../include/ws2812b.h"

Ws2812b::Ws2812b(gpio_num_t io_num){
  this->channel_handler = NULL;
  rmt_tx_channel_config_t channel_config = {
    .gpio_num = io_num,
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = RMT_RESOLUTION,
    .mem_block_symbols = 64,
    .trans_queue_depth = 1,
    .flags = {
      .invert_out = false,
      .with_dma = false
    },
  };
  rmt_copy_encoder_config_t copy_encoder_config = {};
  rmt_new_copy_encoder(&copy_encoder_config, &this->copy_encoder_handler);
  this->transmit_config.loop_count = 0;
  this->transmit_config.flags.eot_level = 0;
  this->transmit_config.flags.queue_nonblocking = 0;
  ESP_ERROR_CHECK(rmt_new_tx_channel(&channel_config, &this->channel_handler));
  ESP_ERROR_CHECK(rmt_enable(this->channel_handler));
}

void Ws2812b::write(uint8_t red, uint8_t green, uint8_t blue){
  rmt_symbol_word_t rmt_words[25];
  int index = 0;

  for(uint8_t i = 0b10000000; i > 0; i >>= 1)
    rmt_words[index++] = green & i ? rmt_word_bit_one : rmt_word_bit_zero;
  for(uint8_t i = 0b10000000; i > 0; i >>= 1)
    rmt_words[index++] = red & i ? rmt_word_bit_one : rmt_word_bit_zero;
  for(uint8_t i = 0b10000000; i > 0; i >>= 1)
    rmt_words[index++] = blue & i ? rmt_word_bit_one : rmt_word_bit_zero;

  rmt_words[25] = rmt_word_reset;

  rmt_transmit(this->channel_handler, this->copy_encoder_handler, rmt_words, sizeof(rmt_symbol_word_t) * 25, &this->transmit_config);
}
