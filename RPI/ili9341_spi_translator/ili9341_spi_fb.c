#include "ili9341_spi_fb.h"
#include "ili9341_spi_bus.h"
#include "ili9341_registers.h"
#include "ili9341_colors.h"

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/module.h>
#include <linux/moduleparam.h>


#define ORIENTATION 1 //0=LANDSCAPE 1=PORTRAIT

#define DISPLAY_WIDTH  ILI9341_PIXEL_WIDTH
#define DISPLAY_HEIGHT ILI9341_PIXEL_HEIGHT

#define DISPLAY_BPP 16

static lcdPropertiesTypeDef lcdProperties = {ILI9341_PIXEL_WIDTH, ILI9341_PIXEL_HEIGHT, LCD_ORIENTATION_PORTRAIT, 1, 1};


void lcdSetCursorPosition(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1)
{
  spi_bus_write_cmd(ILI9341_COLADDRSET);
  spi_bus_write_data((x0 >> 8) & 0xFF);
  spi_bus_write_data(x0 & 0xFF);
  spi_bus_write_data((x1 >> 8) & 0xFF);
  spi_bus_write_data(x1 & 0xFF);
  spi_bus_write_cmd(ILI9341_PAGEADDRSET);
  spi_bus_write_data((y0 >> 8) & 0xFF);
  spi_bus_write_data(y0 & 0xFF);
  spi_bus_write_data((y1 >> 8) & 0xFF);
  spi_bus_write_data(y1 & 0xFF);
  spi_bus_write_cmd(ILI9341_MEMORYWRITE);
}

static void ili9341_update_display(const struct fb_info *info)
{
  printk(KERN_INFO "fb%d: ili9341_update_display\n", info->node);
  int x, y;
  lcdSetCursorPosition(0, 0, lcdProperties.width - 1, lcdProperties.height - 1);
  
  if (ORIENTATION == 0)
  {
    for (y = 0; y < DISPLAY_WIDTH; y++)
    {
      for (x = 0; x < DISPLAY_HEIGHT; x++)
      {
        spi_bus_write_data(info->screen_base[(x * (2 * DISPLAY_WIDTH)) + (y * 2) + 1]);
        //spi_bus_write_data(info->screen_base[(x * (2 * DISPLAY_WIDTH)) + (y * 2) + 2]);
      }
    }
  }
  else
  {
    for (y = (DISPLAY_HEIGHT); y >= 0; y--)
    {
      // for (x = 0; x < DISPLAY_WIDTH; x++)
      // {
      //   unsigned short line =  ((info->screen_base[(y * (2 * DISPLAY_WIDTH)) + (x*2) + 1]) << 8) | 
      //                           (info->screen_base[(y * (2 * DISPLAY_WIDTH)) + (x*2) + 2]);

      //   //spi_bus_write_data(line);
      // }
      u8 buff[DISPLAY_WIDTH] = {0};
      for (x = 0; x < DISPLAY_WIDTH; x++)
      {
        buff[x] =  (info->screen_base[(y * (2 * DISPLAY_WIDTH)) + (x*2) + 1]);

        //spi_bus_write_data(line);
      }
      spi_bus_write_data_array(buff, DISPLAY_WIDTH);
    }
  }
}

static void ili9341_deferred_io(struct fb_info *info, struct list_head *pagelist)
{
  printk(KERN_INFO "fb%d: ili9341_deferred_io\n", info->node);
  ili9341_update_display(info);
}


static void ili9341_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
  printk(KERN_INFO "fb%d: ili9341_fillrect\n", info->node);
  ili9341_update_display(info);
}

static void ili9341_copyarea(struct fb_info *info, const struct fb_copyarea *area)
{
  printk(KERN_INFO "fb%d: ili9341_copyarea\n", info->node);
  ili9341_update_display(info);
}

static void ili9341_imageblit(struct fb_info *info, const struct fb_image *image)
{
  printk(KERN_INFO "fb%d: ili9341_imageblit\n", info->node);
  ili9341_update_display(info);
}


static int ili9341_blank(int blank_mode, struct fb_info *info)
{
  printk(KERN_INFO "fb%d: ili9341_blank\n", info->node);
  return 0;
}

