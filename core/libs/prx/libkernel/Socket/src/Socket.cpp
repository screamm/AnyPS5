#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libkernel/File/include/DescriptorPoll.hpp"
#include "prx/libkernel/Socket/include/SocketPoll.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace {

constexpr int GuestEbadf = 9;
constexpr int GuestEinval = 22;
constexpr int SelectDescriptorLimit = 1024;

struct GuestTimeval {
    std::int64_t seconds;
    std::int64_t microseconds;
};

std::atomic<KernelSocketPoll::Poller> g_socketPoller{nullptr};

bool DescriptorSet(const void* set, int descriptor) {
    return set != nullptr && ((static_cast<const std::uint64_t*>(set)[descriptor / 64] >> (descriptor % 64)) & 1u) != 0;
}

void ClearDescriptor(void* set, int descriptor) {
    if (set != nullptr) static_cast<std::uint64_t*>(set)[descriptor / 64] &= ~(std::uint64_t{1} << (descriptor % 64));
}

bool ReadSelected(const KernelSocketPoll::Entry& entry) {
    return (entry.events & KernelSocketPoll::Readable) != 0 && (entry.revents & (KernelSocketPoll::Readable | KernelSocketPoll::HangUp | KernelSocketPoll::Error)) != 0;
}

bool WriteSelected(const KernelSocketPoll::Entry& entry) {
    return (entry.events & KernelSocketPoll::Writable) != 0 && (entry.revents & (KernelSocketPoll::Writable | KernelSocketPoll::HangUp | KernelSocketPoll::Error)) != 0;
}

bool ExceptSelected(const KernelSocketPoll::Entry& entry) {
    return (entry.events & KernelSocketPoll::Urgent) != 0 && (entry.revents & (KernelSocketPoll::Urgent | KernelSocketPoll::Error)) != 0;
}

int SelectedCount(const KernelSocketPoll::Entry& entry) {
    return static_cast<int>(ReadSelected(entry)) + static_cast<int>(WriteSelected(entry)) + static_cast<int>(ExceptSelected(entry));
}

void ClearUnselected(const KernelSocketPoll::Entry& entry, void* readfds, void* writefds, void* exceptfds) {
    if (!ReadSelected(entry)) ClearDescriptor(readfds, entry.descriptor);
    if (!WriteSelected(entry)) ClearDescriptor(writefds, entry.descriptor);
    if (!ExceptSelected(entry)) ClearDescriptor(exceptfds, entry.descriptor);
}

}

extern "C" {

const char* APS5_VABI __inet_ntop_nid_postfix(int family, const void* source, char* destination, std::uint32_t capacity);
int APS5_VABI __inet_pton_nid_postfix(int family, const char* text, void* destination);

const char* APS5_VABI inet_ntop_nid_postfix(int af, const void* src, char* dst, uint32_t size) {
    return __inet_ntop_nid_postfix(af, src, dst, size);
}

int APS5_VABI inet_pton_nid_postfix(int af, const char* src, void* dst) {
    return __inet_pton_nid_postfix(af, src, dst);
}

int* APS5_VABI __error_nid_postfix();

void KernelSetSocketPoller_nid_no_patch(KernelSocketPoll::Poller poller) {
    g_socketPoller.store(poller);
}

int APS5_VABI select_nid_postfix(int nfds, void* readfds, void* writefds, void* exceptfds, const void* timeout) {
    const auto* limit = static_cast<const GuestTimeval*>(timeout);
    if (nfds < 0 || nfds > SelectDescriptorLimit || (limit != nullptr && (limit->seconds < 0 || limit->microseconds < 0 || limit->microseconds >= 1000000))) {
        *__error_nid_postfix() = GuestEinval;
        return -1;
    }
    std::vector<KernelSocketPoll::Entry> entries;
    for (int descriptor = 0; descriptor < nfds; ++descriptor) {
        short events = 0;
        if (DescriptorSet(readfds, descriptor)) events |= KernelSocketPoll::Readable;
        if (DescriptorSet(writefds, descriptor)) events |= KernelSocketPoll::Writable;
        if (DescriptorSet(exceptfds, descriptor)) events |= KernelSocketPoll::Urgent;
        if (events != 0) entries.push_back({descriptor, events, 0});
    }
    const auto poller = g_socketPoller.load();
    if (poller != nullptr && !entries.empty()) {
        const int result = poller(entries.data(), static_cast<int>(entries.size()), 0);
        if (result < 0) {
            *__error_nid_postfix() = -result;
            return -1;
        }
    }
    std::vector<KernelSocketPoll::Entry> sockets;
    std::vector<KernelSocketPoll::Entry> host;
    for (const auto& entry : entries) {
        if (poller != nullptr && (entry.revents & KernelSocketPoll::Unknown) == 0) sockets.push_back({entry.descriptor, entry.events, 0});
        else host.push_back({entry.descriptor, entry.events, 0});
    }
    const auto start = std::chrono::steady_clock::now();
    const auto deadline = limit == nullptr ? std::chrono::steady_clock::time_point::max()
        : start + std::chrono::seconds(limit->seconds) + std::chrono::microseconds(limit->microseconds);
    for (;;) {
        int waitMilliseconds = 100;
        if (limit != nullptr) {
            const auto remaining = std::chrono::duration_cast<std::chrono::microseconds>(deadline - std::chrono::steady_clock::now()).count();
            waitMilliseconds = static_cast<int>(std::clamp<std::int64_t>((remaining + 999) / 1000, 0, 100));
        }
        int hostReady = 0;
        if (!host.empty()) {
            hostReady = File::PollHostDescriptors(host.data(), static_cast<int>(host.size()), sockets.empty() ? waitMilliseconds : 0, "select");
            if (hostReady < 0) {
                *__error_nid_postfix() = -hostReady;
                return -1;
            }
        }
        if (!sockets.empty()) {
            for (auto& entry : sockets) entry.revents = 0;
            const int result = poller(sockets.data(), static_cast<int>(sockets.size()), hostReady != 0 ? 0 : waitMilliseconds);
            if (result < 0) {
                *__error_nid_postfix() = -result;
                return -1;
            }
            for (const auto& entry : sockets) {
                if ((entry.revents & KernelSocketPoll::Unknown) != 0) {
                    *__error_nid_postfix() = GuestEbadf;
                    return -1;
                }
            }
        }
        if (host.empty() && sockets.empty() && waitMilliseconds > 0) std::this_thread::sleep_for(std::chrono::milliseconds(waitMilliseconds));
        int ready = 0;
        for (const auto* group : {&sockets, &host})
            for (const auto& entry : *group) ready += SelectedCount(entry);
        if (ready > 0) {
            for (const auto* group : {&sockets, &host})
                for (const auto& entry : *group) ClearUnselected(entry, readfds, writefds, exceptfds);
            return ready;
        }
        if (limit != nullptr && std::chrono::steady_clock::now() >= deadline) {
            for (const auto& entry : entries) {
                ClearDescriptor(readfds, entry.descriptor);
                ClearDescriptor(writefds, entry.descriptor);
                ClearDescriptor(exceptfds, entry.descriptor);
            }
            return 0;
        }
    }
}

}
