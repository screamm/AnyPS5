#include <mutex>
#include <algorithm>
#include <bit>
#include <cstdint>
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>

#include "prx/libc/include/General.hpp"
#include "prx/libc/include/SonyCrt.hpp"

namespace {

template<typename TFloat>
std::int64_t RoundToInt64(TFloat value) {
    const TFloat rounded = std::round(value);
    if (!(rounded >= static_cast<TFloat>(INT64_MIN) && rounded < static_cast<TFloat>(INT64_MAX))) {
        std::feraiseexcept(FE_INVALID);
        return INT64_MIN;
    }
    return static_cast<std::int64_t>(rounded);
}

double X87NanOperand(double x, double y) {
    constexpr std::uint64_t Quiet = 0x0008000000000000ull, Fraction = 0x000fffffffffffffull;
    const std::uint64_t xBits = std::bit_cast<std::uint64_t>(x) | Quiet;
    const std::uint64_t yBits = std::bit_cast<std::uint64_t>(y) | Quiet;
    if (!std::isnan(y)) return std::bit_cast<double>(xBits);
    if (!std::isnan(x)) return std::bit_cast<double>(yBits);
    if ((xBits & Fraction) != (yBits & Fraction)) return std::bit_cast<double>((xBits & Fraction) > (yBits & Fraction) ? xBits : yBits);
    return std::bit_cast<double>(std::min(xBits, yBits));
}

void RaiseLibcError(int exceptions) {
    if ((exceptions & (FE_OVERFLOW | FE_UNDERFLOW)) != 0) exceptions |= FE_INEXACT;
    std::feraiseexcept(exceptions);
    const bool range = (exceptions & FE_INVALID) == 0 && (exceptions & (FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW)) != 0;
    errno = range ? ERANGE : EDOM;
}

static_assert(std::numeric_limits<long double>::digits == 64, "guest long double is the x87 80-bit format");

struct ExtendedParts {
    std::uint64_t mantissa;
    std::uint16_t signExponent;
};

ExtendedParts SplitExtended(long double value) {
    unsigned char bytes[sizeof(long double)];
    std::memcpy(bytes, &value, sizeof(value));
    ExtendedParts parts;
    std::memcpy(&parts.mantissa, bytes, sizeof(parts.mantissa));
    std::memcpy(&parts.signExponent, bytes + sizeof(parts.mantissa), sizeof(parts.signExponent));
    return parts;
}

constexpr long double AtanHigh[] = {4.63647609000806116202e-01L, 7.85398163397448309628e-01L, 9.82793723247329067960e-01L, 1.57079632679489661926e+00L};
constexpr long double AtanLow[] = {1.18469937025062860669e-20L, -1.25413940316708300586e-20L, 2.55232234165405176172e-20L, -2.50827880633416601173e-20L};
constexpr long double AtanCoefficients[] = {
    3.33333333333333333017e-01L, -1.99999999999999632011e-01L, 1.42857142857046531280e-01L, -1.11111111100562372733e-01L,
    9.09090902935647302252e-02L, -7.69230552476207730353e-02L, 6.66661718042406260546e-02L, -5.88158892835030888692e-02L,
    5.25499891539726639379e-02L, -4.70119845393155721494e-02L, 4.03539201366454414072e-02L, -2.91303858419364158725e-02L,
    1.24822046299269234080e-02L,
};

long double AtanEven(long double x) {
    const auto& t = AtanCoefficients;
    return t[0] + x * (t[2] + x * (t[4] + x * (t[6] + x * (t[8] + x * (t[10] + x * t[12])))));
}

long double AtanOdd(long double x) {
    const auto& t = AtanCoefficients;
    return t[1] + x * (t[3] + x * (t[5] + x * (t[7] + x * (t[9] + x * t[11]))));
}

}

