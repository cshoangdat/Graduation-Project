#ifndef __IRSEND_H__
#define __IRSEND_H__

#include "stdio.h"
#include "string.h"
#include "esp_err.h"

void rmt_tx_init(void);
esp_err_t rmt_tx_trans(uint16_t signal[], int signal_len);

#endif
