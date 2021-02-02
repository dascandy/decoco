#include <decoco/decoco.hpp>
#include <zlib.h>
#include <assert.h>

namespace Decoco {

struct GzipCompressorS : Compressor {
  static int compressorLevelToZlib(Compressor::Level level) {
    switch(level) {
      default: 
      case Compressor::Level::Balanced: return Z_DEFAULT_COMPRESSION;
      case Compressor::Level::Fast: return Z_BEST_SPEED;
      case Compressor::Level::Small: return Z_BEST_COMPRESSION;
    }
  }
  GzipCompressorS(Compressor::Level level, size_t chunkSize)
  : strm()
  , chunkSize(chunkSize)
  {
    // Lovely magic values here. See https://zlib.net/manual.html under deflateInit2
    int ret = deflateInit2(&strm, compressorLevelToZlib(level), Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
    assert(ret == Z_OK);
  }
  std::vector<uint8_t> compress(std::span<const uint8_t> in) override {
    std::vector<uint8_t> out;

    strm.avail_in = in.size();
    strm.next_in = const_cast<uint8_t*>(in.data());
    do {
      out.resize(out.size() + chunkSize);
      strm.avail_out = chunkSize;
      strm.next_out = out.data() + out.size() - chunkSize;
      int ret = deflate(&strm, Z_NO_FLUSH);
      assert(ret != Z_STREAM_ERROR);
    } while (strm.avail_out == 0);
    out.resize(out.size() - strm.avail_out);

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
      int ret = deflate(&strm, Z_FINISH);
      assert(ret != Z_STREAM_ERROR);
    } while (strm.avail_out == 0);
    out.resize(out.size() - strm.avail_out);

    return out;
  }
  ~GzipCompressorS() {
    deflateEnd(&strm);
  }
  z_stream strm;
  size_t chunkSize;
};

std::unique_ptr<Compressor> GzipCompressor(Compressor::Level level, size_t chunkSize) { return std::make_unique<GzipCompressorS>(level, chunkSize); }

struct GzipDecompressorS : Decompressor {
  GzipDecompressorS(size_t outputChunkSize)
  : strm()
  , outputChunkSize(outputChunkSize)
  {
    // Magic value to tell it to do gzip instead.
    int ret = inflateInit2(&strm, 31);
    assert(ret == Z_OK);
  }
  std::vector<uint8_t> decompress(std::span<const uint8_t> in) override {
    std::vector<uint8_t> out;
    if (strm.avail_in == 0) {
      strm.next_in = const_cast<uint8_t*>(in.data());
      strm.avail_in = in.size();
    } else if (not in.empty()) {
      // you can't put more data in before the rest is taken out
      std::terminate();
    }

    out.resize(outputChunkSize);
    strm.avail_out = out.size();
    strm.next_out = out.data();
    int ret = inflate(&strm, Z_NO_FLUSH);
    assert(ret == Z_OK || ret == Z_STREAM_END);
    out.resize(outputChunkSize - strm.avail_out);
    return out;
  }
  ~GzipDecompressorS() {
    inflateEnd(&strm);
  }
  z_stream strm;
  size_t outputChunkSize;
};

std::unique_ptr<Decompressor> GzipDecompressor(size_t outputChunkSize) { return std::make_unique<GzipDecompressorS>(outputChunkSize); }

}


