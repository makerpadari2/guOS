; Assembly used to upload GDTDescriptor to the CPU to use the GDT
[bits 64]           ; 64 bit file
LoadGDT:
    ; Using rdi because it contains the first paramater 
    ; passed to the LoadGDT function
    lgdt [rdi] 

    ; 0x10 is the selector for the KernelData segment
    mov ax, 0x10   

    ; Move ax to all data segment registers 
    mov ds, ax      
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Pop first return address of LoadGDT function and store 
    ; it in the rdi register
    pop rdi  

    ; Move segment selector of KernelCode segment to rax register       
    mov rax, 0x08  

    ; Push 0x08 to stack 
    push rax    

    ; Push return address to stack    
    push rdi        
    retfq
GLOBAL LoadGDT