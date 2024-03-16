#!/bin/zsh

# 编译 shell.c 文件
echo "Compiling shell.c..."
gcc -o build/shell src/shell.c

# 检查编译是否成功
if [ $? -eq 0 ]; then
    echo "Compilation successful. Executing the program..."
    
    # 执行生成的可执行文件
    ./build/shell
    
    # 检查执行结果
    if [ $? -eq 0 ]; then
        echo "Program executed successfully."
    else
        echo "Error: Program execution failed."
    fi

    rm ./build/shell
else
    echo "Error: Compilation failed."
fi

