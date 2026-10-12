#include "prx/libc/include/ApplicationHeap.hpp"
#include "prx/libc/include/GuestLocale.hpp"
#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>

extern "C" {
extern GuestLocale::Implementation* _ZSt21_sceLibcClassicLocale_nid_postfix;
void APS5_VABI _ZNSt8ios_baseD2Ev_nid_postfix(GuestLocale::IosBase* self);
void APS5_VABI _ZNSt8ios_base7_AddstdEPS__nid_postfix(GuestLocale::IosBase* stream);
void* APS5_VABI _Znwm_nid_postfix(std::size_t size);
extern const std::byte _ZTVN10__cxxabiv117__class_type_infoE_nid_postfix[];
extern const std::byte _ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix[];
extern std::uint64_t _ZNSt7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE2idE_nid_postfix;
extern GuestLocale::NumGetVtable _ZTVSt7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE_nid_postfix;
extern std::uint64_t _ZNSt8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE2idE_nid_postfix;
std::size_t APS5_VABI _ZNSt8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE7_GetcatEPPKNSt6locale5facetEPKS5__nid_postfix(GuestLocale::Facet** facet, const GuestLocale::Implementation* const* locale);
GuestLocale::InputIterator APS5_VABI _ZNKSt8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE3getES3_S3_RSt8ios_baseRNSt5_IosbIiE8_IostateEP2tmPKcSE__nid_postfix(const GuestLocale::Facet* self, GuestLocale::InputIterator first, GuestLocale::InputIterator last, GuestLocale::IosBase* stream, std::int32_t* state, GuestTm* time, const char* format, const char* formatEnd);
}

using GuestLocale::InputIterator;

#define Require(value) RequireAt((value), __LINE__)

static void RequireAt(bool value, int line) {
    if (value) return;
    std::fprintf(stderr, "requirement failed at line %d\n", line);
    std::abort();
}

template<typename TAction>
static bool Throws(TAction action) {
    try {
        action();
    } catch (const std::exception&) {
        return true;
    }
    return false;
}

static std::size_t g_allocations = 0;
static std::size_t g_frees = 0;

static void* APS5_VABI Allocate(std::size_t bytes) {
    ++g_allocations;
    return std::malloc(bytes);
}

static void APS5_VABI Release(void* pointer) {
    ++g_frees;
    std::free(pointer);
}

static void* APS5_VABI AllocateZeroed(std::size_t, std::size_t) { std::abort(); }
static void* APS5_VABI Reallocate(void*, std::size_t) { std::abort(); }
static void* APS5_VABI Align(std::size_t, std::size_t) { std::abort(); }
static void* APS5_VABI Realign(void*, std::size_t, std::size_t) { std::abort(); }
static int APS5_VABI PosixAlign(void**, std::size_t, std::size_t) { std::abort(); }

struct TypeRecord {
    const void* vtable;
    const char* name;
    const TypeRecord* base;
};

struct TestBuffer {
    GuestLocale::StreamBuffer base{};
    char* next = nullptr;
    char* end = nullptr;
    std::string text;
    std::size_t position = 0;
    int underflows = 0;
    int uflows = 0;
};

static std::int32_t APS5_VABI BufferedUnderflow(GuestLocale::StreamBuffer* self) {
    auto* buffer = reinterpret_cast<TestBuffer*>(self);
    ++buffer->underflows;
    return buffer->next != nullptr && buffer->next < buffer->end ? static_cast<unsigned char>(*buffer->next) : -1;
}

static std::int32_t APS5_VABI BufferedUflow(GuestLocale::StreamBuffer* self) {
    auto* buffer = reinterpret_cast<TestBuffer*>(self);
    ++buffer->uflows;
    return buffer->next != nullptr && buffer->next < buffer->end ? static_cast<unsigned char>(*buffer->next++) : -1;
}

static std::int32_t APS5_VABI DirectUnderflow(GuestLocale::StreamBuffer* self) {
    auto* buffer = reinterpret_cast<TestBuffer*>(self);
    ++buffer->underflows;
    return buffer->position < buffer->text.size() ? static_cast<unsigned char>(buffer->text[buffer->position]) : -1;
}

