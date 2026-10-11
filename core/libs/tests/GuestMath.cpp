#include "prx/libc/include/general/VabiMacros.hpp"
#include <bit>
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <initializer_list>
#include <utility>
extern "C" {
double APS5_VABI atof_nid_postfix(const char*);
float APS5_VABI strtof_nid_postfix(const char*, char**);
long double APS5_VABI strtold_nid_postfix(const char*, char**);
std::int64_t APS5_VABI strtol_nid_postfix(const char*, char**, int);
std::uint64_t APS5_VABI strtoul_nid_postfix(const char*, char**, int);
std::intmax_t APS5_VABI strtoimax_nid_postfix(const char*, char**, int);
long long APS5_VABI strtoll_nid_postfix(const char*, char**, int);
unsigned long long APS5_VABI strtoull_nid_postfix(const char*, char**, int);
std::uintmax_t APS5_VABI strtoumax_nid_postfix(const char*, char**, int);
unsigned long long APS5_VABI _Stoull_nid_postfix(const char*, char**, int);
std::uint64_t APS5_VABI _Stoul_nid_postfix(const char*, char**, int);
std::int64_t APS5_VABI atol_nid_postfix(const char*);
long long APS5_VABI atoll_nid_postfix(const char*);
int* APS5_VABI __error_nid_postfix();
double APS5_VABI sqrt_nid_postfix(double);
float APS5_VABI sqrtf_nid_postfix(float);
double APS5_VABI fabs_nid_postfix(double);
int APS5_VABI abs_nid_postfix(int);
double APS5_VABI trunc_nid_postfix(double);
double APS5_VABI fma_nid_postfix(double, double, double);
long double APS5_VABI acosl_nid_postfix(long double);
long double APS5_VABI atanl_nid_postfix(long double);
long double APS5_VABI tanl_nid_postfix(long double);
long double APS5_VABI exp2l_nid_postfix(long double);
long double APS5_VABI expl_nid_postfix(long double);
long double APS5_VABI coshl_nid_postfix(long double);
long double APS5_VABI sinhl_nid_postfix(long double);
long double APS5_VABI tanhl_nid_postfix(long double);
long double APS5_VABI log10l_nid_postfix(long double);
long double APS5_VABI frexpl_nid_postfix(long double, int*);
struct LibcFloatConstant { std::uint32_t bits[4]; };
extern LibcFloatConstant _FInf_nid_postfix;
extern LibcFloatConstant _FNan_nid_postfix;
short APS5_VABI _FDtest_nid_postfix(const float*);
int APS5_VABI __fpclassifyf_nid_postfix(float);
float APS5_VABI fmodf_nid_postfix(float, float);
float APS5_VABI asinf_nid_postfix(float);
float APS5_VABI acosf_nid_postfix(float);
float APS5_VABI atan2f_nid_postfix(float, float);
float APS5_VABI hypotf_nid_postfix(float, float);
double APS5_VABI hypot_nid_postfix(double, double);
float APS5_VABI tanf_nid_postfix(float);
int APS5_VABI rand_nid_postfix();
void APS5_VABI srand_nid_postfix(unsigned int);
int APS5_VABI rand_r_nid_postfix(unsigned int*);
float APS5_VABI log10f_nid_postfix(float);
double APS5_VABI log1p_nid_postfix(double);
float APS5_VABI log1pf_nid_postfix(float);
double APS5_VABI expm1_nid_postfix(double);
float APS5_VABI logbf_nid_postfix(float);
double APS5_VABI exp2_nid_postfix(double);
double APS5_VABI ldexp_nid_postfix(double, int);
long double APS5_VABI ldexpl_nid_postfix(long double, int);
double APS5_VABI logb_nid_postfix(double);
double APS5_VABI remainder_nid_postfix(double, double);
double APS5_VABI scalbn_nid_postfix(double, int);
float APS5_VABI scalbnf_nid_postfix(float, int);
float APS5_VABI nextafterf_nid_postfix(float, float);
double APS5_VABI frexp_nid_postfix(double, int*);
float APS5_VABI frexpf_nid_postfix(float, int*);
std::int64_t APS5_VABI lround_nid_postfix(double);
std::div_t APS5_VABI div_nid_postfix(int, int);
double APS5_VABI sinh_nid_postfix(double);
double APS5_VABI cosh_nid_postfix(double);
float APS5_VABI coshf_nid_postfix(float);
double APS5_VABI asinh_nid_postfix(double);
float APS5_VABI asinhf_nid_postfix(float);
double APS5_VABI acosh_nid_postfix(double);
float APS5_VABI acoshf_nid_postfix(float);
double APS5_VABI atanh_nid_postfix(double);
std::int64_t APS5_VABI lroundf_nid_postfix(float);
std::int64_t APS5_VABI llround_nid_postfix(double);
std::int64_t APS5_VABI llroundf_nid_postfix(float);
int APS5_VABI __isfinitef_nid_postfix(float);
int APS5_VABI __isnormal_nid_postfix(double);
int APS5_VABI __isnormalf_nid_postfix(float);
int APS5_VABI __isinff_nid_postfix(float);
int APS5_VABI __isinf_nid_postfix(double);
std::lldiv_t APS5_VABI lldiv_nid_postfix(long long, long long);
std::lldiv_t APS5_VABI ldiv_nid_postfix(std::int64_t, std::int64_t);
float APS5_VABI nanf_nid_postfix(const char*);
double APS5_VABI nan_nid_postfix(const char*);
}
static void Require(bool value) { if (!value) std::abort(); }

static void CheckSquareRoot() {
    Require(sqrt_nid_postfix(4.) == 2. && sqrtf_nid_postfix(6.25f) == 2.5f);
    Require(std::bit_cast<std::uint64_t>(sqrt_nid_postfix(2.)) == 0x3ff6a09e667f3bcdull);
    Require(std::bit_cast<std::uint32_t>(sqrtf_nid_postfix(2.f)) == 0x3fb504f3u);
    Require(std::bit_cast<std::uint64_t>(sqrt_nid_postfix(0x1p-1074)) == 0x1e60000000000000ull);
    Require(std::bit_cast<std::uint64_t>(sqrt_nid_postfix(-0.)) == 0x8000000000000000ull);
    Require(std::bit_cast<std::uint32_t>(sqrtf_nid_postfix(-0.f)) == 0x80000000u);
    Require(sqrt_nid_postfix(INFINITY) == INFINITY && sqrtf_nid_postfix(INFINITY) == INFINITY);
    Require(std::isnan(sqrt_nid_postfix(-1.)) && std::isnan(sqrtf_nid_postfix(-1.f)));
}

static void CheckExactOperations() {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    Require(fabs_nid_postfix(-3.5) == 3.5 && fabs_nid_postfix(-INFINITY) == INFINITY);
    Require(std::bit_cast<std::uint64_t>(fabs_nid_postfix(-0.)) == 0);
    Require(std::isnan(fabs_nid_postfix(-nan)) && !std::signbit(fabs_nid_postfix(-nan)));
    Require(trunc_nid_postfix(2.7) == 2. && trunc_nid_postfix(-2.7) == -2.);
    Require(std::bit_cast<std::uint64_t>(trunc_nid_postfix(-0.5)) == 0x8000000000000000ull);
    Require(trunc_nid_postfix(4503599627370497.) == 4503599627370497. && trunc_nid_postfix(-1e300) == -1e300);
    Require(trunc_nid_postfix(-INFINITY) == -INFINITY && std::isnan(trunc_nid_postfix(nan)));
    Require(fma_nid_postfix(2., 3., 4.) == 10.);
    volatile double tenth = 0.1;
    Require(fma_nid_postfix(tenth, 10., -1.) == 0x1p-54);
    Require(fma_nid_postfix(0x1p1023, 2., -0x1p1023) == 0x1p1023);
    Require(std::isnan(fma_nid_postfix(INFINITY, 0., 1.)));
}

