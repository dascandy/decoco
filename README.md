# Decoco

Decoco is a small C++ library that wraps various compression libraries in a C++(20)-friendly interface.

License: BSD-2-Clause


## Installation

Retrieve the repository and build it with an Evoke-compatible build tool. It uses the platform-provided compression libraries for gzip, bzip2 and lzma. To install these on a Debian-like system, run

    apt install liblzma-dev zlib1g-dev libbz2-dev

## Usage

### Decompression

First, create a decompressor for the appropriate compression format. There are three ways to do this: Using the appropriate direct function, using the FindDecompressor function with the name as a string, or with SniffDecompressor with a small sample of the target compressed data stream (4 bytes is enough for any supported type).

    std::span<uint8_t> data;
    unique_ptr<Decompressor> decomp = GzipDecompressor(); // OR \\
    unique_ptr<Decompressor> decomp = FindDecompressor("gzip"); // OR \\
    unique_ptr<Decompressor> decomp = SniffDecompressor(data.subspan(4));

Which of these works best for you depends on your use circumstances; the first is guaranteed to work, while the latter two allow runtime detection and choices. Each of them has an additional parameter indicating the desired output chunk size to request from the compression library per call to decompress(); it defaults to 16384 bytes. For FindDecompressor and SniffDecompressor, check the returned pointer against nullptr to see if the requested decompressor was found.

Call the decompress function on it with a block of data, and subsequently keep calling it until a zero-sized output vector is returned.

    std::vector<uint8_t> output = decomp.decompress(data);
    while (true) {
      auto additional = decomp.decompress({});
      if (additional.empty()) break;
      output.insert(output.end, additional.begin(), additional.end());
    }

Note that some small compressed files can expand to huge outputs; while the code above works for illustrative purposes it's susceptible to crashing on untrusted inputs. It's best to either limit the size it can go to, or to immediately process intermediate outputs.

When there is no more input to be processed, destruct the Decompressor object at the target of the unique\_ptr to free any associated resources.

### Compression

Compression works mostly similar to decompression, with the exception that in compression outputs are known to be at most slightly larger than the inputs, and typically much smaller. An additional complication is that compressors typically keep some part of the input internal until compression is known to end, so that they can achieve higher compression ratios and deterministic compression irrespective of the block size being input. The result of this is that before destructing a compressor, you should call the flush() function to retrieve the last bytes of compressed data before disposing of it.

To create a compressor, either specify the correct compressor at compile time or select it at runtime with the name as a string.

    unique_ptr<Compressor> comp = Bzip2Compressor(); // OR \\
    unique_ptr<Compressor> comp = FindCompressor("lzma");

Again, the first will guaranteed work if it compiles, while the second will allow runtime choice. For FindCompressor, test the resulting pointer against nullptr to see if the requested compressor could be found. 

To compress data, feed it in the compress call and store the resulting vector of bytes into the compressed output. You can perform multiple calls in sequence to repeatedly add more data to the same stream. When all data has been retrieved, call the flush() function once to retrieve the final section of compressed data. This also closes the compressor for further data.

    std::vector<uint8_t> compressedData;
    while (data still incoming) {
      auto newOutput = comp.compress(newInput);
      compressedData.insert(compressedData.end(), newOutput.begin(), newOutput.end());
    }
    auto finalOutput = comp.flush();
    compressedData.insert(compressedData.end(), finalOutput.begin(), finalOutput.end());

The compressed output can be slightly larger than the input; some data is relatively incompressible. It should on typical data be much smaller.

### Multithreading

Each compressor or decompressor instance should only be used from a single thread at a time. The compressor and decompressor instantiation functions are fully thread safe and need no thread synchronization.

The library internally does not use threading of any kind. No currently supported compression library supports multithreading in a single compressor or decompressor.

## License

The library is available under the (BSD 2-clause license)[https://opensource.org/licenses/BSD-2-Clause]:

Copyright 2021 Peter Bindels

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
