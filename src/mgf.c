#include <string.h>
#ifdef WIN32
#include <stdint.h>
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif
#include "encparams.h"
#include "poly.h"

#ifdef USE_AES_NI
#include "../aes/aes.h"
#endif

int16_t NTRU_MGF_TRIT_TBL[243][5] = {
    { 0,  0,  0,  0,  0},
    { 1,  0,  0,  0,  0},
    {-1,  0,  0,  0,  0},
    { 0,  1,  0,  0,  0},
    { 1,  1,  0,  0,  0},
    {-1,  1,  0,  0,  0},
    { 0, -1,  0,  0,  0},
    { 1, -1,  0,  0,  0},
    {-1, -1,  0,  0,  0},
    { 0,  0,  1,  0,  0},
    { 1,  0,  1,  0,  0},
    {-1,  0,  1,  0,  0},
    { 0,  1,  1,  0,  0},
    { 1,  1,  1,  0,  0},
    {-1,  1,  1,  0,  0},
    { 0, -1,  1,  0,  0},
    { 1, -1,  1,  0,  0},
    {-1, -1,  1,  0,  0},
    { 0,  0, -1,  0,  0},
    { 1,  0, -1,  0,  0},
    {-1,  0, -1,  0,  0},
    { 0,  1, -1,  0,  0},
    { 1,  1, -1,  0,  0},
    {-1,  1, -1,  0,  0},
    { 0, -1, -1,  0,  0},
    { 1, -1, -1,  0,  0},
    {-1, -1, -1,  0,  0},
    { 0,  0,  0,  1,  0},
    { 1,  0,  0,  1,  0},
    {-1,  0,  0,  1,  0},
    { 0,  1,  0,  1,  0},
    { 1,  1,  0,  1,  0},
    {-1,  1,  0,  1,  0},
    { 0, -1,  0,  1,  0},
    { 1, -1,  0,  1,  0},
    {-1, -1,  0,  1,  0},
    { 0,  0,  1,  1,  0},
    { 1,  0,  1,  1,  0},
    {-1,  0,  1,  1,  0},
    { 0,  1,  1,  1,  0},
    { 1,  1,  1,  1,  0},
    {-1,  1,  1,  1,  0},
    { 0, -1,  1,  1,  0},
    { 1, -1,  1,  1,  0},
    {-1, -1,  1,  1,  0},
    { 0,  0, -1,  1,  0},
    { 1,  0, -1,  1,  0},
    {-1,  0, -1,  1,  0},
    { 0,  1, -1,  1,  0},
    { 1,  1, -1,  1,  0},
    {-1,  1, -1,  1,  0},
    { 0, -1, -1,  1,  0},
    { 1, -1, -1,  1,  0},
    {-1, -1, -1,  1,  0},
    { 0,  0,  0, -1,  0},
    { 1,  0,  0, -1,  0},
    {-1,  0,  0, -1,  0},
    { 0,  1,  0, -1,  0},
    { 1,  1,  0, -1,  0},
    {-1,  1,  0, -1,  0},
    { 0, -1,  0, -1,  0},
    { 1, -1,  0, -1,  0},
    {-1, -1,  0, -1,  0},
    { 0,  0,  1, -1,  0},
    { 1,  0,  1, -1,  0},
    {-1,  0,  1, -1,  0},
    { 0,  1,  1, -1,  0},
    { 1,  1,  1, -1,  0},
    {-1,  1,  1, -1,  0},
    { 0, -1,  1, -1,  0},
    { 1, -1,  1, -1,  0},
    {-1, -1,  1, -1,  0},
    { 0,  0, -1, -1,  0},
    { 1,  0, -1, -1,  0},
    {-1,  0, -1, -1,  0},
    { 0,  1, -1, -1,  0},
    { 1,  1, -1, -1,  0},
    {-1,  1, -1, -1,  0},
    { 0, -1, -1, -1,  0},
    { 1, -1, -1, -1,  0},
    {-1, -1, -1, -1,  0},
    { 0,  0,  0,  0,  1},
    { 1,  0,  0,  0,  1},
    {-1,  0,  0,  0,  1},
    { 0,  1,  0,  0,  1},
    { 1,  1,  0,  0,  1},
    {-1,  1,  0,  0,  1},
    { 0, -1,  0,  0,  1},
    { 1, -1,  0,  0,  1},
    {-1, -1,  0,  0,  1},
    { 0,  0,  1,  0,  1},
    { 1,  0,  1,  0,  1},
    {-1,  0,  1,  0,  1},
    { 0,  1,  1,  0,  1},
    { 1,  1,  1,  0,  1},
    {-1,  1,  1,  0,  1},
    { 0, -1,  1,  0,  1},
    { 1, -1,  1,  0,  1},
    {-1, -1,  1,  0,  1},
    { 0,  0, -1,  0,  1},
    { 1,  0, -1,  0,  1},
    {-1,  0, -1,  0,  1},
    { 0,  1, -1,  0,  1},
    { 1,  1, -1,  0,  1},
    {-1,  1, -1,  0,  1},
    { 0, -1, -1,  0,  1},
    { 1, -1, -1,  0,  1},
    {-1, -1, -1,  0,  1},
    { 0,  0,  0,  1,  1},
    { 1,  0,  0,  1,  1},
    {-1,  0,  0,  1,  1},
    { 0,  1,  0,  1,  1},
    { 1,  1,  0,  1,  1},
    {-1,  1,  0,  1,  1},
    { 0, -1,  0,  1,  1},
    { 1, -1,  0,  1,  1},
    {-1, -1,  0,  1,  1},
    { 0,  0,  1,  1,  1},
    { 1,  0,  1,  1,  1},
    {-1,  0,  1,  1,  1},
    { 0,  1,  1,  1,  1},
    { 1,  1,  1,  1,  1},
    {-1,  1,  1,  1,  1},
    { 0, -1,  1,  1,  1},
    { 1, -1,  1,  1,  1},
    {-1, -1,  1,  1,  1},
    { 0,  0, -1,  1,  1},
    { 1,  0, -1,  1,  1},
    {-1,  0, -1,  1,  1},
    { 0,  1, -1,  1,  1},
    { 1,  1, -1,  1,  1},
    {-1,  1, -1,  1,  1},
    { 0, -1, -1,  1,  1},
    { 1, -1, -1,  1,  1},
    {-1, -1, -1,  1,  1},
    { 0,  0,  0, -1,  1},
    { 1,  0,  0, -1,  1},
    {-1,  0,  0, -1,  1},
    { 0,  1,  0, -1,  1},
    { 1,  1,  0, -1,  1},
    {-1,  1,  0, -1,  1},
    { 0, -1,  0, -1,  1},
    { 1, -1,  0, -1,  1},
    {-1, -1,  0, -1,  1},
    { 0,  0,  1, -1,  1},
    { 1,  0,  1, -1,  1},
    {-1,  0,  1, -1,  1},
    { 0,  1,  1, -1,  1},
    { 1,  1,  1, -1,  1},
    {-1,  1,  1, -1,  1},
    { 0, -1,  1, -1,  1},
    { 1, -1,  1, -1,  1},
    {-1, -1,  1, -1,  1},
    { 0,  0, -1, -1,  1},
    { 1,  0, -1, -1,  1},
    {-1,  0, -1, -1,  1},
    { 0,  1, -1, -1,  1},
    { 1,  1, -1, -1,  1},
    {-1,  1, -1, -1,  1},
    { 0, -1, -1, -1,  1},
    { 1, -1, -1, -1,  1},
    {-1, -1, -1, -1,  1},
    { 0,  0,  0,  0, -1},
    { 1,  0,  0,  0, -1},
    {-1,  0,  0,  0, -1},
    { 0,  1,  0,  0, -1},
    { 1,  1,  0,  0, -1},
    {-1,  1,  0,  0, -1},
    { 0, -1,  0,  0, -1},
    { 1, -1,  0,  0, -1},
    {-1, -1,  0,  0, -1},
    { 0,  0,  1,  0, -1},
    { 1,  0,  1,  0, -1},
    {-1,  0,  1,  0, -1},
    { 0,  1,  1,  0, -1},
    { 1,  1,  1,  0, -1},
    {-1,  1,  1,  0, -1},
    { 0, -1,  1,  0, -1},
    { 1, -1,  1,  0, -1},
    {-1, -1,  1,  0, -1},
    { 0,  0, -1,  0, -1},
    { 1,  0, -1,  0, -1},
    {-1,  0, -1,  0, -1},
    { 0,  1, -1,  0, -1},
    { 1,  1, -1,  0, -1},
    {-1,  1, -1,  0, -1},
    { 0, -1, -1,  0, -1},
    { 1, -1, -1,  0, -1},
    {-1, -1, -1,  0, -1},
    { 0,  0,  0,  1, -1},
    { 1,  0,  0,  1, -1},
    {-1,  0,  0,  1, -1},
    { 0,  1,  0,  1, -1},
    { 1,  1,  0,  1, -1},
    {-1,  1,  0,  1, -1},
    { 0, -1,  0,  1, -1},
    { 1, -1,  0,  1, -1},
    {-1, -1,  0,  1, -1},
    { 0,  0,  1,  1, -1},
    { 1,  0,  1,  1, -1},
    {-1,  0,  1,  1, -1},
    { 0,  1,  1,  1, -1},
    { 1,  1,  1,  1, -1},
    {-1,  1,  1,  1, -1},
    { 0, -1,  1,  1, -1},
    { 1, -1,  1,  1, -1},
    {-1, -1,  1,  1, -1},
    { 0,  0, -1,  1, -1},
    { 1,  0, -1,  1, -1},
    {-1,  0, -1,  1, -1},
    { 0,  1, -1,  1, -1},
    { 1,  1, -1,  1, -1},
    {-1,  1, -1,  1, -1},
    { 0, -1, -1,  1, -1},
    { 1, -1, -1,  1, -1},
    {-1, -1, -1,  1, -1},
    { 0,  0,  0, -1, -1},
    { 1,  0,  0, -1, -1},
    {-1,  0,  0, -1, -1},
    { 0,  1,  0, -1, -1},
    { 1,  1,  0, -1, -1},
    {-1,  1,  0, -1, -1},
    { 0, -1,  0, -1, -1},
    { 1, -1,  0, -1, -1},
    {-1, -1,  0, -1, -1},
    { 0,  0,  1, -1, -1},
    { 1,  0,  1, -1, -1},
    {-1,  0,  1, -1, -1},
    { 0,  1,  1, -1, -1},
    { 1,  1,  1, -1, -1},
    {-1,  1,  1, -1, -1},
    { 0, -1,  1, -1, -1},
    { 1, -1,  1, -1, -1},
    {-1, -1,  1, -1, -1},
    { 0,  0, -1, -1, -1},
    { 1,  0, -1, -1, -1},
    {-1,  0, -1, -1, -1},
    { 0,  1, -1, -1, -1},
    { 1,  1, -1, -1, -1},
    {-1,  1, -1, -1, -1},
    { 0, -1, -1, -1, -1},
    { 1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1}
};

