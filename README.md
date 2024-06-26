# Shell 项目简介

# 速通教程是shell.md!!!!!, 本README只教你怎么用脚本玩这个shell
# 速通教程是shell.md!!!!!, 本README只教你怎么用脚本玩这个shell
# 速通教程是shell.md!!!!!, 本README只教你怎么用脚本玩这个shell

Shell 项目是一个简单的 Unix shell 实现，用于在 OpenEuler 22.03 环境下执行命令行操作。该项目使用 C 语言编写，并借鉴了系统内核的链表实现。

## 目录结构

- `src/`: 包含源代码文件
  - `shell.c`: 主要的 shell 程序文件
  - `list.h`: 内核链表实现的头文件
- `build/`: 用于存放编译后的可执行文件
  - `run.sh`: 编译和执行 shell 程序的脚本

## 环境要求

- OpenEuler 22.03 或兼容系统
- GCC 编译器
- [shell = zsh]

## 使用方法

在终端中进入项目根目录。

运行 `run.sh` 脚本来编译和执行 shell 程序：

`./run.sh`

如果编译成功且执行无误，您将看到 shell 的提示符，可以开始输入命令。

## 注意事项

该项目仅作为学习和演示用途，可能不适用于生产环境。

此外，以下是对项目中 `shell.c` 文件的简要解释：

- **头文件引用**：引用了一系列标准头文件，包括 `stdio.h`、`unistd.h`、`string.h`、`sys/types.h`、`sys/wait.h` 以及 `stdlib.h`。此外，还引用了自定义的 `list.h` 头文件。
- **宏定义**：定义了一些常量，如 `MAXARG`（最大参数数量）和 `LIMIT`（限制）等。
- **全局变量**：`cnt` 记录命令历史数量，`pri_ins` 存储内置指令，如 "quit"、"exit"、"history" 等。
- **函数声明**：声明了一系列静态内联函数和枚举类型 `RetValue`，以及主函数 `main()`。
- **函数实现**：
  - `get_ins()`: 用于获取输入指令类型，如内置指令或历史指令。
  - `handle_execfromhistory()`: 用于处理历史指令的执行。
  - `handler()`: 用于处理输入的指令，包括调用 `handle_execfromhistory()` 执行历史指令或执行外部程序。
  - `initlist()`、`add_list()`、`print_history()`: 用于初始化、添加和打印历史指令列表。
  - `handle_retval()`: 根据返回值处理不同的操作，如打印历史指令列表等。

- **主函数 `main()`**：
  - 初始化历史指令列表。
  - 进入无限循环，读取用户输入的命令并执行。
  - 解析输入命令，调用 `handler()` 处理，并根据返回值进行相应的操作，如执行命令、打印历史指令等。