extern "C" {

std::lldiv_t APS5_VABI lldiv_nid_postfix(long long numerator, long long denominator) {
    return std::lldiv(numerator, denominator);
}

std::lldiv_t APS5_VABI ldiv_nid_postfix(std::int64_t numerator, std::int64_t denominator) {
    return std::lldiv(numerator, denominator);
}

float APS5_VABI fmodf_nid_postfix(float x, float y) { return std::fmod(x, y); }
float APS5_VABI asinf_nid_postfix(float x) { return std::asin(x); }
float APS5_VABI acosf_nid_postfix(float x) { return std::acos(x); }
float APS5_VABI atan2f_nid_postfix(float y, float x) { return std::atan2(y, x); }
float APS5_VABI hypotf_nid_postfix(float x, float y) { return static_cast<float>(std::hypot(static_cast<double>(x), static_cast<double>(y))); }
double APS5_VABI hypot_nid_postfix(double x, double y) {
    const std::uint64_t hx = std::bit_cast<std::uint64_t>(x);
    const std::uint64_t hy = std::bit_cast<std::uint64_t>(y);
    const std::uint64_t ix = hx & 0x7fffffffffffffffull;
    const std::uint64_t iy = hy & 0x7fffffffffffffffull;
    if (ix == 0x7ff0000000000000ull || iy == 0x7ff0000000000000ull) {
        return std::numeric_limits<double>::infinity();
    }
    return std::hypot(x, y);
}
float APS5_VABI tanf_nid_postfix(float x) { return std::tan(x); }
float APS5_VABI log10f_nid_postfix(float x) { return std::log10(x); }
float APS5_VABI logbf_nid_postfix(float x) { return std::logb(x); }
double APS5_VABI exp2_nid_postfix(double x) { return std::exp2(x); }
double APS5_VABI ldexp_nid_postfix(double x, int exponent) { return std::ldexp(x, exponent); }
long double APS5_VABI ldexpl_nid_postfix(long double x, int exponent) {
    const int previousErrno = errno;
    const long double result = std::scalbn(x, exponent);
    errno = previousErrno;
    return result;
}

double APS5_VABI logb_nid_postfix(double x) {
    const std::uint64_t bits = std::bit_cast<std::uint64_t>(x);
    const std::uint32_t high = static_cast<std::uint32_t>(bits >> 32) & 0x7fffffffu;
    if ((high | static_cast<std::uint32_t>(bits)) == 0) return -1.0 / std::fabs(x);
    if (high >= 0x7ff00000u) return x * x;
    if (high < 0x00100000u) {
        const std::uint64_t normalized = std::bit_cast<std::uint64_t>(x * 0x1p54);
        return static_cast<double>(static_cast<int>((normalized >> 52) & 0x7ff) - 1023 - 54);
    }
    return static_cast<double>(static_cast<int>(high >> 20) - 1023);
}

double APS5_VABI remainder_nid_postfix(double x, double y) {
    const bool xNan = std::isnan(x);
    const bool yNan = std::isnan(y);
    if ((std::isinf(x) && !yNan) || (y == 0.0 && !xNan)) RaiseLibcError(FE_INVALID);
    if (xNan || yNan) return X87NanOperand(x, y);
    if (std::isinf(x) || y == 0.0) return std::bit_cast<double>(0xfff8000000000000ull);
    if (std::isinf(y)) return x;
    const std::uint64_t xBits = std::bit_cast<std::uint64_t>(x);
    const std::uint64_t sign = xBits & 0x8000000000000000ull;
    const double divisor = std::fabs(y);
    if (divisor <= 0x1.fffffffffffffp1022) x = std::fmod(x, divisor + divisor);
    if ((xBits & 0x7fffffffffffffffull) == std::bit_cast<std::uint64_t>(divisor)) return 0.0 * x;
    x = std::fabs(x);
    if (divisor < 0x1p-1021) {
        if (x + x > divisor) {
            x -= divisor;
            if (x + x >= divisor) x -= divisor;
        }
    } else {
        const double half = 0.5 * divisor;
        if (x > half) {
            x -= divisor;
            if (x >= half) x -= divisor;
        }
    }
    const std::uint64_t resultBits = std::bit_cast<std::uint64_t>(x);
    return std::bit_cast<double>(((resultBits & 0x7fffffffffffffffull) == 0 ? 0 : resultBits) ^ sign);
}

double APS5_VABI scalbn_nid_postfix(double x, int exponent) { return std::scalbn(x, exponent); }
float APS5_VABI scalbnf_nid_postfix(float x, int exponent) { return std::scalbn(x, exponent); }
double APS5_VABI frexp_nid_postfix(double x, int* exponent) { return std::frexp(x, exponent); }
float APS5_VABI frexpf_nid_postfix(float x, int* exponent) { return std::frexp(x, exponent); }
// Guest long is 64-bit, including on Windows where native long is 32-bit.
std::int64_t APS5_VABI lround_nid_postfix(double x) { return RoundToInt64(x); }
std::int64_t APS5_VABI lroundf_nid_postfix(float x) { return RoundToInt64(x); }
std::int64_t APS5_VABI llround_nid_postfix(double x) { return RoundToInt64(x); }
std::int64_t APS5_VABI llroundf_nid_postfix(float x) { return RoundToInt64(x); }
int APS5_VABI __isfinitef_nid_postfix(float x) { return std::isfinite(x) ? 1 : 0; }
int APS5_VABI __isnormal_nid_postfix(double x) { return std::isnormal(x) ? 1 : 0; }
int APS5_VABI __isnormalf_nid_postfix(float x) { return std::isnormal(x) ? 1 : 0; }
int APS5_VABI __isinff_nid_postfix(float x) { return std::isinf(x) ? 1 : 0; }

double APS5_VABI cbrt_nid_postfix(double x) { return std::cbrt(x); }
double APS5_VABI asin_nid_postfix(double x) { return std::asin(x); }
double APS5_VABI acos_nid_postfix(double x) { return std::acos(x); }
double APS5_VABI sqrt_nid_postfix(double x) { return std::sqrt(x); }
float APS5_VABI sqrtf_nid_postfix(float x) { return std::sqrt(x); }
double APS5_VABI fabs_nid_postfix(double x) { return std::fabs(x); }
int APS5_VABI abs_nid_postfix(int x) { return x < 0 ? static_cast<int>(0u - static_cast<unsigned>(x)) : x; }
double APS5_VABI trunc_nid_postfix(double x) { return std::trunc(x); }
double APS5_VABI fma_nid_postfix(double x, double y, double z) { return std::fma(x, y, z); }
long double APS5_VABI acosl_nid_postfix(long double x) { return std::acos(x); }

long double APS5_VABI atanl_nid_postfix(long double x) {
    const ExtendedParts parts = SplitExtended(x);
    const bool negative = (parts.signExponent & 0x8000) != 0;
    const int exponent = parts.signExponent & 0x7fff;
    if (exponent >= 0x3fff + 65) {
        if (exponent == 0x7fff && (parts.mantissa & 0x7fffffffffffffffull) != 0) return x + x;
        return negative ? -AtanHigh[3] - AtanLow[3] : AtanHigh[3] + AtanLow[3];
    }
    const int exponentAndMantissa = (exponent << 8) | static_cast<int>((parts.mantissa >> 55) & 0xff);
    int id;
    if (exponentAndMantissa < ((0x3fff - 2) << 8) + 0xc0) {
        if (exponent < 0x3fff - 32 && 1.0e300 + x > 1.0L) return x;
        id = -1;
    } else {
        x = std::fabs(x);
        if (exponentAndMantissa < (0x3fff << 8) + 0x30) {
            if (exponentAndMantissa < ((0x3fff - 1) << 8) + 0x60) {
                id = 0;
                x = (2.0L * x - 1.0L) / (2.0L + x);
            } else {
                id = 1;
                x = (x - 1.0L) / (x + 1.0L);
            }
        } else if (exponentAndMantissa < ((0x3fff + 1) << 8) + 0x38) {
            id = 2;
            x = (x - 1.5L) / (1.0L + 1.5L * x);
        } else {
            id = 3;
            x = -1.0L / x;
        }
    }
    const long double z = x * x;
    const long double w = z * z;
    const long double s1 = z * AtanEven(w);
    const long double s2 = w * AtanOdd(w);
    if (id < 0) return x - x * (s1 + s2);
    const long double result = AtanHigh[id] - ((x * (s1 + s2) - AtanLow[id]) - x);
    return negative ? -result : result;
}

long double APS5_VABI frexpl_nid_postfix(long double x, int* exponent) {
    if (!std::isfinite(x)) return x;
    return std::frexp(x, exponent);
}
double APS5_VABI exp_nid_postfix(double x) { return std::exp(x); }
double APS5_VABI atan_nid_postfix(double x) { return std::atan(x); }
double APS5_VABI tan_nid_postfix(double x) { return std::tan(x); }
double APS5_VABI log2_nid_postfix(double x) { return std::log2(x); }
double APS5_VABI log_nid_postfix(double x) { return std::log(x); }
double APS5_VABI log1p_nid_postfix(double x) { return std::log1p(x); }
float APS5_VABI log1pf_nid_postfix(float x) { return std::log1p(x); }
double APS5_VABI expm1_nid_postfix(double x) { return std::expm1(x); }

float APS5_VABI sinf_nid_postfix(float x) { return std::sin(x); }
float APS5_VABI cosf_nid_postfix(float x) { return std::cos(x); }

void APS5_VABI sincosf_nid_postfix(float x, float* sinp, float* cosp) {
    *sinp = std::sin(x);
    *cosp = std::cos(x);
}

double APS5_VABI sin_nid_postfix(double x) { return std::sin(x); }
double APS5_VABI cos_nid_postfix(double x) { return std::cos(x); }
double APS5_VABI sinh_nid_postfix(double x) { return std::sinh(x); }
double APS5_VABI cosh_nid_postfix(double x) { return std::cosh(x); }
float APS5_VABI coshf_nid_postfix(float x) { return std::cosh(x); }
double APS5_VABI asinh_nid_postfix(double x) { return std::asinh(x); }
float APS5_VABI asinhf_nid_postfix(float x) { return std::asinh(x); }
double APS5_VABI acosh_nid_postfix(double x) { return std::acosh(x); }
float APS5_VABI acoshf_nid_postfix(float x) { return std::acosh(x); }
double APS5_VABI atanh_nid_postfix(double x) { return std::atanh(x); }

void APS5_VABI sincos_nid_postfix(double x, double* sinp, double* cosp) {
    *sinp = std::sin(x);
    *cosp = std::cos(x);
}

float APS5_VABI atanf_nid_postfix(float x) { return std::atan(x); }
double APS5_VABI atan2_nid_postfix(double y, double x) { return std::atan2(y, x); }
float APS5_VABI powf_nid_postfix(float base, float exp) { return std::pow(base, exp); }
double APS5_VABI pow_nid_postfix(double base, double exp) { return std::pow(base, exp); }
float APS5_VABI expf_nid_postfix(float x) { return std::exp(x); }
float APS5_VABI exp2f_nid_postfix(float x) { return std::exp2(x); }
float APS5_VABI logf_nid_postfix(float x) { return std::log(x); }
float APS5_VABI log2f_nid_postfix(float x) { return std::log2(x); }
double APS5_VABI log10_nid_postfix(double x) { return std::log10(x); }
float APS5_VABI ldexpf_nid_postfix(float x, int exp) { return std::ldexp(x, exp); }
double APS5_VABI fmod_nid_postfix(double x, double y) { return std::fmod(x, y); }
float APS5_VABI roundf_nid_postfix(float x) { return std::round(x); }
double APS5_VABI round_nid_postfix(double x) { return std::round(x); }
float APS5_VABI cbrtf_nid_postfix(float x) { return std::cbrt(x); }
float APS5_VABI remainderf_nid_postfix(float x, float y) { return std::remainder(x, y); }
float APS5_VABI nextafterf_nid_postfix(float x, float y) { return std::nextafter(x, y); }
int APS5_VABI __isfinite_nid_postfix(double x) { return std::isfinite(x) ? 1 : 0; }
int APS5_VABI __isnan_nid_postfix(double x) { return std::isnan(x) ? 1 : 0; }
int APS5_VABI __isinf_nid_postfix(double x) { return std::isinf(x) ? 1 : 0; }
int APS5_VABI __signbit_nid_postfix(double x) { return std::signbit(x) ? 1 : 0; }
float APS5_VABI nanf_nid_postfix(const char* tagp) {
    (void)tagp;
    return std::bit_cast<float>(0x7fc00000u);
}
double APS5_VABI nan_nid_postfix(const char* tagp) {
    (void)tagp;
    return std::bit_cast<double>(0x7ff8000000000000ull);
}

double APS5_VABI modf_nid_postfix(double x, double* integral) { return std::modf(x, integral); }
float APS5_VABI modff_nid_postfix(float x, float* integral) { return std::modf(x, integral); }
double APS5_VABI tanh_nid_postfix(double x) { return std::tanh(x); }
float APS5_VABI tanhf_nid_postfix(float x) { return std::tanh(x); }
float APS5_VABI _FSinh_nid_postfix(float x, float y) { return y * std::sinh(x); }
float APS5_VABI _FCosh_nid_postfix(float x, float y) { return y * std::cosh(x); }

struct alignas(16) LibcFloatConstant { std::uint32_t bits[4]; };
LibcFloatConstant _FInf_nid_postfix {{0x7f800000u, 0, 0, 0}};
LibcFloatConstant _FNan_nid_postfix {{0x7fc00000u, 0, 0, 0}};

short APS5_VABI _FDtest_nid_postfix(const float* value) {
    constexpr short Denormal = -2, Finite = -1, Zero = 0, Infinite = 1, NotANumber = 2;
    if (value == nullptr) throw std::invalid_argument("_FDtest: null value");
    std::uint32_t bits;
    std::memcpy(&bits, value, sizeof(bits));
    const auto exponent = bits & 0x7f800000u;
    const auto fraction = bits & 0x007fffffu;
    if (exponent == 0x7f800000u) return fraction != 0 ? NotANumber : Infinite;
    if (exponent == 0) return fraction != 0 ? Denormal : Zero;
    return Finite;
}
int APS5_VABI __isnanf_nid_postfix(float x) { return std::isnan(x) ? 1 : 0; }
int APS5_VABI __fpclassifyf_nid_postfix(float x) {
    constexpr int Infinite = 0x01, NotANumber = 0x02, Normal = 0x04, Subnormal = 0x08, Zero = 0x10;
    std::uint32_t bits;
    std::memcpy(&bits, &x, sizeof(bits));
    const auto exponent = bits & 0x7f800000u;
    const auto fraction = bits & 0x007fffffu;
    if (exponent == 0) return fraction != 0 ? Subnormal : Zero;
    if (exponent == 0x7f800000u) return fraction != 0 ? NotANumber : Infinite;
    return Normal;
}
int APS5_VABI __signbitf_nid_postfix(float x) { return std::signbit(x) ? 1 : 0; }

static std::mutex g_randLock;
static std::uint32_t g_randState = 1;

static int NextRand_nid_no_patch(std::uint32_t& state) {
    const std::int64_t x = static_cast<std::int64_t>(state % 0x7ffffffeu) + 1;
    std::int64_t next = 16807 * (x % 127773) - 2836 * (x / 127773);
    if (next < 0) next += 0x7fffffff;
    state = static_cast<std::uint32_t>(next - 1);
    return static_cast<int>(next - 1);
}

int APS5_VABI rand_nid_postfix() {
    std::lock_guard lock(g_randLock);
    return NextRand_nid_no_patch(g_randState);
}

int APS5_VABI rand_r_nid_postfix(unsigned int* state) {
    if (state == nullptr) throw std::invalid_argument("rand_r: null state");
    std::uint32_t value = *state;
    const int result = NextRand_nid_no_patch(value);
    *state = value;
    return result;
}

void APS5_VABI srand_nid_postfix(unsigned int seed) {
    std::lock_guard lock(g_randLock);
    g_randState = seed;
}

int APS5_VABI __fpclassifyd_nid_postfix(double value) {
    return SonyFpclassifyd(value);
}

double APS5_VABI _Sinh_nid_postfix(double x, double y) {
    return SonySinh(x, y);
}

int APS5_VABI _Fltrounds_nid_postfix(void) {
    return SonyFltrounds();
}

}
