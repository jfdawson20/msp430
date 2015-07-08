;msp430 global hw interrupt table 
;holds pointers to each function responsible for handling each 
;interrupt
;see reset.asm for details regarding each function 
.word hang                     /* Address : 0xFFE0 , RESERVED*/
.word hang                     /* Address : 0xFFE2 , RESERVED*/
.word user_IOPORT1_INT         /* Address : 0xFFE4 , Source = I/O Port 1*/
.word user_IOPORT2_INT         /* Address : 0xFFE6 , Source = I/O Port 2*/
.word hang       			   /* Address : 0xFFE8 , RESERVED */
.word user_ADC10_INT           /* Address : 0xFFEA , Source = ADC10*/
.word user_USCI_2_INT          /* Address : 0xFFEC , Source = USCI_A0/USCI_B0 Tx, B0 I2C Rx/Tx*/
.word user_USCI_1_INT          /* Address : 0xFFEE , Source = USCI_A0/USCI_B0 RX, B0 I2C Rx/TX*/
.word user_Timer0_A3_2_INT     /* Address : 0xFFF0 , Source = Timer0_A3*/
.word user_Timer0_A3_1_INT     /* Address : 0xFFF2 , Source = Timer0_A3*/
.word user_Watchdog_Timer_INT  /* Address : 0xFFF4 , Source = Watchdog Timer+*/
.word user_Comparator_A_INT    /* Address : 0xFFF6 , Source = Comparator_A+*/
.word user_Timer1_A3_2_INT     /* Address : 0xFFF8 , Source = Timer1_A3*/
.word user_Timer1_A3_1_INT     /* Address : 0xFFFA , Source = Timer1_A3*/
.word user_NMI_INT             /* Address : 0xFFFC , Source = NMI/OSC Fault/Mem Violation*/
.word reset      			   /* Address : 0xFFFE , Source = POR/ER/WT/FKV/PC*/