static ssize_t ili9341_write(struct fb_info *info, const char __user *buf, size_t count, loff_t *ppos)
{
  printk(KERN_INFO "fb%d: ili9341_write, count: %d\n", info->node, count);
  unsigned long p = *ppos;
  void *dst;
  int err = 0;
  unsigned long total_size;

  if (info->state != FBINFO_STATE_RUNNING)
    return -EPERM;

  total_size = info->screen_size;

  if (total_size == 0)
    total_size = info->fix.smem_len;

  if (p > total_size)
    return -EFBIG;

  if (count > total_size)
  {
    err = -EFBIG;
    count = total_size;
  }

  if (count + p > total_size)
  {
    if (!err)
      err = -ENOSPC;

    count = total_size - p;
  }

  dst = (void __force *)(info->screen_base + p);

  if (info->fbops->fb_sync)
    info->fbops->fb_sync(info);

  if (copy_from_user(dst, buf, count))
    err = -EFAULT;

  if (!err)
    *ppos += count;

  ili9341_update_display(info);

  return (err) ? err : count;
}


static struct fb_fix_screeninfo ili9341_fix = {
    .id = "ili9341",
    .type = FB_TYPE_PACKED_PIXELS,
    .visual = FB_VISUAL_TRUECOLOR,
    .accel = FB_ACCEL_NONE,
    // .line_length = 320 * 2,
    .line_length = 480,
};

static struct fb_var_screeninfo ili9341_var = {
    .width = DISPLAY_WIDTH,
    .height = DISPLAY_HEIGHT,
    .bits_per_pixel = DISPLAY_BPP,
    .xres = DISPLAY_WIDTH,
    .yres = DISPLAY_HEIGHT,
    .xres_virtual = DISPLAY_WIDTH,
    .yres_virtual = DISPLAY_HEIGHT,
    .activate = FB_ACTIVATE_NOW,
    .vmode = FB_VMODE_NONINTERLACED,

    .red = {11, 5, 0},
    .green = {5, 6, 0},
    .blue = {0, 5, 0},
};

static struct fb_ops ili9341_ops = {
    .owner = THIS_MODULE,
    //.fb_read = fb_sys_read,
    .fb_write = ili9341_write,
    .fb_fillrect = ili9341_fillrect,
    .fb_copyarea = ili9341_copyarea,
    .fb_imageblit = ili9341_imageblit,
    //.fb_setcolreg = ili9341_setcolreg,
    .fb_blank = ili9341_blank,
};


static struct fb_deferred_io ili9341_defio = {
    .delay = HZ / 40,
    .deferred_io = ili9341_deferred_io,
};


enum
{
  MemoryAccessControlNormalOrder,
  MemoryAccessControlReverseOrder
} MemoryAccessControlRefreshOrder;

enum
{
  MemoryAccessControlColorOrderRGB,
  MemoryAccessControlColorOrderBGR
} MemoryAccessControlColorOrder;


static unsigned char lcdPortraitConfig = 0;
static unsigned char lcdLandscapeConfig = 0;
static unsigned char lcdPortraitMirrorConfig = 0;
static unsigned char lcdLandscapeMirrorConfig = 0;

// *****************************************************************************
// Functions prototypes
// *****************************************************************************

static unsigned char lcdBuildMemoryAccessControlConfig(
    bool rowAddressOrder,
    bool columnAddressOrder,
    bool rowColumnExchange,
    bool verticalRefreshOrder,
    bool colorOrder,
    bool horizontalRefreshOrder)
{
  unsigned char value = 0;
  if (horizontalRefreshOrder)
    value |= ILI9341_MADCTL_MH;
  if (colorOrder)
    value |= ILI9341_MADCTL_BGR;
  if (verticalRefreshOrder)
    value |= ILI9341_MADCTL_ML;
  if (rowColumnExchange)
    value |= ILI9341_MADCTL_MV;
  if (columnAddressOrder)
    value |= ILI9341_MADCTL_MX;
  if (rowAddressOrder)
    value |= ILI9341_MADCTL_MY;
  return value;
}

