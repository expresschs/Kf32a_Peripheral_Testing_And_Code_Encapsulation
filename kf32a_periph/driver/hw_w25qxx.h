#ifndef _HW_W25QXX_H_
#define _HW_W25QXX_H_

//������ַ4k�ֽڶ���(16ҳ 4096 = 256 * 16 )
#define W25QXX_ERASE_SHIFT              12
#define W25QXX_ERASE_SIZE               (0x01 << W25QXX_ERASE_SHIFT)			//4k
#define W25QXX_ERASE_ADDR_MASK          ((~0U) << W25QXX_ERASE_SHIFT)         //���ֵ��4096 ������ ��1 0000 0000  0000 
//�������ַ64k�ֽڶ���
#define W25QXX_BLOCK_ERASE_SHIFT        16
#define W25QXX_BLOCK_ERASE_SIZE         (0x01 << W25QXX_BLOCK_ERASE_SHIFT)
#define W25QXX_BLOCK_ERASE_ADDR_MASK    ((~0U) << W25QXX_BLOCK_ERASE_SHIFT)

//д���ַ256�ֽڶ���
#define W25QXX_WRITE_SHIFT              8
#define W25QXX_WRITE_SIZE               (0x01 << W25QXX_WRITE_SHIFT)
#define W25QXX_WRITE_ADDR_MASK          ((~0U) << W25QXX_WRITE_SHIFT)

uint8_t w25qxx_read(uint32_t addr, uint8_t *buffer, uint32_t size);
uint8_t w25qxx_write(uint32_t addr, uint8_t *buffer, uint32_t size);
uint8_t w25qxx_erase(uint32_t addr, uint32_t size);
uint8_t w25qxx_get_id(uint16_t *id);


#endif

