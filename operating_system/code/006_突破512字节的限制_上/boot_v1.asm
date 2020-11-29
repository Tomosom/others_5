org 0x7c00

; 这是前面的3个字节的跳转指令
; 为什么要定义这个跳转指令? 因FAT12文件系统的文件头是不可执行代码, 因此必须跳过这段代码.
jmp short start ; 跳转到start这个标签处, short代表短跳转,跳转的字节数比较小, jmp占用一个字节, 目标地址start, 由于是短跳转, 也占用一个字节
nop ; 空指令占用一个指令

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
    
    mov ax, MsgStr ; ax寄存器保存目标字符串的地址
    mov cx, 6 ; 打印6个字符出来
    
    ; 按照BIOS调用里边的规定, 填参数
    mov bp, ax ; 指定目标字符串里边的段内偏移地址
    mov ax, ds
    mov es, ax ; 指定目标字符串所在段的起始地址
    mov ax, 0x1301 ; 指定打印参数
    mov bx, 0x0007
    int 0x10 ; 执行BIOS的0x10号中断
    
last:
    hlt
    jmp last    

; 要打印的字符串,定义在这里
MsgStr db "Hello, DTOS!"
MsgLen equ ($-MsgStr) ; $ : 当前代码的地址. 两个地址之差 
Buf:
    times 510-($-$$) db 0x00
    db 0x55, 0xaa
