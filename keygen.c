#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"

#define OPTIONS "b:i:n:d:s:vh"

// Print help message
void print_help(void) {
    puts("SYNOPSIS\n"
         "   Generates an RSA public/private key pair.\n\n"
         "USAGE\n"
         "   ./keygen [-hv] [-b bits] -n pbfile -d pvfile\n\n"
         "OPTIONS\n"
         "   -h              Display program help and usage.\n"
         "   -v              Display verbose program output.\n"
         "   -b bits         Minimum bits needed for public key n.\n"
         "   -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n"
         "   -n pbfile       Public key file (default: rsa.pub).\n"
         "   -d pvfile       Private key file (default: rsa.priv).\n"
         "   -s seed         Random seed for testing.");
}

// RSA Key generator program
int main(int argc, char **argv) {
    int opt = 0;
    uint64_t nbits = 256; // minimum number of bits for a public key
    uint64_t mr_iters = 50; // number of iterations of Miller-Rabin algorithm
    char *pbfile_name = "rsa.pub"; // name of the rsa public key file
    char *pvfile_name = "rsa.priv"; // name of the rsa private key file
    uint64_t random_seed = time(NULL);
    bool verbose = false;

    // Parse command-line arguments
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b':
            nbits = strtoul(optarg, NULL, 10);
            if (errno == EINVAL) {
                puts("Invalid input. Using 256 bits");
                nbits = 256;
            }
            break;
        case 'i':
            mr_iters = strtoul(optarg, NULL, 10);
            if (errno == EINVAL) {
                puts("Invalid input. Using 50 Miller-Rabin iterations");
                mr_iters = 50;
            }
            break;
        case 'n': pbfile_name = optarg; break;
        case 'd': pvfile_name = optarg; break;
        case 's':
            random_seed = strtoul(optarg, NULL, 10);
            if (errno == EINVAL) {
                random_seed = time(NULL);
                printf("Invalid input. Using random seed %lu", random_seed);
            }
            break;
        case 'v': verbose = true; break;
        case 'h': print_help(); break;
        default: break;
        }
    }

    // Open Private and Public key files
    FILE *pbfile;
    FILE *pvfile;
    pbfile = fopen(pbfile_name, "w+");
    if (!pbfile) {
        fprintf(stderr, "Error: failed to open pbfile.\n");
        print_help();
        exit(1);
    }
    pvfile = fopen(pvfile_name, "w+");
    if (!pvfile) {
        fprintf(stderr, "Error: failed to open pvfile.\n");
        print_help();
        exit(1);
    }
    // Set private key file permissions (read/write for the user only)
    fchmod(fileno(pvfile), 0600);

    randstate_init(random_seed);
    mpz_t p;
    mpz_t q;
    mpz_t n;
    mpz_t e;
    mpz_t d;
    mpz_t s;
    mpz_t u;
    mpz_inits(p, q, n, e, d, s, u, NULL);

    // Create a public key
    rsa_make_pub(p, q, n, e, nbits, mr_iters);
    // Create a private key
    rsa_make_priv(d, e, p, q);

    char *username = getenv("USER");
    mpz_set_str(u, username, 62);
    rsa_sign(s, u, d, n);

    // Write public and private keys
    rsa_write_pub(n, e, s, username, pbfile);
    rsa_write_priv(n, d, pvfile);

    // Verbose output
    if (verbose) {
        printf("username = %s\n", username);
        gmp_printf("signature s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("public modulus n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("public exponent e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("private key d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    randstate_clear();
    mpz_clears(p, q, n, e, d, s, u, NULL);
    fclose(pbfile);
    fclose(pvfile);
    return 0;
}
