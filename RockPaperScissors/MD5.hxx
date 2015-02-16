#include <cstdint>
#include <array>

namespace rps {
namespace crypto {
namespace md5 {

  typedef std::uint64_t doubleword;
  typedef std::uint32_t word;
  typedef std::uint8_t  byte;
  typedef std::array<byte, 16> digest;

  digest generate(const byte *data, std::size_t size);

} // namespace md5
} // namespace crypto
} // namespace rps
