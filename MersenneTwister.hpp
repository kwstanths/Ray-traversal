/* A C-program for MT19937, with initialization improved 2002/1/26.
  Coded by Takuji Nishimura and Makoto Matsumoto.

  Before using, initialize the state by using init_genrand(seed)
  or init_by_array(init_key, key_length).

  Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    3. The names of its contributors may not be used to endorse or promote
       products derived from this software without specific prior written
       permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  Any feedback is very welcome.
  http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
  email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#ifndef __MersenneTwister_hpp__
#define __MersenneTwister_hpp__


#include <stdio.h>
#include <stdexcept>


class MersenneTwisterGenerator {
private:
    /* Period parameters */
    static const int N = 624;
    static const int M = 397;
    static const unsigned long MATRIX_A = 0x9908b0dfUL;     /* constant vector a */
    static const unsigned long UPPER_MASK = 0x80000000UL;   /* most significant w-r bits */
    static const unsigned long LOWER_MASK = 0x7fffffffUL;   /* least significant r bits */

public:
    enum Parameterization {
        ONE,
        TWO,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
    };

    /**
        Initialize with seed
    */
    MersenneTwisterGenerator(unsigned long seed) {
        init_genrand(seed);
    }

    /**
        Use custom set initialization parameters
    */
    MersenneTwisterGenerator(Parameterization p) {

        switch (p)
        {
        case MersenneTwisterGenerator::ONE:
        {
            unsigned long init[25] = {
                0x95f24dab, 0x0b685215, 0xe76ccae7, 0xaf3ec239, 0x715fad23,
                0x24a590ad, 0x69e4b5ef, 0xbf456141, 0x96bc1b7b, 0xa7bdf825,
                0xc1de75b7, 0x8858a9c9, 0x2da87693, 0xb657f9dd, 0xffdc8a9f,
                0x8121da71, 0x8b823ecb, 0x885d05f5, 0x4e20cd47, 0x5a9ad5d9,
                0x512c0c03, 0xea857ccd, 0x4cc1d30f, 0x8891a8a1, 0xa6b7aadb
            };
            init_by_array(init, 25);
            break;
        }
        case MersenneTwisterGenerator::TWO:
        {
            unsigned long init[25] = { 
                0xff76afcb, 0x45a85215, 0xfa846afa, 0xaf3ec239, 0x8925adf3,
                0x98acf349, 0x9fb4b97a, 0x96bc1b7b, 0xbc450928, 0x5f89ab5f,
                0xabcdef12, 0xa09580fc, 0xa7bdf825, 0x264870ac, 0xab89c600,
                0x71224a71, 0x9997264a, 0xc1de75b7, 0x9845683b, 0x9bcf45d9,
                0x412c09ff, 0xcc7dea94, 0x65ffcb10, 0xa5459cfb, 0xbc98a6ff
            };
            init_by_array(init, 25);
            break;
        }
        case MersenneTwisterGenerator::THREE:
        {
            unsigned long init[25] = {
                0x631f1690, 0x25973e32, 0xead6f57, 0x6ec9d844, 0x5c49eaee,
                0x64af49b, 0x397c46bc, 0x7e448de9, 0x5a9cc3e5, 0x1afe3625,
                0x3ca88ecf, 0x6ebe4208, 0xc058df5, 0xcbe5be9, 0x3102bbe2,
                0x26a02c5e, 0x541c8153, 0x67906f60, 0x10db9daa, 0x697d2d2,
                0x6d68ab2, 0x3a966cd0, 0x63f37e85, 0x5895f5fa, 0x38a5d054
            };
            init_by_array(init, 25);
            break;
        }
        case MersenneTwisterGenerator::FOUR:
        {
            unsigned long init[25] = {
                0x4ab26e78, 0x21faa2fa, 0x5451cf49, 0x6181ef69, 0x3e6400e6,
                0x14217e23, 0x710757d0, 0x5015cd1a, 0x424479da, 0x1a9a9e69,
                0x475e256a, 0x368db37e, 0x6a3b714c, 0x327b517e, 0x1f461b51,
                0x29bacf25, 0x5d5babb3, 0x51bf6b48, 0x7e0f6384, 0x2b4b8b53,
                0x72e3413a, 0x116ae494, 0x3494b2fb, 0xb13a31, 0x64429599
            };
            init_by_array(init, 25);
            break;
        }
        case MersenneTwisterGenerator::FIVE:
        {
            unsigned long init[25] = {
                0x4353d0cd, 0xb03e0c6, 0x189a769b, 0x54e49eb4, 0x71f32454,
                0x2ca88611, 0x836c40e, 0x2901d82, 0x3a95f874, 0x8138641,
                0x1e7ff521, 0x7c3dbd3d, 0x737b8ddc, 0x6ceaf087, 0x22221a70,
                0x4516dde9, 0x3006c83e, 0x614fd4a1, 0x419ac241, 0x5577f8e1,
                0x440badfc, 0x5072367, 0x3804823e, 0x77465f01, 0x7724c67e
            };
            init_by_array(init, 25);
            break;
        }
        case MersenneTwisterGenerator::SIX:
        {
            unsigned long init[25] = {
                0x5c482a97, 0x2463b9ea, 0x5e884adc, 0x51ead36b, 0x2d517796,
                0x580bd78f, 0x153ea438, 0x3855585c, 0x70a64e2a, 0x6a2342ec,
                0x2a487cb0, 0x1d4ed43b, 0x725a06fb, 0x2cd89a32, 0x57e4ccaf,
                0x7a6d8d3c, 0x4b588f54, 0x542289ec, 0x6de91b18, 0x38437fdb,
                0x7644a45c, 0x32fff902, 0x684a481a, 0x579478fe, 0x749abb43
            };
            init_by_array(init, 25);
            break;
        }
        case MersenneTwisterGenerator::SEVEN:
        {
            unsigned long init[25] = {
                0x77ae35eb, 0x579be4f1, 0x310c50b3, 0x5ff87e05, 0x2f305def,
                0x25a70bf7, 0x1dbabf00, 0x4ad084e9, 0x1f48eaa1, 0x1381823a,
                0x5db70ae5, 0x100f8fca, 0x6590700b, 0x15014acb, 0x5f5e7fd0,
                0x98a3148, 0x799d0247, 0x6b94764, 0x42c296bd, 0x168e121f,
                0x1eba5d23, 0x661e3f1e, 0x5dc79ea8, 0x540a471c, 0x7bd3ee7b
            };
            init_by_array(init, 25);
            break;
        }
        default:
            throw std::invalid_argument("Out of range");
        }

    }

    /**
        Generates a random number on [0,0xffffffff]-interval 
    */
    unsigned long genrand_int32(void) {

        unsigned long y;
        static unsigned long mag01[2] = { 0x0UL, MATRIX_A };
        /* mag01[x] = x * MATRIX_A  for x=0,1 */

        if (mti >= N) { /* generate N words at one time */
            int kk;

            if (mti == N + 1)   /* if init_genrand() has not been called, */
                init_genrand(5489UL); /* a default initial seed is used */

            for (kk = 0; kk < N - M; kk++) {
                y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
                mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
            }
            for (; kk < N - 1; kk++) {
                y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
                mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
            }
            y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
            mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

            mti = 0;
        }

        y = mt[mti++];

        /* Tempering */
        y ^= (y >> 11);
        y ^= (y << 7) & 0x9d2c5680UL;
        y ^= (y << 15) & 0xefc60000UL;
        y ^= (y >> 18);

        return y;
    }

    /** 
        Generates a random number on [0,0x7fffffff]-interval 
    */
    long genrand_int31(void) {
        return (long)(genrand_int32() >> 1);
    }

    /** 
        Generates a random number on [0,1]-real-interval 
    */
    double genrand_real1(void) {
        return genrand_int32()*(1.0 / 4294967295.0);
        /* divided by 2^32-1 */
    }

    /**
        Generates a random number on [0,1)-real-interval 
    */
    double genrand_real2(void) {
        return genrand_int32()*(1.0 / 4294967296.0);
        /* divided by 2^32 */
    }

    /** 
        Generates a random number on (0,1)-real-interval 
    */
    double genrand_real3(void) {
        return (((double)genrand_int32()) + 0.5)*(1.0 / 4294967296.0);
        /* divided by 2^32 */
    }

    /**
        Generates a random number on [0,1) with 53-bit resolution
    */
    double genrand_res53(void) {
        unsigned long a = genrand_int32() >> 5, b = genrand_int32() >> 6;
        return(a*67108864.0 + b)*(1.0 / 9007199254740992.0);
    }
    /* These real versions are due to Isaku Wada, 2002/01/09 added */

    /**
        Generate a random number in the [0,1]-real-interval
    */
    double rng() {
        return genrand_real1();
    }