static std::int32_t APS5_VABI DirectUflow(GuestLocale::StreamBuffer* self) {
    auto* buffer = reinterpret_cast<TestBuffer*>(self);
    ++buffer->uflows;
    return buffer->position < buffer->text.size() ? static_cast<unsigned char>(buffer->text[buffer->position++]) : -1;
}

static const GuestLocale::StreamBufferVtable g_bufferedVtable{{}, BufferedUnderflow, BufferedUflow};
static const GuestLocale::StreamBufferVtable g_directVtable{{}, DirectUnderflow, DirectUflow};

static void Fill(TestBuffer& buffer, const std::string& text, bool buffered) {
    buffer.text = text;
    buffer.position = 0;
    buffer.base.vtable = buffered ? &g_bufferedVtable : &g_directVtable;
    buffer.next = buffered ? buffer.text.data() : nullptr;
    buffer.end = buffered ? buffer.text.data() + buffer.text.size() : nullptr;
    buffer.base.readNext = &buffer.next;
    buffer.base.readEnd = &buffer.end;
}

static std::size_t Consumed(const TestBuffer& buffer) {
    return buffer.next != nullptr ? static_cast<std::size_t>(buffer.next - buffer.text.data()) : buffer.position;
}

static GuestLocale::IosBase MakeStream() {
    GuestLocale::IosBase stream{};
    stream.locale = &_ZSt21_sceLibcClassicLocale_nid_postfix;
    return stream;
}

static void CheckStandardStreams() {
    std::array<GuestLocale::IosBase, 9> streams{};
    for (auto& stream : streams) stream = MakeStream();
    _ZNSt8ios_base7_AddstdEPS__nid_postfix(&streams[0]);
    Require(streams[0].standardStream == 1);
    _ZNSt8ios_base7_AddstdEPS__nid_postfix(&streams[0]);
    Require(streams[0].standardStream == 1);
    for (std::size_t index = 1; index < 7; ++index) {
        _ZNSt8ios_base7_AddstdEPS__nid_postfix(&streams[index]);
        Require(streams[index].standardStream == static_cast<std::int64_t>(index + 1));
    }
    _ZNSt8ios_base7_AddstdEPS__nid_postfix(&streams[7]);
    _ZNSt8ios_base7_AddstdEPS__nid_postfix(&streams[8]);
    Require(streams[7].standardStream == 8 && streams[8].standardStream == 8);
    _ZNSt8ios_base7_AddstdEPS__nid_postfix(&streams[7]);
    Require(streams[7].standardStream == 8);

    _ZNSt8ios_baseD2Ev_nid_postfix(&streams[0]);
    Require(streams[0].locale == &_ZSt21_sceLibcClassicLocale_nid_postfix);
    _ZNSt8ios_baseD2Ev_nid_postfix(&streams[0]);
    Require(streams[0].locale == nullptr);
    _ZNSt8ios_baseD2Ev_nid_postfix(&streams[1]);
    Require(streams[1].locale == nullptr);
    _ZNSt8ios_baseD2Ev_nid_postfix(&streams[8]);
    _ZNSt8ios_baseD2Ev_nid_postfix(&streams[7]);
    Require(streams[8].locale != nullptr && streams[7].locale != nullptr);
    _ZNSt8ios_baseD2Ev_nid_postfix(&streams[7]);
    Require(streams[7].locale == nullptr);

    _ZNSt8ios_base7_AddstdEPS__nid_postfix(&streams[2]);
    Require(streams[2].standardStream == 3);
    _ZNSt8ios_baseD2Ev_nid_postfix(&streams[2]);
    Require(streams[2].locale != nullptr);

    auto outside = MakeStream();
    outside.standardStream = 9;
    Require(Throws([&] { _ZNSt8ios_baseD2Ev_nid_postfix(&outside); }));
    Require(outside.locale != nullptr);
    Require(Throws([] { _ZNSt8ios_base7_AddstdEPS__nid_postfix(nullptr); }));
}

struct NumberCase {
    GuestLocale::IosBase stream = MakeStream();
    TestBuffer buffer;
    GuestLocale::Facet facet{&_ZTVSt7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE_nid_postfix.facet, 0, 0};
    std::int32_t state = 0;
    InputIterator rest{};

