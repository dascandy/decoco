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
  size_t compress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (not in.empty()) {
      insize = in.size();
      indata = in.data();
    }
    size_t outsize = out.size();
    uint8_t* outdata = out.data();
    size_t totalout = 0;
    bool ok = BrotliEncoderCompressStream(state, BROTLI_OPERATION_PROCESS, &insize, &indata, &outsize, &outdata, &totalout);
    assert(ok);
    return totalout;
  }
  size_t flush(std::span<uint8_t> out) override {
    size_t outsize = out.size();
    uint8_t* outdata = out.data();
    size_t totalout = 0;
    bool ok = BrotliEncoderCompressStream(state, BROTLI_OPERATION_PROCESS, &insize, &indata, &outsize, &outdata, &totalout);
    assert(ok);
    return totalout;
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
  size_t decompress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
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
    BrotliDecoderResult res = BrotliDecoderDecompressStream(state, &insize, &indata, &outsize, &outdata, &totalout);
    if (res == BROTLI_DECODER_RESULT_ERROR) {
      throw std::runtime_error("Decoding failed");
    }
    return totalout;
  }
  ~BrotliDecompressorS() {
    BrotliDecoderDestroyInstance(state);
  }
  BrotliDecoderState* state;
  const uint8_t* indata;
  size_t insize;
};

std::unique_ptr<Decompressor> BrotliDecompressor(size_t outputChunkSize) { return std::make_unique<BrotliDecompressorS>(outputChunkSize); }

}


