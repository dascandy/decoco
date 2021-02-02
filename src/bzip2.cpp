#include <decoco/decoco.hpp>
#include <bzlib.h>
#include <assert.h>

namespace Decoco {

struct Bzip2CompressorS : Compressor {
  static int compressorLevelToBZlib(Compressor::Level level) {
    switch(level) {
      default: 
      case Compressor::Level::Balanced: return 6;
      case Compressor::Level::Fast: return 1;
      case Compressor::Level::Small: return 9;
    }
  }
  Bzip2CompressorS(Compressor::Level level, size_t chunkSize)
  : Compressor(chunkSize)
  , strm()
  {
    int ret = BZ2_bzCompressInit(&strm, compressorLevelToBZlib(level), 0, 30);
    assert(ret == BZ_OK);
  }
  size_t compress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (!in.empty()) {
      strm.avail_in = in.size();
      strm.next_in = const_cast<char*>(reinterpret_cast<const char*>(in.data()));
    }
    strm.avail_out = out.size();
    strm.next_out = reinterpret_cast<char*>(out.data());
    int ret = BZ2_bzCompress(&strm, BZ_RUN);
    assert(ret == BZ_RUN_OK);
    return out.size() - strm.avail_out;
  }
  size_t flush(std::span<uint8_t> out) override {
    strm.avail_in = 0;
    strm.next_in = nullptr;
    strm.avail_out = out.size();
    strm.next_out = const_cast<char*>(reinterpret_cast<const char*>(out.data()));
    int ret = BZ2_bzCompress(&strm, BZ_FINISH);
    assert(ret >= 0);
    return out.size() - strm.avail_out;
  }
  ~Bzip2CompressorS() {
    BZ2_bzCompressEnd(&strm);
  }
  bz_stream strm;
};

std::unique_ptr<Compressor> Bzip2Compressor(Compressor::Level level, size_t chunkSize) { return std::make_unique<Bzip2CompressorS>(level, chunkSize); }

struct Bzip2DecompressorS : Decompressor {
  Bzip2DecompressorS(size_t outputChunkSize)
  : Decompressor(outputChunkSize)
  , strm()
  {
    int ret = BZ2_bzDecompressInit(&strm, 0, 0);
    assert(ret == BZ_OK);
  }
  size_t decompress(std::span<const uint8_t> in, std::span<uint8_t> out) override {
    if (strm.avail_in == 0) {
      strm.next_in = reinterpret_cast<char*>(const_cast<uint8_t*>(in.data()));
      strm.avail_in = in.size();
    } else if (not in.empty()) {
      // you can't put more data in before the rest is taken out
      std::terminate();
    }

    strm.avail_out = out.size();
    strm.next_out = reinterpret_cast<char*>(out.data());
    int ret = BZ2_bzDecompress(&strm);
    assert(ret == BZ_OK || ret == BZ_STREAM_END);
    return out.size() - strm.avail_out;
  }
  ~Bzip2DecompressorS() {
    BZ2_bzDecompressEnd(&strm);
  }
  bz_stream strm;
};

std::unique_ptr<Decompressor> Bzip2Decompressor(size_t outputChunkSize) { return std::make_unique<Bzip2DecompressorS>(outputChunkSize); }

}


