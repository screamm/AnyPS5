#include "prx/libc/include/GuestLocale.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

extern "C" {
extern GuestLocale::Implementation* _ZSt21_sceLibcClassicLocale_nid_postfix;
void APS5_VABI _ZNSt8ios_baseD2Ev_nid_postfix(GuestLocale::IosBase* self);
void APS5_VABI _ZNSt8ios_base7_AddstdEPS__nid_postfix(GuestLocale::IosBase* stream);
}

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

int main() {
    CheckStandardStreams();
    std::puts("Guest Ampr locale checks passed");
}
