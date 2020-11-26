/**
 * @file running_injector.ixx
 * @brief The injector for running processes.
 *
 * @author Chen Zhenshuo (chenzs108@outlook.com)
 * @version 1.0
 * @date 2020-10-09
 * @par GitHub
 * https://github.com/czs108/
 */

module;
#include <shlwapi.h>
#include <windows.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include <cassert>
export module running_injector;

import injector_interface;

import utility.injection;
import utility.windows_error;

#pragma comment(lib, "shlwapi.lib")

/**
 * The injector for running processes.
 * It can inject a Dll into a running process by searching its window title.
 */
export class RunningInjector final : public Injector {
public:
    /**
     * A constructor.
     *
     * @param win_title The window title of a process.
     * @param dll_path  The path of a Dll.
     */
    RunningInjector(const std::string_view win_title,
                    const std::string_view dll_path);

    void Inject() override;

private:
    //! The window title of the process.
    const std::string win_title_;

    //! The path of the Dll.
    std::string dll_path_;
};


module : private;

/**
 * Get the full path of a file.
 *
 * @param path  The relative path of a file.
 */
std::string GetFullFilePath(const std::string_view path);

/**
 * Get the handle of a process by its window title.
 *
 * @param win_title The window title of a process.
 *
 * @warning
 * If the function cannot find the target window,
 * it will throw a @em WindowsError exception containing an @em ERROR_SUCCESS code.
 */
std::unique_ptr<HANDLE, HandleCloser> GetProcessHandle(
    const std::string_view win_title);


RunningInjector::RunningInjector(const std::string_view win_title,
                                 const std::string_view dll_path) :
    win_title_{ win_title }, dll_path_{ dll_path } {
    if (win_title_.empty() || dll_path_.empty()) {
        throw std::invalid_argument{
            "The target windows title or the path of the Dll is null.\n"
        };
    }

    if (PathIsRelativeA(dll_path_.data())) {
        dll_path_ = GetFullFilePath(dll_path_);
    }
}


void RunningInjector::Inject() {
    assert(!win_title_.empty() && !dll_path_.empty());

    const auto proc = GetProcessHandle(win_title_);
    if (proc == nullptr) {
        ThrowLastError();
    }

    CreateRemoteInjectThread(*proc, dll_path_, false);
}


std::string GetFullFilePath(const std::string_view path) {
    assert(!path.empty());

    char full_path[MAX_PATH]{};
    if (const auto length =
            GetFullPathNameA(path.data(), MAX_PATH, full_path, nullptr);
        length == 0) {
        ThrowLastError();
    }

    return full_path;
}


std::unique_ptr<HANDLE, HandleCloser> GetProcessHandle(
    const std::string_view win_title) {
    assert(!win_title.empty());

    const auto win = FindWindowA(nullptr, win_title.data());
    if (win == nullptr) {
        ThrowLastError();
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(win, &pid);

    std::unique_ptr<HANDLE, HandleCloser> proc{ new HANDLE{ nullptr },
                                                default_handle_deleter };
    if (*proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid); *proc == nullptr) {
        ThrowLastError();
    }

    return proc;
}