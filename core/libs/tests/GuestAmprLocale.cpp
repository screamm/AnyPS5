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

int main() {
    const std::array<void*, 10> api{reinterpret_cast<void*>(&Allocate), reinterpret_cast<void*>(&Release), reinterpret_cast<void*>(&AllocateZeroed), reinterpret_cast<void*>(&Reallocate), reinterpret_cast<void*>(&Align), reinterpret_cast<void*>(&Realign), reinterpret_cast<void*>(&PosixAlign)};
    ApplicationHeapRegister_nid_no_patch(api.data());
    CheckStandardStreams();
    CheckNumberTable();
    CheckSignedNumbers();
    CheckUnsignedNumbers();
    CheckBooleans();
    CheckUnsupportedNumbers();
    std::puts("Guest Ampr locale checks passed");
}
