#include <atomic>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>

#include "prx/libc/include/GuestLocale.hpp"

extern "C" {
void APS5_VABI _ZdlPv_nid_postfix(void* pointer);
extern const std::byte _ZTVN10__cxxabiv117__class_type_infoE_nid_postfix[];
extern const std::byte _ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix[];
}

namespace {

using GuestLocale::Facet;
using GuestLocale::InputIterator;
using GuestLocale::IosBase;
using GuestLocale::StreamBuffer;

constexpr std::int32_t EndOfFile = -1;
constexpr std::int32_t EofBit = 1;
constexpr std::int32_t FailBit = 2;
constexpr std::int32_t BaseField = 0x70;
constexpr std::int32_t OctalBase = 0x20;
constexpr std::int32_t HexadecimalBase = 0x40;
constexpr std::int32_t BoolAlpha = 0x8000;
constexpr std::size_t IntegerDigits = 32;
constexpr std::size_t TypeInfoHeader = 2 * sizeof(void*);
constexpr int RangeError = 34;

struct TypeRecord {
    const void* vtable;
    const char* name;
    const TypeRecord* base;
};

const TypeRecord g_facetBaseType{_ZTVN10__cxxabiv117__class_type_infoE_nid_postfix + TypeInfoHeader, "St11_Facet_base", nullptr};
const TypeRecord g_facetType{_ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix + TypeInfoHeader, "NSt6locale5facetE", &g_facetBaseType};
const TypeRecord g_numGetType{_ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix + TypeInfoHeader, "St7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE", &g_facetType};

std::int32_t Available(const StreamBuffer* buffer, const char* next) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(*buffer->readEnd) - reinterpret_cast<std::uintptr_t>(next)));
}

std::int32_t Peek(StreamBuffer* buffer) {
    const char* const next = *buffer->readNext;
    if (next != nullptr && Available(buffer, next) > 0) return static_cast<unsigned char>(*next);
    return buffer->vtable->underflow(buffer);
}

bool AtEnd(const InputIterator& iterator) {
    return iterator.buffer == nullptr || Peek(iterator.buffer) == EndOfFile;
}

bool Same(const InputIterator& left, const InputIterator& right) {
    return AtEnd(left) == AtEnd(right);
}

char Current(InputIterator& iterator) {
    if (!iterator.got) {
        std::int32_t character = EndOfFile;
        if (iterator.buffer != nullptr && (character = Peek(iterator.buffer)) != EndOfFile) iterator.value = static_cast<char>(character);
        else iterator.buffer = nullptr;
        iterator.got = true;
    }
    return iterator.value;
}

void Advance(InputIterator& iterator) {
    StreamBuffer* const buffer = iterator.buffer;
    if (buffer == nullptr) {
        iterator.got = true;
        return;
    }
    char* const next = *buffer->readNext;
    if (next != nullptr && Available(buffer, next) > 0) {
        *buffer->readNext = next + 1;
        iterator.got = false;
    } else if (buffer->vtable->uflow(buffer) == EndOfFile) {
        iterator.buffer = nullptr;
        iterator.got = true;
    } else {
        iterator.got = false;
    }
}

void MarkEnd(const InputIterator& first, const InputIterator& last, std::int32_t* state) {
    if (Same(first, last)) *state |= EofBit;
}

bool IsSpace(char character) {
    return character == ' ' || (character >= '\t' && character <= '\r');
}

char Lower(char character) {
    return character >= 'A' && character <= 'Z' ? static_cast<char>(character - 'A' + 'a') : character;
}

void RequireStream(const IosBase* stream, const std::int32_t* state, const void* value, const char* function) {
    if (stream == nullptr || state == nullptr || value == nullptr) throw std::invalid_argument(std::string(function) + ": null argument");
    if (stream->locale == nullptr || *stream->locale == nullptr || (*stream->locale)->name == nullptr || std::strcmp((*stream->locale)->name, "C") != 0) {
        throw std::invalid_argument(std::string(function) + ": only the C locale is supported");
    }
}