static bool ExtendedBits(long double value, std::uint64_t mantissa, std::uint16_t signExponent) {
    unsigned char bytes[sizeof(long double)]{};
    std::memcpy(bytes, &value, sizeof(value));
    std::uint64_t actualMantissa = 0;
    std::uint16_t actualSignExponent = 0;
    std::memcpy(&actualMantissa, bytes, 8);
    std::memcpy(&actualSignExponent, bytes + 8, 2);
    return actualMantissa == mantissa && actualSignExponent == signExponent;
}

static void CheckHypotSpecialValue() {
    Require(std::bit_cast<std::uint64_t>(hypot_nid_postfix(INFINITY, std::bit_cast<double>(0x7ff0000000000001ull))) == 0x7ff0000000000000ull);
    Require(std::bit_cast<std::uint64_t>(hypot_nid_postfix(std::bit_cast<double>(0x7ff0000000000001ull), INFINITY)) == 0x7ff0000000000000ull);
    Require(std::bit_cast<std::uint64_t>(hypot_nid_postfix(-INFINITY, std::bit_cast<double>(0x7ff0000000000001ull))) == 0x7ff0000000000000ull);
    Require(std::bit_cast<std::uint64_t>(hypot_nid_postfix(std::bit_cast<double>(0x7ff0000000000001ull), -INFINITY)) == 0x7ff0000000000000ull);
}

static void CheckLongDouble() {
    Require(ExtendedBits(acosl_nid_postfix(-1.0L), 0xc90fdaa22168c235ull, 0x4000));
    Require(ExtendedBits(acosl_nid_postfix(0.0L), 0xc90fdaa22168c235ull, 0x3fff));
    Require(ExtendedBits(acosl_nid_postfix(1.0L), 0, 0));
    Require(std::isnan(acosl_nid_postfix(1.5L)) && std::isnan(acosl_nid_postfix(-2.0L)));

    int exponent = 77;
    Require(frexpl_nid_postfix(8.0L, &exponent) == 0.5L && exponent == 4);
    Require(frexpl_nid_postfix(-3.0L, &exponent) == -0.75L && exponent == 2);
    Require(frexpl_nid_postfix(0.375L, &exponent) == 0.75L && exponent == -1);
    long double smallest = 0;
    const std::uint64_t one = 1;
    std::memcpy(&smallest, &one, sizeof(one));
    Require(ExtendedBits(frexpl_nid_postfix(smallest, &exponent), 0x8000000000000000ull, 0x3ffe) && exponent == -16444);
    exponent = 77;
    Require(ExtendedBits(frexpl_nid_postfix(-0.0L, &exponent), 0, 0x8000) && exponent == 0);
    exponent = 77;
    Require(frexpl_nid_postfix(INFINITY, &exponent) == INFINITY && exponent == 77);
    Require(frexpl_nid_postfix(-INFINITY, &exponent) == -INFINITY && exponent == 77);
    Require(std::isnan(frexpl_nid_postfix(NAN, &exponent)) && exponent == 77);
}

static long double MakeExtended(std::uint64_t mantissa, std::uint16_t signExponent) {
    unsigned char bytes[sizeof(long double)]{};
    std::memcpy(bytes, &mantissa, 8);
    std::memcpy(bytes + 8, &signExponent, 2);
    long double value = 0;
    std::memcpy(&value, bytes, sizeof(value));
    return value;
}

static bool SameDouble(double value, std::uint64_t bits) { return std::bit_cast<std::uint64_t>(value) == bits; }

