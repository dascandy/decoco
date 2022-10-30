#pragma once

#include <vector>
#include <span>
#include <string_view>
#include <cstdint>
#include <memory>

namespace Decoco {

class Compressor {
public:
  enum class Level {
    Balanced,
    Fast,
    Small,
  };
  std::vector<uint8_t> compress(std::span<const uint8_t> in);
  virtual std::span<uint8_t> compress(std::span<const uint8_t> in, std::span<uint8_t> out) = 0;
  std::vector<uint8_t> flush();
  virtual std::span<uint8_t> flush(std::span<uint8_t> out) = 0;
  virtual ~Compressor() = default;
protected:
  Compressor(size_t chunkSize) : chunkSize(chunkSize) {}
private:
  size_t chunkSize;
};

class Decompressor {
public:
  std::vector<uint8_t> decompress(std::span<const uint8_t> in);
  virtual std::span<uint8_t> decompress(std::span<const uint8_t> in, std::span<uint8_t> out) = 0;
  virtual ~Decompressor() = default;
  virtual size_t bytesUsed() const = 0;
protected:
  Decompressor(size_t outputChunkSize) 
  : outputChunkSize(outputChunkSize)
  {}
private:
  size_t outputChunkSize;
};

std::unique_ptr<Compressor> GzipCompressor(Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);
std::unique_ptr<Compressor> ZlibCompressor(Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);
std::unique_ptr<Compressor> DeflateCompressor(Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);
std::unique_ptr<Compressor> LzmaCompressor(Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);
std::unique_ptr<Compressor> Bzip2Compressor(Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);
std::unique_ptr<Compressor> BrotliCompressor(Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);
std::unique_ptr<Compressor> ZstdCompressor(Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);
std::unique_ptr<Compressor> FindCompressor(std::string_view name, Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);

std::unique_ptr<Decompressor> GzipDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> ZlibDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> DeflateDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> LzmaDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> Bzip2Decompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> BrotliDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> ZstdDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> FindDecompressor(std::string_view name, size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> SniffDecompressor(std::span<uint8_t> file, size_t outputChunkSize = 16384);

// Convenience functions
std::vector<uint8_t> compress(Decoco::Compressor& c, std::span<const uint8_t> in);
inline std::vector<uint8_t> compress(const std::unique_ptr<Decoco::Compressor>& c, std::span<const uint8_t> in) {
  return compress(*c.get(), in);
}
std::vector<uint8_t> decompress(Decoco::Decompressor& c, std::span<const uint8_t> in);
inline std::vector<uint8_t> decompress(const std::unique_ptr<Decoco::Decompressor>& c, std::span<const uint8_t> in) {
  return decompress(*c.get(), in);
}

std::vector<uint8_t> gzip(std::span<const uint8_t> in);
std::vector<uint8_t> bzip2(std::span<const uint8_t> in);
std::vector<uint8_t> xzip(std::span<const uint8_t> in);

std::vector<uint8_t> gunzip(std::span<const uint8_t> in);
std::vector<uint8_t> bunzip2(std::span<const uint8_t> in);
std::vector<uint8_t> xunzip(std::span<const uint8_t> in);

}


