#define DEBUG_ON
#define SIMUL_ON

#include <sys/stat.h>  // for open
#include <fcntl.h>     // for open, O_RDWR
#include <sys/ioctl.h>      // for ioctl
#include <linux/i2c-dev.h>  // just for I2C_SLAVE
#include <linux/i2c.h>

/* useful functions -- these could be moved to a different file */
unsigned short i2c_read_word(int fd,unsigned char addr,unsigned char reg) {
  struct i2c_rdwr_ioctl_data rbd;
  struct i2c_msg messages[2];
  unsigned char buf[2];
  buf[0] = buf[1] = 0;
  rbd.nmsgs = 2;
  rbd.msgs = messages;
  messages[0].addr = addr;
  messages[0].flags = 0;
  messages[0].len = 1;
  messages[0].buf = &reg;
  messages[1].addr = addr;
  messages[1].flags = I2C_M_RD;
  messages[1].len = 2;
  messages[1].buf = buf;
  ioctl(fd,I2C_RDWR,&rbd);
  return (buf[0]<<8)|buf[1];
}
unsigned char i2c_read_byte(int fd,unsigned char addr,unsigned char reg) {
  struct i2c_rdwr_ioctl_data rbd;
  struct i2c_msg messages[2];
  unsigned char buf = '\0';
  rbd.nmsgs = 2;
  rbd.msgs = messages;
  messages[0].addr = addr;
  messages[0].flags = 0;
  messages[0].len = 1;
  messages[0].buf = &reg;
  messages[1].addr = addr;
  messages[1].flags = I2C_M_RD;
  messages[1].len = 1;
  messages[1].buf = &buf;
  ioctl(fd,I2C_RDWR,&rbd);
  return buf;
}
void i2c_write_word(int fd,unsigned char addr,unsigned char reg,
                    unsigned short val) {
  struct i2c_rdwr_ioctl_data rbd;
  struct i2c_msg messages[2];
  unsigned char buf[2];
  rbd.nmsgs = 2;
  rbd.msgs = messages;
  messages[0].addr = addr;
  messages[0].flags = 0;
  messages[0].len = 1;
  messages[0].buf = &reg;
  messages[1].addr = addr;
  messages[1].flags = 0;
  messages[1].len = 2;
  messages[1].buf = buf;
  buf[0] = (val >> 8);
  buf[1] = val&0xff;
  ioctl(fd,I2C_RDWR,&rbd);
}
void i2c_write_byte(int fd,unsigned char addr,unsigned char reg,
                    unsigned char val) {
  struct i2c_rdwr_ioctl_data rbd;
  struct i2c_msg messages[2];
  unsigned char buf;
  rbd.nmsgs = 2;
  rbd.msgs = messages;
  messages[0].addr = addr;
  messages[0].flags = 0;
  messages[0].len = 1;
  messages[0].buf = &reg;
  messages[1].addr = addr;
  messages[1].flags = I2C_M_RD;
  messages[1].len = 1;
  messages[1].buf = &buf;
  buf = val;
  ioctl(fd,I2C_RDWR,&rbd);
}
