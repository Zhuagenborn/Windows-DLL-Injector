/**
 * @file main.cpp
 * @brief The main program.
 *
 * @author Chen Zhenshuo (chenzs108@outlook.com)
 * @author Liu Guowen (liu.guowen@outlook.com)
 * @version 1.0
 * @date 2020-10-09
 * @par GitHub
 * https://github.com/Zhuagenborn
 */

import injector;

import utility.windows_error;

#include <windows.h>

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <system_error>

#include <cassert>

namespace {

/**
 * Get command line arguments.
 */
std::vector<std::string> GetCmdLineArguments(std::size_t argc,
                                             const char* argv[]) noexcept;

/**
 * Print the help information.
 */
void PrintHelp() noexcept;

}  // namespace


int main(int argc, char* argv[]) {
    assert(argc >= 0);

    const auto args{ GetCmdLineArguments(static_cast<std::size_t>(argc), argv) };

    if (args.size() != 3 && args.size() != 4) {
        PrintHelp();
        return EXIT_SUCCESS;
    }

    try {
        std::unique_ptr<Injector> injector{};
        if (args.size() == 3) {
            injector = std::make_unique<RunningInjector>(args[1], args[2]);

        } else if (args.size() == 4) {
            if (args[1] != "-f") {
                throw std::invalid_argument{
                    "The format of the command arguments is invalid.\n"
                };
            }

            injector = std::make_unique<StartupInjector>(args[2], args[3]);

        } else {
            assert(false);
        }

        assert(injector != nullptr);

        injector->Inject();

        std::cout << "[*] The injection has finished." << std::endl;
        return EXIT_SUCCESS;

    } catch (const std::system_error& exp) {
        if (exp.code().value() == ERROR_SUCCESS) {
            std::cerr << "[!] Error: Maybe Dll-Injector cannot find the "
                         "target window."
                      << std::endl;

        } else {
            std::cerr << "[!] Error: " << exp.what() << std::endl;
        }

    } catch (const std::exception& exp) {
        std::cerr << "[!] Error: " << exp.what() << std::endl;
    }

    return EXIT_FAILURE;
}


namespace {

std::vector<std::string> GetCmdLineArguments(
    const std::size_t argc, const char* const argv[]) noexcept {
    std::vector<std::string> args{};
    std::ranges::copy_n(argv, argc, std::back_inserter(args));
    return args;
}


void PrintHelp() noexcept {
    std::cout << "[!] Usage: Dll-Injector [-f <proc-path> | <win-title>] "
                 "<dll-path>"
              << std::endl
              << std::endl;

    std::cout << "[!] Example (1): Create a new process with a Dll injected."
              << std::endl;
    std::cout << "\t```" << std::endl;
    std::cout << "\tDll-Injector -f <proc-path> <dll-path>" << std::endl;
    std::cout << "\t```" << std::endl;
    std::cout << "\tIf <dll-path> is a relative path, it must be relative "
                 "to the process."
              << std::endl
              << std::endl;

    std::cout << "[!] Example (2): Inject a Dll into a running process by "
                 "its window title."
              << std::endl;
    std::cout << "\t```" << std::endl;
    std::cout << "\tDll-Injector <win-title> <dll-path>" << std::endl;
    std::cout << "\t```" << std::endl;
    std::cout << "\tIf <dll-path> is a relative path, it must be "
                 "relative to the Dll-Injector."
              << std::endl
              << std::endl;
}

}  // namespace