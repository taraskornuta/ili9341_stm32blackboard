
#ifndef ILI_9341_SPI_BUS_H_
#define ILI_9341_SPI_BUS_H_

#include <linux/spi/spi.h>
#include <linux/delay.h>

#define SPI_TRANS_HEAD_CMD          (0x30)
#define SPI_TRANS_HEAD_DATA         (0x31)
#define SPI_TRANS_HEAD_DATA_ARRAY   (0x32)

struct lcd_config {
  char   *name;
  struct spi_board_info  *spi;
  struct platform_device *pdev;
};

static struct lcd_config lcd_config = {
  .name = "ili9341_spi_translater",
  .spi = &(struct spi_board_info) {
    .modalias = "fb_ili9341",
    .max_speed_hz = 48000000,
    .mode = SPI_MODE_1,
  },
};

struct spi_device *spi_device;

//******************************************************************************

static int spi_device_found(struct device *dev, void *data)
{
  struct spi_device *spi = container_of(dev, struct spi_device, dev);

  printk(KERN_INFO ":      %s %s %dHz %d bits mode=0x%02X\n",
    spi->modalias, dev_name(dev), spi->max_speed_hz,
    spi->bits_per_word, spi->mode);
  return 0;
}

static void spi_devices_list(void)
{
  printk(KERN_INFO ":  SPI devices registered:\n");
  bus_for_each_dev(&spi_bus_type, NULL, NULL, spi_device_found);
}

static void spi_device_delete(struct spi_master *master, unsigned cs)
{
  struct device *dev;
  char str[32];

  snprintf(str, sizeof(str), "%s.%u", dev_name(&master->dev), cs);

  dev = bus_find_device_by_name(&spi_bus_type, NULL, str);
  if (dev) {
    printk(KERN_INFO":  Deleting %s\n", str);
    device_del(dev);
  }
}

static int spi_device_register(struct spi_board_info *spi)
{
  struct spi_master *master;

  master = spi_busnum_to_master(spi->bus_num);
  if (!master) {
    printk(KERN_ERR ":  spi_busnum_to_master(%d) returned NULL\n", spi->bus_num);
    return -EINVAL;
  }

  spi_device_delete(master, spi->chip_select);
  spi_device = spi_new_device(master, spi);
  put_device(&master->dev);
  if (!spi_device) {
    printk(KERN_ERR ":    spi_new_device() returned NULL\n");
    return -EPERM;
  }
  return 0;
}

int spi_bus_init(void)
{
  int ret = 0;
  // ret = spi_device_register(lcd_config.spi);
  // if(ret) {
  //   printk(KERN_ERR ":    spi bus register failed returned: :d\n", ret);
  //   return -EPERM;
  // }
  
  //spi_devices_list();
  return ret;
}


// Write an 8 bit command to the IC driver
void spi_bus_write_cmd(unsigned char cmd)
{
  unsigned char out[3] = {SPI_TRANS_HEAD_CMD, cmd, 0};
  spi_write(spi_device, out, 3);
}

// Write an 16 bit data word to the IC driver
void spi_bus_write_data(unsigned short data)
{
  unsigned char out[3] = {SPI_TRANS_HEAD_DATA, (data >> 8), (data & 0xFF)};
  spi_write(spi_device, out, 3);
}
struct mutex		lock;
void spi_bus_write_data_array(unsigned char  *data, u8 len)
{
  unsigned char out[3] = {SPI_TRANS_HEAD_DATA_ARRAY, 0, len};
  mutex_lock(&lock);
  spi_write(spi_device, out, 3);   // first transfer size of future data
  mdelay(1200);
  spi_write(spi_device, data, len);
  mutex_unlock(&lock);
}

#endif