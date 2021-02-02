#include <decoco/decoco.hpp>
#include <bzlib.h>
#include <assert.h>

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
  : strm()
  , chunkSize(chunkSize)
  {
    int ret = BZ2_bzCompressInit(&strm, compressorLevelToBZlib(level), 0, 30);
    assert(ret == BZ_OK);
  }
  std::vector<uint8_t> compress(std::span<const uint8_t> in) override {
    std::vector<uint8_t> out;

    strm.avail_in = in.size();
    strm.next_in = const_cast<char*>(reinterpret_cast<const char*>(in.data()));
    do {
      out.resize(out.size() + chunkSize);
      strm.avail_out = chunkSize;
      strm.next_out = reinterpret_cast<char*>(out.data()) + out.size() - chunkSize;
      int ret = BZ2_bzCompress(&strm, BZ_RUN);
      assert(ret == BZ_RUN_OK);
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
      strm.next_out = reinterpret_cast<char*>(out.data()) + out.size() - chunkSize;
      int ret = BZ2_bzCompress(&strm, BZ_FINISH);
      out.resize(out.size() - strm.avail_out);
      if (ret < 0) {
        assert(ret == BZ_FINISH_OK);
        std::terminate();
      } else if (ret == BZ_STREAM_END) {
        out.resize(out.size() - strm.avail_out);
        return out;
      }
    } while (strm.avail_out == 0);

    return out;
  }
  ~Bzip2CompressorS() {
    BZ2_bzCompressEnd(&strm);
  }
  bz_stream strm;
  size_t chunkSize;
};

std::unique_ptr<Compressor> Bzip2Compressor(Compressor::Level level, size_t chunkSize) { return std::make_unique<Bzip2CompressorS>(level, chunkSize); }

struct Bzip2DecompressorS : Decompressor {
  Bzip2DecompressorS(size_t outputChunkSize)
  : strm()
  , outputChunkSize(outputChunkSize)
  {
    int ret = BZ2_bzDecompressInit(&strm, 0, 0);
    assert(ret == BZ_OK);
  }
  std::vector<uint8_t> decompress(std::span<const uint8_t> in) override {
    std::vector<uint8_t> out;
    if (strm.avail_in == 0) {
      strm.next_in = reinterpret_cast<char*>(const_cast<uint8_t*>(in.data()));
      strm.avail_in = in.size();
    } else if (not in.empty()) {
      // you can't put more data in before the rest is taken out
      std::terminate();
    }

    out.resize(outputChunkSize);
    strm.avail_out = out.size();
    strm.next_out = reinterpret_cast<char*>(out.data());
    int ret = BZ2_bzDecompress(&strm);
    assert(ret == BZ_OK || ret == BZ_STREAM_END);
    out.resize(outputChunkSize - strm.avail_out);
    return out;
  }
  ~Bzip2DecompressorS() {
    BZ2_bzDecompressEnd(&strm);
  }
  bz_stream strm;
  size_t outputChunkSize;
};

std::unique_ptr<Decompressor> Bzip2Decompressor(size_t outputChunkSize) { return std::make_unique<Bzip2DecompressorS>(outputChunkSize); }


