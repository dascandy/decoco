#include <decoco/decoco.hpp>

namespace Decoco {

std::vector<uint8_t> Compressor::compress(std::span<const uint8_t> in) {
  std::vector<uint8_t> out;
  out.resize(chunkSize);

  size_t written = compress(in, out);
  while (written == chunkSize) {
    out.resize(out.size() + chunkSize);
    written = compress({}, std::span<uint8_t>(out).subspan(out.size() - chunkSize));
  }
  out.resize(out.size() - chunkSize + written);
  return out;
}

std::vector<uint8_t> Compressor::flush() {
  std::vector<uint8_t> out;
  size_t used;

  do {
    out.resize(out.size() + chunkSize);
    used = flush(std::span<uint8_t>(out).subspan(out.size() - chunkSize));
  } while (used == chunkSize);
  out.resize(out.size() - chunkSize + used);

  return out;
}

std::vector<uint8_t> Decompressor::decompress(std::span<const uint8_t> in) {
  std::vector<uint8_t> out;
  out.resize(outputChunkSize);
  size_t written = decompress(in, out);
  out.resize(written);
  return out;
}


}