void ili9341_Init(void)
{
  // lcdPortraitConfig = lcdBuildMemoryAccessControlConfig(
  //     MemoryAccessControlNormalOrder,   // rowAddressOrder
  //     MemoryAccessControlReverseOrder,  // columnAddressOrder
  //     MemoryAccessControlNormalOrder,   // rowColumnExchange
  //     MemoryAccessControlNormalOrder,   // verticalRefreshOrder
  //     MemoryAccessControlColorOrderBGR, // colorOrder
  //     MemoryAccessControlNormalOrder);  // horizontalRefreshOrder



    lcdPortraitConfig = lcdBuildMemoryAccessControlConfig(
      MemoryAccessControlReverseOrder,   // rowAddressOrder
      MemoryAccessControlReverseOrder,  // columnAddressOrder
      MemoryAccessControlNormalOrder,   // rowColumnExchange
      MemoryAccessControlNormalOrder,   // verticalRefreshOrder
      MemoryAccessControlColorOrderBGR, // colorOrder
      MemoryAccessControlNormalOrder);  // horizontalRefreshOrder    

  lcdLandscapeConfig = lcdBuildMemoryAccessControlConfig(
      MemoryAccessControlNormalOrder,   // rowAddressOrder
      MemoryAccessControlNormalOrder,   // columnAddressOrder
      MemoryAccessControlReverseOrder,  // rowColumnExchange
      MemoryAccessControlNormalOrder,   // verticalRefreshOrder
      MemoryAccessControlColorOrderBGR, // colorOrder
      MemoryAccessControlNormalOrder);  // horizontalRefreshOrder

  lcdPortraitMirrorConfig = lcdBuildMemoryAccessControlConfig(
      MemoryAccessControlReverseOrder,  // rowAddressOrder
      MemoryAccessControlNormalOrder,   // columnAddressOrder
      MemoryAccessControlNormalOrder,   // rowColumnExchange
      MemoryAccessControlNormalOrder,   // verticalRefreshOrder
      MemoryAccessControlColorOrderBGR, // colorOrder
      MemoryAccessControlNormalOrder);  // horizontalRefreshOrder

  lcdLandscapeMirrorConfig = lcdBuildMemoryAccessControlConfig(
      MemoryAccessControlReverseOrder,  // rowAddressOrder
      MemoryAccessControlReverseOrder,  // columnAddressOrder
      MemoryAccessControlReverseOrder,  // rowColumnExchange
      MemoryAccessControlNormalOrder,   // verticalRefreshOrder
      MemoryAccessControlColorOrderBGR, // colorOrder
      MemoryAccessControlNormalOrder);  // horizontalRefreshOrder

  spi_bus_write_cmd(ILI9341_SOFTRESET);
  mdelay(120);

  spi_bus_write_cmd(ILI9341_DISPLAYOFF);

  spi_bus_write_cmd(0xCF);
  spi_bus_write_data(0x00);
  spi_bus_write_data(0x83);
  spi_bus_write_data(0x30);

  spi_bus_write_cmd(0xED);
  spi_bus_write_data(0x64);
  spi_bus_write_data(0x03);
  spi_bus_write_data(0x12);
  spi_bus_write_data(0x81);

  spi_bus_write_cmd(0xE8);
  spi_bus_write_data(0x85);
  spi_bus_write_data(0x01);
  spi_bus_write_data(0x79);

  spi_bus_write_cmd(0xCB);
  spi_bus_write_data(0x39);
  spi_bus_write_data(0x2C);
  spi_bus_write_data(0x00);
  spi_bus_write_data(0x34);
  spi_bus_write_data(0x02);

  spi_bus_write_cmd(0xF7);
  spi_bus_write_data(0x20);

  spi_bus_write_cmd(0xEA);
  spi_bus_write_data(0x00);
  spi_bus_write_data(0x00);

  spi_bus_write_cmd(ILI9341_POWERCONTROL1);
  spi_bus_write_data(0x26);

  spi_bus_write_cmd(ILI9341_POWERCONTROL2);
  spi_bus_write_data(0x11);

  spi_bus_write_cmd(ILI9341_VCOMCONTROL1);
  spi_bus_write_data(0x35);
  spi_bus_write_data(0x3E);

  spi_bus_write_cmd(ILI9341_VCOMCONTROL2);
  spi_bus_write_data(0xBE);

  spi_bus_write_cmd(ILI9341_MEMCONTROL);
  spi_bus_write_data(lcdPortraitConfig);
  //spi_bus_write_data(lcdPortraitMirrorConfig);

  spi_bus_write_cmd(ILI9341_PIXELFORMAT);
  spi_bus_write_data(0x55);

  spi_bus_write_cmd(ILI9341_FRAMECONTROLNORMAL);
  spi_bus_write_data(0x00);
  spi_bus_write_data(0x1B);

  spi_bus_write_cmd(0xF2);
  spi_bus_write_data(0x08);

  spi_bus_write_cmd(ILI9341_GAMMASET);
  spi_bus_write_data(0x01);

  spi_bus_write_cmd(ILI9341_POSITIVEGAMMCORR);
  spi_bus_write_data(0x1F);
  spi_bus_write_data(0x1A);
  spi_bus_write_data(0x18);
  spi_bus_write_data(0x0A);
  spi_bus_write_data(0x0F);
  spi_bus_write_data(0x06);
  spi_bus_write_data(0x45);
  spi_bus_write_data(0x87);
  spi_bus_write_data(0x32);
  spi_bus_write_data(0x0A);
  spi_bus_write_data(0x07);
  spi_bus_write_data(0x02);
  spi_bus_write_data(0x07);
  spi_bus_write_data(0x05);
  spi_bus_write_data(0x00);

  spi_bus_write_cmd(ILI9341_NEGATIVEGAMMCORR);
  spi_bus_write_data(0x00);
  spi_bus_write_data(0x25);
  spi_bus_write_data(0x27);
  spi_bus_write_data(0x05);
  spi_bus_write_data(0x10);
  spi_bus_write_data(0x09);
  spi_bus_write_data(0x3A);
  spi_bus_write_data(0x78);
  spi_bus_write_data(0x4D);
  spi_bus_write_data(0x05);
  spi_bus_write_data(0x18);
  spi_bus_write_data(0x0D);
  spi_bus_write_data(0x38);
  spi_bus_write_data(0x3A);
  spi_bus_write_data(0x1F);

  spi_bus_write_cmd(ILI9341_COLADDRSET);
  spi_bus_write_data(0x00);
  spi_bus_write_data(0x00);
  spi_bus_write_data(0x00);
  spi_bus_write_data(0xEF);

  spi_bus_write_cmd(ILI9341_PAGEADDRSET);
  spi_bus_write_data(0x00);
  spi_bus_write_data(0x00);
  spi_bus_write_data(0x01);
  spi_bus_write_data(0x3F);

  spi_bus_write_cmd(ILI9341_ENTRYMODE);
  spi_bus_write_data(0x07);

  spi_bus_write_cmd(ILI9341_DISPLAYFUNC);
  spi_bus_write_data(0x0A);
  spi_bus_write_data(0x82);
  spi_bus_write_data(0x27);
  spi_bus_write_data(0x00);

  spi_bus_write_cmd(ILI9341_SLEEPOUT);
  mdelay(120);
  spi_bus_write_cmd(ILI9341_DISPLAYON);
  mdelay(120);
  spi_bus_write_cmd(ILI9341_MEMORYWRITE);
}


