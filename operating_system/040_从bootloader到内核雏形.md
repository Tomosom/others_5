# 从bootloader到内核雏开多
- 整体设计                                    5(3                   ⑶+ C语言
               汇编      的功能；通过BIOS茨取硬件                           KERNEL
            BOOT       mm；                                 运肝32位辦献,
                                                           实现操作系统内核模块
      512宇节内完成加载       到kernel执行
      loader的功能，并
      跳转到loader执行

- 问题
    为什么不能从boot直接加载kernel, 并跳转运行？

- 设计思路
     -boot必须小于512字节，无法完成过多功能
     _ kerne丨需要运行于32位保护模式（汇编+ C语言）
     -使用loader中转：获取必要硬件信息，进入保护模式

-  bootloader重构方案

- 文件功能定义                         功能定义
            文件名                 常量定义，宏定义
          common.asm            实模式下的文件加载功能定义
           blfunc.asm
           boot.asm             加载loader并跳转（引导扇区)
           loader.asm
                                1. 必要硬件初始化
                                2. 加载 kernel
                                3. 进入街户模式
                                4. 岿齡到kernel执行

-  blfunc.asm 接口设计
将bootloader相关函数        %include "blfunc.asm"
扩展到雌，并- 到
BLMain 对书iff           BaseOfStack            equ  0X7C00
                                                   0XB000
                       BaseOfTarget           equ
                                                     tt
                       Target db     "KERNEL
                       TarLen equ    ($-Target)            必须的常畺和
                       BLMain:       bootloade「程序人口
                       Buffer db 0

临时缓冲区起始地址，定义到末尾处

-  blfunc.asm注意事项
    一 ％include "blfunc.asm" 必 须 是 第 _ 条 " 包 含 〃语句
    -%include "blfunc.asm"强制从BLMain标签处开始执行
    - Buffer为必要的内存缓冲区，必须在代码末尾定义

# 编程实验 bootloader 重构

- 内核雏形构造

- 问题
      kerne丨.out加载进内存后能直接被
             x86处理器运行吗？

- 原因分析
    -kernel.out是Linux系统中的可执行程序
    -而Linux中的可执行程序为elf格式的文件（固定数据格式）
    -处理器只"认得"代码和数据，无法正确执行elf可执行程序

-  方案设计
        提取elf文件中的代码段与数据段（删除e|f文件格式信息)
         重定位提取后的代码段和数据段，得到内核文件
         加载内核文件到内存（起始地址可自定义）
         跳转到内核入口地址处执行

-  elf2kobj 介绍
          delphi@delphi-vm:~$ ./elf2kobj
          // ELF file converter for the course of D.T.Software.
          // Created by Delphi Tang.
          // Version: 0.0.1
          Usage:
                 Convert to bin:
                       elf2kobj -c elf_file_in bin_file_out
                 Convert to nasm:
                       elf2kobj -r elf_file_in nasm_file_out
                 Dump ELF file：
                       elf2kobj -d elf_file_in


- 解决方案
     1. nasm -felf kentry.asm -o objs/kentry.o
     2. CFLAGS := -fno-builtin -fno-stack-protector
     3. gcc $(CFLAGS)-c kmain.c -o objs/kmain.o
     4. Id -s objs/kentry.o objs/kmain.o -o exes/kernel.out
     5. ./elf2kobj -CB000 exes/kernel.out kernel
               kerne丨将被加载到内
               存地址OxBOOO处

# 编程实验 内核雏形构建

- 使用nasm和gcc编译得到的是elf目标文件
-  Id将elf目标文件装配成为elf可执行程序
- 使用elf2kobj将elf可执行程序转换为内核文件
- 在实模式下加载转换得到的内核文件
- 进入保护模式后执行跳转到内核起始位置处执行
