org 0x7c00

; 这是前面的3个字节的跳转指令
; 为什么要定义这个跳转指令? 因FAT12文件系统的文件头是不可执行代码, 因此必须跳过这段代码.
jmp short start ; 跳转到start这个标签处, short代表短跳转,跳转的字节数比较小, jmp占用一个字节, 目标地址start, 由于是短跳转, 也占用一个字节
nop ; 空指令占用一个指令

; 自己定义栈空间
define:
    ; 0x7c00是主引导程序的起始地址
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
    mov sp, BaseOfStack ; 让 sp 指针指向栈的起始地址处
    
    mov ax, 34
    mov cx, 1
    mov bx, Buf
    
    call ReadSector
    
    mov bp, Buf
    mov cx, 29
    
    call Print
    
last:
    hlt
    jmp last    

; 用寄存器传递参数
; es:bp --> string address 目标字符串的地址
; cx    --> string length  目标字符串的长度
Print:
    mov ax, 0x1301
    mov bx, 0x0007
    int 0x10
    ret

; 重置软驱
; no parameter
ResetFloppy:
    push ax ; 在栈中备份
    push dx
    
    mov ah, 0x00
    mov dl, [BS_DrvNum]
    int 0x13
    
    pop dx
    pop ax
    
    ret

; 读取软驱数据
; ax    --> logic sector number
; cx    --> number of sector
; es:bx --> target address
ReadSector:
    push bx
    push cx
    push dx
    push ax
    
    call ResetFloppy
    
    push bx
    push cx
    
    mov bl, [BPB_SecPerTrk]
    div bl
    mov cl, ah
    add cl, 1
    mov ch, al
    shr ch, 1
    mov dh, al
    and dh, 1
    mov dl, [BS_DrvNum]
    
    pop ax
    pop bx
    
    mov ah, 0x02

read:    
    int 0x13
    jc read
    
    pop ax
    pop dx
    pop cx
    pop bx
    
    ret

; 要打印的字符串,定义在这里
MsgStr db  "Hello, DTOS!"    
MsgLen equ ($-MsgStr)
Buf:
    times 510-($-$$) db 0x00
    db 0x55, 0xaa