std::uint64_t ToUnsigned(const char* text, const char** end, int base, int* error) {
    static constexpr char Digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static constexpr std::int8_t SignificantDigits[37] {0, 0, 65, 41, 33, 28, 25, 23, 22, 21, 20, 19, 18, 18, 17, 17, 17, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 13};
    *error = 0;
    const char* cursor = text;
    while (IsSpace(*cursor)) ++cursor;
    char sign = *cursor == '-' || *cursor == '+' ? *cursor++ : '+';
    if (base < 0 || base == 1 || base > 36) {
        *end = text;
        return 0;
    }
    if (base > 0) {
        if (base == 16 && cursor[0] == '0' && (cursor[1] == 'x' || cursor[1] == 'X')) cursor += 2;
    } else if (cursor[0] != '0') {
        base = 10;
    } else if (cursor[1] == 'x' || cursor[1] == 'X') {
        base = 16;
        cursor += 2;
    } else {
        base = 8;
    }
    const char* const start = cursor;
    while (*cursor == '0') ++cursor;
    const char* const significant = cursor;
    std::uint64_t value = 0;
    std::uint64_t previous = 0;
    std::uint64_t digit = 0;
    for (const char* found; *cursor != '\0' && (found = static_cast<const char*>(std::memchr(Digits, Lower(*cursor), static_cast<std::size_t>(base)))) != nullptr; ++cursor) {
        previous = value;
        digit = static_cast<std::uint64_t>(found - Digits);
        value = value * static_cast<std::uint64_t>(base) + digit;
    }
    if (start == cursor) {
        *end = text;
        return 0;
    }
    const std::ptrdiff_t excess = (cursor - significant) - SignificantDigits[base];
    if (excess >= 0 && (excess > 0 || value < value - digit || (value - digit) / static_cast<std::uint64_t>(base) != previous)) {
        errno = RangeError;
        *error = 1;
        value = std::numeric_limits<std::uint64_t>::max();
        sign = '+';
    }
    *end = cursor;
    return sign == '-' ? 0 - value : value;
}

std::int64_t ToSigned(const char* text, const char** end, int base, int* error) {
    const char* cursor = text;
    while (IsSpace(*cursor)) ++cursor;
    const char sign = *cursor == '-' || *cursor == '+' ? *cursor++ : '+';
    const std::uint64_t value = ToUnsigned(cursor, end, base, error);
    if (cursor == *end) *end = text;
    constexpr std::uint64_t largest = static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
    if ((text == *end && value != 0) || (sign == '+' && value > largest) || (sign == '-' && value > largest + 1)) {
        errno = RangeError;
        *error = 1;
        return sign == '-' ? std::numeric_limits<std::int64_t>::min() : std::numeric_limits<std::int64_t>::max();
    }
    return sign == '-' ? static_cast<std::int64_t>(0 - value) : static_cast<std::int64_t>(value);
}

int GatherInteger(char* field, InputIterator& first, InputIterator& last, std::int32_t flags) {
    static constexpr char Source[] = "0123456789ABCDEFabcdef-+Xx";
    char* cursor = field;
    if (!Same(first, last)) {
        if (Current(first) == '+') {
            *cursor++ = '+';
            Advance(first);
        } else if (Current(first) == '-') {
            *cursor++ = '-';
            Advance(first);
        }
    }
    flags &= BaseField;
    int base = flags == OctalBase ? 8 : flags == HexadecimalBase ? 16 : flags == 0 ? 0 : 10;
    bool seenDigit = false;
    bool nonzero = false;
    if (!Same(first, last) && Current(first) == '0') {
        seenDigit = true;
        Advance(first);
        if (!Same(first, last) && (Current(first) == 'x' || Current(first) == 'X') && (base == 0 || base == 16)) {
            base = 16;
            seenDigit = false;
            Advance(first);
        } else if (base == 0) {
            base = 8;
        }
    }
    const std::size_t digits = base == 0 || base == 10 ? 10 : base == 8 ? 8 : 22;
    for (char* const limit = field + IntegerDigits - 1; !Same(first, last); Advance(first)) {
        const char* const found = static_cast<const char*>(std::memchr(Source, Current(first), sizeof(Source) - 1));
        const std::size_t index = found == nullptr ? sizeof(Source) - 1 : static_cast<std::size_t>(found - Source);
        if (index >= digits) break;
        *cursor = Source[index];
        if ((nonzero || *cursor != '0') && cursor < limit) {
            ++cursor;
            nonzero = true;
        }
        seenDigit = true;
    }
    if (seenDigit && !nonzero) *cursor++ = '0';
    else if (!seenDigit) cursor = field;
    *cursor = '\0';
    return base;
}

