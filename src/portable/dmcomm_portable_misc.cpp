// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "dmcomm_portable_misc.h"

namespace DMComm {

int8_t hex2val(int8_t hexdigit) {
    int8_t value;
    if (hexdigit >= '0' && hexdigit <= '9') {
        value = hexdigit - 0x30;
    } else if (hexdigit >= 'a' && hexdigit <= 'f') {
        value = hexdigit - 0x57;
    } else if (hexdigit >= 'A' && hexdigit <= 'F') {
        value = hexdigit - 0x37;
    } else {
        value = -1;
    }
    return value;
}

int8_t val2hex(int8_t value) {
    value &= 0xF;
    if (value > 9) {
        return value + 0x37;
    } else {
        return value + 0x30;
    }
}

}  // namespace DMComm