    explicit NumberCase(const std::string& text, std::int32_t flags = 0x11, bool buffered = true) {
        stream.flags = flags;
        Fill(buffer, text, buffered);
    }

    InputIterator First() {
        return {&buffer.base, false, 0};
    }
};

template<typename TValue, typename TGet>
static TValue Read(NumberCase& item, TGet get) {
    TValue value{};
    item.rest = get(&item.facet, item.First(), InputIterator{}, &item.stream, &item.state, &value);
    return value;
}

static void CheckNumberTable() {
    const auto& table = _ZTVSt7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE_nid_postfix;
    Require(_ZNSt7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE2idE_nid_postfix == 0);
    Require(table.offset == 0);
    const auto* type = static_cast<const TypeRecord*>(table.type);
    Require(type->vtable == _ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix + 16);
    Require(std::strcmp(type->name, "St7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE") == 0);
    Require(type->base->vtable == _ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix + 16 && std::strcmp(type->base->name, "NSt6locale5facetE") == 0);
    Require(type->base->base->vtable == _ZTVN10__cxxabiv117__class_type_infoE_nid_postfix + 16 && std::strcmp(type->base->base->name, "St11_Facet_base") == 0);
    Require(type->base->base->base == nullptr);

    const auto allocations = g_allocations;
    const auto frees = g_frees;
    auto* facet = static_cast<GuestLocale::Facet*>(_Znwm_nid_postfix(sizeof(GuestLocale::Facet)));
    *facet = {&table.facet, 0, 0};
    facet->vtable->retain(facet);
    Require(facet->references == 1);
    Require(facet->vtable->release(facet) == facet && facet->references == 0);
    facet->vtable->destroy(facet);
    facet->vtable->deleteObject(facet);
    Require(g_allocations == allocations + 1 && g_frees == frees + 1);
}

static void CheckSignedNumbers() {
    const auto& table = _ZTVSt7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE_nid_postfix;
    NumberCase decimal("-123x");
    Require(Read<std::int64_t>(decimal, table.getLong) == -123 && decimal.state == 0);
    Require(Consumed(decimal.buffer) == 4 && decimal.rest.buffer == &decimal.buffer.base);
    NumberCase whole("42");
    Require(Read<std::int64_t>(whole, table.getLongLong) == 42 && whole.state == 1 && whole.rest.buffer == &whole.buffer.base && !whole.rest.got);
    NumberCase hex("ff", 0x41);
    Require(Read<std::int64_t>(hex, table.getLong) == 255 && hex.state == 1);
    NumberCase prefixed("0x1A", 0x01);
    Require(Read<std::int64_t>(prefixed, table.getLong) == 26);
    NumberCase octal("017", 0x01);
    Require(Read<std::int64_t>(octal, table.getLong) == 15);
    NumberCase octalFlag("19", 0x21);
    Require(Read<std::int64_t>(octalFlag, table.getLong) == 1 && octalFlag.state == 0 && Consumed(octalFlag.buffer) == 1);
    NumberCase zeros(std::string(40, '0') + "12");
    Require(Read<std::int64_t>(zeros, table.getLong) == 12 && zeros.state == 1);
    NumberCase nothing("");
    Require(Read<std::int64_t>(nothing, table.getLong) == 0 && nothing.state == 3);
    NumberCase sign("-");
    Require(Read<std::int64_t>(sign, table.getLong) == 0 && sign.state == 3);
    errno = 0;
    NumberCase large("99999999999999999999");
    Require(Read<std::int64_t>(large, table.getLong) == std::numeric_limits<std::int64_t>::max() && large.state == 3 && errno == 34);
    NumberCase smallest("-9223372036854775808");
    Require(Read<std::int64_t>(smallest, table.getLong) == std::numeric_limits<std::int64_t>::min() && smallest.state == 1);
    NumberCase direct("77 ", 0x11, false);
    Require(Read<std::int64_t>(direct, table.getLong) == 77 && direct.state == 0 && direct.buffer.position == 2 && direct.buffer.uflows == 2);
    Require(direct.rest.buffer == &direct.buffer.base && direct.rest.got && direct.rest.value == ' ');
}

