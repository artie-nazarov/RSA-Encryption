#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// Generate RSA public key
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    // p is p_bits long (in bits), where p_bits in [nbits/4, (3*nbits)/4)
    // q is q_bits long (in bits), where q_bits = nbits - pbits
    // adding 1 to and q_bits ensures log(n) >= nbits
    srand(time(NULL));
    uint64_t p_bits = random() % ((2 * nbits) / 4) + nbits / 4;
    uint64_t q_bits = (nbits + 1) - p_bits;

    mpz_t totient_n;
    mpz_t res1;
    mpz_t res2;
    mpz_inits(totient_n, res1, res2, NULL);
    make_prime(p, p_bits, iters);
    make_prime(q, q_bits, iters);
    mpz_mul(n, p, q);

    // Compute totient of n
    mpz_set(res1, p);
    mpz_set(res2, q);
    mpz_sub_ui(res1, res1, 1);
    mpz_sub_ui(res2, res2, 1);
    mpz_mul(totient_n, res1, res2);

    // Compute public exponent e
    mpz_set_ui(res1, 2);
    mpz_pow_ui(res1, res1, nbits - 1);
    do {
        mpz_urandomb(e, state, nbits);
        // If e is less than nbits long
        if (mpz_cmp(res1, e) > 0) {
            mpz_add(e, e, res1);
        }
        gcd(res2, e, totient_n);
    } while (mpz_cmp_ui(res2, 1) != 0);
    mpz_clears(totient_n, res1, res2, NULL);
}

// Write public key to file
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    // Write n
    gmp_fprintf(pbfile, "%Zx\n", n);
    // Write e
    gmp_fprintf(pbfile, "%Zx\n", e);
    // Write s
    gmp_fprintf(pbfile, "%Zx\n", s);
    // Write username
    gmp_fprintf(pbfile, "%s\n", username);
}

// Read public key from a file
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    // Read values n, e, s, username from pbfile
    gmp_fscanf(pbfile,
        "%Zx\n"
        "%Zx\n"
        "%Zx\n"
        "%s\n",
        n, e, s, username);
}

// Generate RSA private key
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t totient_n;
    mpz_t res1;
    mpz_t res2;
    mpz_inits(totient_n, res1, res2, NULL);

    // Compute totient of n
    mpz_set(res1, p);
    mpz_set(res2, q);
    mpz_sub_ui(res1, res1, 1);
    mpz_sub_ui(res2, res2, 1);
    mpz_mul(totient_n, res1, res2);

    // Compute Modular inverse
    mod_inverse(d, e, totient_n);

    mpz_clears(totient_n, res1, res2, NULL);
}

// Write private key to a file
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    // Write n
    gmp_fprintf(pvfile, "%Zx\n", n);
    // Write d
    gmp_fprintf(pvfile, "%Zx\n", d);
}

// Read private key from a file
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    // Read values n, d from pvfile
    gmp_fscanf(pvfile,
        "%Zx\n"
        "%Zx\n",
        n, d);
}

// RSA Encryption
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    // Compute cyphertext c
    // E(m) = c = m^e (mod n)
    pow_mod(c, m, e, n);
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    // Compute block size k
    uint64_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    uint8_t *block = calloc(k, sizeof(uint8_t));
    block[0] = 0xFF;
    // While more unprocessed bytes to read
    uint64_t j = 0;
    mpz_t m;
    mpz_t c;
    mpz_inits(m, c, NULL);
    do {
        j = fread(block + 1, sizeof(uint8_t), k - 1, infile);
        if (j > 0) {
            mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block);
            rsa_encrypt(c, m, e, n);
            gmp_fprintf(outfile, "%Zx\n", c);
        }
    } while (j > 0);

    mpz_clears(m, c, NULL);
    free(block);
}

// RSA Decryption
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    // Compute message m from cyphertext c
    pow_mod(m, c, d, n);
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    // Compute block size k
    uint64_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    uint8_t *block = calloc(k, sizeof(uint8_t));
    // While more unprocessed bytes to read
    uint64_t j = 0;
    mpz_t m;
    mpz_t c;
    mpz_inits(m, c, NULL);
    while (gmp_fscanf(infile, "%Zx\n", c) != EOF) {
        rsa_decrypt(m, c, d, n);
        mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(block + 1, sizeof(uint8_t), j - 1, outfile);
    }
    mpz_clears(m, c, NULL);
    free(block);
}

// Create a signature
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
}

// Verify a signature
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n);
    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    }
    mpz_clear(t);
    return false;
}