void ili9341_Test(void)
{
  lcdSetCursorPosition(0, 0, lcdProperties.width - 1, lcdProperties.height - 1);

  char stripSize = lcdProperties.height / 8;
  int y = 0;
  for (; y < lcdProperties.height; y++)
  {
    int x = 0;
    for (; x < lcdProperties.width; x++)
    {
      if (y > lcdProperties.height - 1 - (stripSize * 1))
        spi_bus_write_data(COLOR_WHITE);
      else if (y > lcdProperties.height - 1 - (stripSize * 2))
        spi_bus_write_data(COLOR_BLUE);
      else if (y > lcdProperties.height - 1 - (stripSize * 3))
        spi_bus_write_data(COLOR_GREEN);
      else if (y > lcdProperties.height - 1 - (stripSize * 4))
        spi_bus_write_data(COLOR_CYAN);
      else if (y > lcdProperties.height - 1 - (stripSize * 5))
        spi_bus_write_data(COLOR_RED);
      else if (y > lcdProperties.height - 1 - (stripSize * 6))
        spi_bus_write_data(COLOR_MAGENTA);
      else if (y > lcdProperties.height - 1 - (stripSize * 7))
        spi_bus_write_data(COLOR_YELLOW);
      else
        spi_bus_write_data(COLOR_BLACK);
    }
  }
}