int MatchName(InputIterator& first, InputIterator& last, std::size_t fields, const char* names) {
    for (std::size_t offset = 0; names[offset] != '\0'; ++offset) {
        if (names[offset] == names[0]) ++fields;
    }
    std::string columns(fields, '\0');
    int answer = -2;
    for (std::size_t column = 1;; ++column, Advance(first), answer = -1) {
        bool prefix = false;
        std::size_t offset = 0;
        for (std::size_t field = 0; field < fields; ++field) {
            while (names[offset] != '\0' && names[offset] != names[0]) ++offset;
            if (columns[field] != '\0') {
                offset += static_cast<std::size_t>(columns[field]);
            } else if (names[offset += column] == names[0] || names[offset] == '\0') {
                columns[field] = static_cast<char>(column < 127 ? column : 127);
                answer = static_cast<int>(field);
            } else if (Same(first, last) || names[offset] != Current(first)) {
                columns[field] = static_cast<char>(column < 127 ? column : 127);
            } else {
                prefix = true;
            }
        }
        if (!prefix || Same(first, last)) break;
    }
    return answer;
}

template<typename TValue>
InputIterator GetNarrowUnsigned(InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, TValue* value) {
    RequireStream(stream, state, value, "num_get::do_get");
    char field[IntegerDigits];
    const int base = GatherInteger(field, first, last, stream->flags);
    const char* const digits = field[0] == '-' ? field + 1 : field;
    const char* end = nullptr;
    int error = 0;
    const std::uint64_t result = ToUnsigned(digits, &end, base, &error);
    MarkEnd(first, last, state);
    if (end == digits || (result != 0 && field[0] == '-')) {
        *state |= FailBit;
        *value = 0;
    } else if (result > std::numeric_limits<TValue>::max() || error != 0) {
        *state |= FailBit;
        *value = std::numeric_limits<TValue>::max();
    } else {
        *value = static_cast<TValue>(result);
    }
    return first;
}

InputIterator GetSigned(InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::int64_t* value) {
    RequireStream(stream, state, value, "num_get::do_get");
    char field[IntegerDigits];
    const int base = GatherInteger(field, first, last, stream->flags);
    const char* end = nullptr;
    int error = 0;
    const std::int64_t result = ToSigned(field, &end, base, &error);
    MarkEnd(first, last, state);
    if (end == field || error != 0) *state |= FailBit;
    *value = result;
    return first;
}

InputIterator GetUnsigned(InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::uint64_t* value) {
    RequireStream(stream, state, value, "num_get::do_get");
    char field[IntegerDigits];
    const int base = GatherInteger(field, first, last, stream->flags);
    const char* end = nullptr;
    int error = 0;
    const std::uint64_t result = ToUnsigned(field, &end, base, &error);
    MarkEnd(first, last, state);
    *value = result;
    if (end == field || (result != 0 && field[0] == '-')) {
        *state |= FailBit;
        *value = 0;
    }
    if (error != 0) *state |= FailBit;
    return first;
}

void APS5_VABI DestroyNumGet(Facet*) {}

