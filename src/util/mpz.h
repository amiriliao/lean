/*
Copyright (c) 2013 Microsoft Corporation. All rights reserved. 
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura 
*/
#pragma once
#include <iostream>
#include <gmp.h>
#include "num_macros.h"
#include "debug.h"

namespace lean {
class mpq;

// Wrapper for GMP integers
class mpz {
    friend class mpq;
    mpz_t m_val;
    mpz(__mpz_struct const * v) { mpz_init_set(m_val, v); }
public:
    mpz() { mpz_init(m_val); }
    mpz(char const * v) { mpz_init_set_str(m_val, const_cast<char*>(v), 10); } 
    mpz(unsigned long int v) { mpz_init_set_ui(m_val, v); }
    mpz(long int v) { mpz_init_set_si(m_val, v); }
    mpz(unsigned int v) { mpz_init_set_ui(m_val, v); }
    mpz(int v) { mpz_init_set_si(m_val, v); }
    mpz(mpz const & s) { mpz_init_set(m_val, s.m_val); }
    mpz(mpz && s):mpz() { mpz_swap(m_val, s.m_val); }
    ~mpz() { mpz_clear(m_val); }
    
    int sgn() const { return mpz_sgn(m_val); }
    friend int sgn(mpz const & a) { return a.sgn(); }
    DEFINE_SIGN_METHODS()
    
    void neg() { mpz_neg(m_val, m_val); }
    void abs() { mpz_abs(m_val, m_val); }
    friend mpz abs(mpz a) { a.abs(); return a; }
    friend mpz neg(mpz a) { a.neg(); return a; }

    bool even() const { return mpz_even_p(m_val) != 0; }
    bool odd() const { return !even(); }

    void swap(mpz & o) { mpz_swap(m_val, o.m_val); }

    unsigned hash() const { return static_cast<unsigned>(mpz_get_si(m_val)); }

    bool is_int() const { return mpz_fits_sint_p(m_val) != 0; }
    bool is_unsigned_int() const { return mpz_fits_uint_p(m_val) != 0; }
    bool is_long_int() const { return mpz_fits_slong_p(m_val) != 0; }
    bool is_unsigned_long_int() const { return mpz_fits_ulong_p(m_val) != 0; }

    long int get_long_int() const { lean_assert(is_long_int()); return mpz_get_si(m_val); }
    int get_int() const { lean_assert(is_int()); return static_cast<int>(get_long_int()); }
    unsigned long int get_unsigned_long_int() const { lean_assert(is_unsigned_long_int()); return mpz_get_ui(m_val); }
    unsigned int get_unsigned_int() const { lean_assert(is_unsigned_int()); return static_cast<unsigned>(get_unsigned_long_int()); }

    friend mpz abs(mpz const & a) { mpz r; mpz_abs(r.m_val, a.m_val); return r; }

    friend int cmp(mpz const & a, mpz const & b) { return mpz_cmp(a.m_val, b.m_val); }
    friend int cmp(mpz const & a, unsigned b) { return mpz_cmp_ui(a.m_val, b); }
    friend int cmp(mpz const & a, int b) { return mpz_cmp_si(a.m_val, b); }
    DEFINE_ORDER_OPS(mpz)
    DEFINE_EQ_OPS(mpz)