static int ili9341_probe(struct platform_device *pdev)
{
  struct fb_info *info;
  int retval = -ENOMEM;
  int vmem_size;
  unsigned char *vmem;

  vmem_size = ili9341_var.width * ili9341_var.height * ili9341_var.bits_per_pixel / 8;
  vmem = vzalloc(vmem_size);
  if (!vmem)
  {
    return -ENOMEM;
  }
  memset(vmem, 0, vmem_size);

  info = framebuffer_alloc(0, &pdev->dev);
  if (!info)
  {
    vfree(vmem);
    return -ENOMEM;
  }

  info->screen_base = (char __force __iomem *)vmem;
  info->fbops = &ili9341_ops;
  info->fix = ili9341_fix;
  info->fix.smem_start = (unsigned long)vmem;
  info->fix.smem_len = vmem_size;
  info->var = ili9341_var;
  info->flags = FBINFO_DEFAULT | FBINFO_VIRTFB;

  info->fbdefio = &ili9341_defio;
  // if (0 < fps)
  // {
  //   info->fbdefio->delay = HZ / fps;
  // }

  fb_deferred_io_init(info);

  retval = register_framebuffer(info);
  if (retval < 0)
  {
    framebuffer_release(info);
    vfree(vmem);
    return retval;
  }

  spi_bus_init();

  struct device *spidevice = bus_find_device_by_name(&spi_bus_type, NULL, "spi0.0");

  if (!spidevice)
  {
    dev_err(&pdev->dev, "%s: Couldn't find SPI device\n", __func__);
    //return -ENODEV;
  }
  spi_device = to_spi_device(spidevice);
  spi_setup(spi_device);

  platform_set_drvdata(pdev, info);

  ili9341_Init();
  //ili9341_Test();

  printk(KERN_INFO "fb%d: ili9341 LCD framebuffer device\n", info->node);
  return 0;
}

static int ili9341_remove(struct platform_device *dev)
{
  struct fb_info *info = platform_get_drvdata(dev);

  if (info)
  {
    unregister_framebuffer(info);
    fb_deferred_io_cleanup(info);
    vfree((void __force *)info->screen_base);

    framebuffer_release(info);
  }
  printk(KERN_INFO "ili9341 LCD driver removed\n");
  return 0;
}

static struct platform_driver ili9341_driver = {
    .probe = ili9341_probe,
    .remove = ili9341_remove,
    .driver = {
        .name = "ili9341",
    },
};

static struct platform_device *ili9341_device;

static int __init ili9341_init(void)
{
  int ret = platform_driver_register(&ili9341_driver);
  if (0 == ret)
  {
    ili9341_device = platform_device_alloc("ili9341", 0);
    if (ili9341_device)
    {
      ret = platform_device_add(ili9341_device);
    }
    else
    {
      ret = -ENOMEM;
    }
    if (0 != ret)
    {
      platform_device_put(ili9341_device);
      platform_driver_unregister(&ili9341_driver);
    }
  }
  return ret;
}

static void __exit ili9341_exit(void)
{
  platform_device_unregister(ili9341_device);
  platform_driver_unregister(&ili9341_driver);
}


module_init(ili9341_init);
module_exit(ili9341_exit);

MODULE_DESCRIPTION("ili9341 LCD framebuffer driver over SPI translator");
MODULE_AUTHOR("taraskornuta@gmail.com");
MODULE_LICENSE("GPL");