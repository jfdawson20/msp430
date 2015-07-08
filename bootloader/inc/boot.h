/* Memory Map */
#define SFR_BASE             0x0000
#define SFR_TOP              0x000F
#define R8_BIT_PERIPH_BASE   0x0010
#define R8_BIT_PERIPH_TOP    0x00FF
#define R16_BIT_PERIPH_BASE  0x100
#define R16_BIT_PERIPH_TOP   0x1FF
#define RAM_BASE             0x0200
#define RAM_TOP              0x03FF
#define INFO_BASE            0x1000
#define INFO_TOP             0x10FF
#define USER_VEC_TABLE_BASE  0xC000
#define USER_VEC_TABLE_TOP   0xC01E
#define USER_FLASH_BASE      0xC020
#define USER_FLASH_TOP       0xF9FF
#define BOOT_FLASH_BASE      0xFBFF
#define BOOT_FLASH_TOP       0xFFDE
#define MSR_VEC_TABLE_BASE   0xFFE0
#define MSR_VEC_TABLE_TOP    0xFFFE

/* other defines */
#define ENTER_BOOT 0xAA

/* Function Prototypes */
int  BootGetC_NonBlocking(char *readVal);
char BootGetC_Blocking();
int  BootPutC(char c);
int  UartWriteBuffer(char *s);
int  EraseFlash();
int  WriteFlashByte(unsigned char data, unsigned int address);
int  WriteLineFlash(unsigned char * buf, unsigned int address, unsigned int byteCount);
unsigned int StrToHex(unsigned char * buf, int start, int end);
unsigned int AsciiToHex(char ascii) ;
int  CalcCB(unsigned char *data, int len, unsigned char expected);
char ParseLine(unsigned char * buffer, int length);