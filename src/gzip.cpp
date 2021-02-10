#include <decoco/decoco.hpp>
#include "zlib/zlib.h"
#include <assert.h>

namespace Decoco {

struct GzipCompressorS : Compressor {
  static int compressorLevelToZlib(Compressor::Level level) {
    switch(level) {
      default: 
      case Compressor::Level::Balanced: return Zlib::Z_DEFAULT_COMPRESSION;
      case Compressor::Level::Fast: return Zlib::Z_BEST_SPEED;
      case Compressor::Level::Small: return Zlib::Z_BEST_COMPRESSION;
    }
  }
  GzipCompressorS(Compressor::Level level, size_t chunkSize)
  : Compressor(chunkSize)
  , strm()
  {
    // Lovely magic values here. See https://zlib.net/manual.html under deflateInit2
    int ret = deflateInit2(&strm, compressorLevelToZlib(level), Zlib::Z_DEFLATED, 31, 8, Zlib::Z_DEFAULT_STRATEGY);
    assert(ret == Zlib::Z_OK);
  }
  size_t compress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (!in.empty()) {
      strm.avail_in = in.size();
      strm.next_in = const_cast<uint8_t*>(in.data());
    }
    strm.avail_out = out.size();
    strm.next_out = out.data();
    int ret = deflate(&strm, Zlib::Z_NO_FLUSH);
    assert(ret != Zlib::Z_STREAM_ERROR);
    return out.size() - strm.avail_out;
  }
  size_t flush(std::span<uint8_t> out) override {
    strm.avail_in = 0;
    strm.next_in = nullptr;
    strm.avail_out = out.size();
    strm.next_out = out.data();
    int ret = deflate(&strm, Zlib::Z_FINISH);
    assert(ret != Zlib::Z_STREAM_ERROR);
    return out.size() - strm.avail_out;
  }
  ~GzipCompressorS() {
    deflateEnd(&strm);
  }
  Zlib::z_stream strm;
};

std::unique_ptr<Compressor> GzipCompressor(Compressor::Level level, size_t chunkSize) { return std::make_unique<GzipCompressorS>(level, chunkSize); }

struct GzipDecompressorS : Decompressor {
  GzipDecompressorS(size_t outputChunkSize)
  : Decompressor(outputChunkSize)
  , strm()
  {
    // Magic value to tell it to do gzip instead.
    int ret = inflateInit2(&strm, 31);
    assert(ret == Zlib::Z_OK);
  }
  size_t decompress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (strm.avail_in == 0) {
      strm.next_in = const_cast<uint8_t*>(in.data());
      strm.avail_in = in.size();
    } else if (not in.empty()) {
      // you can't put more data in before the rest is taken out
      std::terminate();
    }

    strm.avail_out = out.size();
    strm.next_out = out.data();
    int ret = inflate(&strm, Zlib::Z_NO_FLUSH);
    assert(ret == Zlib::Z_OK || ret == Zlib::Z_STREAM_END);
    return out.size() - strm.avail_out;
  }
  ~GzipDecompressorS() {
    inflateEnd(&strm);
  }
  Zlib::z_stream strm;
};

std::unique_ptr<Decompressor> GzipDecompressor(size_t outputChunkSize) { return std::make_unique<GzipDecompressorS>(outputChunkSize); }

}


