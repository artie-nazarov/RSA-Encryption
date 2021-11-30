RSA Encryption
## Description
The following program implements the **RSA Encryption** algorithm.
<br /> RSA Encryption is a widely used public-key cryptosystem for secure data transmission.

## Files
* decrypt.c: Implementation of the decrypt program.
* encrypt.c: Implementation of the encrypt program.
* keygen.c: Implementation of the keygen program.
* numtheory.c: Implementation of the number theory module.
* numtheory.h: Interface for the number theory module.
* randstate.c: Implementation of the random state interface.
* randstate.h: Interface for initializing and clearing the random state.
* rsa.c: Implementation of the RSA library.
* rsa.h: Interface for the RSA library.

## Getting started
Execute the following on a **UNIX** machine
### Install dependencies:
`sudo apt install pkg-config libgmp-dev`
### Run programs:
#### Keygen
**Usage:**
`./keygen [-hv] [-b bits] -n pbfile -d pvfile`
<br />**Options:**
<br /> **-h**  Display program help and usage.
<br /> **-v**  Display verbose program output.
<br /> **-b bits**  Minimum bits needed for public key n.
<br /> **-i iterations**  Miller-Rabin iterations for testing primes (default: 50).
<br /> **-n pbfile**  Public key file (default: rsa.pub).
<br /> **-d pvfile**  Private key file (default: rsa.priv).
<br /> **-s seed**  Random seed for testing.
#### Encrypt
**Usage:**
`./encrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey`
<br />**Options:**
<br /> **-h**  Display program help and usage.
<br /> **-v**  Display verbose program output.
<br /> **-i infile**  Input file of data to encrypt (default: stdin).
<br /> **-o outfile**  Output file for encrypted data (default: stdout).
<br /> **-n pbfile**  Public key file (default: rsa.pub).
#### Decrypt
**Usage:**
`./decrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey`
<br />**Options:**
<br /> **-h**  Display program help and usage.
<br /> **-v**  Display verbose program output.
<br /> **-i infile**  Input file of data to decrypt (default: stdin).
<br /> **-o outfile**  Output file for decrypted data (default: stdout).
<br /> **-d pvfile**  Private key file (default: rsa.priv).

## Number Theory Module
The following module implements functions used in the RSA library
#### Modular exponentiation
Uses the **Right-to-Left binary method** for computing modular exponentiation.
#### Primality testing
A probabilistic algorithm for determining primality of a number was implemented in this module.
<br />The **Miller–Rabin primality test** was selected as an algorithm of choice.
#### Modular Inverses
Uses **Eucledian algorithm** and **Bezouts identity** for computing the gcd of 2 numbers and multiplicative inverses.
> ax + by = gcd(a,b)
