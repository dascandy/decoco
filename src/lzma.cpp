#include <decoco/decoco.hpp>
#include <lzma.h>
#include <assert.h>

struct LzmaCompressorS : Compressor {
  static int compressorLevelToLzmalib(Compressor::Level level) {
    switch(level) {
      default: 
      case Compressor::Level::Balanced: return 6;
      case Compressor::Level::Fast: return 1;
      case Compressor::Level::Small: return 9;
    }
  }
  LzmaCompressorS(Compressor::Level level, size_t chunkSize)
  : strm()
  , chunkSize(chunkSize)
  {
    int ret = lzma_easy_encoder(&strm, compressorLevelToLzmalib(level), LZMA_CHECK_CRC64);
    assert(ret == LZMA_OK);
  }
  std::vector<uint8_t> compress(std::span<const uint8_t> in) override {
    std::vector<uint8_t> out;

    strm.avail_in = in.size();
    strm.next_in = in.data();
    do {
      out.resize(out.size() + chunkSize);
      strm.avail_out = chunkSize;
      strm.next_out = out.data() + out.size() - chunkSize;
      int ret = lzma_code(&strm, LZMA_RUN);
      assert(ret == LZMA_STREAM_END || ret == LZMA_OK);
    } while (strm.avail_out == 0);

    return out;
  }
  std::vector<uint8_t> flush() override {
    std::vector<uint8_t> out;

    do {
      out.resize(out.size() + chunkSize);
      strm.avail_in = 0;
      strm.next_in = nullptr;
      strm.avail_out = chunkSize;
      strm.next_out = out.data() + out.size() - chunkSize;
      int ret = lzma_code(&strm, LZMA_FINISH);
      assert(ret == LZMA_OK || ret == LZMA_STREAM_END);
      out.resize(out.size() - strm.avail_out);
    } while (strm.avail_out == 0);

    return out;
  }
  ~LzmaCompressorS() {
    lzma_end(&strm);
  }
  lzma_stream strm;
  size_t chunkSize;
};

std::unique_ptr<Compressor> LzmaCompressor(Compressor::Level level, size_t chunkSize) { return std::make_unique<LzmaCompressorS>(level, chunkSize); }

struct LzmaDecompressorS : Decompressor {
  LzmaDecompressorS(size_t outputChunkSize)
  : strm()
  , outputChunkSize(outputChunkSize)
  {
    int ret = lzma_stream_decoder(&strm, UINT64_MAX, 0);
    assert(ret == LZMA_OK);
  }
  std::vector<uint8_t> decompress(std::span<const uint8_t> in) override {
    std::vector<uint8_t> out;
    if (strm.avail_in == 0) {
      strm.next_in = in.data();
      strm.avail_in = in.size();
    } else if (not in.empty()) {
      // you can't put more data in before the rest is taken out
      std::terminate();
    }

    out.resize(outputChunkSize);
    strm.avail_out = out.size();
    strm.next_out = out.data();
    int ret = lzma_code(&strm, LZMA_RUN);
    assert(ret == LZMA_OK || ret == LZMA_STREAM_END);
    out.resize(outputChunkSize - strm.avail_out);
    return out;
  }
  ~LzmaDecompressorS() {
    lzma_end(&strm);
  }
  lzma_stream strm;
  size_t outputChunkSize;
};

std::unique_ptr<Decompressor> LzmaDecompressor(size_t outputChunkSize) { return std::make_unique<LzmaDecompressorS>(outputChunkSize); }