private:
    unsigned long mt[N];    /* the array for the state vector */
    int mti = N + 1;        /* mti==N+1 means mt[N] is not initialized */


    /* initializes mt[N] with a seed */
    void init_genrand(unsigned long s) {
        mt[0] = s & 0xffffffffUL;
        for (mti = 1; mti < N; mti++) {
            mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
            /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
            /* In the previous versions, MSBs of the seed affect   */
            /* only MSBs of the array mt[].                        */
            /* 2002/01/09 modified by Makoto Matsumoto             */
            mt[mti] &= 0xffffffffUL;
            /* for >32 bit machines */
        }
    }

    /* initialize by an array with array-length */
    /* init_key is the array for initializing keys */
    /* key_length is its length */
    /* slight change for C++, 2004/2/26 */
    void init_by_array(unsigned long init_key[], int key_length) {
        int i, j, k;
        init_genrand(19650218UL);
        i = 1; j = 0;
        k = (N > key_length ? N : key_length);
        for (; k; k--) {
            mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525UL))
                + init_key[j] + j; /* non linear */
            mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
            i++; j++;
            if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
            if (j >= key_length) j = 0;
        }
        for (k = N - 1; k; k--) {
            mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941UL))
                - i; /* non linear */
            mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
            i++;
            if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
        }
        mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
    }
};

#endif