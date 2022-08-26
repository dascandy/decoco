#include <decoco/decoco.hpp>

std::vector<uint8_t> compress(std::unique_ptr<Decoco::Compressor> c, std::span<const uint8_t> in) {
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

std::vector<uint8_t> decompress(std::unique_ptr<Decoco::Decompressor> c, std::span<const uint8_t> in) {
  std::vector<uint8_t> data;
  c->decompress(in, data);
  while (true) {
    std::vector<uint8_t> nextbit;
    nextbit.resize(32768);
    nextbit.resize(c->decompress({}, nextbit));
    if (nextbit.empty()) return data;
    data.insert(data.end(), nextbit.begin(), nextbit.end());
  }
}

std::vector<uint8_t> Decoco::gunzip(std::span<const uint8_t> in) {
  return decompress(GzipDecompressor(), in);
}

std::vector<uint8_t> Decoco::bunzip2(std::span<const uint8_t> in) {
  return decompress(Bzip2Decompressor(), in);
}

std::vector<uint8_t> Decoco::xunzip(std::span<const uint8_t> in) {
  return decompress(LzmaDecompressor(), in);
}


