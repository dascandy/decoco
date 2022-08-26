#include <decoco/decoco.hpp>
#include "zlib/zlib.h"
#include <assert.h>

namespace Decoco {

struct ZlibCompressorS : Compressor {
  static int compressorLevelToZlib(Compressor::Level level) {
    switch(level) {
      default: 
      case Compressor::Level::Balanced: return Zlib::Z_DEFAULT_COMPRESSION;
      case Compressor::Level::Fast: return Zlib::Z_BEST_SPEED;
      case Compressor::Level::Small: return Zlib::Z_BEST_COMPRESSION;
    }
  }
  ZlibCompressorS(Compressor::Level level, size_t chunkSize)
  : Compressor(chunkSize)
  , strm()
  {
    int ret = deflateInit(&strm, compressorLevelToZlib(level));
    assert(ret == Zlib::Z_OK);
  }
  std::span<uint8_t> compress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (!in.empty()) {
      strm.avail_in = in.size();
      strm.next_in = const_cast<uint8_t*>(in.data());
    }
    strm.avail_out = out.size();
    strm.next_out = out.data();
    int ret = deflate(&strm, Zlib::Z_NO_FLUSH);
    assert(ret != Zlib::Z_STREAM_ERROR);
    return out.subspan(0, out.size() - strm.avail_out);
  }
  std::span<uint8_t> flush(std::span<uint8_t> out) override {
    strm.avail_in = 0;
    strm.next_in = nullptr;
    strm.avail_out = out.size();
    strm.next_out = out.data();
    int ret = deflate(&strm, Zlib::Z_FINISH);
    assert(ret != Zlib::Z_STREAM_ERROR);
    return out.subspan(0, out.size() - strm.avail_out);
  }
  ~ZlibCompressorS() {
    deflateEnd(&strm);
  }
  Zlib::z_stream strm;
};

std::unique_ptr<Compressor> ZlibCompressor(Compressor::Level level, size_t chunkSize) { return std::make_unique<ZlibCompressorS>(level, chunkSize); }

struct ZlibDecompressorS : Decompressor {
  ZlibDecompressorS(size_t outputChunkSize)
  : Decompressor(outputChunkSize)
  , strm()
  {
    int ret = inflateInit(&strm);
    assert(ret == Zlib::Z_OK);
  }
  std::span<uint8_t> decompress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (strm.avail_in == 0 && not in.empty()) {
      strm.next_in = const_cast<uint8_t*>(in.data());
      strm.avail_in = in.size();
    } else if (not in.empty()) {
      // you can't put more data in before the rest is taken out
      std::terminate();
    }

    strm.avail_out = out.size();
    strm.next_out = out.data();
    int ret = inflate(&strm, Zlib::Z_SYNC_FLUSH);
    assert(ret == Zlib::Z_OK || ret == Zlib::Z_STREAM_END);
    return out.subspan(0, out.size() - strm.avail_out);
  }
  ~ZlibDecompressorS() {
    inflateEnd(&strm);
  }
  Zlib::z_stream strm;
};

std::unique_ptr<Decompressor> ZlibDecompressor(size_t outputChunkSize) { return std::make_unique<ZlibDecompressorS>(outputChunkSize); }

}


