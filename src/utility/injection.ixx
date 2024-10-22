/**
 * @file injection.ixx
 * @brief Some injection tools.
 *
 * @author Chen Zhenshuo (chenzs108@outlook.com)
 * @author Liu Guowen (liu.guowen@outlook.com)
 * @version 1.0
 * @date 2020-10-09
 * @par GitHub
 * https://github.com/Zhuagenborn
 */

module;
#include <windows.h>

#include <functional>
#include <memory>
#include <string_view>

#include <cassert>
#include <cstddef>
export module utility.injection;

import utility.windows_error;

//! The function type for closing a handle.
export using HandleCloser = std::function<void(HANDLE*)>;

//! A default function for closing a handle.
export HandleCloser default_handle_closer{ [](HANDLE* const handle) noexcept {
    assert(handle != nullptr);

    if (*handle != nullptr) {
        CloseHandle(*handle);
    }
} };

//! A default function for closing a handle and deleting its memory.
export HandleCloser default_handle_deleter{ [](HANDLE* const handle) noexcept {
    assert(handle != nullptr);

    default_handle_closer(handle);
    delete handle;
} };

/**
 * @brief Allocate memory within a process.
 *
 * @param proc The handle of a process.
 * @param size The required size.
 * @return The base address of the memory.
 */
export void* AllocRemoteMemory(HANDLE proc, std::size_t size);

/**
 * @brief Create a dynamic-link library injection thread within a process.
 *
 * @param proc The handle of a process.
 * @param size The path of a dynamic-link library.
 * @param wait Whether to wait for the thread to end.
 */
export void CreateRemoteInjectThread(HANDLE proc,
                                     std::string_view dll_path,
                                     bool wait);


module : private;

void* AllocRemoteMemory(const HANDLE proc, const std::size_t size) {
    assert(proc != nullptr);
    assert(size != 0);

    // The memory cannot be deleted. Otherwise, the target process might crash.
    if (const auto buffer{ VirtualAllocEx(
            proc, nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE) };
        buffer == nullptr) {
        ThrowLastError();
    } else {
        return buffer;
    }
}


void CreateRemoteInjectThread(const HANDLE proc,
                              const std::string_view dll_path,
                              const bool wait) {
    assert(proc != nullptr);
    assert(!dll_path.empty());

    const auto dll_path_size{ dll_path.length() + 1 };
    const auto buffer{ AllocRemoteMemory(proc, dll_path_size) };
    if (!WriteProcessMemory(proc, buffer, dll_path.data(), dll_path_size,
                            nullptr)) {
        ThrowLastError();
    }

    const auto kernel32{ GetModuleHandleA("kernel32.dll") };
    if (kernel32 == nullptr) {
        ThrowLastError();
    }

    const auto load_library{ GetProcAddress(kernel32, "LoadLibraryA") };
    if (load_library == nullptr) {
        ThrowLastError();
    }

    HANDLE thread{ nullptr };
    std::unique_ptr<HANDLE, HandleCloser> thread_raii{ &thread,
                                                       default_handle_closer };
    if (thread = CreateRemoteThread(
            proc, nullptr, 0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(load_library), buffer, 0,
            nullptr);
        thread == nullptr) {
        ThrowLastError();
    }

    if (wait) {
        if (WaitForSingleObject(thread, INFINITE) == WAIT_FAILED) {
            ThrowLastError();
        }
    }
}