#include <decoco/decoco.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("Short-form roundtrip zlib") {
  std::string blob = "hello\n";
  std::vector<uint8_t> buffer;
  for (auto& c : blob) {
    buffer.push_back(c);
  }
  std::vector<uint8_t> compressedData = Decoco::compress(Decoco::ZlibCompressor(), buffer);
  REQUIRE(buffer == Decoco::decompress(Decoco::ZlibDecompressor(), compressedData));
}

