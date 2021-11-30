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
         "   Decrypts data using RSA decryption.\n"
         "   Encrypted data is encrypted by the encrypt program.\n\n"
         "USAGE\n"
         "   ./decrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey\n\n"
         "OPTIONS\n"
         "   -h              Display program help and usage.\n"
         "   -v              Display verbose program output.\n"
         "   -i infile       Input file of data to decrypt (default: stdin).\n"
         "   -o outfile      Output file for decrypted data (default: stdout).\n"
         "   -d pvfile       Private key file (default: rsa.priv).");
}

// RSA Decryption program
int main(int argc, char **argv) {
    int opt = 0;
    FILE *infile = stdin;
    FILE *outfile = stdout;
    char *key_file_name = "rsa.priv";
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

    // Open private key file
    FILE *pvfile;
    pvfile = fopen(key_file_name, "r");
    if (!pvfile) {
        fprintf(stderr, "Error: failed to open pvfile.\n");
        print_help();
        exit(1);
    }

    mpz_t n;
    mpz_t d;
    mpz_inits(n, d, NULL);
    mpz_set_ui(n, 0);
    // Read private key
    rsa_read_priv(n, d, pvfile);

    // Verbose output
    if (verbose) {
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    // Decrypt file
    rsa_decrypt_file(infile, outfile, n, d);

    mpz_clears(n, d, NULL);
    fclose(pvfile);
    fclose(infile);
    fclose(outfile);
    return 0;
}
