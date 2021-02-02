#include <decoco/decoco.hpp>
#include <memory>

std::unique_ptr<Compressor> FindCompressor(std::string_view name, Compressor::Level level, size_t chunkSize) {
  if (name == "gzip") return GzipCompressor(level, chunkSize);
  if (name == "lzma") return LzmaCompressor(level, chunkSize);
  if (name == "bzip2") return Bzip2Compressor(level, chunkSize);
  return nullptr;
}

std::unique_ptr<Decompressor> FindDecompressor(std::string_view name, size_t outputChunkSize) {
  if (name == "gzip") return GzipDecompressor(outputChunkSize);
  if (name == "lzma") return LzmaDecompressor(outputChunkSize);
  if (name == "bzip2") return Bzip2Decompressor(outputChunkSize);
  return nullptr;
}

std::unique_ptr<Decompressor> SniffDecompressor(std::span<uint8_t> file, size_t outputChunkSize) {
  if (file.size() >= 2 && file[0] == 0x1F && file[1] == 0x8B) return GzipDecompressor(outputChunkSize);
  if (file.size() >= 3 && file[0] == 0x42 && file[1] == 0x5A && file[2] == 0x68) return Bzip2Decompressor(outputChunkSize);
  if (file.size() >= 4 && file[0] == 0xFD && file[1] == 0x37 && file[2] == 0x7A && file[3] == 0x58) return LzmaDecompressor(outputChunkSize);
  return nullptr;
}


