# Dll-Injector

![C++](badges/C++-20.svg)
[![Windows](badges/Microsoft-Windows.svg)](https://www.microsoft.com/en-ie/windows)
[![License](badges/License-GPL-3.0.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![DOI](https://zenodo.org/badge/302512489.svg)](https://zenodo.org/badge/latestdoi/302512489)

## 翻译

- [English](https://github.com/czs108/Dll-Injector/blob/main/README.md)
- [简体中文](https://github.com/czs108/Dll-Injector/blob/main/README-CN.md)

## 简介

***Dll-Injector***是一款使用*C++20*编写的**Windows动态链接库**注入工具。它可以通过搜索窗口标题将`.dll`文件注入至运行中的进程，或在创建新进程时进行注入。

## 开始

**警告**

> 目前没有编译器可以完全支持*C++20*，所以我并未提交工程配置文件。后续更新会解决该问题。
>
> 当前源代码可以使用*Visual Studio 2019 Preview v16.8*手动构建。

### 前置条件

该项目需要编译为**Windows 32位**版本。

## 使用

```
Dll-Injector [-f <proc-path> | <win-title>] <dll-path>
```

**将Dll注入至运行中的进程**

对运行中的进程进行注入时，需要指定目标进程的*窗口标题*和`.dll`文件的*路径*。如果`dll-path`参数是一个相对路径，则其必须相对于`Dll-Injector.exe`文件。

```bash
Dll-Injector <win-title> <dll-path>
```

例如，将`dllmain_msg.dll`（假定其位于*Dll-Injector*目录）注入*Windows Calculator*进程：

```bash
Dll-Injector Calculator dllmain_msg.dll
```

**创建新进程时进行注入**

若需要在创建新进程时进行注入，必须先设置`-f`选项，然后再指定目标进程和`.dll`文件的*路径*。如果`dll-path`参数是一个相对路径，则其必须相对于进程文件。

```bash
Dll-Injector -f <proc-path> <dll-path>
```

## 文档

可以使用[*Doxygen*](http://www.doxygen.nl/)生成项目文档。

为了避免扫描`.md`文件，必须将`*.md`添加至`EXCLUDE_PATTERNS`配置选项。

```
EXCLUDE_PATTERNS = *.md
```

该选项位于“***Expert***” -> “***Input***”界面。

## 许可证

使用*GNU General Public*协议，请参考`LICENSE`文件。

## 作者

***GitHub***：https://github.com/czs108/

***E-Mail***：chenzs108@outlook.com

***微信***：chenzs108