#include <decoco/decoco.hpp>
#include <catch2/catch.hpp>

static std::vector<uint8_t> hello = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x0a };

static std::vector<uint8_t> helloBzip2 = { 0x42, 0x5a, 0x68, 0x36, 0x31, 0x41, 0x59, 0x26, 0x53, 0x59, 0xc1, 0xc0, 0x80, 0xe2, 0x00, 0x00, 0x01, 0x41, 0x00, 0x00, 0x10, 0x02, 0x44, 0xa0, 0x00, 0x30, 0xcd, 0x00, 0xc3, 0x46, 0x29, 0x97, 0x17, 0x72, 0x45, 0x38, 0x50, 0x90, 0xc1, 0xc0, 0x80, 0xe2 };

TEST_CASE("Basic roundtrip bzip2") {
  auto compressor = Decoco::Bzip2Compressor();
  auto bzip2Data = compressor->compress(hello);
  auto end = compressor->flush();
  bzip2Data.insert(bzip2Data.end(), end.begin(), end.end());
  REQUIRE(bzip2Data == helloBzip2);
  auto decompressor = Decoco::Bzip2Decompressor();
  auto plainData = decompressor->decompress(bzip2Data);
  REQUIRE(plainData == hello);
}

TEST_CASE("Basic bzip2 of hello") {
  auto decompressor = Decoco::Bzip2Decompressor();
  auto unzippedData = decompressor->decompress(helloBzip2);
  REQUIRE(unzippedData == hello);
}


