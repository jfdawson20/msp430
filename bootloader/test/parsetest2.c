#include<stdio.h> 
#include<stdlib.h> 

unsigned int strToHex(char * buf, int start, int end);
unsigned int asciiToHex(char ascii) ;
int calcCB(char *data, int len, unsigned char expected);
int writeMem(char *workBuffer,unsigned int address, unsigned char byteCount);
char parseLine(char * buffer, int length);

int main()
{
	const char * testLine[] = {":10FBDE005E4203001EF30520DF42660000000F4365" \
				              ,":10FBEE003041FF4300003F4330415F4203001FF3AB" \
				              ,":10FBFE00FC275F42660030415E4265001EF3FC2327" \
				              ,":10FC0E00C24F67000F4330416D4F7D900A000B24A9" \
				              ,":10FC1E001F535E4265001EF3FC23C24D67007D4FED" \
				              ,":10FC2E007D900A00F6235F4265001FF3FC23F2402D" \
				              ,":10FC3E000A0067005F4265001FF3FC23F2400D00CF" \
				              ,":10FC4E0067003041B240805A2001C24356005F42E5" \
				              ,":10FC5E00F9107FF0CFFFC24F5700D242F810560076" \
				              ,":10FC6E00E2425800F24020005300F2400600260007" \
				              ,":10FC7E00F24016004100D2432200C2432100D2437B" \
				              ,":10FC8E006100C2436000F24081FF6100E243620006" \
				              ,":10FC9E00C2436300F24017006400F2F0FEFF610001" \
				              ,":10FCAE003F40A6FDB01216FC5F4203001FF3FC2777" \
				              ,":10FCBE005F4266007F9045000B247F9058000D2414" \
				              ,":10FCCE007F903A000F243F40D6FDB01216FCEC3F59" \
				              ,":10FCDE003F40ADFDB01216FCE73F3F40BEFDB012F7" \
				              ,":10FCEE0016FCE23F3F40D0FDB01216FCDD3F1E93E6" \
				              ,":10FCFE0008380D430C4F0C5DCC4300001D530D9E78" \
				              ,":10FD0E00F92330410B120A122E9314380A431B4367" \
				              ,":10FD1E000C4F0C5B6C4C8C110A5C1B530B9EF82326" \
				              ,":10FD2E003AE35A534D9A04200F433A413B41304136" \
				              ,":10FD3E003F43FB3F4A43F63F0F9309205F42650066" \
				              ,":10FD4E001FF3FC23F2402B0067000F4330415F424C" \
				              ,":10FD5E0065001FF3FC23F2402D0067000F43304176" \
				              ,":10FD6E000F4330410F4330417F50CFFF7F9016003D" \
				              ,":10FD7E00042C4F4F5F4FE9FD30414F4330418F11FF" \
				              ,":10FD8E000F5F0F5F0F5F0F5F4FDE3041314000049A" \
				              ,":08FD9E00B01252FC003CFF3FD3" \
				              ,":10FDA600686572700D0A0065726173652073656C13" \
				              ,":10FDB60065637465640D0A004578697420426F6F47" \
				              ,":10FDC600746C6F616465720D0A004865780D0A00EF" \
				              ,":10FDD600496E7661696C696420436F6D6D616E640E" \
				              ,":10FDE6000D0A0001020304050607080900000000C9" \
				              ,":09FDF6000000000A0B0C0D0E0FB9" \
				              ,":10FFE000A4FDA4FD9AFD9AFDA4FD9AFD9AFD9AFD3B" \
				              ,":10FFF0009AFD9AFD9AFD9AFD9AFD9AFD9AFD9AFD49" \
				              ,":040000030000FBDE20"
				              ,":00000001FF"};



	int l = 0;
	unsigned int i = 0;
	unsigned int x = 0;
	unsigned char workBuffer[64]; 
	int bufCount  = 0;
	int dataCount = 0;
	unsigned int sum = 0; 
	int cb = 0;


	printf("Sample Line: %s\n",testLine[0] );

	/* parse incoming line */
	for(l = 0; l < 39; l++)
	{
	    i = 1;  
		while(testLine[l][i] != '\0')
		{
			workBuffer[i-1] = testLine[l][i];
			i++;
		}

      	parseLine(workBuffer,i-1);
    }
	return (0);
}

unsigned int strToHex(char * buf, int start, int end)
{
	unsigned int n = 0x00000000;
	int c  = start;
	while(c <= end)
	{
		n = (n << 4) + asciiToHex(buf[c]);
		c++;
	}
	
	return(n);
}

unsigned int asciiToHex(char ascii) 
{
	char Dict[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	int i = 0; 
	for (i = 0; i < 16;i++) 
	{
		if (Dict[i] == ascii)
		{
			return(i);
		}
	}

	return (-1);
}

int calcCB(char *data, int len, unsigned char expected)
{
	int i;
	unsigned int sum;
	for (i = 0; i< len;i=i+2)
	{
        sum += strToHex(data,i,i+1);
	}
	
	sum = (~sum) + 1; 
	if ((unsigned char)sum != expected) 
	{
        return(-1);
	}
    else
    {
        return(0);
    }
}

int writeMem(char *workBuffer,unsigned int address, unsigned char byteCount)
{
    int i;
    unsigned char data = 0;
    for (i =8;(i-8) < (byteCount * 2); i = i + 2)
    {
        data = strToHex(workBuffer,i,i+1);
        printf("Writing Address -> 0x%x, Value -> 0x%x\n", address, data);
        address++;
    }
    return(0);
}

char parseLine(char *workBuffer, int i)
{
        unsigned char byteCount  = 0; 
	    unsigned short address   = 0;
	    char hexID      = 0;
	    unsigned char checkByte  = 0;
	    unsigned char cb  = 0;

        byteCount  = strToHex(workBuffer,0,1); 
	    address    = strToHex(workBuffer,2,5); 
	    hexID      = strToHex(workBuffer,6,7);
	    checkByte  = strToHex(workBuffer,i-2,i-1);
	    
	    cb = calcCB(workBuffer,i-3 , checkByte);
        
        if (cb == 0 )
        {
            /* write program data */
		    if(hexID == 0x00)
		    {
                writeMem(workBuffer,address,byteCount);
		    }
		
		    /* End of File */
		    else if (hexID == 0x01) 
		    {
			    printf("EOF Detected \n");
		    }
				
		    /* extended segment address , no used/supported */
		    else if (hexID == 0x02) 
		    {

		    }
				        
		    /* Start Segment Address, sets instruction pointer to start of code base */
		    else if (hexID == 0x03) 
		    {
                 printf("IP Update Detected \n");
		    }
				
		    /* Extended linear address, 32 bit addressing, not used/supported */
		    else if (hexID == 0x04) 
		    {
               
		    }
				
		    /* start linear address, not supported/used */
		    else if (hexID == 0x05)
		    {
				
		    }
				
		    /* invalid hex ID */
		    else
		    {
			
		    }
		}
		else
		{
		    return('-');
		}
		
	
	return('+');
}
