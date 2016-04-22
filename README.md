# C implementation of NTRUEncrypt

This is a fork of [libntru by Tim Buktu](https://github.com/tbuktu/libntru) with added AVX2 and AVX512 vectorization and usage of AES instead of SHA-* for faster randomness generation.

The goal is to demonstrate possible performance improvements by leveraging modern processor architecture features. 

The ```VEC``` environment variable controls at compile time which instructions are used. Set ```VEC``` to one of ```none, sse, avx2, avx512``` before compilation to use the corresponding type of SIMD instructions (e.g., ```export VEC=avx2```).

The following table presents our performance improvements in operations per second (so higher is better), examplarily for the EES1171EP1 paremeter set, measured with the included testbench on an Intel(R) Core(TM) i7-4600U CPU @ 2.70 GHz.

| Optimization   | Encryption | Improvement | Decryption | Improvement |
| ---------------| ----------:| -----------:| ----------:| -----------:|
| Baseline (SSE) |      16668 |             |      11190 |             |
| AVX2           |      20903 |       1.25x |      15297 |       1.37x |

Since there is no processor available yet that supports AVX512 instructions, we measured the instruction count (here lower is better) using the [Intel Software Development Emulator](https://software.intel.com/en-us/articles/intel-software-development-emulator) tool. This is a good indicator for the future performance on real silicon.

| Optimization   | Encryption | Improvement | Decryption | Improvement |
| ---------------| ----------:| -----------:| ----------:| -----------:|
| Baseline (SSE) |     624236 |             |     854348 |             |
| AVX2           |     490453 |       0.79x |     588465 |       0.69x |
| AVX512         |     423979 |       0.68x |     449091 |       0.53x |

Note that this a purely academic proof-of-concept implementation, thus platform dependent makefiles and installation targets have been stripped out to focus just on the optimizations. The original README is still available [here](README_.md)

Authors:
--------

* Shay Gueron (1, 2)
* Fabian Schlieker (3)

(1) Intel Corporation, Israel Development Center, Haifa, Israel  
(2) University of Haifa, Israel  
(3) Ruhr University Bochum, Germany