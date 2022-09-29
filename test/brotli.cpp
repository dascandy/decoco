#include <decoco/decoco.hpp>
#include <catch2/catch_all.hpp>

static std::vector<uint8_t> hello = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x0a };

// compression from command line results in 0x8f at the start, but that only indicates requested
// compression level. Default for us is balanced, which is 7, while command line uses 11; hence 4 lower.
static std::vector<uint8_t> helloBrotli = { 0x8b, 0x02, 0x80, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x0a, 0x03 };

TEST_CASE("Basic roundtrip brotli") {
  auto compressor = Decoco::BrotliCompressor();
  auto brotliData = compressor->compress(hello);
  auto end = compressor->flush();
  brotliData.insert(brotliData.end(), end.begin(), end.end());
  REQUIRE(brotliData == helloBrotli);
}

TEST_CASE("Basic brotli of hello") {
  auto decompressor = Decoco::BrotliDecompressor();
  auto unzippedData = decompressor->decompress(helloBrotli);
  REQUIRE(unzippedData == hello);
}


