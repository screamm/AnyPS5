#include "SceTypes.hpp"
#include "prx/libc/include/Shutdown.hpp"
#include "tests/VideoOutTestEnvironment.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

extern "C" {
int APS5_VABI sceVideoOutOpen(int userId, int busType, int index, const void* param);
int APS5_VABI sceVideoOutClose(int handle);
int APS5_VABI sceVideoOutGetOutputStatus(int handle, VideoOutOutputStatus* status);
int APS5_VABI sceVideoOutGetOutputStatusWithVrr(int handle, void* status);
int APS5_VABI sceVideoOutGetResolutionStatus(int handle, VideoOutResolutionStatus* status);
int APS5_VABI sceVideoOutVrrPegToFixedRate(int handle, std::uint64_t arg1, std::uint64_t arg2);
int APS5_VABI sceVideoOutVrrUnpegFromFixedRate(int handle);
}

static constexpr int SYSTEM_USER = 255;
static constexpr int MAIN_BUS = 0;
static constexpr int NEVER_OPENED_HANDLE = 2;
static constexpr int INVALID_ADDRESS = static_cast<int>(0x80290002);
static constexpr int INVALID_HANDLE = static_cast<int>(0x8029000B);
static constexpr std::size_t VRR_STATUS_SIZE = 80;
static constexpr std::uint8_t UNTOUCHED = 0xAA;

using VrrBuffer = std::array<std::uint8_t, VRR_STATUS_SIZE + 16>;

static void Require(bool value) { if (!value) std::abort(); }

static VrrBuffer Untouched() {
    VrrBuffer buffer;
    buffer.fill(UNTOUCHED);
    return buffer;
}

static bool AllUntouched(const VrrBuffer& buffer) {
    for (std::uint8_t byte : buffer) {
        if (byte != UNTOUCHED) return false;
    }
    return true;
}

static void RequireVrrStatus(int handle, const VideoOutOutputStatus& output) {
    VrrBuffer buffer = Untouched();
    Require(sceVideoOutGetOutputStatusWithVrr(handle, buffer.data()) == 0);
    Require(std::memcmp(buffer.data(), &output, sizeof(output)) == 0);
    for (std::size_t offset = sizeof(output); offset < VRR_STATUS_SIZE; ++offset) Require(buffer[offset] == 0);
    for (std::size_t offset = VRR_STATUS_SIZE; offset < buffer.size(); ++offset) Require(buffer[offset] == UNTOUCHED);
}

static void RequireVrrHandleRejected(int handle) {
    VrrBuffer buffer = Untouched();
    Require(sceVideoOutGetOutputStatusWithVrr(handle, buffer.data()) == INVALID_HANDLE);
    Require(sceVideoOutGetOutputStatusWithVrr(handle, nullptr) == INVALID_ADDRESS);
    Require(AllUntouched(buffer));
}

static bool Rejects(int handle, VideoOutResolutionStatus* status) {
    try {
        sceVideoOutGetResolutionStatus(handle, status);
    } catch (const std::runtime_error&) {
        return true;
    }
    return false;
}

int main() {
    VideoOutTestEnvironment environment;
    int handle = 0;
    try {
        for (int unopened : {-1, 0, 1, NEVER_OPENED_HANDLE, 0x7FFFFFFF}) RequireVrrHandleRejected(unopened);
        handle = sceVideoOutOpen(SYSTEM_USER, MAIN_BUS, 0, nullptr);
    } catch (const std::runtime_error& error) {
        if (std::getenv("ANYPS5_REQUIRE_DISPLAY") != nullptr) throw;
        std::printf("skipped, no display or Vulkan device: %s\n", error.what());
        return 77;
    }
    Require(handle > 0);

    VideoOutResolutionStatus status;
    std::memset(&status, 0xff, sizeof(status));
    Require(sceVideoOutGetResolutionStatus(handle, &status) == 0);
    Require(status.fullWidth == 1920 && status.fullHeight == 1080);
    Require(status.paneWidth == status.fullWidth && status.paneHeight == status.fullHeight);
    Require(status.screenSizeInInch == 0.0f && status.flags == 0 && status.reserved0 == 0);
    Require(status.reserved1[0] == 0 && status.reserved1[1] == 0 && status.reserved1[2] == 0);

    VideoOutOutputStatus output{};
    Require(sceVideoOutGetOutputStatus(handle, &output) == 0);
    Require(status.refreshRate == output.refreshRate);
    Require(output.resolution == 1 && output.dynamicRange == 1 && output.refreshRate == 3 && output.flags == 0);

    RequireVrrStatus(handle, output);
    Require(sceVideoOutVrrPegToFixedRate(handle, 0, 0) == 0);
    RequireVrrStatus(handle, output);
    Require(sceVideoOutVrrUnpegFromFixedRate(handle) == 0);
    RequireVrrStatus(handle, output);

    Require(sceVideoOutGetOutputStatusWithVrr(handle, nullptr) == INVALID_ADDRESS);
    for (int unopened : {-1, 0, NEVER_OPENED_HANDLE, 0x7FFFFFFF}) RequireVrrHandleRejected(unopened);

    Require(Rejects(handle, nullptr));
    Require(Rejects(0, &status));
    Require(Rejects(-1, &status));
    Require(Rejects(NEVER_OPENED_HANDLE, &status));

    Require(sceVideoOutClose(handle) == 0);
    Require(Rejects(handle, &status));
    RequireVrrHandleRejected(handle);
    LibcRunShutdown_nid_postfix();
}
