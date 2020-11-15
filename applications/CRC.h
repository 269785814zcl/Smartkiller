#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
extern uint16_t CRC_Compute(uint8_t *puchMsg, uint16_t usDataLen);

#ifdef __cplusplus
}
#endif