static void CheckUnsignedNumbers() {
    const auto& table = _ZTVSt7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE_nid_postfix;
    NumberCase shortValue("123");
    Require(Read<std::uint16_t>(shortValue, table.getUnsignedShort) == 123 && shortValue.state == 1);
    NumberCase shortLarge("70000");
    Require(Read<std::uint16_t>(shortLarge, table.getUnsignedShort) == 0xffff && shortLarge.state == 3);
    NumberCase shortNegative("-1");
    Require(Read<std::uint16_t>(shortNegative, table.getUnsignedShort) == 0 && shortNegative.state == 3);
    NumberCase shortZero("-0");
    Require(Read<std::uint16_t>(shortZero, table.getUnsignedShort) == 0 && shortZero.state == 1);
    NumberCase intLargest("4294967295");
    Require(Read<std::uint32_t>(intLargest, table.getUnsignedInt) == 0xffffffffu && intLargest.state == 1);
    NumberCase intLarge("4294967296");
    Require(Read<std::uint32_t>(intLarge, table.getUnsignedInt) == 0xffffffffu && intLarge.state == 3);
    NumberCase longNegative("-5");
    Require(Read<std::uint64_t>(longNegative, table.getUnsignedLong) == 0 && longNegative.state == 3);
    NumberCase longLargest("18446744073709551615");
    Require(Read<std::uint64_t>(longLargest, table.getUnsignedLongLong) == std::numeric_limits<std::uint64_t>::max() && longLargest.state == 1);
    NumberCase longLarge("18446744073709551616");
    Require(Read<std::uint64_t>(longLarge, table.getUnsignedLong) == std::numeric_limits<std::uint64_t>::max() && longLarge.state == 3);
    NumberCase pointer("10", 0x11);
    Require(Read<void*>(pointer, table.getPointer) == reinterpret_cast<void*>(0x10) && pointer.state == 1);
    NumberCase pointerPrefix("0xBEEFg");
    Require(Read<void*>(pointerPrefix, table.getPointer) == reinterpret_cast<void*>(0xbeef) && pointerPrefix.state == 0);
}

static void CheckBooleans() {
    const auto& table = _ZTVSt7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE_nid_postfix;
    NumberCase one("1");
    Require(Read<bool>(one, table.getBool) && one.state == 1);
    NumberCase zero("0");
    Require(!Read<bool>(zero, table.getBool) && zero.state == 1);
    NumberCase two("2");
    Require(Read<bool>(two, table.getBool) && two.state == 3);
    NumberCase letter("x");
    Require(Read<bool>(letter, table.getBool) && letter.state == 2);
    NumberCase alphaTrue("true", 0x8011);
    Require(Read<bool>(alphaTrue, table.getBool) && alphaTrue.state == 1);
    NumberCase alphaFalse("false!", 0x8011);
    Require(!Read<bool>(alphaFalse, table.getBool) && alphaFalse.state == 0 && Consumed(alphaFalse.buffer) == 5);
    NumberCase alphaShort("fa", 0x8011);
    Require(Read<bool>(alphaShort, table.getBool) && alphaShort.state == 3);
    NumberCase alphaWrong("tx", 0x8011);
    Require(Read<bool>(alphaWrong, table.getBool) && alphaWrong.state == 2 && Consumed(alphaWrong.buffer) == 1);
}

static void CheckUnsupportedNumbers() {
    const auto& table = _ZTVSt7num_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE_nid_postfix;
    NumberCase floating("1.5");
    Require(Throws([&] { Read<double>(floating, table.getDouble); }));
    Require(Throws([&] { Read<float>(floating, table.getFloat); }));
    Require(Throws([&] { Read<long double>(floating, table.getLongDouble); }));
    Require(floating.state == 0 && Consumed(floating.buffer) == 0);

    GuestLocale::Implementation named = *_ZSt21_sceLibcClassicLocale_nid_postfix;
    named.name = "fr_FR";
    GuestLocale::Implementation* namedPointer = &named;
    NumberCase other("1");
    other.stream.locale = &namedPointer;
    Require(Throws([&] { Read<std::int64_t>(other, table.getLong); }));
    NumberCase missing("1");
    Require(Throws([&] { table.getLong(&missing.facet, missing.First(), InputIterator{}, &missing.stream, nullptr, nullptr); }));
    Require(Consumed(other.buffer) == 0 && Consumed(missing.buffer) == 0);
}

