import spidev
from time import sleep

spi = spidev.SpiDev()
spi.open(0, 0)

spi.max_speed_hz = 1000000

# Send a null byte to check for value
print("RaspberryPi SPI Tester")

send_byte = 0x30
rcv_byte = spi.xfer2([0x30, 0x01])
#while True:
#  rcv_byte = spi.xfer2([0x30, 0x01])

  # repeat to check for a response
#  send_byte += 1
#  rcv_byte = spi.xfer2([send_byte])

 # data_recv = rcv_byte[0]
 # if (data_recv == 0x80):
  #  print("\nData received-------------------------------------------!\n")
  #else:
  #  print(send_byte)

  #sleep(2)
