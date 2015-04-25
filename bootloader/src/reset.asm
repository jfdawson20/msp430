.global user_NMI_INT
user_NMI_INT: 
    mov &0xC01C, r0
    
.global user_Timer1_A3_1_INT 
user_Timer1_A3_1_INT:
    mov &0xC01A, r0

.global user_Timer1_A3_2_INT
user_Timer1_A3_2_INT:
    mov &0xC018, r0

.global user_Comparator_A_INT
user_Comparator_A_INT:
    mov &0xC016, r0
    
.global user_Watchdog_Timer_INT
user_Watchdog_Timer_INT:
    mov &0xC014, r0
    
.global user_Timer0_A3_1_INT
user_Timer0_A3_1_INT:
    mov &0xC012, r0
    
.global user_Timer0_A3_2_INT
user_Timer0_A3_2_INT:
    mov &0xC012, r0
    
.global user_USCI_1_INT
user_USCI_1_INT:
    mov &0xC00E, r0
    
.global user_USCI_2_INT 
user_USCI_2_INT:
    mov &0xC00C, r0

.global user_ADC10_INT
user_ADC10_INT:
    mov &0xC00A, r0 

.global user_IOPORT2_INT
user_IOPORT2_INT:
    mov &0xC006, r0
    
.global user_IOPORT1_INT
user_IOPORT1_INT:
    mov &0xC004, r0

.global reset
reset:

  mov  #0x0400,r1 
  call #bootloader
  mov  #0x0400,r1
  mov &0xC01E, r0
  ;call #0xc020 
  ;jmp hang

.global hang
hang:
  jmp hang
  
  
