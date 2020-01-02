/*
 * =====================================================================================
 *
 *       Filename:  intersection.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/01/2019 07:29:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef  intersection_INC
#define  intersection_INC
#include <cmath>
typedef double real_t;
int comp_seg_intersection(const real_t s1sx, const real_t s1sy, const real_t s1ex, const real_t s1ey,
                          const real_t s2sx, const real_t s2sy, const real_t s2ex, const real_t s2ey,
                          real_t& x, real_t&y);

inline int quick_pow10(int n) {
    static int pow10[9] = {1,10,100,1000,10000,100000,1000000,10000000,100000000};
    return pow10[n];
}
inline real_t set_precision(real_t val,int prec=5) {
    real_t p = quick_pow10(prec);
    real_t v = std::round(val*p);
    return v/p;
}
#endif   /* ----- #ifndef intersection_INC  ----- */
