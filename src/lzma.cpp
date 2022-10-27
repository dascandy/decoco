#include <decoco/decoco.hpp>
#include <lzma.h>
#include <assert.h>

namespace Decoco {

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
  : Compressor(chunkSize)
  , strm()
  {
    int ret = lzma_easy_encoder(&strm, compressorLevelToLzmalib(level), LZMA_CHECK_CRC64);
    assert(ret == LZMA_OK);
  }
  std::span<uint8_t> compress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (!in.empty()) {
      strm.avail_in = in.size();
      strm.next_in = in.data();
    }
    strm.avail_out = out.size();
    strm.next_out = out.data();
    int ret = lzma_code(&strm, LZMA_RUN);
    assert(ret == LZMA_OK || ret == LZMA_STREAM_END);
    return out.subspan(0, out.size() - strm.avail_out);
  }
  std::span<uint8_t> flush(std::span<uint8_t> out) override {
    strm.avail_in = 0;
    strm.next_in = nullptr;
    strm.avail_out = out.size();
    strm.next_out = out.data();
    int ret = lzma_code(&strm, LZMA_FINISH);
    assert(ret == LZMA_OK || ret == LZMA_STREAM_END);
    return out.subspan(0, out.size() - strm.avail_out);
  }
  ~LzmaCompressorS() {
    lzma_end(&strm);
  }
  lzma_stream strm;
};

std::unique_ptr<Compressor> LzmaCompressor(Compressor::Level level, size_t chunkSize) { return std::make_unique<LzmaCompressorS>(level, chunkSize); }

struct LzmaDecompressorS : Decompressor {
  LzmaDecompressorS(size_t outputChunkSize)
  : Decompressor(outputChunkSize)
  , strm()
  {
    int ret = lzma_stream_decoder(&strm, UINT64_MAX, 0);
    assert(ret == LZMA_OK);
  }
  std::span<uint8_t> decompress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (strm.avail_in == 0) {
      strm.next_in = in.data();
      strm.avail_in = in.size();
    } else if (not in.empty()) {
      // you can't put more data in before the rest is taken out
      std::terminate();
    }

    strm.avail_out = out.size();
    strm.next_out = out.data();
    in_used += strm.avail_in;
    int ret = lzma_code(&strm, LZMA_RUN);
    in_used -= strm.avail_in;
    assert(ret == LZMA_OK || ret == LZMA_STREAM_END);
    return out.subspan(0, out.size() - strm.avail_out);
  }
  ~LzmaDecompressorS() {
    lzma_end(&strm);
  }
  size_t bytesUsed() const override {
    return in_used;
  }
  size_t in_used = 0;
  lzma_stream strm;
};

std::unique_ptr<Decompressor> LzmaDecompressor(size_t outputChunkSize) { return std::make_unique<LzmaDecompressorS>(outputChunkSize); }

}