    mpz & operator+=(mpz const & o) { mpz_add(m_val, m_val, o.m_val); return *this; }
    mpz & operator+=(unsigned u) { mpz_add_ui(m_val, m_val, u); return *this; }
    mpz & operator+=(int u) { if (u >= 0) mpz_add_ui(m_val, m_val, u); else mpz_sub_ui(m_val, m_val, u); return *this; }
    mpz & operator-=(mpz const & o) { mpz_sub(m_val, m_val, o.m_val); return *this; }
    mpz & operator-=(unsigned u) { mpz_sub_ui(m_val, m_val, u); return *this; }
    mpz & operator-=(int u) { if (u >= 0) mpz_sub_ui(m_val, m_val, u); else mpz_add_ui(m_val, m_val, u); return *this; }
    mpz & operator*=(mpz const & o) { mpz_mul(m_val, m_val, o.m_val); return *this; }
    mpz & operator*=(unsigned u) { mpz_mul_ui(m_val, m_val, u); return *this; }
    mpz & operator*=(int u) { mpz_mul_si(m_val, m_val, u); return *this; }
    mpz & operator/=(mpz const & o) { mpz_tdiv_q(m_val, m_val, o.m_val); return *this; }
    mpz & operator/=(unsigned u) { mpz_tdiv_q_ui(m_val, m_val, u); return *this; }
    friend mpz rem(mpz const & a, mpz const & b) { mpz r; mpz_tdiv_r(r.m_val, a.m_val, b.m_val); return r; }
    mpz & operator%=(mpz const & o) { mpz r(*this % o); mpz_swap(m_val, r.m_val); return *this; }
    DEFINE_ARITH_OPS(mpz)
    friend mpz operator%(mpz const & a, mpz const & b);

    mpz & operator&=(mpz const & o) { mpz_and(m_val, m_val, o.m_val); return *this; }
    mpz & operator|=(mpz const & o) { mpz_ior(m_val, m_val, o.m_val); return *this; }
    mpz & operator^=(mpz const & o) { mpz_xor(m_val, m_val, o.m_val); return *this; }
    void comp() { mpz_com(m_val, m_val); }

    // this <- this + a*b
    void addmul(mpz const & a, mpz const & b) { mpz_addmul(m_val, a.m_val, b.m_val); }
    // this <- this - a*b
    void submul(mpz const & a, mpz const & b) { mpz_submul(m_val, a.m_val, b.m_val); }

    // this <- this * 2^k
    void mul2k(unsigned k) { mpz_mul_2exp(m_val, m_val, k); }
    // this <- this / 2^k
    void div2k(unsigned k) { mpz_tdiv_q_2exp(m_val, m_val, k); }
    
    /**
       \brief Return the position of the most significant bit.
       Return 0 if the number is negative
    */
    unsigned log2() const;

    /**
       \brief log2(-n)
       Return 0 if the number is nonegative
    */
    unsigned mlog2() const;

    bool perfect_square() const { return mpz_perfect_square_p(m_val); }

    bool is_power_of_two() const { return is_pos() && mpz_popcount(m_val) == 1; }
    bool is_power_of_two(unsigned & shift) const;

    friend mpz power(mpz const & a, unsigned k) { mpz r; mpz_pow_ui(r.m_val, a.m_val, k); return r; }

    friend void rootrem(mpz & root, mpz & rem, mpz const & a, unsigned k) { mpz_rootrem(root.m_val, rem.m_val, a.m_val, k); }
    friend void root(mpz & root, mpz const & a, unsigned k) { mpz_root(root.m_val, a.m_val, k); }
    friend mpz root(mpz const & a, unsigned k) { mpz r; root(r, a, k); return r; }

    friend mpz operator&(mpz a, mpz const & b) { return a &= b; }
    friend mpz operator|(mpz a, mpz const & b) { return a |= b; }
    friend mpz operator^(mpz a, mpz const & b) { return a ^= b; }
    friend mpz operator~(mpz a) { a.comp(); return a; }

    friend void gcd(mpz & g, mpz const & a, mpz const & b) { mpz_gcd(g.m_val, a.m_val, b.m_val); }
    friend mpz gcd(mpz const & a, mpz const & b) { mpz r; gcd(r, a, b); return r; }
    friend void gcdext(mpz & g, mpz & s, mpz & t, mpz const & a, mpz const & b) { mpz_gcdext(g.m_val, s.m_val, t.m_val, a.m_val, b.m_val); }
    friend void lcm(mpz & l, mpz const & a, mpz const & b) { mpz_lcm(l.m_val, a.m_val, b.m_val); }
    friend mpz lcm(mpz const & a, mpz const & b) { mpz l; lcm(l, a, b); return l; }

    friend std::ostream & operator<<(std::ostream & out, mpz const & v); 
};

}
