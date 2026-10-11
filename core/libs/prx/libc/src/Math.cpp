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

long double JoinExtended(std::uint64_t mantissa, std::uint16_t signExponent) {
    unsigned char bytes[sizeof(long double)]{};
    std::memcpy(bytes, &mantissa, sizeof(mantissa));
    std::memcpy(bytes + sizeof(mantissa), &signExponent, sizeof(signExponent));
    long double value;
    std::memcpy(&value, bytes, sizeof(value));
    return value;
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

constexpr std::int32_t TwoOverPiBits[] = {
    0xA2F983, 0x6E4E44, 0x1529FC, 0x2757D1, 0xF534DD, 0xC0DB62,
    0x95993C, 0x439041, 0xFE5163, 0xABDEBB, 0xC561B7, 0x246E3A,
    0x424DD2, 0xE00649, 0x2EEA09, 0xD1921C, 0xFE1DEB, 0x1CB129,
    0xA73EE8, 0x8235F5, 0x2EBB44, 0x84E99C, 0x7026B4, 0x5F7E41,
    0x3991D6, 0x398353, 0x39F49C, 0x845F8B, 0xBDF928, 0x3B1FF8,
    0x97FFDE, 0x05980F, 0xEF2F11, 0x8B5A0A, 0x6D1F6D, 0x367ECF,
    0x27CB09, 0xB74F46, 0x3F669E, 0x5FEA2D, 0x7527BA, 0xC7EBE5,
    0xF17B3D, 0x0739F7, 0x8A5292, 0xEA6BFB, 0x5FB11F, 0x8D5D08,
    0x560330, 0x46FC7B, 0x6BABF0, 0xCFBC20, 0x9AF436, 0x1DA9E3,
    0x91615E, 0xE61B08, 0x659985, 0x5F14A0, 0x68408D, 0xFFD880,
    0x4D7327, 0x310606, 0x1556CA, 0x73A8C9, 0x60E27B, 0xC08C6B,
    0x47C419, 0xC367CD, 0xDCE809, 0x2A8359, 0xC4768B, 0x961CA6,
    0xDDAF44, 0xD15719, 0x053EA5, 0xFF0705, 0x3F7E33, 0xE832C2,
    0xDE4F98, 0x327DBB, 0xC33D26, 0xEF6B1E, 0x5EF89F, 0x3A1F35,
    0xCAF27F, 0x1D87F1, 0x21907C, 0x7C246A, 0xFA6ED5, 0x772D30,
    0x433B15, 0xC614B5, 0x9D19C3, 0xC2C4AD, 0x414D2C, 0x5D000C,
    0x467D86, 0x2D71E3, 0x9AC69B, 0x006233, 0x7CD2B4, 0x97A7B4,
    0xD55537, 0xF63ED7, 0x1810A3, 0xFC764D, 0x2A9D64, 0xABD770,
    0xF87C63, 0x57B07A, 0xE71517, 0x5649C0, 0xD9D63B, 0x3884A7,
    0xCB2324, 0x778AD6, 0x23545A, 0xB91F00, 0x1B0AF1, 0xDFCE19,
    0xFF319F, 0x6A1E66, 0x615799, 0x47FBAC, 0xD87F7E, 0xB76522,
    0x89E832, 0x60BFE6, 0xCDC4EF, 0x09366C, 0xD43F5D, 0xD7DE16,
    0xDE3B58, 0x929BDE, 0x2822D2, 0xE88628, 0x4D58E2, 0x32CAC6,
    0x16E308, 0xCB7DE0, 0x50C017, 0xA71DF3, 0x5BE018, 0x34132E,
    0x621283, 0x014883, 0x5B8EF5, 0x7FB0AD, 0xF2E91E, 0x434A48,
    0xD36710, 0xD8DDAA, 0x425FAE, 0xCE616A, 0xA4280A, 0xB499D3,
    0xF2A606, 0x7F775C, 0x83C2A3, 0x883C61, 0x78738A, 0x5A8CAF,
    0xBDD76F, 0x63A62D, 0xCBBFF4, 0xEF818D, 0x67C126, 0x45CA55,
    0x36D9CA, 0xD2A828, 0x8D61C2, 0x77C912, 0x142604, 0x9B4612,
    0xC459C4, 0x44C5C8, 0x91B24D, 0xF31700, 0xAD43D4, 0xE54929,
    0x10D5FD, 0xFCBE00, 0xCC941E, 0xEECE70, 0xF53E13, 0x80F1EC,
    0xC3E7B3, 0x28F8C7, 0x940593, 0x3E71C1, 0xB3092E, 0xF3450B,
    0x9C1288, 0x7B20AB, 0x9FB52E, 0xC29247, 0x2F327B, 0x6D550C,
    0x90A772, 0x1FE76B, 0x96CB31, 0x4A1679, 0xE27941, 0x89DFF4,
    0x9794E8, 0x84E6E2, 0x973199, 0x6BED88, 0x365F5F, 0x0EFDBB,
    0xB49A48, 0x6CA467, 0x427271, 0x325D8D, 0xB8159F, 0x09E5BC,
    0x25318D, 0x3974F7, 0x1C0530, 0x010C0D, 0x68084B, 0x58EE2C,
    0x90AA47, 0x02E774, 0x24D6BD, 0xA67DF7, 0x72486E, 0xEF169F,
    0xA6948E, 0xF691B4, 0x5153D1, 0xF20ACF, 0x339820, 0x7E4BF5,
    0x6863B2, 0x5F3EDD, 0x035D40, 0x7F8985, 0x295255, 0xC06437,
    0x10D86D, 0x324832, 0x754C5B, 0xD4714E, 0x6E5445, 0xC1090B,
    0x69F52A, 0xD56614, 0x9D0727, 0x50045D, 0xDB3BB4, 0xC576EA,
    0x17F987, 0x7D6B49, 0xBA271D, 0x296996, 0xACCCC6, 0x5414AD,
    0x6AE290, 0x89D988, 0x50722C, 0xBEA404, 0x940777, 0x7030F3,
    0x27FC00, 0xA871EA, 0x49C266, 0x3DE064, 0x83DD97, 0x973FA3,
    0xFD9443, 0x8C860D, 0xDE4131, 0x9D3992, 0x8C70DD, 0xE7B717,
    0x3BDF08, 0x2B3715, 0xA0805C, 0x93805A, 0x921110, 0xD8E80F,
    0xAF806C, 0x4BFFDB, 0x0F9038, 0x761859, 0x15A562, 0xBBCB61,
    0xB989C7, 0xBD4010, 0x04F2D2, 0x277549, 0xF6B6EB, 0xBB22DB,
    0xAA140A, 0x2F2689, 0x768364, 0x333B09, 0x1A940E, 0xAA3A51,
    0xC2A31D, 0xAEEDAF, 0x12265C, 0x4DC26D, 0x9C7A2D, 0x9756C0,
    0x833F03, 0xF6F009, 0x8C402B, 0x99316D, 0x07B439, 0x15200C,
    0x5BC3D8, 0xC492F5, 0x4BADC6, 0xA5CA4E, 0xCD37A7, 0x36A9E6,
    0x9492AB, 0x6842DD, 0xDE6319, 0xEF8C76, 0x528B68, 0x37DBFC,
    0xABA1AE, 0x3115DF, 0xA1AE00, 0xDAFB0C, 0x664D64, 0xB705ED,
    0x306529, 0xBF5657, 0x3AFF47, 0xB9F96A, 0xF3BE75, 0xDF9328,
    0x3080AB, 0xF68C66, 0x15CB04, 0x0622FA, 0x1DE4D9, 0xA4B33D,
    0x8F1B57, 0x09CD36, 0xE9424E, 0xA4BE13, 0xB52333, 0x1AAAF0,
    0xA8654F, 0xA5C1D2, 0x0F3F0B, 0xCD785B, 0x76F923, 0x048B7B,
    0x721789, 0x53A6C6, 0xE26E6F, 0x00EBEF, 0x584A9B, 0xB7DAC4,
    0xBA66AA, 0xCFCF76, 0x1D02D1, 0x2DF1B1, 0xC1998C, 0x77ADC3,
    0xDA4886, 0xA05DF7, 0xF480C6, 0x2FF0AC, 0x9AECDD, 0xBC5C3F,
    0x6DDED0, 0x1FC790, 0xB6DB2A, 0x3A25A3, 0x9AAF00, 0x9353AD,
    0x0457B6, 0xB42D29, 0x7E804B, 0xA707DA, 0x0EAA76, 0xA1597B,
    0x2A1216, 0x2DB7DC, 0xFDE5FA, 0xFEDB89, 0xFDBE89, 0x6C76E4,
    0xFCA906, 0x70803E, 0x156E85, 0xFF87FD, 0x073E28, 0x336761,
    0x86182A, 0xEABD4D, 0xAFE7B3, 0x6E6D8F, 0x396795, 0x5BBF31,
    0x48D784, 0x16DF30, 0x432DC7, 0x356125, 0xCE70C9, 0xB8CB30,
    0xFD6CBF, 0xA200A4, 0xE46C05, 0xA0DD5A, 0x476F21, 0xD21262,
    0x845CB9, 0x496170, 0xE0566B, 0x015299, 0x375550, 0xB7D51E,
    0xC4F133, 0x5F6E13, 0xE4305D, 0xA92E85, 0xC3B21D, 0x3632A1,
    0xA4B708, 0xD4B1EA, 0x21F716, 0xE4698F, 0x77FF27, 0x80030C,
    0x2D408D, 0xA0CD4F, 0x99A520, 0xD3A2B3, 0x0A5D2F, 0x42F9B4,
    0xCBDA11, 0xD0BE7D, 0xC1DB9B, 0xBD17AB, 0x81A2CA, 0x5C6A08,
    0x17552E, 0x550027, 0xF0147F, 0x8607E1, 0x640B14, 0x8D4196,
    0xDEBE87, 0x2AFDDA, 0xB6256B, 0x34897B, 0xFEF305, 0x9EBFB9,
    0x4F6A68, 0xA82A4A, 0x5AC44F, 0xBCF82D, 0x985AD7, 0x95C7F4,
    0x8D4D0D, 0xA63A20, 0x5F57A4, 0xB13F14, 0x953880, 0x0120CC,
    0x86DD71, 0xB6DEC9, 0xF560BF, 0x11654D, 0x6B0701, 0xACB08C,
    0xD0C0B2, 0x485551, 0x0EFB1E, 0xC37295, 0x3B06A3, 0x3540C0,
    0x7BDC06, 0xCC45E0, 0xFA294E, 0xC8CAD6, 0x41F3E8, 0xDE647C,
    0xD8649B, 0x31BED9, 0xC397A4, 0xD45877, 0xC5E369, 0x13DAF0,
    0x3C3ABA, 0x461846, 0x5F7555, 0xF5BDD2, 0xC6926E, 0x5D2EAC,
    0xED440E, 0x423E1C, 0x87C461, 0xE9FD29, 0xF3D6E7, 0xCA7C22,
    0x35916F, 0xC5E008, 0x8DD7FF, 0xE26A6E, 0xC6FDB0, 0xC10893,
    0x745D7C, 0xB2AD6B, 0x9D6ECD, 0x7B723E, 0x6A11C6, 0xA9CFF7,
    0xDF7329, 0xBAC9B5, 0x5100B7, 0x0DB2E2, 0x24BA74, 0x607DE5,
    0x8AD874, 0x2C150D, 0x0C1881, 0x94667E, 0x162901, 0x767A9F,
    0xBEFDFD, 0xEF4556, 0x367ED9, 0x13D9EC, 0xB9BA8B, 0xFC97C4,
    0x27A831, 0xC36EF1, 0x36C594, 0x56A8D8, 0xB5A8B4, 0x0ECCCF,
    0x2D8912, 0x34576F, 0x89562C, 0xE3CE99, 0xB920D6, 0xAA5E6B,
    0x9C2A3E, 0xCC5F11, 0x4A0BFD, 0xFBF4E1, 0x6D3B8E, 0x2C86E2,
    0x84D4E9, 0xA9B4FC, 0xD1EEEF, 0xC9352E, 0x61392F, 0x442138,
    0xC8D91B, 0x0AFC81, 0x6A4AFB, 0xD81C2F, 0x84B453, 0x8C994E,
    0xCC2254, 0xDC552A, 0xD6C6C0, 0x96190B, 0xB8701A, 0x649569,
    0x605A26, 0xEE523F, 0x0F117F, 0x11B5F4, 0xF5CBFC, 0x2DBC34,
    0xEEBC34, 0xCC5DE8, 0x605EDD, 0x9B8E67, 0xEF3392, 0xB817C9,
    0x9B5861, 0xBC57E1, 0xC68351, 0x103ED8, 0x4871DD, 0xDD1C2D,
    0xA118AF, 0x462C21, 0xD7F359, 0x987AD9, 0xC0549E, 0xFA864F,
    0xFC0656, 0xAE79E5, 0x362289, 0x22AD38, 0xDC9367, 0xAAE855,
    0x382682, 0x9BE7CA, 0xA40D51, 0xB13399, 0x0ED7A9, 0x480569,
    0xF0B265, 0xA7887F, 0x974C88, 0x36D1F9, 0xB39221, 0x4A827B,
    0x21CF98, 0xDC9F40, 0x5547DC, 0x3A74E1, 0x42EB67, 0xDF9DFE,
    0x5FD45E, 0xA4677B, 0x7AACBA, 0xA2F655, 0x23882B, 0x55BA41,
    0x086E59, 0x862A21, 0x834739, 0xE6E389, 0xD49EE5, 0x40FB49,
    0xE956FF, 0xCA0F1C, 0x8A59C5, 0x2BFA94, 0xC5C1D3, 0xCFC50F,
    0xAE5ADB, 0x86C547, 0x624385, 0x3B8621, 0x94792C, 0x876110,
    0x7B4C2A, 0x1A2C80, 0x12BF43, 0x902688, 0x893C78, 0xE4C4A8,
    0x7BDBE5, 0xC23AC4, 0xEAF426, 0x8A67F7, 0xBF920D, 0x2BA365,
    0xB1933D, 0x0B7CBD, 0xDC51A4, 0x63DD27, 0xDDE169, 0x19949A,
    0x9529A8, 0x28CE68, 0xB4ED09, 0x209F44, 0xCA984E, 0x638270,
    0x237C7E, 0x32B90F, 0x8EF5A7, 0xE75614, 0x08F121, 0x2A9DB5,
    0x4D7E6F, 0x5119A5, 0xABF9B5, 0xD6DF82, 0x61DD96, 0x023616,
    0x9F3AC4, 0xA1A283, 0x6DED72, 0x7A8D39, 0xA9B882, 0x5C326B,
    0x5B2746, 0xED3400, 0x7700D2, 0x55F4FC, 0x4D5901, 0x8071E0,
};

constexpr double PiOverTwoPieces[] = {
    1.57079625129699707031e+00, 7.54978941586159635335e-08, 5.39030252995776476554e-15, 3.28200341580791294123e-22,
    1.27065575308067607349e-29, 1.22933308981111328932e-36, 2.73370053816464559624e-44, 2.16741683877804819444e-51,
};

int ReducePiOverTwoLarge(const double* x, double* y, int e0, int nx) {
    constexpr int jk = 4, jp = 4;
    constexpr double two24 = 0x1p24, twon24 = 0x1p-24;
    std::int32_t iq[20];
    double f[20], fq[20], q[20];
    const int jx = nx - 1;
    const int jv = std::max((e0 - 3) / 24, 0);
    int q0 = e0 - 24 * (jv + 1);
    for (int i = 0, j = jv - jx; i <= jx + jk; i++, j++) f[i] = j < 0 ? 0.0 : static_cast<double>(TwoOverPiBits[j]);
    for (int i = 0; i <= jk; i++) {
        double fw = 0.0;
        for (int j = 0; j <= jx; j++) fw += x[j] * f[jx + i - j];
        q[i] = fw;
    }
    int jz = jk;
    int n;
    int ih;
    double z;
    for (;;) {
        z = q[jz];
        for (int i = 0, j = jz; j > 0; i++, j--) {
            const double fw = static_cast<double>(static_cast<std::int32_t>(twon24 * z));
            iq[i] = static_cast<std::int32_t>(z - two24 * fw);
            z = q[j - 1] + fw;
        }
        z = std::scalbn(z, q0);
        z -= 8.0 * std::floor(z * 0.125);
        n = static_cast<std::int32_t>(z);
        z -= static_cast<double>(n);
        ih = 0;
        if (q0 > 0) {
            const std::int32_t high = iq[jz - 1] >> (24 - q0);
            n += high;
            iq[jz - 1] -= high << (24 - q0);
            ih = iq[jz - 1] >> (23 - q0);
        } else if (q0 == 0) {
            ih = iq[jz - 1] >> 23;
        } else if (z >= 0.5) {
            ih = 2;
        }
        if (ih > 0) {
            n += 1;
            bool carry = false;
            for (int i = 0; i < jz; i++) {
                const std::int32_t j = iq[i];
                if (!carry) {
                    if (j != 0) {
                        carry = true;
                        iq[i] = 0x1000000 - j;
                    }
                } else {
                    iq[i] = 0xffffff - j;
                }
            }
            if (q0 == 1) iq[jz - 1] &= 0x7fffff;
            if (q0 == 2) iq[jz - 1] &= 0x3fffff;
            if (ih == 2) {
                z = 1.0 - z;
                if (carry) z -= std::scalbn(1.0, q0);
            }
        }
        if (z != 0.0) break;
        std::int32_t bits = 0;
        for (int i = jz - 1; i >= jk; i--) bits |= iq[i];
        if (bits != 0) break;
        int k = 1;
        while (iq[jk - k] == 0) k++;
        for (int i = jz + 1; i <= jz + k; i++) {
            f[jx + i] = static_cast<double>(TwoOverPiBits[jv + i]);
            double fw = 0.0;
            for (int j = 0; j <= jx; j++) fw += x[j] * f[jx + i - j];
            q[i] = fw;
        }
        jz += k;
    }
    if (z == 0.0) {
        jz -= 1;
        q0 -= 24;
        while (iq[jz] == 0) {
            jz--;
            q0 -= 24;
        }
    } else {
        z = std::scalbn(z, -q0);
        if (z >= two24) {
            const double fw = static_cast<double>(static_cast<std::int32_t>(twon24 * z));
            iq[jz] = static_cast<std::int32_t>(z - two24 * fw);
            jz += 1;
            q0 += 24;
            iq[jz] = static_cast<std::int32_t>(fw);
        } else {
            iq[jz] = static_cast<std::int32_t>(z);
        }
    }
    double fw = std::scalbn(1.0, q0);
    for (int i = jz; i >= 0; i--) {
        q[i] = fw * static_cast<double>(iq[i]);
        fw *= twon24;
    }
    for (int i = jz; i >= 0; i--) {
        fw = 0.0;
        for (int k = 0; k <= jp && k <= jz - i; k++) fw += PiOverTwoPieces[k] * q[i + k];
        fq[jz - i] = fw;
    }
    fw = 0.0;
    for (int i = jz; i >= 0; i--) fw += fq[i];
    y[0] = ih == 0 ? fw : -fw;
    fw = fq[0] - fw;
    for (int i = 1; i <= jz; i++) fw += fq[i];
    y[1] = ih == 0 ? fw : -fw;
    return n & 7;
}

int ReducePiOverTwo(long double x, long double* y) {
    constexpr double piOverTwo1 = 1.57079632679597125389e+00, piOverTwo2 = -1.07463465549783099519e-12, piOverTwo3 = 6.36831716351370313614e-25;
    const long double twoOverPi = static_cast<long double>(6.3661977236758138e-01) + -3.9356538861223811e-17;
    const long double piOverTwo1Tail = static_cast<long double>(-1.0746346554971943e-12) + 8.8451028997905949e-29;
    const long double piOverTwo2Tail = static_cast<long double>(6.3683171635109499e-25) + 2.3183081793789774e-41;
    const long double piOverTwo3Tail = static_cast<long double>(-2.7529965190440717e-37) + -4.2006647512740502e-54;
    const ExtendedParts parts = SplitExtended(x);
    const int ex = parts.signExponent & 0x7fff;
    if (ex < 0x3fff + 25 || (ex == 0x3fff + 25 && (parts.mantissa >> 32) < 0xc90fdaa2u)) {
        long double fn = x * twoOverPi + 0x1.8p63L;
        fn = fn - 0x1.8p63L;
        const int n = static_cast<int>(fn);
        long double r = x - fn * piOverTwo1;
        long double w = fn * piOverTwo1Tail;
        y[0] = r - w;
        if (ex - (SplitExtended(y[0]).signExponent & 0x7fff) > 22) {
            long double t = r;
            w = fn * piOverTwo2;
            r = t - w;
            w = fn * piOverTwo2Tail - ((t - r) - w);
            y[0] = r - w;
            if (ex - (SplitExtended(y[0]).signExponent & 0x7fff) > 61) {
                t = r;
                w = fn * piOverTwo3;
                r = t - w;
                w = fn * piOverTwo3Tail - ((t - r) - w);
                y[0] = r - w;
            }
        }
        y[1] = (r - y[0]) - w;
        return n;
    }
    const int e0 = ex - 0x3fff - 23;
    long double z = JoinExtended(parts.mantissa, static_cast<std::uint16_t>(ex - e0));
    double tx[3];
    for (int i = 0; i < 2; i++) {
        tx[i] = static_cast<double>(static_cast<std::int32_t>(z));
        z = (z - tx[i]) * 0x1p24;
    }
    tx[2] = static_cast<double>(z);
    int nx = 3;
    while (tx[nx - 1] == 0.0) nx--;
    double ty[2];
    const int n = ReducePiOverTwoLarge(tx, ty, e0, nx);
    const long double r = static_cast<long double>(ty[0]) + ty[1];
    const long double w = ty[1] - (r - ty[0]);
    if ((parts.signExponent & 0x8000) != 0) {
        y[0] = -r;
        y[1] = -w;
        return -n;
    }
    y[0] = r;
    y[1] = w;
    return n;
}

long double KernelTan(long double x, long double y, bool odd) {
    const long double t3 = static_cast<long double>(0.33333333333333331) + 1.8350121769317163e-17;
    const long double t5 = static_cast<long double>(0.13333333333333336) + 1.3051083651294260e-17;
    const long double t7 = static_cast<long double>(0.053968253968250494) + 3.1509625637859973e-18;
    const long double pio4 = static_cast<long double>(0.78539816339744828) + 3.0628711372715500e-17;
    const long double pio4lo = static_cast<long double>(-1.2541394031670831e-20) + 6.1493048227390915e-37;
    constexpr double t9 = 0.021869488536312216, t11 = 0.0088632355256619590, t13 = 0.0035921281113786528, t15 = 0.0014558334756312418;
    constexpr double t17 = 0.00059003538700862256, t19 = 0.00023907843576635544, t21 = 0.000097154625656538905, t23 = 0.000038440165747303162;
    constexpr double t25 = 0.000018082171885432524, t27 = 0.0000024196006108814377, t29 = 0.0000078293456938132840, t31 = -0.0000032609076735050182;
    constexpr double t33 = 0.0000023261313142559411;
    const int iy = odd ? -1 : 1;
    const long double osign = x >= 0 ? 1.0L : -1.0L;
    bool reflected = false;
    if (std::fabs(x) >= 0.67434) {
        if (x < 0) {
            x = -x;
            y = -y;
        }
        x = (pio4 - x) + (pio4lo - y);
        y = 0.0L;
        reflected = true;
    }
    long double z = x * x;
    long double w = z * z;
    long double r = t5 + w * (t9 + w * (t13 + w * (t17 + w * (t21 + w * (t25 + w * (t29 + w * t33))))));
    long double v = z * (t7 + w * (t11 + w * (t15 + w * (t19 + w * (t23 + w * (t27 + w * t31))))));
    long double s = z * x;
    r = y + z * (s * (r + v) + y);
    r += t3 * s;
    w = x + r;
    if (reflected) {
        v = static_cast<long double>(iy);
        return osign * (v - 2.0L * (x - (w * w / (w + v) - r)));
    }
    if (iy == 1) return w;
    z = w;
    z = z + 0x1p32L - 0x1p32L;
    v = r - (z - x);
    const long double a = -1.0L / w;
    long double t = a;
    t = t + 0x1p32L - 0x1p32L;
    s = 1.0L + t * z;
    return t + a * (s + t * v);
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

long double APS5_VABI tanl_nid_postfix(long double x) {
    const ExtendedParts parts = SplitExtended(x);
    const int exponent = parts.signExponent & 0x7fff;
    if (exponent == 0) return x;
    if (exponent == 0x7fff) {
        if (parts.mantissa == 0) RaiseLibcError(FE_INVALID);
        return (x - x) / (x - x);
    }
    if ((parts.mantissa >> 63) == 0) {
        if (exponent > 0x3fff + 25) throw std::domain_error("tanl: unnormal argument above 2^25 * pi / 2 is not supported");
        return (x - x) / (x - x);
    }
    const long double magnitude = JoinExtended(parts.mantissa, static_cast<std::uint16_t>(exponent));
    if (magnitude < 0.7853981633974483) {
        const long double result = KernelTan(magnitude, 0.0L, false);
        return (parts.signExponent & 0x8000) != 0 ? -result : result;
    }
    long double reduced[2];
    const int quadrant = ReducePiOverTwo(x, reduced);
    return KernelTan(reduced[0], reduced[1], (quadrant & 1) != 0);
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