static GuestLocale::Facet* MakeTimeFacet() {
    GuestLocale::Facet* facet = nullptr;
    Require(_ZNSt8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE7_GetcatEPPKNSt6locale5facetEPKS5__nid_postfix(&facet, &_ZSt21_sceLibcClassicLocale_nid_postfix) == 5);
    Require(facet != nullptr);
    return facet;
}

static const GuestLocale::TimeGetFunctions& TimeTable(const GuestLocale::Facet* facet) {
    return *reinterpret_cast<const GuestLocale::TimeGetFunctions*>(facet->vtable);
}

struct TimeCase {
    GuestLocale::IosBase stream = MakeStream();
    TestBuffer buffer;
    GuestTm time{};
    std::int32_t state = 0;
    InputIterator rest{};

    explicit TimeCase(const std::string& text) {
        Fill(buffer, text, true);
        time.tm_year = -1;
    }

    InputIterator First() {
        return {&buffer.base, false, 0};
    }

    void Get(const GuestLocale::Facet* facet, const std::string& format) {
        rest = _ZNKSt8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE3getES3_S3_RSt8ios_baseRNSt5_IosbIiE8_IostateEP2tmPKcSE__nid_postfix(facet, First(), InputIterator{}, &stream, &state, &time, format.data(), format.data() + format.size());
    }
};

static void CheckTimeCategory() {
    Require(_ZNSt8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE2idE_nid_postfix == 0);
    const auto allocations = g_allocations;
    Require(_ZNSt8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE7_GetcatEPPKNSt6locale5facetEPKS5__nid_postfix(nullptr, nullptr) == 5);
    GuestLocale::Facet existing{};
    GuestLocale::Facet* present = &existing;
    Require(_ZNSt8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE7_GetcatEPPKNSt6locale5facetEPKS5__nid_postfix(&present, nullptr) == 5 && present == &existing);
    Require(g_allocations == allocations);

    auto* facet = MakeTimeFacet();
    Require(g_allocations == allocations + 1);
    const auto* time = reinterpret_cast<const GuestLocale::TimeGetFacet*>(facet);
    Require(facet->references == 0);
    Require(std::strcmp(time->days, ":Sun:Sunday:Mon:Monday:Tue:Tuesday:Wed:Wednesday:Thu:Thursday:Fri:Friday:Sat:Saturday") == 0);
    Require(std::strncmp(time->months, ":Jan:January:Feb:February:", 26) == 0 && std::strcmp(time->meridiems, ":AM:am:PM:pm") == 0);
    Require(time->dateOrder == 2 && TimeTable(facet).dateOrder(facet) == 2);
    const auto* header = reinterpret_cast<const void* const*>(facet->vtable);
    Require(header[-2] == nullptr);
    const auto* type = static_cast<const TypeRecord*>(header[-1]);
    Require(std::strcmp(type->name, "St8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE") == 0 && type->vtable == _ZTVN10__cxxabiv120__si_class_type_infoE_nid_postfix + 16);
    Require(std::strcmp(type->base->name, "St9time_base") == 0 && std::strcmp(type->base->base->name, "NSt6locale5facetE") == 0);

    GuestLocale::Implementation named = *_ZSt21_sceLibcClassicLocale_nid_postfix;
    named.name = "de_DE";
    const GuestLocale::Implementation* namedPointer = &named;
    GuestLocale::Facet* missing = nullptr;
    Require(Throws([&] { _ZNSt8time_getIcSt19istreambuf_iteratorIcSt11char_traitsIcEEE7_GetcatEPPKNSt6locale5facetEPKS5__nid_postfix(&missing, &namedPointer); }));
    Require(missing == nullptr && g_allocations == allocations + 1);

    facet->vtable->retain(facet);
    Require(facet->vtable->release(facet) == facet);
    const auto frees = g_frees;
    facet->vtable->destroy(facet);
    facet->vtable->deleteObject(facet);
    Require(g_frees == frees + 1);
}

