#include <decoco/decoco.hpp>
#include <zstd.h>
#include <assert.h>
#include <iostream>

namespace Decoco {

struct ZstdCompressorS : Compressor {
  static int compressorLevelToZSTD(Compressor::Level level) {
    switch(level) {
      default: 
      case Compressor::Level::Balanced: return 7;
      case Compressor::Level::Fast: return 1;
      case Compressor::Level::Small: return 18;
    }
  }
  ZstdCompressorS(Compressor::Level level, size_t chunkSize)
  : Compressor(chunkSize)
  {
    cstream = ZSTD_createCStream();
    if (cstream==NULL) { throw std::runtime_error("Could not initialize ZSTD library"); }
    size_t const initResult = ZSTD_initCStream(cstream, compressorLevelToZSTD(level));
    if (ZSTD_isError(initResult)) { throw std::runtime_error("Could not initialize ZSTD library"); }

    size_t const checksumResult = ZSTD_CCtx_setParameter(cstream, ZSTD_c_checksumFlag, 1);
    if (ZSTD_isError(checksumResult)) { throw std::runtime_error("Zstd refuses to checksum"); }
  }
  std::span<uint8_t> compress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (!in.empty()) {
      input = { in.data(), in.size(), 0 };
    }

    ZSTD_outBuffer output = { out.data(), out.size(), 0 };
    auto rv = ZSTD_compressStream(cstream, &output, &input);
    if (ZSTD_isError(rv)) { throw std::runtime_error("internal error in zstd"); }
    return out.subspan(0, output.pos);
  }
  std::span<uint8_t> flush(std::span<uint8_t> out) override {
    ZSTD_outBuffer output = { out.data(), out.size(), 0 };
    size_t const remainingToFlush = ZSTD_endStream(cstream, &output);
    if (remainingToFlush) { throw std::runtime_error("Flush incomplete"); }
    return out.subspan(0, output.pos);
  }
  ~ZstdCompressorS() {
    ZSTD_freeCStream(cstream);
  }
  ZSTD_CStream* cstream;
  ZSTD_inBuffer input = {};
};

std::unique_ptr<Compressor> ZstdCompressor(Compressor::Level level, size_t chunkSize) { return std::make_unique<ZstdCompressorS>(level, chunkSize); }

struct ZstdDecompressorS : Decompressor {
  ZstdDecompressorS(size_t outputChunkSize)
  : Decompressor(outputChunkSize)
  {
    dstream = ZSTD_createDStream();
    if (dstream==NULL) { throw std::runtime_error("Could not initialize ZSTD library"); }
    size_t const initResult = ZSTD_initDStream(dstream);
    if (ZSTD_isError(initResult)) { throw std::runtime_error("Could not initialize ZSTD library"); }
  }
  std::span<uint8_t> decompress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (input.pos == input.size) {
      input = { in.data(), in.size(), 0 };
    } else if (not in.empty()) {
      std::terminate();
    }

    ZSTD_outBuffer output = { out.data(), out.size(), 0 };
    in_used += input.size - input.pos;
    auto rv = ZSTD_decompressStream(dstream, &output, &input);
    in_used -= input.size - input.pos;
    if (ZSTD_isError(rv)) { throw std::runtime_error("ZSTD invalid data in stream"); }
    return out.subspan(0, output.pos);
  }
  ~ZstdDecompressorS() {
    ZSTD_freeDStream(dstream);
  }
  size_t bytesUsed() const override {
    return in_used;
  }
  size_t in_used = 0;
  ZSTD_DStream* dstream;
  ZSTD_inBuffer input = {};
};

std::unique_ptr<Decompressor> ZstdDecompressor(size_t outputChunkSize) { return std::make_unique<ZstdDecompressorS>(outputChunkSize); }

}


