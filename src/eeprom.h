#ifndef EEPROM_INCLUDE
#define EEPROM_INCLUDE

typedef unsigned char BYTE;
typedef unsigned int WORD;

#define SECTOR_SIZE  512
#define SECTOR_ADDR(sector) (sector*SECTOR_SIZE)


BYTE IapReadByte(WORD addr);
void IapProgramByte(WORD addr, BYTE dat);
//需要擦除的扇区地址，以扇区的绝对地址

void IapEraseSector(WORD addr);

#endif