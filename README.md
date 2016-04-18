# C implementation of NTRUEncrypt

This is a fork of [libntru by Tim Buktu](https://github.com/tbuktu/libntru) with added AVX2 and AVX512 vectorization and usage of AES instead of SHA-* for faster randomness generation.

The goal is to demonstrate possible performance improvements by leveraging modern processor architecture features. 

The ```VEC´´´ environment variable controls at compile time which instructions are used. Set ```VEC´´´ to one of ```none, sse, avx2, avx512´´´ to use the corresponding type of SIMD instructions (e.g., ```export VEC=avx2´´´).

Note that this a purely academic proof-of-concept implementation, thus platform dependent makefiles and installation targets have been stripped out to focus just on the optimizations. The original README is still available [here](README_.md)

Authors:
--------

* Shay Gueron (1, 2)
* Fabian Schlieker (3)

(1) Intel Corporation, Israel Development Center, Haifa, Israel  
(2) University of Haifa, Israel  
(3) Ruhr University Bochum, Germany