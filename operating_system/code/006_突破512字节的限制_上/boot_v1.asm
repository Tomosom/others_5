org 0x7c00

; 这是前面的3个字节的跳转指令
; 为什么要定义这个跳转指令? 因FAT12文件系统的文件头是不可执行代码, 因此必须跳过这段代码.
jmp short start ; 跳转到start这个标签处, short代表短跳转,跳转的字节数比较小, jmp占用一个字节, 目标地址start, 由于是短跳转, 也占用一个字节
nop ; 空指令占用一个指令

define:
    ; 为什么主引导程序的起始地址能作为栈空间的起始地址?
    ; 因为代码的执行使用低地址向高地址执行, 而栈的增长方向是从高地址向低地址增长
    BaseOfStack equ 0x7c00

; fat12文件系统需要的文件头,我们现在依赖fat12文件系统,必然的第0扇区(主引导区),必须要有这些信息. 这些信息的偏移位置为3, 前面3个字节是跳转指令.
header:
    BS_OEMName     db "D.T.Soft"
    BPB_BytsPerSec dw 512
    BPB_SecPerClus db 1
    BPB_RsvdSecCnt dw 1
    BPB_NumFATs    db 2
    BPB_RootEntCnt dw 224
    BPB_TotSec16   dw 2880
    BPB_Media      db 0xF0
    BPB_FATSz16    dw 9
    BPB_SecPerTrk  dw 18
    BPB_NumHeads   dw 2
    BPB_HiddSec    dd 0
    BPB_TotSec32   dd 0
    BS_DrvNum      db 0
    BS_Reserved1   db 0
    BS_BootSig     db 0x29
    BS_VolID       dd 0
    BS_VolLab      db "D.T.OS-0.01"
    BS_FileSysType db "FAT12   "

start:
    mov ax, cs ; 基本的寄存器初始化. 在我们这个主引导程序中, 代码段/数据段/附加段全部都是在一起, 所以没必要细分, 直接都初始化为一样的值.
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov sp, ax
    
    mov ax, MsgStr
    mov cx, 6
    
    mov bp, ax
    mov ax, ds
    mov es, ax
    mov ax, 0x1301
    mov bx, 0x0007
    int 0x10
    
last:
    hlt
    jmp last    

; 要打印的字符串,定义在这里
MsgStr db "Hello, DTOS!"    
Buf:
    times 510-($-$$) db 0x00
    db 0x55, 0xaa
