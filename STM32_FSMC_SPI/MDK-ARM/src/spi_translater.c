#include "spi_translater.h"
#include "stdbool.h"

extern SPI_HandleTypeDef hspi1;

bool init_state = false;
bool rx_done = false;


#define SPI_TRANS_HEAD_CMD          (0x30)
#define SPI_TRANS_HEAD_DATA         (0x31)
#define SPI_TRANS_HEAD_DATA_ARRAY   (0x32)


#define BUFFER_SIZE   (1024U)

uint8_t img_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];
bool is_data_array = false;


void SPI_Translater_Init(void)
{
  HAL_SPI_Receive_DMA(&hspi1, rx_buffer, 3);
}


void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef * hspi)
{
  switch(rx_buffer[0])
  {
    case SPI_TRANS_HEAD_CMD:
      LCD_CmdWrite(rx_buffer[1]);
      break;
    case SPI_TRANS_HEAD_DATA:
      LCD_DataWrite((rx_buffer[1] << 8) | rx_buffer[2]);
      break;
    case SPI_TRANS_HEAD_DATA_ARRAY:
      HAL_SPI_Receive_DMA(&hspi1, img_buffer, (uint16_t)rx_buffer[2]);
      is_data_array = true;
      break;  
    default:
      break;
  }
  
  if (true == is_data_array)
  {
    lcdPrintf("%d", rx_buffer[2]);
    for(uint8_t i = 0; i < rx_buffer[2]; i+=2)
    {
      LCD_DataWrite((img_buffer[i] << 8) | img_buffer[i+1]);
    }
    
    HAL_SPI_Receive_DMA(&hspi1, rx_buffer, 3);  // finish receiving array, wait service cmds
    is_data_array = false;
  }
  
  rx_buffer[0] = 0;
}









