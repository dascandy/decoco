#include <decoco/decoco.hpp>

static std::vector<uint8_t> compress(std::unique_ptr<Decoco::Compressor> c, std::span<const uint8_t> in) {
  std::vector<uint8_t> data = c->compress(in);
  std::vector<uint8_t> end = c->flush();
  data.insert(data.end(), end.begin(), end.end());
  return data;
}

std::vector<uint8_t> Decoco::gzip(std::span<const uint8_t> in) {
  return compress(GzipCompressor(), in);
}

std::vector<uint8_t> Decoco::bzip2(std::span<const uint8_t> in) {
  return compress(Bzip2Compressor(), in);
}

std::vector<uint8_t> Decoco::xzip(std::span<const uint8_t> in) {
  return compress(LzmaCompressor(), in);
}


