#ifndef CORE_LIBS_PRX_LIBC_INCLUDE_GUESTLOCALE_HPP
#define CORE_LIBS_PRX_LIBC_INCLUDE_GUESTLOCALE_HPP

#include <cstddef>
#include <cstdint>
#include "general/VabiMacros.hpp"

namespace GuestLocale {

struct LocinfoStorage {
    alignas(std::uint64_t) unsigned char bytes[64];
};

struct Facet;

struct FacetVtable {
    void (APS5_VABI *destroy)(Facet* self);
    void (APS5_VABI *deleteObject)(Facet* self);
    void (APS5_VABI *retain)(Facet* self);
    Facet* (APS5_VABI *release)(Facet* self);
};

struct Facet {
    const FacetVtable* vtable;
    std::uint32_t references;
    std::uint32_t reserved;
};

struct Implementation {
    Facet base;
    Facet** facets;
    std::uint64_t facetCount;
    std::uint32_t category;
    bool transparent;
    const char* name;
};

struct String {
    std::uint64_t reserved;
    union {
        char buffer[16];
        char* pointer;
    };
    std::uint64_t size;
    std::uint64_t capacity;
};

struct CollateFacet {
    Facet base;
    const void* collation;
    const void* wideCollation;
};

struct CollateVtable {
    FacetVtable facet;
    int (APS5_VABI *compare)(const CollateFacet* self, const char* first1, const char* last1, const char* first2, const char* last2);
    String* (APS5_VABI *transform)(String* result, const CollateFacet* self, const char* first, const char* last);
    std::int64_t (APS5_VABI *hash)(const CollateFacet* self, const char* first, const char* last);
};

struct IosBase {
    const void* vtable;
    std::int64_t standardStream;
    std::int32_t state;
    std::int32_t exceptions;
    std::int32_t flags;
    std::int32_t precision;
    std::int32_t width;
    std::uint32_t reserved;
    void* storage;
    void* callbacks;
    Implementation** locale;
};

struct StreamBuffer;

struct StreamBufferVtable {
    const void* reserved[7];
    std::int32_t (APS5_VABI *underflow)(StreamBuffer* self);
    std::int32_t (APS5_VABI *uflow)(StreamBuffer* self);
};

struct StreamBuffer {
    const StreamBufferVtable* vtable;
    std::uint64_t reserved[6];
    char** readNext;
    std::uint64_t writeReserved[3];
    char** readEnd;
};

struct InputIterator {
    StreamBuffer* buffer;
    bool got;
    char value;
};

struct NumGetVtable {
    std::int64_t offset;
    const void* type;
    FacetVtable facet;
    InputIterator (APS5_VABI *getBool)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, bool* value);
    InputIterator (APS5_VABI *getUnsignedShort)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::uint16_t* value);
    InputIterator (APS5_VABI *getUnsignedInt)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::uint32_t* value);
    InputIterator (APS5_VABI *getLong)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::int64_t* value);
    InputIterator (APS5_VABI *getUnsignedLong)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::uint64_t* value);
    InputIterator (APS5_VABI *getLongLong)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::int64_t* value);
    InputIterator (APS5_VABI *getUnsignedLongLong)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, std::uint64_t* value);
    InputIterator (APS5_VABI *getFloat)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, float* value);
    InputIterator (APS5_VABI *getDouble)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, double* value);
    InputIterator (APS5_VABI *getLongDouble)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, long double* value);
    InputIterator (APS5_VABI *getPointer)(const Facet* self, InputIterator first, InputIterator last, IosBase* stream, std::int32_t* state, void** value);
};

static_assert(sizeof(Facet) == 0x10);
static_assert(offsetof(Implementation, facets) == 0x10);
static_assert(offsetof(Implementation, facetCount) == 0x18);
static_assert(offsetof(Implementation, transparent) == 0x24);
static_assert(offsetof(Implementation, name) == 0x28);
static_assert(offsetof(IosBase, state) == 0x10);
static_assert(offsetof(IosBase, storage) == 0x28);
static_assert(offsetof(IosBase, callbacks) == 0x30);
static_assert(offsetof(IosBase, locale) == 0x38);
static_assert(sizeof(IosBase) == 0x40);
static_assert(offsetof(String, buffer) == 0x8);
static_assert(offsetof(String, size) == 0x18);
static_assert(sizeof(String) == 0x28);
static_assert(offsetof(CollateFacet, collation) == 0x10);
static_assert(sizeof(CollateFacet) == 0x20);
static_assert(offsetof(CollateVtable, compare) == 0x20);
static_assert(offsetof(CollateVtable, hash) == 0x30);
static_assert(alignof(LocinfoStorage) == 8);
static_assert(offsetof(IosBase, flags) == 0x18);
static_assert(offsetof(StreamBufferVtable, underflow) == 0x38);
static_assert(offsetof(StreamBufferVtable, uflow) == 0x40);
static_assert(offsetof(StreamBuffer, readNext) == 0x38);
static_assert(offsetof(StreamBuffer, readEnd) == 0x58);
static_assert(offsetof(InputIterator, got) == 8);
static_assert(offsetof(InputIterator, value) == 9);
static_assert(sizeof(InputIterator) == 0x10);
static_assert(offsetof(NumGetVtable, facet) == 0x10);
static_assert(offsetof(NumGetVtable, getBool) == 0x30);
static_assert(sizeof(NumGetVtable) == 0x88);

}

extern "C" {
void IosBaseAddStandard_nid_no_patch(GuestLocale::IosBase* stream);
GuestLocale::NumGetVtable NumGetVtable_nid_no_patch();
}

#endif