void ntru_MGF(uint8_t *seed, uint16_t seed_len, const NtruEncParams *params, NtruIntPoly *i) {
    uint16_t N = params->N;
    i->N = N;
    uint16_t min_calls_mask = params->min_calls_mask;
    uint16_t hlen = params->hlen;

    uint8_t buf[min_calls_mask * hlen];
    uint16_t buf_len = 0;
    uint8_t Z[hlen];
    params->hash(seed, seed_len, (uint8_t*)&Z);   /* hashSeed is always true */
    uint16_t counter = 0;


#ifndef USE_AES_NI
    uint8_t H[hlen];
    uint16_t inp_len = hlen + sizeof counter;
    uint8_t hash_inp[inp_len];
    while (counter < min_calls_mask-3) {
        uint8_t H_arr[4][NTRU_MAX_HASH_LEN];

        uint8_t j;
        uint8_t hash_inp_arr[4][inp_len];
        uint8_t *hash_inp[4];
        for (j=0; j<4; j++) {
            uint16_t counter_endian = htons(counter);   /* convert to network byte order */
            memcpy(&hash_inp_arr[j], Z, sizeof Z);
            memcpy((uint8_t*)&hash_inp_arr[j] + sizeof Z, &counter_endian, sizeof counter_endian);
            hash_inp[j] = hash_inp_arr[j];
            counter++;
        }
        uint8_t *H[4];
        for (j=0; j<4; j++)
            H[j] = H_arr[j];
        params->hash_4way(hash_inp, inp_len, H);

        uint16_t i;
        for (j=0; j<4; j++)
            for (i=0; i<hlen; i++)
                if (H[j][i] < 243) {   /* 243 = 3^5 */
                    buf[buf_len] = H[j][i];
                    buf_len++;
                }
    }
    while (counter < min_calls_mask) {
        uint16_t counter_endian = htons(counter);   /* convert to network byte order */
        memcpy(&hash_inp, Z, sizeof Z);
        memcpy((uint8_t*)&hash_inp + sizeof Z, &counter_endian, sizeof counter_endian);
        params->hash((uint8_t*)&hash_inp, inp_len, (uint8_t*)&H);

        uint16_t i;
        for (i=0; i<hlen; i++)
            if (H[i] < 243) {   /* 243 = 3^5 */
                buf[buf_len] = H[i];
                buf_len++;
            }

        counter++;
    }
#else

    uint8_t H[32*min_calls_mask];

    unsigned char plaintext[32*min_calls_mask];
    memset(plaintext, 0, 32*min_calls_mask);

    AES_KEY enc_key;

    uint8_t key_inp[32];
    memset(key_inp, 0, 32);

    memcpy(&key_inp, Z, hlen);

    AES_set_encrypt_key(key_inp, 256, &enc_key);

    while (counter < 2*min_calls_mask) {
        uint16_t counter_endian = htons(counter);   /* convert to network byte order */

        memcpy(plaintext+counter*16, &counter_endian, sizeof counter_endian);

        counter++;
    }
    AES_ECB_encrypt(plaintext, H, 32*min_calls_mask, enc_key.KEY, enc_key.nr);

    counter = 0;


    while (counter < min_calls_mask) {

        uint16_t i;
        for (i=0; i<hlen; i++) {
            if (H[i+counter*32] < 243) {   /* 243 = 3^5 */
                buf[buf_len] = H[i+counter*32];
                buf_len++;
            }
        }

        counter++;
    }
#endif


    for (;;) {
        uint16_t cur = 0;
        uint16_t j;
        for (j=0; j<buf_len; j++) {
            uint8_t O = buf[j];
            memcpy(&i->coeffs[cur], NTRU_MGF_TRIT_TBL[O], 10);   /* copy 5 uint16_t's */
            cur += 5;
            if (cur >= N)
                return;
        }

        if (cur >= N)
            return;


#ifndef USE_AES_NI
        memcpy(&hash_inp, Z, sizeof Z);
        memcpy((uint8_t*)&hash_inp + hlen, &counter, sizeof counter);
        params->hash((uint8_t*)&hash_inp, inp_len, (uint8_t*)&H);
#else
        memset(key_inp, 0, 32);

        if (sizeof Z > 32)
            memcpy(&key_inp, Z, 32);
        else
            memcpy(&key_inp, Z, sizeof Z);
        
        AES_set_encrypt_key(key_inp, 256, &enc_key);
        memset(&plaintext, 0, 32);
        memcpy(&plaintext, &counter, sizeof counter);
        counter++;
        memcpy(&plaintext+16, &counter, sizeof counter);
        AES_ECB_encrypt(plaintext, H, 32, enc_key.KEY, enc_key.nr);
#endif /* USE_AES_NI */


        memcpy(&buf, &H, hlen);
        buf_len = hlen;
    }
}