void APS5_VABI DeleteNumGet(Facet* self) {
    if (self == nullptr) throw std::invalid_argument("num_get delete: null facet");
    _ZdlPv_nid_postfix(self);
}

void APS5_VABI RetainFacet(Facet* self) {
    if (self == nullptr) throw std::invalid_argument("facet retain: null facet");
    std::atomic_ref<std::uint32_t>(self->references).fetch_add(1);
}

Facet* APS5_VABI ReleaseFacet(Facet* self) {
    if (self == nullptr) throw std::invalid_argument("facet release: null facet");
    return std::atomic_ref<std::uint32_t>(self->references).fetch_sub(1) == 1 ? self : nullptr;
}

InputIterator APS5_VABI NumGetBool(const Facet*, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, bool* value) {
    RequireStream(stream, state, value, "num_get::do_get");
    int result = -1;
    if ((stream->flags & BoolAlpha) == 0) {
        char field[IntegerDigits];
        const int base = GatherInteger(field, first, last, stream->flags);
        const char* end = nullptr;
        int error = 0;
        const std::int64_t number = ToSigned(field, &end, base, &error);
        if (end != field && error == 0 && static_cast<std::uint64_t>(number) < 2) result = static_cast<int>(number);
    } else {
        static constexpr char Names[] = "\0false\0true";
        result = MatchName(first, last, 2, Names);
    }
    MarkEnd(first, last, state);
    if (result < 0) {
        *state |= FailBit;
        *value = true;
    } else {
        *value = result != 0;
    }
    return first;
}

InputIterator APS5_VABI NumGetUnsignedShort(const Facet*, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::uint16_t* value) {
    return GetNarrowUnsigned(first, last, stream, state, value);
}

InputIterator APS5_VABI NumGetUnsignedInt(const Facet*, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::uint32_t* value) {
    return GetNarrowUnsigned(first, last, stream, state, value);
}

InputIterator APS5_VABI NumGetLong(const Facet*, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::int64_t* value) {
    return GetSigned(first, last, stream, state, value);
}

InputIterator APS5_VABI NumGetUnsignedLong(const Facet*, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::uint64_t* value) {
    return GetUnsigned(first, last, stream, state, value);
}

InputIterator APS5_VABI NumGetFloat(const Facet*, InputIterator, InputIterator, IosBase*, std::int32_t*, float*) {
    throw std::runtime_error("num_get::do_get: float input is not supported");
}

InputIterator APS5_VABI NumGetDouble(const Facet*, InputIterator, InputIterator, IosBase*, std::int32_t*, double*) {
    throw std::runtime_error("num_get::do_get: double input is not supported");
}

InputIterator APS5_VABI NumGetLongDouble(const Facet*, InputIterator, InputIterator, IosBase*, std::int32_t*, long double*) {
    throw std::runtime_error("num_get::do_get: long double input is not supported");
}

InputIterator APS5_VABI NumGetPointer(const Facet*, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, void** value) {
    RequireStream(stream, state, value, "num_get::do_get");
    char field[IntegerDigits];
    const int base = GatherInteger(field, first, last, HexadecimalBase);
    const char* end = nullptr;
    int error = 0;
    const std::uint64_t result = ToUnsigned(field, &end, base, &error);
    MarkEnd(first, last, state);
    if (end == field || error != 0) *state |= FailBit;
    *value = reinterpret_cast<void*>(static_cast<std::uintptr_t>(result));
    return first;
}

const GuestLocale::NumGetVtable g_numGetVtable{
    0, &g_numGetType, {DestroyNumGet, DeleteNumGet, RetainFacet, ReleaseFacet},
    NumGetBool, NumGetUnsignedShort, NumGetUnsignedInt, NumGetLong, NumGetUnsignedLong, NumGetLong, NumGetUnsignedLong,
    NumGetFloat, NumGetDouble, NumGetLongDouble, NumGetPointer};

}

extern "C" {

GuestLocale::NumGetVtable NumGetVtable_nid_no_patch() {
    return g_numGetVtable;
}

}
