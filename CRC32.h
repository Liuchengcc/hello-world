#ifndef __CRC32_H_
#define __CRC32_H_

#ifdef __cplusplus
extern "C" {
#endif

void crc32_initStatic(unsigned poly);
unsigned crc32Static(unsigned crc, void* input, int len);
void DoCrcStatic(char *CrcDataBuf,int size,char *result);

#ifdef __cplusplus
}
#endif

#endif
