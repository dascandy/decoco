#include <decoco/decoco.hpp>

namespace Decoco {

std::vector<uint8_t> Compressor::compress(std::span<const uint8_t> in) {
  std::vector<uint8_t> chunk;
  std::vector<uint8_t> out;
  chunk.resize(chunkSize);
  std::span<uint8_t> compressed;
  compressed = compress(in, chunk);
  out.insert(out.end(), compressed.begin(), compressed.end());
  if (compressed.size() != chunkSize) 
    return out;

  do {
    compressed = compress({}, chunk);
    out.insert(out.end(), compressed.begin(), compressed.end());
  } while (compressed.size() == chunkSize);
  return out;
}

std::vector<uint8_t> Compressor::flush() {
  std::vector<uint8_t> chunk;
  std::vector<uint8_t> out;
  chunk.resize(chunkSize);
  std::span<uint8_t> compressed;
  do {
    compressed = flush(chunk);
    out.insert(out.end(), compressed.begin(), compressed.end());
  } while (compressed.size() == chunkSize);
  return out;
}

std::vector<uint8_t> Decompressor::decompress(std::span<const uint8_t> in) {
  std::vector<uint8_t> chunk;
  std::vector<uint8_t> out;
  chunk.resize(outputChunkSize);
  std::span<uint8_t> decompressed;
  decompressed = decompress(in, chunk);
  out.insert(out.end(), decompressed.begin(), decompressed.end());
  if (decompressed.size() != outputChunkSize)
    return out;

  do {
    decompressed = decompress({}, chunk);
    out.insert(out.end(), decompressed.begin(), decompressed.end());
  } while (decompressed.size() == outputChunkSize);
  return out;
}

}


