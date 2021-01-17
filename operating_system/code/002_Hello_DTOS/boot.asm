org 0x7c00          ;指明入口, 主引导程序的入口是0x7c00这个物理地址, 类似于c/c++的main函数

start:              ;定义标签, 也是后面跟着的一条指令的地址
    ; cs : 代码段
    ; ds : 数据段
    ; es : 附加数据段
    ; 在这个程序里边,并没有明显地分段, 即数据段和代码段是集中在一起的, 因此下面这样赋值
    mov ax, cs      ;初始化各个段寄存器, 将关键寄存器的值设置为0, cs这个段寄存器里边的值是0
    mov ss, ax
    mov ds, ax
    mov es, ax
    
    mov si, msg     ; 将msg标签所对应的地址放到si寄存器中
    
print:
    mov al, [si]    ; 取数据, []类似c中的*
    add si, 1
    cmp al, 0x00    ; 判断是否到字符串的结束
    je last         ; je : 看上面的比较结果是否相等, 若相等, 则跳转
    mov ah, 0x0e    ; 下面三条指令是打印
    mov bx, 0x0f
    int 0x10
    jmp print   ; 循环
    
last:
    hlt
    jmp last
    
msg:
    db 0x0a, 0x0a                   ;db : define byte, 定义了两个数据, 0x0a就是换行
    db "Hello, DTOS!"               ;在内存里边就是一系列连续的ASCII码
    db 0x0a, 0x0a
    times 510-($-$$) db 0x00        ; 512byte的其他字节填零, $:当前行的地址, $$当前汇编的起始地址, ($-$$):当前代码占用多少个字节
    db 0x55, 0xaa   ; 主引导程序的标识 0x55aa