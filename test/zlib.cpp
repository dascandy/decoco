#include <decoco/decoco.hpp>
#include <catch2/catch.hpp>

static std::vector<uint8_t> hello = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x0a };

static std::vector<uint8_t> helloZlib = { 0x78, 0x9C, 0xCB, 0x48, 0xCD, 0xC9, 0xC9, 0xE7, 0x02, 0x00, 0x08, 0x4B, 0x02, 0x1F };

TEST_CASE("Basic roundtrip zlib") {
  auto compressor = Decoco::ZlibCompressor();
  auto zlibData = compressor->compress(hello);
  auto end = compressor->flush();
  zlibData.insert(zlibData.end(), end.begin(), end.end());
  REQUIRE(zlibData == helloZlib);
  auto decompressor = Decoco::ZlibDecompressor();
  auto plainData = decompressor->decompress(zlibData);
  REQUIRE(plainData == hello);
}

TEST_CASE("Basic zlib decompress of hello") {
  auto decompressor = Decoco::ZlibDecompressor();
  auto unzippedData = decompressor->decompress(helloZlib);
  REQUIRE(unzippedData == hello);
}


