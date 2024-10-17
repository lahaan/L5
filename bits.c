#include "bits.h"

uint32_t ToggledBits(uint32_t data_reg, uint32_t before_toggle) {
    return data_reg ^ before_toggle;
}
