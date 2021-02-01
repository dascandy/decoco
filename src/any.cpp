#include <decoco/decoco.hpp>
#include <memory>

std::unique_ptr<Compressor> FindCompressor(std::string_view name, Compressor::Level level, size_t chunkSize) {
  if (name == "gzip") return GzipCompressor(level, chunkSize);
  if (name == "lzma") return LzmaCompressor(level, chunkSize);
  if (name == "bzip2") return Bzip2Compressor(level, chunkSize);
  return nullptr;
}

std::unique_ptr<Decompressor> FindDecompressor(std::string_view name) {
  if (name == "gzip") return GzipDecompressor();
  if (name == "lzma") return LzmaDecompressor();
  if (name == "bzip2") return Bzip2Decompressor();
  return nullptr;
}


