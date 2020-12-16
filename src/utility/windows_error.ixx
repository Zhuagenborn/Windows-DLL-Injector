/**
 * @file windows_error.ixx
 * @brief Windows error handling.
 *
 * @author Chen Zhenshuo (chenzs108@outlook.com)
 * @version 1.0
 * @date 2020-10-09
 * @par GitHub
 * https://github.com/czs108/
 *
 * @see https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes/
 */

module;
#include <windows.h>

#include <string>
#include <system_error>
export module utility.windows_error;

/**
 * Throw a @em WindowsError exception containing the last-error.
 *
 * @warning This function does not return.
 */
export [[noreturn]] void ThrowLastError();

/**
 * The Windows-related exception containing error information.
 */
export class WindowsError : public std::system_error {
public:
    /**
     * A constructor.
     *
     * @param err_code  An error code.
     */
    WindowsError(const int err_code) noexcept;

    /**
     * Get the explanatory message.
     */
    const char* what() const noexcept override;

private:
    //! The explanatory message.
    const std::string msg_;
};

/**
 * The map between error codes and their explanatory messages.
 */
export class WindowsCategory : public std::error_category {
public:
    /**
     * Get an instance of @em WindowsCategory.
     */
    static const std::error_category& Instance() noexcept;

    /**
     * Get the name of the category.
     */
    const char* name() const noexcept override;

    /**
     * Get the explanatory message from an error code.
     */
    std::string message(const int err_code) const override;
};


module : private;

/**
 * Get the explanatory message from an error code.
 *
 * @param err_code  An error code.
 */
std::string FormatErrMsg(const int err_code);


const std::error_category& WindowsCategory::Instance() noexcept {
    // The global instance of @em WindowsCategory.
    static const WindowsCategory inst_{};
    return inst_;
}


const char* WindowsCategory::name() const noexcept {
    return "Microsoft Windows";
}


std::string WindowsCategory::message(const int err_code) const {
    return FormatErrMsg(err_code);
}


WindowsError::WindowsError(const int err_code) noexcept :
    std::system_error{ err_code, WindowsCategory::Instance() },
    msg_{ FormatErrMsg(err_code) } {}


const char* WindowsError::what() const noexcept {
    return msg_.c_str();
}


[[noreturn]] void ThrowLastError() {
    throw WindowsError{ static_cast<int>(GetLastError()) };
}


std::string FormatErrMsg(const int err_code) {
    char* raw_msg{ nullptr };
    if (const auto length = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, err_code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            reinterpret_cast<char*>(&raw_msg), 0, nullptr);
        length == 0) {
        ThrowLastError();
    }

    std::string msg{ raw_msg };
    LocalFree(raw_msg);
    return msg;
}