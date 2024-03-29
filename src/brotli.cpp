#include <decoco/decoco.hpp>
#include <brotli/encode.h>
#include <brotli/decode.h>
#include <assert.h>

namespace Decoco {

struct BrotliCompressorS : Compressor {
  static int compressorLevelToBrotli(Compressor::Level level) {
    switch(level) {
      default: 
      case Compressor::Level::Balanced: return 7;
      case Compressor::Level::Fast: return 0;
      case Compressor::Level::Small: return 11;
    }
  }
  BrotliCompressorS(Compressor::Level level, size_t chunkSize)
  : Compressor(chunkSize)
  , indata(nullptr)
  , insize(0)
  {
    state = BrotliEncoderCreateInstance(
            +[](void*, size_t count) { return malloc(count); },
            +[](void*, void* ptr) { return free(ptr); },
            nullptr);
    BrotliEncoderSetParameter(state, BROTLI_PARAM_QUALITY, compressorLevelToBrotli(level));
  }
  std::span<uint8_t> compress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (not in.empty()) {
      insize = in.size();
      indata = in.data();
    }
    size_t outsize = out.size();
    uint8_t* outdata = out.data();
    size_t totalout = 0;
    bool ok = BrotliEncoderCompressStream(state, BROTLI_OPERATION_PROCESS, &insize, &indata, &outsize, &outdata, &totalout);
    assert(ok);
    return out.subspan(0, totalout);
  }
  std::span<uint8_t> flush(std::span<uint8_t> out) override {
    size_t outsize = out.size();
    uint8_t* outdata = out.data();
    size_t totalout = 0;
    bool ok = BrotliEncoderCompressStream(state, BROTLI_OPERATION_FINISH, &insize, &indata, &outsize, &outdata, &totalout);
    assert(ok);
    return out.subspan(0, totalout);
  }
  ~BrotliCompressorS() {
    BrotliEncoderDestroyInstance(state);
  }
  BrotliEncoderState* state;
  const uint8_t* indata;
  size_t insize;
};

std::unique_ptr<Compressor> BrotliCompressor(Compressor::Level level, size_t chunkSize) { return std::make_unique<BrotliCompressorS>(level, chunkSize); }

struct BrotliDecompressorS : Decompressor {
  BrotliDecompressorS(size_t outputChunkSize)
  : Decompressor(outputChunkSize)
  , indata(nullptr)
  , insize(0)
  {
    state = BrotliDecoderCreateInstance(
            +[](void*, size_t count) { return malloc(count); },
            +[](void*, void* ptr) { return free(ptr); },
            nullptr);
  }
  std::span<uint8_t> decompress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (not in.empty()) {
      if (insize == 0) {
        insize = in.size();
        indata = in.data();
      } else {
        // you can't put more data in before the rest is taken out
        std::terminate();
      }
    }

    size_t totalout = 0;
    size_t outsize = out.size();
    uint8_t* outdata = out.data();
    in_used += insize;
    BrotliDecoderResult res = BrotliDecoderDecompressStream(state, &insize, &indata, &outsize, &outdata, &totalout);
    in_used -= insize;
    if (res == BROTLI_DECODER_RESULT_ERROR) {
      throw std::runtime_error("Decoding failed");
    }
    return out.subspan(0, totalout);
  }
  ~BrotliDecompressorS() {
    BrotliDecoderDestroyInstance(state);
  }
  size_t bytesUsed() const override {
    return in_used;
  }
  BrotliDecoderState* state;
  const uint8_t* indata;
  size_t insize;
  size_t in_used = 0;
};

std::unique_ptr<Decompressor> BrotliDecompressor(size_t outputChunkSize) { return std::make_unique<BrotliDecompressorS>(outputChunkSize); }

}