static void CheckTimeFormats() {
    auto* facet = MakeTimeFacet();
    TimeCase full("2024-03-15 13:45:30");
    full.Get(facet, "%Y-%m-%d %H:%M:%S");
    Require(full.state == 1 && full.time.tm_year == 124 && full.time.tm_mon == 2 && full.time.tm_mday == 15);
    Require(full.time.tm_hour == 13 && full.time.tm_min == 45 && full.time.tm_sec == 30);
    TimeCase names("Fri  Mar 5");
    names.Get(facet, "%a %b %e");
    Require(names.state == 1 && names.time.tm_wday == 5 && names.time.tm_mon == 2 && names.time.tm_mday == 5);
    TimeCase longNames("Wednesday, December");
    longNames.Get(facet, "%A, %B!");
    Require(longNames.state == 3 && longNames.time.tm_wday == 3 && longNames.time.tm_mon == 11);
    TimeCase composite("Fri Mar 15 13:45:30 2024");
    composite.Get(facet, "%c");
    Require(composite.state == 1 && composite.time.tm_wday == 5 && composite.time.tm_year == 124 && composite.time.tm_sec == 30);
    TimeCase years("99 05 1969 69");
    years.Get(facet, "%y");
    Require(years.time.tm_year == 99);
    years.Get(facet, " %y");
    Require(years.time.tm_year == 105);
    TimeCase fullYear("69");
    fullYear.Get(facet, "%Y");
    Require(fullYear.state == 1 && fullYear.time.tm_year == -1831);
    TimeCase century("20");
    century.Get(facet, "%C");
    Require(century.time.tm_year == 100);
    TimeCase afternoon("12:30 PM");
    afternoon.Get(facet, "%I:%M %p");
    Require(afternoon.state == 1 && afternoon.time.tm_hour == 12 && afternoon.time.tm_min == 30);
    TimeCase morning("12:05:09 am");
    morning.Get(facet, "%r");
    Require(morning.state == 1 && morning.time.tm_hour == 0 && morning.time.tm_sec == 9);
    TimeCase percent("10%x");
    percent.Get(facet, "%H%%x");
    Require(percent.state == 1 && percent.time.tm_hour == 10);
    TimeCase trailing("10%");
    trailing.Get(facet, "%H%");
    Require(trailing.state == 1 && trailing.time.tm_hour == 10);
    TimeCase modified("2001");
    modified.Get(facet, "%EY");
    Require(modified.state == 1 && modified.time.tm_year == 101);
    TimeCase modifierLiteral("%E");
    modifierLiteral.Get(facet, "%E");
    Require(modifierLiteral.state == 1);
    TimeCase days("366 060");
    days.Get(facet, "%j %S");
    Require(days.state == 1 && days.time.tm_yday == 366 && days.time.tm_sec == 60);

    TimeCase hour("25");
    hour.Get(facet, "%H");
    Require(hour.state == 3 && hour.time.tm_hour == 0);
    TimeCase unknown("1");
    unknown.Get(facet, "%Z");
    Require(unknown.state == 2 && Consumed(unknown.buffer) == 0);
    TimeCase literal("10-20");
    literal.Get(facet, "%H:%M");
    Require(literal.state == 2 && literal.time.tm_hour == 10 && Consumed(literal.buffer) == 2);
    TimeCase weekday("Xyz");
    weekday.Get(facet, "%a");
    Require(weekday.state == 2);

    GuestLocale::Implementation named = *_ZSt21_sceLibcClassicLocale_nid_postfix;
    named.name = "de_DE";
    GuestLocale::Implementation* namedPointer = &named;
    TimeCase other("10");
    other.stream.locale = &namedPointer;
    Require(Throws([&] { other.Get(facet, "%H"); }));
    Require(Throws([&] { other.Get(nullptr, "%H"); }));
    facet->vtable->deleteObject(facet);
}

