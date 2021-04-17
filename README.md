# Dll-Injector

![C++](badges/C++-20-brightgreen.svg)
[![License](badges/License-GPL-3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)

## Languages

- [English](https://github.com/czs108/Dll-Injector/blob/main/README.md)
- [简体中文](https://github.com/czs108/Dll-Injector/blob/main/README-CN.md)

## About The Project

***Dll-Injector*** is a **Windows Dynamic Link Library** injection tool written in *C++20*. It can inject a `.dll` file into a running process by searching its window title or create a new process with an injection.

## Getting Started

**Warning**

> Because there is no compiler can completely support *C++20* now, I did not commit my building configuration files. The following updates will fix this.
>
> The current source code can be built manually with *Visual Studio 2019 Preview v16.8*.

### Prerequisites

The project need to configure on/for **Windows 32-bit**.

## Usage

```
Dll-Injector [-f <proc-path> | <win-title>] <dll-path>
```

**Inject a Dll into a Running Process**

To inject a `.dll` file into a running process, you need to specify the *window title* of the target process and the *path* of the `.dll` file. If `dll-path` is a relative path, it must be relative to the `Dll-Injector.exe`.

```bash
Dll-Injector <win-title> <dll-path>
```

For example, inject the `dllmain_msg.dll` (assume it is in the same directory as *Dll-Injector*) into *Windows Calculator*:

```bash
Dll-Injector dllmain_msg.dll Calculator
```

**Create a New Process with an Injection**

To create a new process with an injection, you must enable the `-f` option firstly and then specify the *paths* of the target process and `.dll` files. If `dll-path` is a relative path, it must be relative to the process file.

```bash
Dll-Injector -f <proc-path> <dll-path>
```

## Documents

You can use [*Doxygen*](http://www.doxygen.nl/) to generate the document.

In order to avoid scanning `.md` files, you must add `*.md` to the `EXCLUDE_PATTERNS` configuration option.

```
EXCLUDE_PATTERNS = *.md
```

This option is in "***Expert***" -> "***Input***" page.

## License

Distributed under the *GNU General Public* License. See `LICENSE` for more information.

## Contact

***GitHub***: https://github.com/czs108/

***E-Mail***: chenzs108@outlook.com

***WeChat***: chenzs108