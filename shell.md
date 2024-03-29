# Shell:半小时速通最简单的shell实验

**作者：** withm  
**日期：** 2024年3月29日 
**适合读者：** 有一定C语言基础（会写链表）

### Shell是什么？

`GPT`：

> Shell（Shell程序）是计算机操作系统中的一种用户界面，它允许用户与操作系统进行交互。Shell是用户与操作系统内核之间的接口，它接受来自用户的命令，并将这些命令传递给操作系统内核以执行。

> Shell可以通过命令行界面（CLI）或图形用户界面（GUI）进行交互。在命令行界面中，用户通过键入命令来与系统进行交互，而在图形用户界面中，用户通过图形化的界面元素（如按钮、菜单和对话框）与系统进行交互。

> Shell不仅仅是一个命令解释器，它还可以执行文件操作、管理进程、执行程序、处理输入输出等操作。在Unix、Linux和类Unix系统中，常见的Shell包括Bash（Bourne Again Shell）、Bourne Shell（sh）、C Shell（csh）等。在Windows系统中，常见的Shell包括Command Prompt（cmd.exe）和PowerShell等。

> 总的来说，Shell是用户与操作系统之间进行交互的重要工具，它提供了一种灵活、强大的方式来控制计算机系统。

这听起来很复杂。但总结起来就是`shell`负责
- 读取指令（这里的指令不是机器指令，而是用户输入的指令，比如`ls`(unix)，`dir`(windows)这样的查看当前目录的指令）
- 执行指令（`shell`：我发现你输入了`ls`，接下来我要这么做...）。

所以为了完成一个`玩具shell`，需要我们做的非常少。我们只需要
- 调用库函数从标准输入中读取输入
- 调用系统调用对输入进行解释

我们需要达成的效果如下：

