#include <atomic>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <limits>
#include <stdexcept>
#include <string>

#include "prx/libc/include/GuestLocale.hpp"

extern "C" {
void* APS5_VABI _Znwm_nid_postfix(std::size_t size);
void APS5_VABI _ZdlPv_nid_postfix(void* pointer);
std::mbstate_t* APS5_VABI _Getpmbstate_nid_postfix();
std::mbstate_t* APS5_VABI _Getpwcstate_nid_postfix();
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
constexpr std::size_t TimeCategory = 5;
constexpr std::int32_t NoOrder = 0;
constexpr std::int32_t DayMonthYear = 1;
constexpr std::int32_t MonthDayYear = 2;
constexpr std::int32_t YearMonthDay = 3;
constexpr std::int32_t YearDayMonth = 4;
constexpr char Days[] = ":Sun:Sunday:Mon:Monday:Tue:Tuesday:Wed:Wednesday:Thu:Thursday:Fri:Friday:Sat:Saturday";
constexpr char Months[] = ":Jan:January:Feb:February:Mar:March:Apr:April:May:May:Jun:June:Jul:July:Aug:August:Sep:September:Oct:October:Nov:November:Dec:December";
constexpr char Meridiems[] = ":AM:am:PM:pm";

struct TypeRecord {
    const void* vtable;
    const char* name;
    const TypeRecord* base;
};

const TypeRecord g_facetBaseType{_ZTVN10__cxxabiv117__class_type_infoE_nid_postfix + TypeInfoHeader, "St11_Facet_base", nullptr};
const TypeRecord g_facetType{_ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix + TypeInfoHeader, "NSt6locale5facetE", &g_facetBaseType};
const TypeRecord g_numGetType{_ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix + TypeInfoHeader, "St7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE", &g_facetType};
const TypeRecord g_timeBaseType{_ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix + TypeInfoHeader, "St9time_base", &g_facetType};
const TypeRecord g_timeGetType{_ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix + TypeInfoHeader, "St8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE", &g_timeBaseType};

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

bool IsDigit(char character) {
    return character >= '0' && character <= '9';
}

void SkipSpaces(InputIterator& first, InputIterator& last) {
    while (!Same(first, last) && IsSpace(Current(first))) Advance(first);
}

char Lower(char character) {
    return character >= 'A' && character <= 'Z' ? static_cast<char>(character - 'A' + 'a') : character;
}

bool IsClassic(const GuestLocale::Implementation* const* locale) {
    return locale != nullptr && *locale != nullptr && (*locale)->name != nullptr && std::strcmp((*locale)->name, "C") == 0;
}

void RequireStream(const IosBase* stream, const std::int32_t* state, const void* value, const char* function) {
    if (stream == nullptr || state == nullptr || value == nullptr) throw std::invalid_argument(std::string(function) + ": null argument");
    if (!IsClassic(stream->locale)) throw std::invalid_argument(std::string(function) + ": only the C locale is supported");
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

void APS5_VABI DestroyFacet(Facet*) {}

void APS5_VABI DeleteFacet(Facet* self) {
    if (self == nullptr) throw std::invalid_argument("facet delete: null facet");
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
    0, &g_numGetType, {DestroyFacet, DeleteFacet, RetainFacet, ReleaseFacet},
    NumGetBool, NumGetUnsignedShort, NumGetUnsignedInt, NumGetLong, NumGetUnsignedLong, NumGetLong, NumGetUnsignedLong,
    NumGetFloat, NumGetDouble, NumGetLongDouble, NumGetPointer};

const GuestLocale::TimeGetFunctions& TimeVirtuals(const Facet* self) {
    return *reinterpret_cast<const GuestLocale::TimeGetFunctions*>(self->vtable);
}

const GuestLocale::TimeGetFacet* TimeFacet(const Facet* self) {
    if (self == nullptr) throw std::invalid_argument("time_get: null facet");
    return reinterpret_cast<const GuestLocale::TimeGetFacet*>(self);
}

std::int32_t GetDigits(InputIterator& first, InputIterator& last, int low, int high, int* value) {
    char field[IntegerDigits];
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
    bool seenDigit = false;
    for (; !Same(first, last) && Current(first) == '0'; Advance(first)) seenDigit = true;
    if (seenDigit) *cursor++ = '0';
    for (char* const limit = field + IntegerDigits - 1; !Same(first, last) && IsDigit(Current(first)); seenDigit = true, Advance(first)) {
        *cursor = Current(first);
        if (cursor < limit) ++cursor;
    }
    if (!seenDigit) cursor = field;
    *cursor = '\0';
    const char* end = nullptr;
    int error = 0;
    const std::int64_t answer = ToSigned(field, &end, 10, &error);
    std::int32_t state = 0;
    if (Same(first, last)) state |= EofBit;
    if (end == field || error != 0 || answer < low || answer > high) state |= FailBit;
    else *value = static_cast<int>(answer);
    return state;
}

void SkipDateSeparator(InputIterator& first, InputIterator& last) {
    SkipSpaces(first, last);
    if (!Same(first, last)) {
        const char separator = Current(first);
        if (separator == ':' || separator == ',' || separator == '/') Advance(first);
    }
    SkipSpaces(first, last);
}

InputIterator GetFormat(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, GuestTm* time, const char* format) {
    const auto& virtuals = TimeVirtuals(self);
    for (; *format != '\0'; ++format) {
        if (*format == '%') {
            ++format;
            first = virtuals.get(self, first, last, stream, state, time, *format, '\0');
        } else if (*format == ' ') {
            SkipSpaces(first, last);
        } else if (Current(first) != *format) {
            *state |= FailBit;
            break;
        } else {
            Advance(first);
        }
    }
    MarkEnd(first, last, state);
    return first;
}

std::int32_t APS5_VABI TimeGetDateOrder(const Facet* self) {
    return TimeFacet(self)->dateOrder;
}

InputIterator APS5_VABI TimeGetTime(const Facet*, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, GuestTm* time) {
    RequireStream(stream, state, time, "time_get::do_get_time");
    *state |= GetDigits(first, last, 0, 23, &time->tm_hour);
    if (*state != 0 || Current(first) != ':') {
        *state |= FailBit;
        return first;
    }
    Advance(first);
    *state |= GetDigits(first, last, 0, 59, &time->tm_min);
    if (*state != 0 || Current(first) != ':') {
        *state |= FailBit;
        return first;
    }
    Advance(first);
    *state |= GetDigits(first, last, 0, 60, &time->tm_sec);
    return first;
}

InputIterator APS5_VABI TimeGetDate(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, GuestTm* time) {
    RequireStream(stream, state, time, "time_get::do_get_date");
    const auto& virtuals = TimeVirtuals(self);
    std::int32_t order = virtuals.dateOrder(self);
    if (order == NoOrder) order = MonthDayYear;
    if (!Same(first, last)) {
        if (!IsDigit(Current(first))) {
            first = virtuals.getMonthName(self, first, last, stream, state, time);
            order = MonthDayYear;
        } else if (order == MonthDayYear) {
            *state |= GetDigits(first, last, 1, 12, &time->tm_mon);
            --time->tm_mon;
        } else if (order == DayMonthYear) {
            *state |= GetDigits(first, last, 1, 31, &time->tm_mday);
        } else {
            first = virtuals.getYear(self, first, last, stream, state, time);
        }
    }
    SkipDateSeparator(first, last);
    if (!Same(first, last)) {
        if (!IsDigit(Current(first))) {
            if (order == MonthDayYear) {
                *state |= FailBit;
            } else {
                first = virtuals.getMonthName(self, first, last, stream, state, time);
                if (order == YearDayMonth) order = YearMonthDay;
            }
        } else if (order == DayMonthYear || order == YearMonthDay) {
            *state |= GetDigits(first, last, 1, 12, &time->tm_mon);
            --time->tm_mon;
        } else {
            *state |= GetDigits(first, last, 1, 31, &time->tm_mday);
        }
    }
    SkipDateSeparator(first, last);
    if (Same(first, last)) {
        *state |= FailBit;
    } else if (!IsDigit(Current(first))) {
        if (order != YearDayMonth) *state |= FailBit;
        else first = virtuals.getMonthName(self, first, last, stream, state, time);
    } else if (order == YearDayMonth) {
        *state |= GetDigits(first, last, 1, 12, &time->tm_mon);
        --time->tm_mon;
    } else if (order == YearMonthDay) {
        *state |= GetDigits(first, last, 1, 31, &time->tm_mday);
    } else {
        first = virtuals.getYear(self, first, last, stream, state, time);
    }
    MarkEnd(first, last, state);
    return first;
}

InputIterator APS5_VABI TimeGetWeekday(const Facet* self, InputIterator first, InputIterator last, IosBase*, std::int32_t* state, GuestTm* time) {
    if (state == nullptr || time == nullptr) throw std::invalid_argument("time_get::do_get_weekday: null argument");
    const int number = MatchName(first, last, 0, TimeFacet(self)->days);
    if (number < 0) *state |= FailBit;
    else time->tm_wday = number >> 1;
    return first;
}

InputIterator APS5_VABI TimeGetMonthName(const Facet* self, InputIterator first, InputIterator last, IosBase*, std::int32_t* state, GuestTm* time) {
    if (state == nullptr || time == nullptr) throw std::invalid_argument("time_get::do_get_monthname: null argument");
    const int number = MatchName(first, last, 0, TimeFacet(self)->months);
    if (number < 0) *state |= FailBit;
    else time->tm_mon = number >> 1;
    return first;
}

InputIterator APS5_VABI TimeGetYear(const Facet*, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, GuestTm* time) {
    RequireStream(stream, state, time, "time_get::do_get_year");
    int answer = 0;
    const std::int32_t result = GetDigits(first, last, 0, 9999, &answer);
    *state |= result;
    if ((result & FailBit) == 0) time->tm_year = answer < 69 ? answer + 100 : answer < 100 ? answer : answer - 1900;
    return first;
}

InputIterator APS5_VABI TimeGetField(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, GuestTm* time, char specifier, char) {
    RequireStream(stream, state, time, "time_get::do_get");
    const auto& virtuals = TimeVirtuals(self);
    int answer = 0;
    *state = 0;
    switch (specifier) {
    case 'a':
    case 'A':
        first = virtuals.getWeekday(self, first, last, stream, state, time);
        break;
    case 'b':
    case 'B':
    case 'h':
        first = virtuals.getMonthName(self, first, last, stream, state, time);
        break;
    case 'c':
        first = GetFormat(self, first, last, stream, state, time, "%a %b %d %H : %M : %S %Y");
        break;
    case 'C':
        *state |= GetDigits(first, last, 0, 99, &answer);
        if ((*state & FailBit) == 0) time->tm_year = answer * 100 - 1900;
        break;
    case 'd':
    case 'e':
        *state |= GetDigits(first, last, 1, 31, &time->tm_mday);
        break;
    case 'D':
        first = GetFormat(self, first, last, stream, state, time, "%m / %d / %y");
        break;
    case 'H':
        *state |= GetDigits(first, last, 0, 23, &time->tm_hour);
        break;
    case 'I':
        *state |= GetDigits(first, last, 1, 12, &answer);
        if ((*state & FailBit) == 0) time->tm_hour = answer == 12 ? 0 : answer;
        break;
    case 'j':
        *state |= GetDigits(first, last, 1, 366, &time->tm_yday);
        break;
    case 'm':
        *state |= GetDigits(first, last, 1, 12, &answer);
        if ((*state & FailBit) == 0) time->tm_mon = answer - 1;
        break;
    case 'M':
        *state |= GetDigits(first, last, 0, 59, &time->tm_min);
        break;
    case 'n':
    case 't':
        first = GetFormat(self, first, last, stream, state, time, " ");
        break;
    case 'p':
        answer = MatchName(first, last, 0, Meridiems);
        if (answer < 0) *state |= FailBit;
        else if (answer > 1) time->tm_hour += 12;
        break;
    case 'r':
        first = GetFormat(self, first, last, stream, state, time, "%I : %M : %S %p");
        break;
    case 'R':
        first = GetFormat(self, first, last, stream, state, time, "%H : %M");
        break;
    case 'S':
        *state |= GetDigits(first, last, 0, 60, &time->tm_sec);
        break;
    case 'T':
    case 'X':
        first = GetFormat(self, first, last, stream, state, time, "%H : %M : %S");
        break;
    case 'U':
    case 'W':
        *state |= GetDigits(first, last, 0, 53, &time->tm_yday);
        break;
    case 'w':
        *state |= GetDigits(first, last, 0, 6, &time->tm_wday);
        break;
    case 'x':
        first = GetFormat(self, first, last, stream, state, time, "%d / %m / %y");
        break;
    case 'y':
        *state |= GetDigits(first, last, 0, 99, &answer);
        if ((*state & FailBit) == 0) time->tm_year = answer < 69 ? answer + 100 : answer;
        break;
    case 'Y':
        *state |= GetDigits(first, last, 0, 9999, &answer);
        if ((*state & FailBit) == 0) time->tm_year = answer - 1900;
        break;
    case '%':
        if (!Same(first, last) && Current(first) == '%') Advance(first);
        break;
    default:
        *state = FailBit;
        break;
    }
    MarkEnd(first, last, state);
    return first;
}

struct TimeGetVtable {
    std::int64_t offset;
    const void* type;
    GuestLocale::TimeGetFunctions functions;
};

const TimeGetVtable g_timeGetVtable{
    0, &g_timeGetType,
    {{DestroyFacet, DeleteFacet, RetainFacet, ReleaseFacet}, TimeGetDateOrder, TimeGetTime, TimeGetDate, TimeGetWeekday, TimeGetMonthName, TimeGetYear, TimeGetField}};

}

extern "C" {

GuestLocale::NumGetVtable NumGetVtable_nid_no_patch() {
    return g_numGetVtable;
}

std::size_t TimeGetCategory_nid_no_patch(Facet** facet, const GuestLocale::Implementation* const* locale) {
    if (facet != nullptr && *facet == nullptr) {
        if (!IsClassic(locale)) throw std::invalid_argument("time_get::_Getcat: only the C locale is supported");
        auto* created = static_cast<GuestLocale::TimeGetFacet*>(_Znwm_nid_postfix(sizeof(GuestLocale::TimeGetFacet)));
        *created = {{&g_timeGetVtable.functions.facet, 0, 0}, Days, Months, Meridiems, MonthDayYear, 0, _Getpmbstate_nid_postfix(), _Getpwcstate_nid_postfix()};
        *facet = &created->base;
    }
    return TimeCategory;
}

InputIterator TimeGetFormat_nid_no_patch(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, GuestTm* time, const char* format, const char* formatEnd) {
    if (self == nullptr || format == nullptr || formatEnd < format) throw std::invalid_argument("time_get::get: invalid facet or format range");
    RequireStream(stream, state, time, "time_get::get");
    const auto& virtuals = TimeVirtuals(self);
    *state = 0;
    for (; format != formatEnd; ++format) {
        if (*format != '%') {
            if (IsSpace(*format)) {
                SkipSpaces(first, last);
            } else if (Current(first) != *format) {
                *state |= FailBit;
                break;
            } else {
                Advance(first);
            }
        } else if (++format == formatEnd) {
            if (Current(first) != format[-1]) *state |= FailBit;
            else Advance(first);
            break;
        } else {
            char specifier = *format;
            char modifier = '\0';
            const char percent = format[-1];
            if (specifier == 'E' || specifier == 'O' || specifier == 'Q' || specifier == '#') {
                if (++format == formatEnd) {
                    bool matched = Current(first) == percent;
                    if (matched) {
                        Advance(first);
                        matched = !Same(first, last) && Current(first) == specifier;
                    }
                    if (matched) Advance(first);
                    else *state |= FailBit;
                    break;
                }
                modifier = specifier;
                specifier = *format;
            }
            first = virtuals.get(self, first, last, stream, state, time, specifier, modifier);
        }
    }
    MarkEnd(first, last, state);
    return first;
}

}