static void CheckTimeFields() {
    auto* facet = MakeTimeFacet();
    const auto& table = TimeTable(facet);
    TimeCase clock("13:45:60x");
    clock.rest = table.getTime(facet, clock.First(), InputIterator{}, &clock.stream, &clock.state, &clock.time);
    Require(clock.state == 0 && clock.time.tm_hour == 13 && clock.time.tm_min == 45 && clock.time.tm_sec == 60 && Consumed(clock.buffer) == 8);
    TimeCase badClock("13-45");
    badClock.rest = table.getTime(facet, badClock.First(), InputIterator{}, &badClock.stream, &badClock.state, &badClock.time);
    Require(badClock.state == 2 && badClock.time.tm_hour == 13 && Consumed(badClock.buffer) == 2);
    TimeCase date("03/15/2024");
    date.rest = table.getDate(facet, date.First(), InputIterator{}, &date.stream, &date.state, &date.time);
    Require(date.state == 1 && date.time.tm_mon == 2 && date.time.tm_mday == 15 && date.time.tm_year == 124);
    TimeCase namedDate("Mar 15, 68");
    namedDate.rest = table.getDate(facet, namedDate.First(), InputIterator{}, &namedDate.stream, &namedDate.state, &namedDate.time);
    Require(namedDate.state == 1 && namedDate.time.tm_mon == 2 && namedDate.time.tm_mday == 15 && namedDate.time.tm_year == 168);
    TimeCase shortDate("03/15");
    shortDate.rest = table.getDate(facet, shortDate.First(), InputIterator{}, &shortDate.stream, &shortDate.state, &shortDate.time);
    Require(shortDate.state == 3);
    TimeCase weekday("Saturday");
    weekday.rest = table.getWeekday(facet, weekday.First(), InputIterator{}, &weekday.stream, &weekday.state, &weekday.time);
    Require(weekday.state == 0 && weekday.time.tm_wday == 6 && Consumed(weekday.buffer) == 8);
    TimeCase month("Decem");
    month.rest = table.getMonthName(facet, month.First(), InputIterator{}, &month.stream, &month.state, &month.time);
    Require(month.state == 2);
    TimeCase year("2024");
    year.rest = table.getYear(facet, year.First(), InputIterator{}, &year.stream, &year.state, &year.time);
    Require(year.state == 1 && year.time.tm_year == 124);
    TimeCase pivot("68");
    pivot.rest = table.getYear(facet, pivot.First(), InputIterator{}, &pivot.stream, &pivot.state, &pivot.time);
    Require(pivot.time.tm_year == 168);
    facet->vtable->deleteObject(facet);
}

static int g_derivedCalls = 0;

static InputIterator APS5_VABI DerivedGet(const GuestLocale::Facet*, InputIterator first, InputIterator, GuestLocale::IosBase*, std::int32_t* state, GuestTm* time, char specifier, char modifier) {
    ++g_derivedCalls;
    time->tm_wday = specifier;
    time->tm_yday = modifier;
    *state = 0;
    return first;
}

static void CheckDerivedTimeFacet() {
    auto* facet = MakeTimeFacet();
    GuestLocale::TimeGetFunctions functions = TimeTable(facet);
    functions.get = DerivedGet;
    GuestLocale::TimeGetFacet derived = *reinterpret_cast<const GuestLocale::TimeGetFacet*>(facet);
    derived.base.vtable = &functions.facet;
    TimeCase item("");
    item.Get(&derived.base, "%Oq");
    Require(g_derivedCalls == 1 && item.time.tm_wday == 'q' && item.time.tm_yday == 'O' && item.state == 1);
    facet->vtable->deleteObject(facet);
}

int main() {
    const std::array<void*, 10> api{reinterpret_cast<void*>(&Allocate), reinterpret_cast<void*>(&Release), reinterpret_cast<void*>(&AllocateZeroed), reinterpret_cast<void*>(&Reallocate), reinterpret_cast<void*>(&Align), reinterpret_cast<void*>(&Realign), reinterpret_cast<void*>(&PosixAlign)};
    ApplicationHeapRegister_nid_no_patch(api.data());
    CheckStandardStreams();
    CheckNumberTable();
    CheckSignedNumbers();
    CheckUnsignedNumbers();
    CheckBooleans();
    CheckUnsupportedNumbers();
    CheckTimeCategory();
    CheckTimeFormats();
    CheckTimeFields();
    CheckDerivedTimeFacet();
    std::puts("Guest Ampr locale checks passed");
}