[shell效果展示](https://postimg.cc/CdLGqnWM)

[history功能展示](https://postimg.cc/sBMrtKWv)

[quit功能展示](https://postimg.cc/qhDmxXgX)

简单总结一下就是
- 能够解释用户指令（如ls, ls -l, mkdir testdir）
- 添加history指令和quit指令的处理（这两个不是bash提供的，需要我们自己实现）
- history允许用户访问最近输入的命令
- ！[数字]来执行history中的指令
- quit允许程序正常退出

> Q：什么叫正常退出？

我们想让一个程序结束有很多方式。
如果你经常使用`shell`我想你大部分时候会选择`Ctrl + C`进行退出。但你也会发现一些用这种方式退出不了的程序。参考如下情景
```zsh
-> xiongzile: ~/workspace/shell git:(master) ✗  python3
Python 3.10.12 (main, Nov 20 2023, 15:14:05) [GCC 11.4.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>>
```
这里我输入了`python3`进入这个程序如果我输入`Ctrl + C`,会显示如下结果：
```zsh
-> xiongzile: ~/workspace/shell git:(master) ✗  python3
Python 3.10.12 (main, Nov 20 2023, 15:14:05) [GCC 11.4.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>>
KeyboardInterrupt
>>>
```
这里它提示你输入了一个中断，但它似乎并不打算配合你退出程序。是的，大部分时候这个中断可以让你的程序中止，但程序也有能力专门处理这个中断，不退出。

所以`Ctrl + C`的语义并不是结束这个程序，而是提供一个中断给程序处理。
程序可能会做一下决定（但不仅仅是这几种可能）
- 立即配合你退出。
- 输出一行字符提醒你这样粗暴的退出是不被喜欢的，请你按照该程序规定的方式进行退出。
- 暴躁地给你一拳。

如果你使用`WSL`的话你可以关闭`powershell`这样不管`WSL`里在运行什么都会退出。这是显然的，这对于`WSL`来说就相当于把电源关了，它当然会结束所有的进程。

> 以上提到的都不是正确的退出。

但是一个`完整的程序`，就算是一个玩具，它至少要告诉你该怎么终止它，所以我实现了`quit`来进行正常的退出。（你也不希望你的程序每次都靠`Ctrl + C`退出，喜提一个为`130`的错误返回值吧）

好，言归正传，正式开始我们的速通。

### 实现

#### `int main(int argc, char *argv[])`   
我希望你知道这两个参数的含义，如果不理解请参考我的另一篇博客刚开始的部分：
[Unix编程](https://github.com/Arrtoria/cs631-labs)

当然如果不考虑后续拓展功能，其实`不传参也是可以完成该实验`的。

考虑`main函数`的部分。我们的预期是如果我们不主动退出，这个程序就不会退出。所以我们的main函数里一定有一个死循环。死循环的`终止条件`是我们决定的。

好了所以你可以开始写了
```C
int main(int argc, char *argv[]) {
    for (; ;) {
     //TODO   
    }
    return 0;
}
```
我个人的习惯是头文件在出`warning`的时候加，因为不太记得什么头文件里声明了什么函数。如果没有用到也不用一股脑地`stdio.h` `stdlib.h`，需要再说。

每一个`shell`都会输出一些特殊的字符来提示你你可以进行输入了，我们的也不例外，课本上的要求是输出`osh>`
所以我们要在`for`循环内部加一个`printf("osh>")`
```C
  1 #include <stdio.h>
  2 int main () {
  3     for (; ;) {
  4         printf("osh>");
  5         char buf[64];
  6         scanf("%s", buf);
  7         printf("%s\n", buf);
  8     }
  9     return 0;
 10 }
```
这一段代码很好理解
- 我们要在每一个循环里等待用户的输入。
- 打印看我们的输入是否正确（这一步是阶段测试，方便debug）

```zsh
osh>dw
dw
osh>fw
fw
osh>fw
fw
osh>fw wfwf
fw
osh>wfwf
osh>wfwf wf w
wfwf
osh>wf
osh>w
osh>
```
我们可以很明显的看到，输出是和预期不符的，不符的原因也很简单，scanf函数的读取碰到`空格`是会停止的，没有办法把读取`包括空格的整行输入`。

要解决这个问题有很多办法，我们可以调整`scanf`内部的格式不用`"%s"`，换成其他的格式。也可以用更简单的办法。因为C提供了读取整行输入的函数。
`char *fgets(char *str, int n, FILE *stream);`
`str`：指向字符数组的指针，用于存储读取的字符串。
`n`：要读取的最大字符数，包括终止 null 字符。
`stream`：指向 FILE 对象的指针，标识要从中读取的文件流。
```C
  1 #include <stdio.h>
  2 #include <string.h>
  3 int main () {
  4     for (; ;) {
  5         printf("osh>");
  6         char buf[64];
  7         memset(buf, 0, 64);
  8         fgets(buf, 64, stdin);
  9         printf("%s\n", buf);
 10     }
 11     return 0;
 12 }
 ```
 然后我们再来看一下效果。
 ```
osh>dw
dw

osh>fwfwf
fwfwf

osh>dwd
dwd

osh>dw w w dw wd
dw w w dw wd

osh>
```
发现还是不太符合预期，不过这显然比读取不了好得多！我们只是在`printf`的时候多打印了一个换行字符。

我们写的`shell`到目前为止已经可以正确的读入用户输入的指令了，也就是我们的任务已经完成一半了！是不是很轻松呢？

但是我们得到的字符串是一个包括空格的一个字符串，我们希望能把他还原成类似`char *argv[]` 的形式。所以我们要通过空格进行分隔，C库提供了这样的函数。

`strtok()`函数用于将字符串分割成一系列小字符串，它可以根据指定的分隔符将一个字符串分解成多个子字符串。在 `C 语言`中，`strtok()` 函数的原型如下：

`char *strtok(char *str, const char *delim);`

`str`：要分割的字符串，第一次调用时传入要分割的字符串，后续调用传入 `NULL`。
`delim`：包含分隔符的字符串，用于指定分割字符串的标志。
`strtok()` 函数会返回被分割后的子字符串，并在每次调用后记住位置以供下一次调用。当没有更多的子字符串可供返回时，返回 `NULL`。

到此为止我们已经完全完成了主函数的所有读取内容，上代码！
```C
int main() {
    printf("Welcome to my shell\n");
    initlist();
    char *argv[MAXARG];
    for (; ;) {
        cnt++;
        printf("osh> ");
        fflush(stdout);
        memset(argv, 0, 40);
        char c;
        int index = 0;
        char buf[64];
        memset(buf, 0, 64);
        fgets(buf, 64, stdin);
        if(buf[0] == '\n' || buf[0] == '\0') 
            continue;
        buf[strlen(buf) - 1] = '\0';
        //printf("%s", buf);
        char *token = strtok(buf, " ");
        while(token && index < MAXARG) {
            argv[index++] = strdup(token);
            token = strtok(NULL, " ");
        }
        if (index == 1 && strcmp(argv[0], "history") == 0)
            cnt--;

        handle_retval(handler(argv,index));
        /*
        for (int i = 0; i < index; i++) {
            printf("%s ", argv[i]);
        }*/
        //should_run = 0;
    }
    return 0;
}
```

里面有些细节是必要的，比如把buf的最后一位置0，都是debug的时候血和泪的教训。注释起来的是我debug留下的，如果追求代码的简洁性可以把这些删去。对于这种`逻辑简单`而且涉及`内存管理`还不算多的程序，完全不需要用gdb来调试，如果你是个稍有经验的编程者，`printf`绝对能满足该程序调试的需要。

主要讲解我们的`handler`函数，这是这个程序的核心实现部分。

在展示代码之前，我们可以思考一下`handler`要完成什么事情。这直接决定了我们需要传入什么参数，以及返回什么值。

我们要处理我们刚才得到的字符串数组，所以我们要把它作为参数传进去，为了方便遍历，我们把`argc`也传进去。

上代码：
```C
static inline int handler(char **argv, int argc)
{   /*
    printf("will execute: ");

    for (int i = 0; i < index; i++) {
        printf("%s ", argv[i]);
    }
    */

    /*handle instruction: history */
    int ins = get_ins(argv, argc);
    switch (ins) {
        case QUIT:
        case EXIT:
            printf("lshell is closing. Goodbye!\n");
            exit(0);
        case HISTRY:
            return HISTORY;
        default:
            break;
    }

    /* handler "!!" or "!1" ... */
    if (argv[0][0] == '!')
        handle_execfromhistory(argv, &argc);
    /*
    for(int i = 0; argv[i]; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
        printf("argc = %d\n",argc);
    }
    */

    /* handler '&' */
    int flag = 0;
    if (strcmp(argv[argc - 1], "&") == 0) {
        flag = 1;
        free(argv[argc - 1]);
        argv[argc - 1] = NULL;
        argc--;
    }

    if (fork() == 0) {
        if(execvp(argv[0], argv) < 0)
            printf("error message: %s failed\n", argv[0]);
        exit(0);
    } else {
        add_list(cnt, argv);
        if (flag == 1) {
            //do nothing
        } else {
              wait(NULL);
        }
    }
    return EXEC;
}
```

可以看见这段代码的逻辑是，先考虑是否为我们后期加入的指令，如`exit`， `quit`和`history`
如果是，我们进行单独的处理。如果不是，我们就调用`execvp`这个系统调用，让操作系统帮我们处理。所以你能理解为什么这么小的一个`shell`却能处理那么多指令吗？ 大部分指令我们都是通过系统调用处理的，并不需要我们自己手动实现。尽管如此，为了实现`history`我们的代码量还是来到了两百行左右。

其他指令的处理就很简单了，我们直接调用`execvp`这个系统调用进行处理，它会把一切完成的很好的！
需要强调的是，`execvp`会直接导致当前进程转到另一个进程里去，所以他是不会返回的。如果返回说明它出错了。我们要输出一串提示我们某个命令出错的字符串。而且我们要在主进程的子进程中调用，因为如果我们直接调用，那我们的进程就结束了转到了另一个进程，成了一次性的。这样的说法应该很好理解。

除此之外题目还额外要求如果指令末尾输入`&`，主进程应该和子进程并行执行，所以我们进行一个判断，在这种情况下不需要等待子进程完成。

接下来考虑`history`的实现。我们能回访历史指令意味着，我们的程序在运行的时候实际上对每一个执行过的指令进行了记录，并且要求只记录`前10条`。

### 大声的告诉我，这样的要求，你应该用什么数据结构？

如果你的回答是`数组`，很好。你的代码实现会非常简单，但是维护会变得比较困难。因为我想你既然会选择用数组，你大概率会选择用索引代表历史指令的条数，然后这个数字会一直增加。这样的实现听起来很简单，做起来也不困难。但我想请你思考一个问题，你开多大的数组？

这样的实现显然是不优雅的，应付应付作业可以。但我既然带你速通我就不会写这样的东西。

#### 如果要用数组怎么实现？
答案是`结构体数组`。这个结构体要包括命令本身（即一个字符串）和它的编号，然后我们造一个十个结构体大小的数组，对这个数组`循环利用`。

```C
struct instruction {
    int no;
    int argc;
    char **argv;
};
struct instrction ins[10];
```
这样的实现，足够优雅也节省下来了空间，并且代码量会相对少很多（毕竟没有繁琐的分配内存释放内存）

`（说自己写链表也从不释放内存的拖出去）`

我推荐使用`数组`的同学选择这样实现。但我本人是用的`链表`，所以我接下来会讲`链表`的。

#### 当然是用链表

我们用`头插法`，每次执行了一条命令，就插在`链表`头。
```C
struct history_commands {
    struct list_head run;
    int position;
    char **argv;
    int argc;
} hiscom;
```

因为个人一直比较偏爱`kernel`，写这个的时候突然想到了`kernel`的链表实现似乎很巧妙，我就把`kernel`的链表拿过来用了。

值得一提的是在代码里`include`某个头文件是没办法直接使用的。因为内核C文件之间的`关联关系`比你想的要复杂。我从里面挑了一部分本次实验用得到的，整合成了一个我自己的头文件。

`kernel链表`实现这篇教学不打算讲，感兴趣的可以等我之后的博客。

言归正传，讲到这里，我们的速通之旅已经过了大半。我们`只需要造个链表把历史指令串起来`，然后在检测到用户输入history的时候`打印`这个链表就可以。

我的实现唯一特别的地方是我把`清除链表的过程放在了打印的过程`里，这是`lazy`的思想（如果你打过算法比赛你应该会知道线段树，就是那里面的`懒惰标记`）

在检测到history的指令的时候会打印链表的前十条，顺便把后面的都清空了。

history完成了之后，与之配套的有个`！[数字]`的指令。可以通过`访问命令编号`直接调用命令。这里我们单独写一个函数处理这种情况。

```C
static inline void handle_execfromhistory(char **argv, int *argc)
{
    if(*argc != 1) return ;
    int position;
    if(strcmp(argv[0], "!!") == 0) {
        position = cnt - 1;
        if (cnt - 1 <= 0) {
            printf("No commands in history.\n");
            return ;
        }
    }else {
        char *p =argv[0];
        position = 0;
        p++;
        while (*p) {
            if(*p > '9' || *p < '0')
                return ;
            position = position * 10 + *p - '0';
            p++;
        }
    }

    //printf("position = %d\n", position);
    struct list_head *node;
    int count = 0;
    list_for_each(node, &hiscom.run) {
        struct history_commands *p = (struct history_commands *)node;
        count += 1;
        if (count > 10) {
            printf("You can only search for nearly ten instructions!\n");
            return ;
        }
        if (p->position == position) {
            *argc = p->argc;
            for (int i = 0; p->argv[i]; i++) {
                argv[i] = p->argv[i];
            }
            return ;
        }
    }
    printf("No such command in history.\n");
}
```
前半部分是处理`！！`以及把`！[数字]`后面的数字转化成`int`类型。因为我们的所有读取都是字符串类型的。（这就是一个很简单的大一的C程序设计题目啦）当然你也可选择更优雅的方式把它直接读成数字类型的，一些特别的scanf提供了这种功能：比如`sscanf()`
以下是一段GPT提供的sscanf用法：
```C
#include <stdio.h>

int main() {
    char str[] = "!2313";
    int number;
    if (sscanf(str, "!%d", &number) == 1) {
        printf("Number: %d\n", number);
    } else {
        printf("Failed to read number.\n");
    }
    return 0;
}
```
这样你就可以把`！2313`后面的数字读到`number`里。
后面的部分就是根据你读到的数字走到链表的某个节点，读取它的命令并执行了。`记得返回一些不符合预期的错误（既是实验要求，也是你应该做的）`。

讲到这里，速通的主要内容就完全结束了。我还没有讲解链表插入等问题。但这些是`细节`方面的问题，如果C基础不好，我讲解了，你也会在复制到你的vscode或者devc++的时候产生一些不明的错误。本人所有编辑和测试都在`linux`下完成，文本编辑器用的是`vim`，如果`copy`到你的电脑上格式可能会有问题。不建议这么做。

想要获取我的代码请直接访问

[shell源码](https://github.com/Arrtoria/shell/tree/master)

用法另一个README写的很详细。

感谢您的阅读！有任何问题欢迎发邮件给我`xiongzile99@gmail.com`

