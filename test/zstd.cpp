#include <decoco/decoco.hpp>
#include <catch2/catch.hpp>

static std::vector<uint8_t> hello = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x0a };

static std::vector<uint8_t> helloZstd = { 0x28, 0xb5, 0x2f, 0xfd, 0x04, 0x58, 0x31, 0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x0a, 0x53, 0x88, 0xbd, 0x91 };

TEST_CASE("Basic roundtrip zstd") {
  auto compressor = Decoco::ZstdCompressor();
  auto zstdData = compressor->compress(hello);
  auto end = compressor->flush();
  zstdData.insert(zstdData.end(), end.begin(), end.end());
  REQUIRE(zstdData == helloZstd);
}

TEST_CASE("Basic zstd of hello") {
  auto decompressor = Decoco::ZstdDecompressor();
  auto unzippedData = decompressor->decompress(helloZstd);
  REQUIRE(unzippedData == hello);
}


