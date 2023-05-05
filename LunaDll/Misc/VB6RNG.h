#include <cstdint>

namespace VB6RNG {
    std::uint32_t getSeed();
    void setSeed(std::uint32_t newSeed);
    float getLastGeneratedNumber();
    float generateNumber();
}