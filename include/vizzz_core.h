#pragma once

#include <stdint.h>

namespace vizzz {

constexpr uint16_t packUniverse(uint8_t artNet, uint8_t artSubnet, uint8_t artUni) {
  return (uint16_t(artNet) << 8) | (uint16_t(artSubnet) << 4) | (artUni & 0x0F);
}

constexpr uint8_t applyMaster(uint8_t value, uint8_t master) {
  return master >= 255 ? value : uint8_t((uint16_t(value) * master) >> 8);
}

constexpr uint8_t clampLevel(uint8_t value, uint8_t ceiling) {
  return value > ceiling ? ceiling : value;
}

inline uint8_t slewToward(uint8_t current, uint8_t target, uint8_t step) {
  if (step == 0 || current == target) return target;
  if (target > current) {
    uint16_t next = uint16_t(current) + step;
    return next > target ? target : uint8_t(next);
  }
  uint16_t down = uint16_t(target) + step;
  return down < current ? uint8_t(current - step) : target;
}

}  // namespace vizzz
