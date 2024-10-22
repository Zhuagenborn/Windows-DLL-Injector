/**
 * @file windows_error.ixx
 * @brief Windows error handling.
 *
 * @author Chen Zhenshuo (chenzs108@outlook.com)
 * @author Liu Guowen (liu.guowen@outlook.com)
 * @version 1.0
 * @date 2020-10-09
 * @par GitHub
 * https://github.com/Zhuagenborn
 *
 * @see https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes
 */

module;
#include <windows.h>

#include <system_error>
export module utility.windows_error;

//! Throw a @p std::system_error exception containing the last-error.
export [[noreturn]] void ThrowLastError();

module : private;

[[noreturn]] void ThrowLastError() {
    throw std::system_error{ static_cast<int>(GetLastError()), std::system_category() };
}