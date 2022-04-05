#include "CRC32.h"
#include "stdio.h"
static unsigned table[256];
//???
static unsigned bitrev(unsigned input, int bw)
{
	int i;
    unsigned var;
	var = 0;
	for(i=0;i<bw;i++)
	{
		if(input & 0x01)
		{
            var |= 1<<(bw-1-i);
        }
		input>>=1;
	}
	return var;
}

//????
//?:X32+X26+...X1+1,poly=(1<<26)|...|(1<<1)|(1<<0)
void crc32_initStatic(unsigned poly)
{
	int i;
	int j;
    unsigned c;
	poly=bitrev(poly,32);
	for(i=0; i<256; i++)
	{
		c = i;
		for (j=0; j<8; j++)
        {
			if(c&1)
			{
				c=poly^(c>>1);
			}
			else
			{
				c=c>>1;
			}
		}
		table[i] = c;
	}
}

unsigned crc32Static(unsigned crc, void* input, int len)
{
	int i;
	unsigned char index;
	unsigned char* pch;
	pch = (unsigned char*)input;
	for(i=0;i<len;i++)
	{
		index = (unsigned char)(crc^*pch);
		crc = (crc>>8)^table[index];
		pch++;
	}
	return crc;
}

void DoCrcStatic(char *CrcDataBuf,int size,char *result)
{
    unsigned crc;
    crc32_initStatic(0x4C11DB7);
	crc = 0xFFFFFFFF;
    crc = crc32Static(crc, CrcDataBuf, size);
	crc ^= 0xFFFFFFFF;
    sprintf(result, "%0X", crc);
}
