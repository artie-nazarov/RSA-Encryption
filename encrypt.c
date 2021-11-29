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

#define OPTIONS "i:o:n:vh"

// Print help message
void print_help(void) {
    puts("SYNOPSIS\n"
         "   Encrypts data using RSA encryption.\n"
         "   Encrypted data is decrypted by the decrypt program.\n\n"
         "USAGE\n"
         "   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey\n\n"
         "OPTIONS\n"
         "   -h              Display program help and usage.\n"
         "   -v              Display verbose program output.\n"
         "   -i infile       Input file of data to encrypt (default: stdin).\n"
         "   -o outfile      Output file for encrypted data (default: stdout).\n"
         "   -n pbfile       Public key file (default: rsa.pub).");
}

// RSA Encryption program
int main(int argc, char **argv) {
    int opt = 0;
    FILE *infile = stdin;
    FILE *outfile = stdout;
    char *key_file_name = "rsa.pub";
    bool verbose = false;

    // Parse command-line arguments
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            infile = fopen(optarg, "r");
            if (!infile) {
                fprintf(stderr, "Error: failed to open infile.\n");
                print_help();
                exit(1);
            }
            break;
        case 'o':
            outfile = fopen(optarg, "w+");
            if (!outfile) {
                fprintf(stderr, "Error: failed to open outfile.\n");
                print_help();
                exit(1);
            }
            break;
        case 'n': key_file_name = optarg; break;
        case 'v': verbose = true; break;
        case 'h': print_help(); break;
        default: break;
        }
    }

    // Open public key file
    FILE *pbfile;
    pbfile = fopen(key_file_name, "r");
    if (!pbfile) {
        fprintf(stderr, "Error: failed to open pbfile.\n");
        print_help();
        exit(1);
    }

    mpz_t n;
    mpz_t e;
    mpz_t s;
    mpz_t u;
    mpz_inits(n, e, s, u, NULL);
    char username[1024];

    // Read RSA public key
    rsa_read_pub(n, e, s, username, pbfile);

    // Verbose output
    if (verbose) {
        printf("username = %s\n", username);
        gmp_printf("signature s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("public modulus n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("public exponent e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    // Verify signature
    mpz_set_str(u, username, 62);
    if (!rsa_verify(u, s, e, n)) {
        fprintf(stderr, "Error: signature could not be verified.\n");
        exit(1);
    }

    // Encrypt file
    rsa_encrypt_file(infile, outfile, n, e);

    mpz_clears(n, e, s, u, NULL);
    fclose(pbfile);
    fclose(infile);
    fclose(outfile);
    return 0;
}
