#include <stdlib.h>
#include <string.h>
#include "idxgen.h"
#include "ntru_endian.h"

#ifdef USE_AES_NI
#include "../aes/aes.h"
#endif

void ntru_IGF_init(uint8_t *seed, uint16_t seed_len, const NtruEncParams *params, NtruIGFState *s) {
    s->Z = seed;
    s->zlen = seed_len;
    s->N = params->N;
    s->c = params->c;
    s->rnd_thresh = (1<<s->c) - (1<<s->c)%s->N;
    s->hlen = params->hlen;
    s->rem_len = params->min_calls_r * 8 * s->hlen;
    s->hash = params->hash;
    s->hash_4way = params->hash_4way;
    s->counter = 0;

    s->buf.num_bytes = 0;
    s->buf.last_byte_bits = 0;

#ifndef USE_AES_NI
    while (s->counter < params->min_calls_r-3) {
        uint8_t H_arr[4][NTRU_MAX_HASH_LEN];
        uint16_t inp_len = s->zlen + sizeof s->counter;

        uint8_t j;
        uint8_t hash_inp_arr[4][inp_len];
        uint8_t *hash_inp[4];
        for (j=0; j<4; j++) {
            memcpy(&hash_inp_arr[j], (uint8_t*)s->Z, s->zlen);
            uint16_t counter_endian = htole16(s->counter);
            memcpy((uint8_t*)&hash_inp_arr[j] + s->zlen, &counter_endian, sizeof s->counter);
            hash_inp[j] = hash_inp_arr[j];
            s->counter++;
        }
        uint8_t *H[4];
        for (j=0; j<4; j++)
            H[j] = H_arr[j];
        s->hash_4way(hash_inp, inp_len, H);
        for (j=0; j<4; j++)
            ntru_append(&s->buf, H[j], s->hlen);
    }
    while (s->counter < params->min_calls_r) {
        uint8_t H[NTRU_MAX_HASH_LEN];
        uint16_t inp_len = s->zlen + sizeof s->counter;
        uint8_t hash_inp[inp_len];
        memcpy(&hash_inp, (uint8_t*)s->Z, s->zlen);
        uint16_t counter_endian = htole16(s->counter);
        memcpy((uint8_t*)&hash_inp + s->zlen, &counter_endian, sizeof s->counter);
        s->hash((uint8_t*)&hash_inp, inp_len, (uint8_t*)&H);

        ntru_append(&s->buf, (uint8_t*)&H, s->hlen);
        s->counter++;
    }
#else
    uint8_t plaintext[32*params->min_calls_r];
    memset(plaintext, 0, 32*params->min_calls_r);

    AES_KEY enc_key;

    uint8_t key_inp[32];
    memset((uint8_t *)key_inp, 0, 32);

    if (s->zlen > 32)
        memcpy((uint8_t *)key_inp, s->Z, 32);
    else
        memcpy((uint8_t *)key_inp, s->Z, s->zlen);

    AES_set_encrypt_key((uint8_t *)&key_inp, 256, &enc_key);
    uint8_t H[32*params->min_calls_r];
    int i;
    for (i = 0; i < 32*params->min_calls_r; i++) {
        H[i] = 0;
    }


    while (s->counter < 2*params->min_calls_r) {
        memcpy((uint8_t *)plaintext+s->counter*16, &s->counter, sizeof s->counter);
        s->counter++;
    }

    if (params->min_calls_r > 0)
        AES_ECB_encrypt((uint8_t *)plaintext, (uint8_t *)H, 32*params->min_calls_r, enc_key.KEY, enc_key.nr);

    s->counter = 0;

    while (s->counter < params->min_calls_r) {
        ntru_append(&s->buf, (uint8_t*)&H+s->counter*32, s->hlen);
        s->counter++;
    }

#endif /* USE_AES_NI */

}

void ntru_IGF_next(NtruIGFState *s, uint16_t *i) {
    uint16_t N = s-> N;
    uint16_t c = s-> c;

#ifndef USE_AES_NI
    uint8_t H[NTRU_MAX_HASH_LEN];

    for (;;) {
        if (s->rem_len < c) {
            NtruBitStr M;
            ntru_trailing(&s->buf, s->rem_len, &M);
            uint16_t tmp_len = c - s->rem_len;
            uint16_t c_thresh = s->counter + (tmp_len+s->hlen-1) / s->hlen;
            while (s->counter < c_thresh) {
                uint16_t inp_len = s->zlen + sizeof s->counter;
                uint8_t hash_inp[inp_len];
                memcpy(&hash_inp, (uint8_t*)s->Z, s->zlen);
                uint16_t counter_endian = htole16(s->counter);
                memcpy((uint8_t*)&hash_inp + s->zlen, &counter_endian, sizeof s->counter);
                s->hash((uint8_t*)&hash_inp, inp_len, (uint8_t*)&H);

                ntru_append(&M, (uint8_t*)&H, s->hlen);
                s->counter++;
                s->rem_len += 8 * s->hlen;
            }
            s->buf = M;
        }

        *i = ntru_leading(&s->buf, c);   /* assume c<32 */
        ntru_truncate(&s->buf, c);
        s->rem_len -= c;
        if (*i < s->rnd_thresh) {   /* if (*i < (1<<c)-(1<<c)%N) */
            while (*i >= N)
                *i -= N;
            return;
        }
    }

#else

    for (;;) {
        if (s->rem_len < c) {
            NtruBitStr M;
            ntru_trailing(&s->buf, s->rem_len, &M);
            uint16_t tmp_len = c - s->rem_len;
            uint16_t c_thresh = s->counter + (tmp_len+s->hlen-1) / s->hlen;


            uint8_t H[32*c_thresh];

            unsigned char plaintext[32*c_thresh];
            memset(plaintext, 0, 32*c_thresh);

            AES_KEY enc_key;

            uint8_t key_inp[32];
            memset(key_inp, 0, 32);

            if (s->zlen > 32)
                memcpy(&key_inp, s->Z, 32);
            else
                memcpy(&key_inp, s->Z, s->zlen);

            AES_set_encrypt_key((uint8_t *)key_inp, 256, &enc_key);


            uint16_t tmp_cnt = s->counter;
            while (s->counter < 2*c_thresh) {
                memcpy(plaintext+s->counter*16, &s->counter, sizeof s->counter);
                s->counter++;

            }
            AES_ECB_encrypt(plaintext, H, 32*c_thresh, enc_key.KEY, enc_key.nr);

            s->counter = tmp_cnt;


            while (s->counter < c_thresh) {
                ntru_append(&M, (uint8_t*)&H+s->counter*32, s->hlen);

                s->counter++;
                s->rem_len += 8 * s->hlen;
            }
            s->buf = M;
        }

        *i = ntru_leading(&s->buf, c);   /* assume c<32 */
        ntru_truncate(&s->buf, c);
        s->rem_len -= c;

        if (*i < s->rnd_thresh) {   /* if (*i < (1<<c)-(1<<c)%N) */
            while (*i >= N)
                *i -= N;
            return;
        }
    }
#endif /* USE_AES_NI */

}
