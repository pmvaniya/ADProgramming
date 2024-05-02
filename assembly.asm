           global     _start    
           section    .text     
_start:                         
           ; line 1: push int           
           mov        rax, 64   
           push       rax       
           ; line 1: push int           
           mov        rax, 32   
           push       rax       
           ; line 1: add           
           pop        rax       
           pop        rbx       
           add        rbx, rax  
           push       rbx       
exit:                           
           mov        rax, 60   
           pop        rdi       
           syscall              
