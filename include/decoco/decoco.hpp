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
  virtual size_t compress(std::span<const uint8_t> in, std::span<uint8_t> out) = 0;
  std::vector<uint8_t> flush();
  virtual size_t flush(std::span<uint8_t> out) = 0;
  virtual ~Compressor() = default;
protected:
  Compressor(size_t chunkSize) : chunkSize(chunkSize) {}
private:
  size_t chunkSize;
};

class Decompressor {
public:
  std::vector<uint8_t> decompress(std::span<const uint8_t> in);
  virtual size_t decompress(std::span<const uint8_t> in, std::span<uint8_t> out) = 0;
  virtual ~Decompressor() = default;
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
std::unique_ptr<Compressor> FindCompressor(std::string_view name, Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);

std::unique_ptr<Decompressor> GzipDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> ZlibDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> DeflateDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> LzmaDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> Bzip2Decompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> FindDecompressor(std::string_view name, size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> SniffDecompressor(std::span<uint8_t> file, size_t outputChunkSize = 16384);

}


