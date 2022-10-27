#include <decoco/decoco.hpp>
#include "zlib/zlib.h"
#include <assert.h>

namespace Decoco {

struct DeflateCompressorS : Compressor {
  static int compressorLevelToDeflate(Compressor::Level level) {
    switch(level) {
      default: 
      case Compressor::Level::Balanced: return Zlib::Z_DEFAULT_COMPRESSION;
      case Compressor::Level::Fast: return Zlib::Z_BEST_SPEED;
      case Compressor::Level::Small: return Zlib::Z_BEST_COMPRESSION;
    }
  }
  DeflateCompressorS(Compressor::Level level, size_t chunkSize)
  : Compressor(chunkSize)
  , strm()
  {
    int ret = deflateInit2(&strm, compressorLevelToDeflate(level), Zlib::Z_DEFLATED, -1, 8, Zlib::Z_DEFAULT_STRATEGY);
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
  ~DeflateCompressorS() {
    deflateEnd(&strm);
  }
  Zlib::z_stream strm;
};

std::unique_ptr<Compressor> DeflateCompressor(Compressor::Level level, size_t chunkSize) { return std::make_unique<DeflateCompressorS>(level, chunkSize); }

struct DeflateDecompressorS : Decompressor {
  DeflateDecompressorS(size_t outputChunkSize)
  : Decompressor(outputChunkSize)
  , strm()
  {
    int ret = inflateInit2(&strm, -1);
    assert(ret == Zlib::Z_OK);
  }
  std::span<uint8_t> decompress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (strm.avail_in == 0) {
      strm.next_in = const_cast<uint8_t*>(in.data());
      strm.avail_in = in.size();
    } else if (not in.empty()) {
      // you can't put more data in before the rest is taken out
      std::terminate();
    }

    strm.avail_out = out.size();
    strm.next_out = out.data();
    in_used += strm.avail_in;
    int ret = inflate(&strm, Zlib::Z_NO_FLUSH);
    in_used -= strm.avail_in;
    assert(ret == Zlib::Z_OK || ret == Zlib::Z_STREAM_END);
    return out.subspan(0, out.size() - strm.avail_out);
  }
  ~DeflateDecompressorS() {
    inflateEnd(&strm);
  }
  size_t bytesUsed() const override {
    return in_used;
  }
  size_t in_used = 0;
  Zlib::z_stream strm;
};

std::unique_ptr<Decompressor> DeflateDecompressor(size_t outputChunkSize) { return std::make_unique<DeflateDecompressorS>(outputChunkSize); }

}


