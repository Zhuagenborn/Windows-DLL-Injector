/**
 * @file startup_injector.ixx
 * @brief The injector for new starting processes.
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
#include <stdexcept>
#include <string>
#include <string_view>
#include <optional>

#include <cassert>
export module startup_injector;

import injector_interface;

import utility.injection;
import utility.windows_error;

/**
 * @brief
 * The injector for new starting processes.
 * It can create a new process with a dynamic-link library injected.
 */
export class StartupInjector final : public Injector {
public:
    /**
     * @brief A constructor.
     *
     * @param proc_path The path of a process.
     * @param dll_path  The path of a dynamic-link library.
     */
    StartupInjector(std::string_view proc_path, std::string_view dll_path);

    void Inject() override;

    /**
     * @brief Get the information of the process.
     *
     * @return If the process failed to create, the return value is empty.
     */
    std::optional<PROCESS_INFORMATION> GetHandle() const noexcept;

    /**
     * @brief Check whether the process is running.
     *
     * @return @p true if the process is running, otherwise @p false.
     */
    bool Running() const noexcept;

    //! Terminate the process.
    void Terminate() noexcept;

private:
    //! The function type for closing all handles of a process.
    using HandleCloser = std::function<void(PROCESS_INFORMATION*)>;

    //! The path of the process.
    std::string proc_path_;

    //! The path of the dynamic-link library.
    std::string dll_path_;

    //! The process information.
    std::unique_ptr<PROCESS_INFORMATION, HandleCloser> proc_{};
};


module : private;

/**
 * @brief Close all handles of a process.
 *
 * @param proc The @p PROCESS_INFORMATION structure of a process.
 */
void CloseHandles(PROCESS_INFORMATION* proc) noexcept;

/**
 * @brief Terminate a process.
 *
 * @param proc The @p PROCESS_INFORMATION structure of a process.
 */
void Terminate(PROCESS_INFORMATION* proc) noexcept;


StartupInjector::StartupInjector(const std::string_view proc_path,
                                 const std::string_view dll_path) :
    proc_path_{ proc_path }, dll_path_{ dll_path } {
    if (proc_path_.empty() || dll_path_.empty()) {
        throw std::invalid_argument{
            "The path of the process or dynamic-link library is null.\n"
        };
    }
}


void StartupInjector::Inject() {
    assert(!proc_path_.empty() && !dll_path_.empty());

    auto proc_terminator{ [](PROCESS_INFORMATION* const proc) noexcept {
        assert(proc != nullptr);

        ::Terminate(proc);
        CloseHandles(proc);
        delete proc;
    } };

    std::unique_ptr<PROCESS_INFORMATION, decltype(proc_terminator)> proc{
        new PROCESS_INFORMATION{}, proc_terminator
    };

    STARTUPINFOA startup{ .cb = sizeof(startup) };
    if (CreateProcessA(proc_path_.data(), nullptr, nullptr, nullptr, FALSE,
                       CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS, nullptr,
                       nullptr, &startup, proc.get())
        == FALSE) {
        ThrowLastError();
    }

    CreateRemoteInjectThread(proc->hProcess, dll_path_, true);

    if (ResumeThread(proc->hThread) == static_cast<DWORD>(-1)) {
        ThrowLastError();
    }

    auto proc_closer{ [](PROCESS_INFORMATION* const proc) noexcept {
        assert(proc != nullptr);

        CloseHandles(proc);
        delete proc;
    } };

    proc_ = { proc.release(), proc_closer };
}


std::optional<PROCESS_INFORMATION> StartupInjector::GetHandle() const noexcept {
    return Running() ? std::make_optional(*proc_) : std::nullopt;
}


bool StartupInjector::Running() const noexcept {
    return proc_ != nullptr;
}


void StartupInjector::Terminate() noexcept {
    if (Running()) {
        ::Terminate(proc_.get());
        proc_.reset();
    }
}


void CloseHandles(PROCESS_INFORMATION* const proc) noexcept {
    assert(proc != nullptr);

    if (proc->hThread != nullptr) {
        CloseHandle(proc->hThread);
    }

    if (proc->hProcess != nullptr) {
        CloseHandle(proc->hProcess);
    }
}


void Terminate(PROCESS_INFORMATION* const proc) noexcept {
    assert(proc != nullptr);

    if (proc->hProcess != nullptr) {
        TerminateProcess(proc->hProcess, EXIT_SUCCESS);
    }
}