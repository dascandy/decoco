#include <decoco/decoco.hpp>
#include <catch2/catch.hpp>

static std::vector<uint8_t> hello = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x0a };

static std::vector<uint8_t> helloGzip = { 0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xcb, 0x48, 0xcd, 0xc9, 0xc9, 0xe7, 0x02, 0x00, 0x20, 0x30, 0x3a, 0x36, 0x06, 0x00, 0x00, 0x00 };

TEST_CASE("Basic roundtrip gzip") {
  auto compressor = Decoco::GzipCompressor();
  auto gzData = compressor->compress(hello);
  auto end = compressor->flush();
  gzData.insert(gzData.end(), end.begin(), end.end());
  REQUIRE(gzData == helloGzip);
  auto decompressor = Decoco::GzipDecompressor();
  auto plainData = decompressor->decompress(gzData);
  REQUIRE(plainData == hello);
}

TEST_CASE("Basic GunZIP of hello") {
  auto decompressor = Decoco::GzipDecompressor();
  auto unzippedData = decompressor->decompress(helloGzip);
  REQUIRE(unzippedData == hello);
}


