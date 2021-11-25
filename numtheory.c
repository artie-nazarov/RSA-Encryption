#include "numtheory.h"
#include "randstate.h"

void gcd(mpz_t g, mpz_t a, mpz_t b) {
    mpz_t t;
    mpz_t atemp;
    mpz_t btemp;
    mpz_inits(t, atemp, btemp, NULL);
    mpz_set(atemp, a);
    mpz_set(btemp, b);
    while (mpz_cmp_ui(btemp, 0) != 0) {
        mpz_set(t, btemp);
        mpz_mod(btemp, atemp, btemp);
        mpz_set(atemp, t);
    }
    mpz_set(g, atemp);
    mpz_clears(t, atemp, btemp, NULL);
}

void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
    mpz_t r;
    mpz_t r_p;
    mpz_init_set(r, n);
    mpz_init_set(r_p, a);
    mpz_t t;
    mpz_t t_p;
    mpz_init_set_ui(t, 0);
    mpz_init_set_ui(t_p, 1);
    mpz_t temp1;
    mpz_t temp2;
    mpz_t q;
    mpz_inits(temp1, temp2, q, NULL);
    while (mpz_cmp_ui(r_p, 0) != 0) {
        mpz_fdiv_q(q, r, r_p);

        mpz_set(temp1, r);
        mpz_mul(temp2, q, r_p);
        mpz_sub(temp1, temp1, temp2);
        mpz_set(r, r_p);
        mpz_set(r_p, temp1);

        mpz_set(temp1, t);
        mpz_mul(temp2, q, t_p);
        mpz_sub(temp1, temp1, temp2);
        mpz_set(t, t_p);
        mpz_set(t_p, temp1);
    }
    if (mpz_cmp_ui(r, 1) > 0) {
        // No inverse
        mpz_set_ui(o, 0);
        mpz_clears(r, r_p, t, t_p, temp1, temp2, q, NULL);
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n);
    }
    mpz_set(o, t);
    mpz_clears(r, r_p, t, t_p, temp1, temp2, q, NULL);
}

// Modular exponentiation
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
    // initialize mpz values
    mpz_t res;
    mpz_t acc;
    mpz_t ifres;
    mpz_t dtemp;
    mpz_inits(res, acc, ifres, dtemp, NULL);
    mpz_set_ui(res, 1);
    mpz_set(acc, a);
    mpz_set(dtemp, d);

    while (mpz_cmp_ui(dtemp, 0) > 0) {
        mpz_mod_ui(ifres, dtemp, 2);
        if (mpz_cmp_ui(ifres, 1) == 0) {
            mpz_mul(res, res, acc);
            mpz_mod(res, res, n);
        }
        mpz_mul(acc, acc, acc);
        mpz_mod(acc, acc, n);
        mpz_fdiv_q_ui(dtemp, dtemp, 2);
    }
    mpz_set(o, res);
    mpz_clears(res, acc, ifres, dtemp, NULL);
}

bool is_prime(mpz_t n, uint64_t iters) {
    if (mpz_cmp_ui(n, 2) < 0) {
        return false;
    }
    if (mpz_cmp_ui(n, 4) < 0) {
        return true;
    }
    mpz_t res1;
    mpz_t res2;
    mpz_t a;
    mpz_t y;
    mpz_inits(res1, res2, a, y, NULL);
    // Return false if n is divisible by 2
    mpz_mod_ui(res1, n, 2);
    if (mpz_cmp_ui(res1, 0) == 0) {
        mpz_clears(res1, res2, a, y, NULL);
        return false;
    }
    mpz_t two;
    mpz_init_set_ui(two, 2);
    // Compute n-1
    mpz_t n_minus1;
    mpz_t pw2;
    mpz_init_set(n_minus1, n);
    mpz_init_set_ui(pw2, 2);
    mpz_sub_ui(n_minus1, n_minus1, 1);
    uint64_t s = 1;
    mpz_t r;
    mpz_init_set(r, n_minus1);
    mpz_fdiv_q(r, r, pw2);
    // Solve for s and r, such that r is odd
    // n-1 = 2^(s) * r
    while (mpz_cmp(n_minus1, pw2) > 0) {
        mpz_mod(res1, n_minus1, pw2);
        mpz_mod_ui(res2, r, 2);
        if (mpz_cmp_ui(res1, 0) == 0 && mpz_cmp_ui(res2, 1) == 0) {
            break;
        }
        s += 1;
        mpz_mul_ui(pw2, pw2, 2);
        mpz_fdiv_q(r, n_minus1, pw2);
    }

    uint64_t j = 1;
    for (uint64_t i = 1; i <= iters; i++) {
        // Generate a random number
        mpz_sub_ui(res1, n, 3); // res1 = n-3
        mpz_urandomm(a, state, res1); // a in {0, n-4}
        mpz_add_ui(a, a, 2); // a in {2, n-2}
        pow_mod(y, a, r, n);
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n_minus1) != 0) {
            j = 1;
            while (j <= s - 1 && mpz_cmp(y, n_minus1) != 0) {
                pow_mod(y, y, two, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(n_minus1, r, res1, res2, a, y, two, pw2, NULL);
                    return false;
                }
                j += 1;
            }
            if (mpz_cmp(y, n_minus1) != 0) {

                mpz_clears(n_minus1, r, res1, res2, a, y, two, pw2, NULL);
                return false;
            }
        }
    }

    mpz_clears(n_minus1, r, res1, res2, a, y, two, pw2, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    // Generate a number of size <bits> bits
    mpz_t num;
    mpz_init_set_ui(num, 2);
    mpz_pow_ui(num, num, bits - 1);
    // Find a prime number
    do {
        mpz_urandomb(p, state, bits);
        if (mpz_cmp(num, p) > 0) {
            mpz_add(p, p, num);
        }
    } while (!is_prime(p, iters));

    mpz_clear(num);
    return;
}
/*
int main(void) {
    randstate_init(56456);
    mpz_t a; mpz_t b; mpz_t c; mpz_t d; mpz_t e; 
    mpz_inits(a, b, c, d, e, NULL);
    mpz_set_ui(a, 280001); mpz_set_ui(b, 123); mpz_set_ui(c, 11); mpz_set_ui(d, 45);
     
    is_prime(a, 100) ? puts("prime") : puts("composite");


    mpz_clears(a, b, c, d, e, NULL);
    randstate_clear();
    return 0;
}*/