static void CheckExactLibm() {
    *__error_nid_postfix() = 12345;
    Require(ExtendedBits(ldexpl_nid_postfix(0.75L, 4), 0xc000000000000000ull, 0x4002));
    Require(ExtendedBits(ldexpl_nid_postfix(1.0L, -16445), 1, 0) && ExtendedBits(ldexpl_nid_postfix(1.0L, -16446), 0, 0));
    Require(ExtendedBits(ldexpl_nid_postfix(3.0L, -16446), 2, 0) && ExtendedBits(ldexpl_nid_postfix(-0.0L, 100), 0, 0x8000));
    Require(ExtendedBits(ldexpl_nid_postfix(MakeExtended(1, 0), 16445), 0x8000000000000000ull, 0x3fff));
    Require(ExtendedBits(ldexpl_nid_postfix(MakeExtended(0xffffffffffffffffull, 0x7ffe), 1), 0x8000000000000000ull, 0x7fff));
    Require(ExtendedBits(ldexpl_nid_postfix(-1.0L, std::numeric_limits<int>::max()), 0x8000000000000000ull, 0xffff));
    Require(ExtendedBits(ldexpl_nid_postfix(1.0L, std::numeric_limits<int>::min()), 0, 0));
    Require(std::isnan(ldexpl_nid_postfix(std::numeric_limits<long double>::quiet_NaN(), 3)));

    Require(logb_nid_postfix(8.) == 3. && logb_nid_postfix(-0.75) == -1. && logb_nid_postfix(0x1p-1022) == -1022.);
    Require(logb_nid_postfix(std::numeric_limits<double>::denorm_min()) == -1074. && logb_nid_postfix(0x0.fffffffffffffp-1022) == -1023.);
    Require(logb_nid_postfix(-std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
    Require(std::isnan(logb_nid_postfix(std::numeric_limits<double>::quiet_NaN())));
    std::feclearexcept(FE_ALL_EXCEPT);
    Require(logb_nid_postfix(-0.) == -std::numeric_limits<double>::infinity() && std::fetestexcept(FE_DIVBYZERO) != 0);

    Require(remainder_nid_postfix(5., 2.) == 1. && SameDouble(remainder_nid_postfix(-7., 2.), 0x3ff0000000000000ull));
    Require(SameDouble(remainder_nid_postfix(-5.5, 2.), 0x3fe0000000000000ull) && SameDouble(remainder_nid_postfix(0x1p1023, 3.), 0xbff0000000000000ull));
    Require(SameDouble(remainder_nid_postfix(1e308, 7.), 0x4008000000000000ull) && SameDouble(remainder_nid_postfix(-4., 2.), 0x8000000000000000ull));
    Require(SameDouble(remainder_nid_postfix(0x7p-1074, 0x2p-1074), 0x8000000000000001ull) && remainder_nid_postfix(3., INFINITY) == 3.);
    Require(SameDouble(remainder_nid_postfix(std::bit_cast<double>(0xfff8000000000001ull), std::bit_cast<double>(0x7ff8000000000005ull)), 0x7ff8000000000005ull));
    Require(SameDouble(remainder_nid_postfix(std::bit_cast<double>(0xfff8000000000000ull), std::bit_cast<double>(0x7ff8000000000000ull)), 0x7ff8000000000000ull));
    Require(SameDouble(remainder_nid_postfix(std::bit_cast<double>(0x7ff0000000000001ull), 0.), 0x7ff8000000000001ull));
    Require(*__error_nid_postfix() == 12345);
    for (const auto& operands : {std::pair{1., 0.}, std::pair{-0., -0.}, std::pair{static_cast<double>(INFINITY), 2.}, std::pair{-static_cast<double>(INFINITY), 0.}}) {
        std::feclearexcept(FE_ALL_EXCEPT);
        *__error_nid_postfix() = 0;
        Require(SameDouble(remainder_nid_postfix(operands.first, operands.second), 0xfff8000000000000ull));
        Require(*__error_nid_postfix() == EDOM && std::fetestexcept(FE_INVALID) != 0);
    }
    *__error_nid_postfix() = 0;
}

struct ExtendedCase {
    std::uint64_t inputMantissa;
    std::uint16_t inputSignExponent;
    std::uint64_t outputMantissa;
    std::uint16_t outputSignExponent;
};

using ExtendedFunction = long double (APS5_VABI*)(long double);

static void CheckExtendedCases(const char* name, ExtendedFunction function, std::initializer_list<ExtendedCase> cases) {
    for (const auto& test : cases) {
        *__error_nid_postfix() = 12345;
        const long double result = function(MakeExtended(test.inputMantissa, test.inputSignExponent));
        if (!ExtendedBits(result, test.outputMantissa, test.outputSignExponent) || *__error_nid_postfix() != 12345) {
            std::fprintf(stderr, "Guest %s failed for %016llx:%04x\n", name, static_cast<unsigned long long>(test.inputMantissa), test.inputSignExponent);
            std::abort();
        }
    }
    *__error_nid_postfix() = 0;
}

static void CheckAtanl() {
    CheckExtendedCases("atanl", atanl_nid_postfix, {
        {0, 0x0000, 0, 0x0000}, {0, 0x8000, 0, 0x8000}, {1, 0x0000, 1, 0x0000},
        {0x8000000000000000ull, 0x3fde, 0x8000000000000000ull, 0x3fde}, {0x8000000000000000ull, 0xbfd7, 0x8000000000000000ull, 0xbfd7},
        {0x8000000000000000ull, 0x3ffd, 0xfadbafc96406eb15ull, 0x3ffc}, {0xe000000000000000ull, 0x3ffd, 0xd327761e611fe5b6ull, 0x3ffd},
        {0x8000000000000000ull, 0xbffe, 0xed63382b0dda7b45ull, 0xbffd}, {0xb000000000000000ull, 0x3ffe, 0x9a2f80e671bdda20ull, 0x3ffe},
        {0x8000000000000000ull, 0x3fff, 0xc90fdaa22168c235ull, 0x3ffe}, {0x9800000000000000ull, 0xbfff, 0xdef3876a1cff365dull, 0xbffe},
        {0x8000000000000000ull, 0x4000, 0x8db70c975df22363ull, 0x3fff}, {0x9c00000000000000ull, 0x4000, 0x973ab94419d2a08cull, 0x3fff},
        {0xa000000000000000ull, 0xc002, 0xbc4de960b0029c17ull, 0xbfff}, {0x9502f90000000000ull, 0x4020, 0xc90fdaa1ea6f0281ull, 0x3fff},
        {0x8000000000000000ull, 0x403f, 0xc90fdaa22168c234ull, 0x3fff}, {0x8000000000000000ull, 0x4040, 0xc90fdaa22168c235ull, 0x3fff},
        {0x8000000000000000ull, 0xc040, 0xc90fdaa22168c235ull, 0xbfff}, {0x8000000000000000ull, 0x7fff, 0xc90fdaa22168c235ull, 0x3fff},
        {0x8000000000000000ull, 0xffff, 0xc90fdaa22168c235ull, 0xbfff}, {0xc000000000000001ull, 0x7fff, 0xc000000000000001ull, 0x7fff},
    });
}

static void CheckTanl() {
    CheckExtendedCases("tanl", tanl_nid_postfix, {
        {0, 0x0000, 0, 0x0000}, {0, 0x8000, 0, 0x8000}, {1, 0x0000, 1, 0x0000}, {0x8000000000000000ull, 0x3fd7, 0x8000000000000000ull, 0x3fd7},
        {0x8000000000000000ull, 0x3ffe, 0x8bda7adf9a3a5219ull, 0x3ffe}, {0xaca18bd66277c45dull, 0xbffe, 0xcca1b88ee1674910ull, 0xbffe},
        {0xc90fdaa22168c183ull, 0x3ffe, 0xfffffffffffffe9cull, 0x3ffe}, {0xcccccccccccccccdull, 0x3ffe, 0x83cb323c9db05471ull, 0x3fff},
        {0x8000000000000000ull, 0x3fff, 0xc75922e5f71d2dc5ull, 0x3fff}, {0xc000000000000000ull, 0xbfff, 0xe19f6a85c43bbad3ull, 0xc002},
        {0xc90fdaa22168c183ull, 0x3fff, 0xb854579eb3551760ull, 0x4036}, {0xc90fdaa22168c233ull, 0x4000, 0xe2633145c06e0e69ull, 0xbfc1},
        {0xa000000000000000ull, 0xc002, 0xa5faf9a5f1bc12f0ull, 0xbffe}, {0xbebc200000000000ull, 0x4019, 0xa414f41fa4ac45c1ull, 0xc000},
        {0xc90fdaa200000000ull, 0x4018, 0x85a314f6a015a8e4ull, 0xbff6}, {0xc90fdaa300000000ull, 0x4018, 0xde9abf5f5051270full, 0x3ff8},
        {0x8000000000000000ull, 0x4063, 0xe437dfd4676618f8ull, 0xbfff}, {0xbf21e44003acdd2dull, 0xc3e3, 0x805558d665b191bcull, 0x3fff},
        {0xffffffffffffffffull, 0x7ffe, 0xfde654994ce86fdbull, 0xc001}, {0x8000000000000000ull, 0x7fff, 0xc000000000000000ull, 0xffff},
        {0x8000000000000000ull, 0xffff, 0xc000000000000000ull, 0xffff}, {0xc000000000000001ull, 0x7fff, 0xc000000000000001ull, 0x7fff},
    });
    for (const std::uint16_t signExponent : {std::uint16_t{0x7fff}, std::uint16_t{0xffff}}) {
        std::feclearexcept(FE_ALL_EXCEPT);
        *__error_nid_postfix() = 0;
        Require(std::isnan(tanl_nid_postfix(MakeExtended(0, signExponent))) && *__error_nid_postfix() == EDOM && std::fetestexcept(FE_INVALID) != 0);
    }
    *__error_nid_postfix() = 0;
    std::feclearexcept(FE_ALL_EXCEPT);
    Require(ExtendedBits(tanl_nid_postfix(MakeExtended(0x4000000000000000ull, 0x4018)), 0xc000000000000000ull, 0xffff) && std::fetestexcept(FE_INVALID) != 0);
    bool threw = false;
    try { tanl_nid_postfix(MakeExtended(0x4000000000000000ull, 0x4019)); } catch (...) { threw = true; }
    Require(threw && *__error_nid_postfix() == 0);
}

static void CheckExp2l() {
    CheckExtendedCases("exp2l", exp2l_nid_postfix, {
        {0, 0x0000, 0x8000000000000000ull, 0x3fff}, {0, 0x8000, 0x8000000000000000ull, 0x3fff},
        {0x8000000000000000ull, 0x3fbd, 0x8000000000000000ull, 0x3fff}, {0x8000000000000000ull, 0x3fbe, 0x8000000000000000ull, 0x3fff},
        {0x8000000000000000ull, 0x3ffe, 0xb504f333f9de6484ull, 0x3fff}, {0x8000000000000000ull, 0xbffe, 0xb504f333f9de6484ull, 0x3ffe},
        {0x8000000000000000ull, 0x3fff, 0x8000000000000000ull, 0x4000}, {0xcccccccccccccccdull, 0x3ffb, 0x892fdf7128332132ull, 0x3fff},
        {0xc000000000000000ull, 0xbfff, 0xb504f333f9de6484ull, 0x3ffd}, {0xa000000000000000ull, 0x4002, 0x8000000000000000ull, 0x4009},
        {0xc880000000000000ull, 0x4005, 0x9837f0518db8a96full, 0x4063}, {0xfffe000000000000ull, 0x400c, 0xb504f333f9de6484ull, 0x7ffe},
        {0xfffffef9db22d0e5ull, 0x400c, 0xffd296f30e08e880ull, 0x7ffe}, {0x8000000000000000ull, 0x400d, 0x8000000000000000ull, 0x7fff},
        {0xc350000000000000ull, 0x400f, 0x8000000000000000ull, 0x7fff}, {0xfff8000000000000ull, 0xc00c, 0x8000000000000000ull, 0x0001},
        {0x802099999999999aull, 0xc00d, 0x000019fdf8bcce53ull, 0x0000}, {0x807a000000000000ull, 0xc00d, 0x0000000000000001ull, 0x0000},
        {0x807b000000000000ull, 0xc00d, 0x0000000000000001ull, 0x0000}, {0x807c000000000000ull, 0xc00d, 0, 0x0000},
        {0x8000000000000000ull, 0x7fff, 0x8000000000000000ull, 0x7fff}, {0x8000000000000000ull, 0xffff, 0, 0x0000},
        {0xc000000000000001ull, 0xffff, 0xc000000000000001ull, 0xffff},
    });
    std::feclearexcept(FE_ALL_EXCEPT);
    Require(std::isinf(exp2l_nid_postfix(16384.0L)) && std::fetestexcept(FE_OVERFLOW) == 0);
    Require(exp2l_nid_postfix(-16446.0L) == 0.0L && std::fetestexcept(FE_UNDERFLOW) != 0);
}

static void CheckExpl() {
    CheckExtendedCases("expl", expl_nid_postfix, {
        {0, 0x0000, 0x8000000000000000ull, 0x3fff}, {0, 0x8000, 0x8000000000000000ull, 0x3fff}, {1, 0x0000, 0x8000000000000000ull, 0x3fff},
        {0x8000000000000000ull, 0x3fb9, 0x8000000000000000ull, 0x3fff}, {0xbce5086492111aebull, 0x3fbc, 0x8000000000000000ull, 0x3fff},
        {0x8000000000000000ull, 0x3ffe, 0xd3094c70f034de4dull, 0x3fff}, {0x8000000000000000ull, 0x3fff, 0xadf85458a2bb4a9bull, 0x4000},
        {0x8000000000000000ull, 0xbfff, 0xbc5ab1b16779be36ull, 0x3ffd}, {0x8000000000000000ull, 0x4000, 0xec7325c6a6ed6e61ull, 0x4001},
        {0xa000000000000000ull, 0x4002, 0xac14ee7ca82afcf8ull, 0x400d}, {0xa000000000000000ull, 0xc002, 0xbe6bcdab23e4d4e3ull, 0x3ff0},
        {0xc800000000000000ull, 0x4005, 0x9a4a54d8b8dfa566ull, 0x408f}, {0xb172000000000000ull, 0x400c, 0xfa13dba329dd768dull, 0x7ffe},
        {0xb16c000000000000ull, 0xc00c, 0x92cf78ab801959c4ull, 0x0001}, {0xe665000000000000ull, 0xc00d, 0, 0x0000},
        {0x8000000000000000ull, 0x7fff, 0x8000000000000000ull, 0x7fff}, {0x8000000000000000ull, 0xffff, 0, 0x0000},
        {0xc000000000000001ull, 0x7fff, 0xc000000000000001ull, 0x7fff}, {0x4000000000000000ull, 0x3fff, 0xc000000000000000ull, 0xffff},
    });
    const struct { long double input; bool overflow; } rangeErrors[] = {
        {11357.0L, true}, {29490.0L, true}, {29490.5L, true}, {-11400.0L, false}, {-11500.0L, false}, {-29490.0L, false},
    };
    for (const auto& test : rangeErrors) {
        std::feclearexcept(FE_ALL_EXCEPT);
        *__error_nid_postfix() = 0;
        const long double result = expl_nid_postfix(test.input);
        Require(*__error_nid_postfix() == ERANGE && (test.overflow ? std::isinf(result) : result == 0.0L));
        Require(std::fetestexcept(test.overflow ? FE_OVERFLOW : FE_UNDERFLOW) != 0);
    }
    *__error_nid_postfix() = 0;
}

static void CheckHyperbolicLongDouble() {
    CheckExtendedCases("coshl", coshl_nid_postfix, {
        {0, 0x0000, 0x8000000000000000ull, 0x3fff}, {0, 0x8000, 0x8000000000000000ull, 0x3fff}, {1, 0x0000, 0x8000000000000000ull, 0x3fff},
        {0x8000000000000000ull, 0x3fd7, 0x8000000000000000ull, 0x3fff}, {0x8000000000000000ull, 0x3ffe, 0x90560c3157468323ull, 0x3fff},
        {0x8000000000000000ull, 0xbfff, 0xc583aa8ecfaa8261ull, 0x3fff}, {0x8000000000000000ull, 0x4000, 0xf0c7d06f96cde149ull, 0x4000},
        {0xa000000000000000ull, 0xc002, 0xac14ee829b896a51ull, 0x400c}, {0xb400000000000000ull, 0x4003, 0xb025b46782fb2fd5ull, 0x401e},
        {0xb470a3d70a3d70a4ull, 0x4003, 0xba1b549ac285772aull, 0x401e}, {0xb4cccccccccccccdull, 0xc003, 0xc2ac4043595231ffull, 0x401e},
        {0xc800000000000000ull, 0x4005, 0x9a4a54d8b8dfa566ull, 0x408e}, {0xb16c000000000000ull, 0x400c, 0xdf330b6d400a14d9ull, 0x7ffb},
        {0xb172000000000000ull, 0x400c, 0xfa13dba329dd768dull, 0x7ffd}, {0xb174000000000000ull, 0xc00c, 0xce2773666cc8cc68ull, 0x7ffe},
        {0x8000000000000000ull, 0x7fff, 0x8000000000000000ull, 0x7fff}, {0x8000000000000000ull, 0xffff, 0x8000000000000000ull, 0x7fff},
        {0xc000000000000001ull, 0x7fff, 0xc000000000000001ull, 0x7fff},
    });
    CheckExtendedCases("sinhl", sinhl_nid_postfix, {
        {0, 0x0000, 0, 0x0000}, {0, 0x8000, 0, 0x8000}, {1, 0x0000, 1, 0x0000},
        {0x8000000000000000ull, 0x3fdd, 0x8000000000000000ull, 0x3fdd}, {0x8000000000000000ull, 0xbfde, 0x8000000000000000ull, 0xbfde},
        {0x8000000000000000ull, 0x3ffd, 0x815666ce84080f33ull, 0x3ffd}, {0x8000000000000000ull, 0xbffe, 0x8566807f31dcb652ull, 0xbffe},
        {0x8c9eecbfb15b573full, 0x3ffe, 0x93ccc32456171481ull, 0x3ffe}, {0x999999999999999aull, 0x3ffe, 0xa2fbbaaa353beda6ull, 0x3ffe},
        {0x8000000000000000ull, 0xbfff, 0x966cfe2275cc12d4ull, 0xbfff}, {0x8000000000000000ull, 0x4000, 0xe81e7b1db70cfb79ull, 0x4000},
        {0xff5c28f5c28f5c29ull, 0x4000, 0xd8254354050620e1ull, 0x4003}, {0x8000000000000000ull, 0x4001, 0xda51c01b81b18649ull, 0x4003},
        {0xa000000000000000ull, 0xc002, 0xac14ee76b4cc8f9full, 0xc00c}, {0xa000000000000000ull, 0x4003, 0xe758445b47401fcaull, 0x401a},
        {0xa028f5c28f5c28f6ull, 0x4003, 0xec04ad33ee68d3aaull, 0x401a}, {0xa800000000000000ull, 0xc003, 0x9d370fecf767eab3ull, 0xc01c},
        {0xc800000000000000ull, 0x4005, 0x9a4a54d8b8dfa566ull, 0x408e}, {0xb174000000000000ull, 0x400c, 0xce2773666cc8cc68ull, 0x7ffe},
        {0x8000000000000000ull, 0x7fff, 0x8000000000000000ull, 0x7fff}, {0x8000000000000000ull, 0xffff, 0x8000000000000000ull, 0xffff},
        {0xc000000000000001ull, 0x7fff, 0xc000000000000001ull, 0x7fff}, {0, 0x7fff, 0xc000000000000000ull, 0xffff},
    });
    CheckExtendedCases("tanhl", tanhl_nid_postfix, {
        {0, 0x0000, 0, 0x0000}, {0, 0x8000, 0, 0x8000}, {1, 0x0000, 1, 0x0000},
        {0x8000000000000000ull, 0x3fdd, 0x8000000000000000ull, 0x3fdd}, {0x8000000000000000ull, 0xbfde, 0x8000000000000000ull, 0xbfde},
        {0x8000000000000000ull, 0x3ffd, 0xfacbf534d0e42f8aull, 0x3ffc}, {0x999999999999999aull, 0xbffd, 0x9526ed3ec8a7d089ull, 0xbffd},
        {0xcccccccccccccccdull, 0x3ffd, 0xc288ab9e1216ae2bull, 0x3ffd}, {0x999999999999999aull, 0xbffe, 0x897c14969667df08ull, 0xbffe},
        {0xb333333333333333ull, 0x3ffe, 0x9ab7d8bd79748804ull, 0x3ffe}, {0x8000000000000000ull, 0x3fff, 0xc2f7d5a8a79ca2acull, 0x3ffe},
        {0x8000000000000000ull, 0xc000, 0xf6ca82f0de1e9e9aull, 0xbffe}, {0xa000000000000000ull, 0x4002, 0xffffffee4b79aaa9ull, 0x3ffe},
        {0xc4cccccccccccccdull, 0x4003, 0x8000000000000000ull, 0x3fff}, {0xc4e147ae147ae148ull, 0x4003, 0x8000000000000000ull, 0x3fff},
        {0xf000000000000000ull, 0xc003, 0x8000000000000000ull, 0xbfff}, {0x8000000000000000ull, 0x7fff, 0x8000000000000000ull, 0x3fff},
        {0x8000000000000000ull, 0xffff, 0x8000000000000000ull, 0xbfff}, {0xc000000000000001ull, 0x7fff, 0xc000000000000001ull, 0x7fff},
    });
    const struct { ExtendedFunction function; long double input; long double output; } rangeErrors[] = {
        {coshl_nid_postfix, 11400.0L, std::numeric_limits<long double>::infinity()},
        {coshl_nid_postfix, -29490.5L, std::numeric_limits<long double>::infinity()},
        {sinhl_nid_postfix, 11400.0L, std::numeric_limits<long double>::infinity()},
        {sinhl_nid_postfix, -11400.0L, -std::numeric_limits<long double>::infinity()},
    };
    for (const auto& test : rangeErrors) {
        std::feclearexcept(FE_ALL_EXCEPT);
        *__error_nid_postfix() = 0;
        Require(test.function(test.input) == test.output && *__error_nid_postfix() == ERANGE && std::fetestexcept(FE_OVERFLOW) != 0);
    }
    *__error_nid_postfix() = 0;
}

static void CheckLog10l() {
    CheckExtendedCases("log10l", log10l_nid_postfix, {
        {0x8000000000000000ull, 0x3fff, 0, 0x0000}, {0xa000000000000000ull, 0x4002, 0x8000000000000000ull, 0x3fff},
        {0xc800000000000000ull, 0x4005, 0x8000000000000000ull, 0x4000}, {0xfa00000000000000ull, 0x4008, 0xc000000000000000ull, 0x4000},
        {0xcccccccccccccccdull, 0x3ffb, 0x8000000000000000ull, 0xbfff}, {0x83126e978d4fdf3bull, 0x3ff5, 0xc000000000000000ull, 0xc000},
        {0x8000000000000000ull, 0x4000, 0x9a209a84fbcff799ull, 0x3ffd}, {0x8000000000000000ull, 0x3ffe, 0x9a209a84fbcff799ull, 0xbffd},
        {0xb504f333f9de6433ull, 0x3ffe, 0x9a209a84fbcff860ull, 0xbffc}, {0xb504f333f9de6484ull, 0x3ffe, 0x9a209a84fbcff799ull, 0xbffc},
        {0xc000000000000000ull, 0x4000, 0xf4493cb27eafe846ull, 0x3ffd}, {0xd1ba8323fe558c61ull, 0x73e6, 0xfa00000000000000ull, 0x400a},
        {0x9c3d73864f3805c0ull, 0x0c17, 0xfa00000000000000ull, 0xc00a}, {1, 0x0000, 0x9ab38198428ff9d1ull, 0xc00b},
        {0xffffffffffffffffull, 0x7ffe, 0x9a209a84fbcff799ull, 0x400b}, {0xffffffffffffffffull, 0x3ffe, 0xde5bd8a937287196ull, 0xbfbd},
        {0x8000000000000001ull, 0x3fff, 0xde5bd8a937287194ull, 0x3fbe}, {0xffffffffffffffffull, 0x0000, 0x9a1960fdbd9429d9ull, 0xc00b},
        {0x8000000000000000ull, 0x7fff, 0x8000000000000000ull, 0x7fff}, {0xc000000000000001ull, 0x7fff, 0xc000000000000001ull, 0x7fff},
        {0x4000000000000000ull, 0x3fff, 0x9a209a84fbcff799ull, 0xbffd},
    });
    const struct { std::uint64_t mantissa; std::uint16_t signExponent; int error; int exception; } domainErrors[] = {
        {0, 0x0000, ERANGE, FE_DIVBYZERO}, {0, 0x8000, ERANGE, FE_DIVBYZERO}, {0, 0x3fff, ERANGE, FE_DIVBYZERO},
        {0x8000000000000000ull, 0xbfff, EDOM, FE_INVALID}, {1, 0x8000, EDOM, FE_INVALID}, {0x8000000000000000ull, 0xffff, EDOM, FE_INVALID},
    };
    for (const auto& test : domainErrors) {
        std::feclearexcept(FE_ALL_EXCEPT);
        *__error_nid_postfix() = 0;
        const long double result = log10l_nid_postfix(MakeExtended(test.mantissa, test.signExponent));
        const bool expected = test.error == ERANGE ? ExtendedBits(result, 0x8000000000000000ull, 0xffff) : ExtendedBits(result, 0xc000000000000000ull, 0x7fff);
        Require(expected && *__error_nid_postfix() == test.error && std::fetestexcept(test.exception) != 0);
    }
    *__error_nid_postfix() = 0;
}

static void CheckIntegerConversions() {
    for (const long long numerator : {4294967301LL, -4294967301LL}) {
        for (const long long denominator : {3LL, -3LL}) {
            const auto result = lldiv_nid_postfix(numerator, denominator);
            Require(result.quot == numerator / denominator && result.rem == numerator % denominator);
            Require(result.quot * denominator + result.rem == numerator);
            const auto wide = ldiv_nid_postfix(numerator, denominator);
            Require(wide.quot == result.quot && wide.rem == result.rem);
        }
    }
    const auto extreme = ldiv_nid_postfix(INT64_MIN, 2);
    Require(extreme.quot == INT64_MIN / 2 && extreme.rem == 0);
    struct SignedCase {
        const char* text;
        int base;
        std::int64_t value;
        std::size_t consumed;
        int error;
    };
    const SignedCase signedCases[] = {
        {"-42tail", 10, -42, 3, 0},
        {"2147483648!", 10, INT64_C(2147483648), 10, 0},
        {"-2147483649!", 10, -INT64_C(2147483649), 11, 0},
        {"4294967296!", 10, INT64_C(4294967296), 10, 0},
        {"9223372036854775807!", 10, INT64_MAX, 19, 0},
        {"-9223372036854775808!", 10, INT64_MIN, 20, 0},
        {"9223372036854775808!", 10, INT64_MAX, 19, 34},
        {"-9223372036854775809!", 10, INT64_MIN, 20, 34},
        {"18446744073709551616000!", 10, INT64_MAX, 23, 34},
        {" \t+0x100000000z", 0, INT64_C(4294967296), 14, 0},
        {"-0x8000000000000000!", 0, INT64_MIN, 19, 0},
        {"0x8000000000000000!", 16, INT64_MAX, 18, 34},
        {"0100000000000!", 0, INT64_C(8589934592), 13, 0},
        {"100000000000000000000000000000000!", 2, INT64_C(4294967296), 33, 0},
        {"0b101", 0, 0, 1, 0},
        {"0b101", 2, 0, 1, 0},
        {" -0B1!", 0, 0, 3, 0},
        {"0b2", 0, 0, 1, 0},
        {"0b101", 16, 0xb101, 5, 0},
        {" -0xz!", 0, 0, 3, 0},
        {"0xg", 16, 0, 1, 0},
        {"0x", 16, 0, 1, 0},
        {"z!", 36, 35, 1, 0},
        {"", 10, 0, 0, 0},
        {" \t+!", 10, 0, 0, 0},
        {"123!", 10, 123, 3, 0}
    };
    for (const auto& test : signedCases) {
        char* end = nullptr;
        *__error_nid_postfix() = 0;
        const auto value = strtol_nid_postfix(test.text, &end, test.base);
        if (value != test.value || end != test.text + test.consumed || *__error_nid_postfix() != test.error) {
            std::fprintf(stderr, "Guest strtol failed for '%s' in base %d\n", test.text, test.base);
            std::abort();
        }
        *__error_nid_postfix() = 0;
        const auto maxValue = strtoimax_nid_postfix(test.text, &end, test.base);
        if (maxValue != test.value || end != test.text + test.consumed || *__error_nid_postfix() != test.error) {
            std::fprintf(stderr, "Guest strtoimax failed for '%s' in base %d\n", test.text, test.base);
            std::abort();
        }
    }
    struct UnsignedCase {
        const char* text;
        int base;
        std::uint64_t value;
        std::size_t consumed;
        int error;
    };
    const UnsignedCase unsignedCases[] = {
        {"4294967296!", 10, UINT64_C(4294967296), 10, 0},
        {"9223372036854775808!", 10, UINT64_C(9223372036854775808), 19, 0},
        {"18446744073709551615!", 10, UINT64_MAX, 20, 0},
        {"18446744073709551616!", 10, UINT64_MAX, 20, 34},
        {"18446744073709551616000!", 10, UINT64_MAX, 23, 34},
        {"-1!", 10, UINT64_MAX, 2, 0},
        {"-4294967296!", 10, UINT64_MAX - UINT64_C(4294967295), 11, 0},
        {"-18446744073709551615!", 10, 1, 21, 0},
        {"-18446744073709551616!", 10, UINT64_MAX, 21, 34},
        {" \t+0xffffffffffffffffz", 0, UINT64_MAX, 21, 0},
        {"0x10000000000000000!", 16, UINT64_MAX, 19, 34},
        {"0100000000000!", 0, UINT64_C(8589934592), 13, 0},
        {"100000000000000000000000000000000!", 2, UINT64_C(4294967296), 33, 0},
        {"0b101", 0, 0, 1, 0},
        {"0B11", 2, 0, 1, 0},
        {" +0b1!", 2, 0, 3, 0},
        {"0b101", 16, 0xb101, 5, 0},
        {" +0Xg!", 0, 0, 3, 0},
        {"0x!", 16, 0, 1, 0},
        {"z!", 36, 35, 1, 0},
        {"", 10, 0, 0, 0},
        {" \t-!", 10, 0, 0, 0},
        {"123!", 10, 123, 3, 0}
    };
    for (const auto& test : unsignedCases) {
        char* end = nullptr;
        *__error_nid_postfix() = 0;
        const auto value = strtoul_nid_postfix(test.text, &end, test.base);
        if (value != test.value || end != test.text + test.consumed || *__error_nid_postfix() != test.error) {
            std::fprintf(stderr, "Guest strtoul failed for '%s' in base %d\n", test.text, test.base);
            std::abort();
        }
    }
    for (const int base : {0, 2}) {
        const char text[] = " -0B11";
        char* end = nullptr;
        Require(strtoll_nid_postfix(text, &end, base) == 0 && end == text + 3);
        Require(strtoull_nid_postfix(text, &end, base) == 0 && end == text + 3);
        Require(strtoumax_nid_postfix(text, &end, base) == 0 && end == text + 3);
        Require(_Stoull_nid_postfix(text, &end, base) == 0 && end == text + 3);
        Require(_Stoul_nid_postfix(text, &end, base) == 0 && end == text + 3);
    }
    Require(atol_nid_postfix(" \t-4294967296tail") == -INT64_C(4294967296));
    Require(atol_nid_postfix("9223372036854775807") == INT64_MAX && atol_nid_postfix("+12") == 12);
    Require(atol_nid_postfix("0x10") == 0 && atol_nid_postfix("010") == 10 && atol_nid_postfix("") == 0);
    Require(atoll_nid_postfix("-9223372036854775808") == INT64_MIN && atoll_nid_postfix("4294967297x") == 4294967297LL);
    Require(atoll_nid_postfix("  -0012") == -12 && atoll_nid_postfix("z1") == 0);
    *__error_nid_postfix() = 13;
    Require(strtol_nid_postfix("-4294967296", nullptr, 10) == -INT64_C(4294967296));
    Require(*__error_nid_postfix() == 13);
    Require(strtoul_nid_postfix("4294967296", nullptr, 10) == UINT64_C(4294967296));
    Require(*__error_nid_postfix() == 13);
    Require(_Stoul_nid_postfix("4294967296", nullptr, 10) == UINT64_C(4294967296));
    Require(*__error_nid_postfix() == 13);
    Require(_Stoul_nid_postfix("18446744073709551615", nullptr, 10) == UINT64_MAX);
    Require(_Stoul_nid_postfix("-1", nullptr, 10) == UINT64_MAX && *__error_nid_postfix() == 13);
    *__error_nid_postfix() = 0;
}

static bool Near(double value, double expected, double tolerance) { return std::abs(value - expected) <= tolerance * std::abs(expected); }

static void CheckHyperbolic() {
    const double infinity = std::numeric_limits<double>::infinity();
    Require(sinh_nid_postfix(0.) == 0. && std::signbit(sinh_nid_postfix(-0.)));
    Require(cosh_nid_postfix(0.) == 1. && coshf_nid_postfix(0.f) == 1.f);
    Require(asinh_nid_postfix(0.) == 0. && std::signbit(asinh_nid_postfix(-0.)) && std::signbit(asinhf_nid_postfix(-0.f)));
    Require(acosh_nid_postfix(1.) == 0. && acoshf_nid_postfix(1.f) == 0.f);
    Require(atanh_nid_postfix(0.) == 0. && atanh_nid_postfix(1.) == infinity && atanh_nid_postfix(-1.) == -infinity);
    Require(std::isnan(acosh_nid_postfix(0.5)) && std::isnan(acoshf_nid_postfix(0.5f)) && std::isnan(atanh_nid_postfix(2.)));
    Require(Near(sinh_nid_postfix(1.), 1.1752011936438014, 1e-15));
    Require(Near(cosh_nid_postfix(1.), 1.5430806348152437, 1e-15));
    Require(Near(coshf_nid_postfix(1.f), 1.5430806348152437, 1e-6));
    Require(Near(asinh_nid_postfix(1.), 0.881373587019543, 1e-15));
    Require(Near(asinhf_nid_postfix(1.f), 0.881373587019543, 1e-6));
    Require(Near(acosh_nid_postfix(2.), 1.3169578969248166, 1e-15));
    Require(Near(acoshf_nid_postfix(2.f), 1.3169578969248166, 1e-6));
    Require(Near(atanh_nid_postfix(0.5), 0.5493061443340549, 1e-15));
    Require(sinh_nid_postfix(1e-300) == 1e-300 && asinh_nid_postfix(1e-300) == 1e-300 && atanh_nid_postfix(1e-300) == 1e-300);
    Require(std::isfinite(cosh_nid_postfix(710.4)) && std::isfinite(sinh_nid_postfix(-710.4)) && sinh_nid_postfix(-710.4) < 0.);
    Require(Near(asinh_nid_postfix(-1e300), -691.4686750787736, 1e-15));
    Require(Near(acosh_nid_postfix(1e300), 691.4686750787736, 1e-15));
}

static void CheckFloatClassification() {
    Require(_FInf_nid_postfix.bits[0] == 0x7f800000u && _FNan_nid_postfix.bits[0] == 0x7fc00000u);
    for (int word = 1; word < 4; ++word) Require(_FInf_nid_postfix.bits[word] == 0 && _FNan_nid_postfix.bits[word] == 0);
    const struct { std::uint32_t bits; short code; } cases[] = {
        {0x00000000u, 0}, {0x80000000u, 0}, {0x00000001u, -2}, {0x807fffffu, -2}, {0x00800000u, -1},
        {0xbf800000u, -1}, {0x7f7fffffu, -1}, {0x7f800000u, 1}, {0xff800000u, 1}, {0x7f800001u, 2},
        {0x7fc00000u, 2}, {0xff810000u, 2}, {0x7f810000u, 2},
    };
    for (const auto& test : cases) {
        float value;
        std::memcpy(&value, &test.bits, sizeof(value));
        if (_FDtest_nid_postfix(&value) != test.code) {
            std::fprintf(stderr, "Guest _FDtest failed for %08x\n", test.bits);
            std::abort();
        }
        const int fpclass = test.code == 0 ? 0x10 : test.code == -2 ? 0x08 : test.code == -1 ? 0x04 : test.code == 1 ? 0x01 : 0x02;
        if (__fpclassifyf_nid_postfix(value) != fpclass) {
            std::fprintf(stderr, "Guest __fpclassifyf failed for %08x\n", test.bits);
            std::abort();
        }
    }
    Require(_FDtest_nid_postfix(reinterpret_cast<const float*>(&_FInf_nid_postfix)) == 1);
    Require(_FDtest_nid_postfix(reinterpret_cast<const float*>(&_FNan_nid_postfix)) == 2);
}

static void CheckDoubleRounding(double value, std::int64_t expected, bool invalid) {
    for (const bool useLlround : {false, true}) {
        std::feclearexcept(FE_ALL_EXCEPT);
        errno = 12345;
        const std::int64_t result = useLlround ? llround_nid_postfix(value) : lround_nid_postfix(value);
        Require(result == expected);
        Require((std::fetestexcept(FE_INVALID) != 0) == invalid);
        Require(errno == 12345);
    }
}

static void CheckFloatRounding(float value, std::int64_t expected, bool invalid) {
    for (const bool useLlround : {false, true}) {
        std::feclearexcept(FE_ALL_EXCEPT);
        errno = 12345;
        const std::int64_t result = useLlround ? llroundf_nid_postfix(value) : lroundf_nid_postfix(value);
        Require(result == expected);
        Require((std::fetestexcept(FE_INVALID) != 0) == invalid);
        Require(errno == 12345);
    }
}

int main() {
    CheckFloatClassification();
    CheckIntegerConversions();
    Require(atof_nid_postfix(" -12.5tail") == -12.5);
    char* end = nullptr;
    const char input[] = "0x1.8p+2 remainder";
    Require(strtof_nid_postfix(input, &end) == 6.f && end == input + 8);
    const char invalid[] = "invalid";
    Require(strtof_nid_postfix(invalid, &end) == 0.f && end == invalid);
    *__error_nid_postfix() = 0;
    Require(std::isinf(strtof_nid_postfix("1e1000", nullptr)));
    Require(*__error_nid_postfix() == 34);
    Require(strtold_nid_postfix("1.0000000000000000001!", &end) > 1.L && *end == '!');
    Require(fmodf_nid_postfix(5.5f, 2.f) == 1.5f);
    Require(fmodf_nid_postfix(-5.5f, 2.f) == -1.5f);
    Require(std::signbit(fmodf_nid_postfix(-4.f, 2.f)));
    Require(std::isnan(fmodf_nid_postfix(1.f, 0.f)));
    Require(std::bit_cast<std::uint32_t>(nanf_nid_postfix(nullptr)) == 0x7fc00000u);
    Require(std::bit_cast<std::uint32_t>(nanf_nid_postfix("")) == 0x7fc00000u);
    Require(std::bit_cast<std::uint32_t>(nanf_nid_postfix("123")) == 0x7fc00000u);
    Require(!std::signbit(nanf_nid_postfix("")));
    Require(std::bit_cast<std::uint64_t>(nan_nid_postfix(nullptr)) == 0x7ff8000000000000ull);
    Require(std::bit_cast<std::uint64_t>(nan_nid_postfix("")) == 0x7ff8000000000000ull);
    Require(std::bit_cast<std::uint64_t>(nan_nid_postfix("123")) == 0x7ff8000000000000ull);
    Require(!std::signbit(nan_nid_postfix("")));
    Require(std::abs(asinf_nid_postfix(0.5f) - 0.5235988f) < 0.000001f);
    Require(std::abs(acosf_nid_postfix(0.5f) - 1.0471976f) < 0.000001f);
    Require(std::abs(atan2f_nid_postfix(1.f, -1.f) - 2.3561945f) < 0.000001f);
    Require(tanf_nid_postfix(0.f) == 0.f);
    unsigned int randomState = 1;
    Require(rand_r_nid_postfix(&randomState) == 33613 && randomState == 33613u);
    randomState = 0;
    Require(rand_r_nid_postfix(&randomState) == 16806 && randomState == 16806u);
    for (const unsigned int seed : {0u, 1u, 12345u, 0x7ffffffeu, 0xffffffffu}) {
        srand_nid_postfix(seed);
        randomState = seed;
        for (int step = 0; step < 8; ++step) Require(rand_r_nid_postfix(&randomState) == rand_nid_postfix());
    }
    bool threw = false;
    try { rand_r_nid_postfix(nullptr); } catch (...) { threw = true; }
    Require(threw);
    Require(hypot_nid_postfix(3.0, 4.0) == 5.0 && hypot_nid_postfix(-3.0, -4.0) == 5.0 && hypotf_nid_postfix(3.f, -4.f) == 5.f);
    Require(std::abs(hypot_nid_postfix(1e308, 1e308) / 1.4142135623730951e308 - 1.0) < 1e-15);
    Require(std::abs(hypotf_nid_postfix(2e38f, 2e38f) / 2.8284271e38f - 1.f) < 1e-6f);
    Require(std::isinf(hypot_nid_postfix(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN())));
    Require(std::isinf(hypotf_nid_postfix(std::numeric_limits<float>::quiet_NaN(), -std::numeric_limits<float>::infinity())));
    Require(std::isnan(hypot_nid_postfix(1.0, std::numeric_limits<double>::quiet_NaN())));
    for (const bool signalingFirst : {true, false}) {
        const float signaling = std::bit_cast<float>(std::uint32_t{0x7f800001});
        const float infinity = std::numeric_limits<float>::infinity();
        std::feclearexcept(FE_ALL_EXCEPT);
        const float result = signalingFirst ? hypotf_nid_postfix(signaling, infinity) : hypotf_nid_postfix(infinity, signaling);
        Require(std::isinf(result) && result > 0.f && std::fetestexcept(FE_INVALID) != 0);
    }
    Require(log10f_nid_postfix(100.f) == 2.f);
    Require(log1p_nid_postfix(0.) == 0. && std::signbit(log1p_nid_postfix(-0.)));
    Require(log1p_nid_postfix(-1.) == -std::numeric_limits<double>::infinity() && std::isnan(log1p_nid_postfix(-2.)));
    Require(log1p_nid_postfix(std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
    Require(log1p_nid_postfix(1e-300) == 1e-300 && log1pf_nid_postfix(1e-30f) == 1e-30f);
    Require(std::abs(log1p_nid_postfix(1.) - 0.6931471805599453) < 1e-16);
    Require(std::abs(log1p_nid_postfix(1e-10) - 9.9999999995e-11) < 1e-25);
    Require(std::abs(log1pf_nid_postfix(1.f) - 0.6931472f) < 1e-7f && std::isnan(log1pf_nid_postfix(-2.f)));
    Require(expm1_nid_postfix(0.) == 0. && std::signbit(expm1_nid_postfix(-0.)));
    Require(expm1_nid_postfix(1e-300) == 1e-300 && expm1_nid_postfix(-1000.) == -1.);
    Require(expm1_nid_postfix(-std::numeric_limits<double>::infinity()) == -1.);
    Require(std::abs(expm1_nid_postfix(1.) - 1.718281828459045) < 1e-15);
    Require(std::abs(expm1_nid_postfix(1e-10) - 1.00000000005e-10) < 1e-25);
    Require(logbf_nid_postfix(8.f) == 3.f && logbf_nid_postfix(-0.75f) == -1.f);
    Require(logbf_nid_postfix(std::numeric_limits<float>::denorm_min()) == -149.f);
    Require(logbf_nid_postfix(0.f) == -std::numeric_limits<float>::infinity());
    Require(logbf_nid_postfix(-std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
    Require(std::isnan(logbf_nid_postfix(std::numeric_limits<float>::quiet_NaN())));
    Require(exp2_nid_postfix(-3.) == 0.125);
    Require(ldexp_nid_postfix(0.75, 4) == 12.);
    CheckSquareRoot();
    CheckExactOperations();
    CheckLongDouble();
    CheckExactLibm();
    CheckAtanl();
    CheckTanl();
    CheckExp2l();
    CheckExpl();
    CheckHyperbolicLongDouble();
    CheckLog10l();
    Require(scalbn_nid_postfix(0.75, -2) == 0.1875);
    Require(scalbnf_nid_postfix(0.75f, 4) == 12.f);
    Require(nextafterf_nid_postfix(1.f, 2.f) == 0x1.000002p0f && nextafterf_nid_postfix(1.f, 0.f) == 0x1.fffffep-1f);
    Require(nextafterf_nid_postfix(0.f, 1.f) == std::numeric_limits<float>::denorm_min());
    Require(nextafterf_nid_postfix(0.f, -1.f) == -std::numeric_limits<float>::denorm_min());
    Require(nextafterf_nid_postfix(std::numeric_limits<float>::denorm_min(), -1.f) == 0.f);
    Require(nextafterf_nid_postfix(1.f, 1.f) == 1.f && !std::signbit(nextafterf_nid_postfix(-0.f, 0.f)));
    Require(nextafterf_nid_postfix(std::numeric_limits<float>::max(), std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
    Require(std::isnan(nextafterf_nid_postfix(std::numeric_limits<float>::quiet_NaN(), 1.f)) && std::isnan(nextafterf_nid_postfix(1.f, std::numeric_limits<float>::quiet_NaN())));
    int exponent = 0;
    Require(frexp_nid_postfix(12., &exponent) == 0.75 && exponent == 4);
    Require(frexpf_nid_postfix(-12.f, &exponent) == -0.75f && exponent == 4);
    Require(lround_nid_postfix(4294967296.5) == INT64_C(4294967297));
    Require(lround_nid_postfix(-2.5) == -3);
    Require(lroundf_nid_postfix(4294967296.f) == INT64_C(4294967296));
    Require(lroundf_nid_postfix(2.5f) == 3);
    Require(llround_nid_postfix(-4294967296.5) == -INT64_C(4294967297));
    Require(llroundf_nid_postfix(2.5f) == 3 && llroundf_nid_postfix(-2.5f) == -3 && llroundf_nid_postfix(-0.4f) == 0);
    Require(llroundf_nid_postfix(8589934592.f) == INT64_C(8589934592) && llroundf_nid_postfix(-0x1p62f) == -(INT64_C(1) << 62));
    Require(llroundf_nid_postfix(0.49999997f) == 0 && llroundf_nid_postfix(16777215.f) == 16777215);
    Require(lround_nid_postfix(0.49999999999999994) == 0 && lround_nid_postfix(-0.49999999999999994) == 0);
    constexpr double kTwo63 = 9223372036854775808.0;
    constexpr double kTwo63Below = 9223372036854774784.0;
    constexpr float kTwo63f = 9223372036854775808.f;
    constexpr float kTwo63fBelow = 9223371487098961920.f;
    CheckDoubleRounding(-kTwo63, INT64_MIN, false);
    CheckDoubleRounding(kTwo63Below, INT64_C(9223372036854774784), false);
    CheckDoubleRounding(-kTwo63Below, -INT64_C(9223372036854774784), false);
    CheckDoubleRounding(4503599627370497.0, INT64_C(4503599627370497), false);
    CheckFloatRounding(-kTwo63f, INT64_MIN, false);
    CheckFloatRounding(kTwo63fBelow, INT64_C(9223371487098961920), false);
    CheckFloatRounding(-kTwo63fBelow, -INT64_C(9223371487098961920), false);
    for (const double value : {kTwo63, -9223374235878031360.0, 1e30, -1e30, std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()}) {
        CheckDoubleRounding(value, INT64_MIN, true);
        CheckFloatRounding(static_cast<float>(value), INT64_MIN, true);
    }
    CheckFloatRounding(kTwo63f, INT64_MIN, true);
    std::feclearexcept(FE_ALL_EXCEPT);
    const auto infinity = std::numeric_limits<float>::infinity();
    const auto nan = std::numeric_limits<float>::quiet_NaN();
    Require(__isinff_nid_postfix(infinity) == 1 && __isinff_nid_postfix(-infinity) == 1);
    Require(__isinff_nid_postfix(nan) == 0 && __isinff_nid_postfix(1.f) == 0);
    Require(__isinf_nid_postfix(std::numeric_limits<double>::infinity()) == 1 && __isinf_nid_postfix(-std::numeric_limits<double>::infinity()) == 1);
    Require(__isinf_nid_postfix(std::numeric_limits<double>::quiet_NaN()) == 0 && __isinf_nid_postfix(std::numeric_limits<double>::max()) == 0);
    Require(__isinf_nid_postfix(0.) == 0 && __isinf_nid_postfix(1e308 * 10) == 1);
    Require(__isfinitef_nid_postfix(0.f) == 1 && __isfinitef_nid_postfix(infinity) == 0);
    Require(__isfinitef_nid_postfix(nan) == 0);
    Require(__isnormalf_nid_postfix(1.f) == 1 && __isnormalf_nid_postfix(0.f) == 0);
    Require(__isnormalf_nid_postfix(std::numeric_limits<float>::denorm_min()) == 0);
    Require(__isnormal_nid_postfix(1.) == 1 && __isnormal_nid_postfix(0.) == 0);
    Require(__isnormal_nid_postfix(std::numeric_limits<double>::denorm_min()) == 0);
    const auto quotient = div_nid_postfix(7, 2);
    Require(quotient.quot == 3 && quotient.rem == 1);
    const auto negativeNumerator = div_nid_postfix(-7, 2);
    Require(negativeNumerator.quot == -3 && negativeNumerator.rem == -1);
    const auto negativeDenominator = div_nid_postfix(7, -2);
    Require(negativeDenominator.quot == -3 && negativeDenominator.rem == 1);
    const auto minimum = div_nid_postfix(std::numeric_limits<int>::min(), 10);
    Require(minimum.quot == -214748364 && minimum.rem == -8);
    Require(abs_nid_postfix(0) == 0);
    Require(abs_nid_postfix(42) == 42);
    Require(abs_nid_postfix(-42) == 42);
    Require(abs_nid_postfix(std::numeric_limits<int>::max()) == std::numeric_limits<int>::max());
    Require(abs_nid_postfix(std::numeric_limits<int>::min()) == std::numeric_limits<int>::min());
    CheckHyperbolic();
    CheckHypotSpecialValue();
}
