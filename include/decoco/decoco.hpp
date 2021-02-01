#pragma once

#include <vector>
#include <span>
#include <string_view>
#include <cstdint>
#include <memory>

struct Compressor {
  enum class Level {
    Balanced,
    Fast,
    Small,
  };
  virtual std::vector<uint8_t> compress(std::span<const uint8_t> in) = 0;
  virtual std::vector<uint8_t> flush() = 0;
  virtual ~Compressor() = default;
};

struct Decompressor {
  virtual std::vector<uint8_t> decompress(std::span<const uint8_t> in) = 0;
  virtual ~Decompressor() = default;
};

std::unique_ptr<Compressor> GzipCompressor(Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);
std::unique_ptr<Compressor> LzmaCompressor(Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);
std::unique_ptr<Compressor> Bzip2Compressor(Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);
std::unique_ptr<Compressor> FindCompressor(std::string_view name, Compressor::Level level = Compressor::Level::Balanced, size_t chunkSize = 16384);

std::unique_ptr<Decompressor> GzipDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> LzmaDecompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> Bzip2Decompressor(size_t outputChunkSize = 16384);
std::unique_ptr<Decompressor> FindDecompressor(std::string_view name, size_t outputChunkSize = 16384);


