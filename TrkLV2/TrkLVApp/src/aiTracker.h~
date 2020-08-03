#define DEBUG_ON
#define SIMUL_ON

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>  // for open
#include <sys/types.h> // for open
#include <sys/stat.h>  // for open
#include <fcntl.h>     // for open, O_RDWR
#include <sys/ioctl.h>      // for ioctl
//#include <linux/i2c-dev.h>  // just for I2C_SLAVE
//#include <linux/i2c.h>


#include "alarm.h"
#include "cvtTable.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "recSup.h"
#include "devSup.h"
#include "aiRecord.h"
#include "aoRecord.h"
#include "link.h"
#include "epicsExport.h"



/* useful functions -- these could be moved to a different file */
//unsigned short i2c_read_word(int fd,unsigned char addr,unsigned char reg) {
//  struct i2c_rdwr_ioctl_data rbd;
//  struct i2c_msg messages[2];
//  unsigned char buf[2];
//  buf[0] = buf[1] = 0;
//  rbd.nmsgs = 2;
//  rbd.msgs = messages;
//  messages[0].addr = addr;
//  messages[0].flags = 0;
//  messages[0].len = 1;
//  messages[0].buf = &reg;
//  messages[1].addr = addr;
//  messages[1].flags = I2C_M_RD;
//  messages[1].len = 2;
//  messages[1].buf = buf;
//  ioctl(fd,I2C_RDWR,&rbd);
//  return (buf[0]<<8)|buf[1];
//}
//unsigned char i2c_read_byte(int fd,unsigned char addr,unsigned char reg) {
//  struct i2c_rdwr_ioctl_data rbd;
//  struct i2c_msg messages[2];
//  unsigned char buf = '\0';
//  rbd.nmsgs = 2;
//  rbd.msgs = messages;
//  messages[0].addr = addr;
//  messages[0].flags = 0;
//  messages[0].len = 1;
//  messages[0].buf = &reg;
//  messages[1].addr = addr;
//  messages[1].flags = I2C_M_RD;
//  messages[1].len = 1;
//  messages[1].buf = &buf;
//  ioctl(fd,I2C_RDWR,&rbd);
//  return buf;
//}
//void i2c_write_word(int fd,unsigned char addr,unsigned char reg,
//                    unsigned short val) {
//  struct i2c_rdwr_ioctl_data rbd;
//  struct i2c_msg messages[2];
//  unsigned char buf[2];
//  rbd.nmsgs = 2;
//  rbd.msgs = messages;
//  messages[0].addr = addr;
//  messages[0].flags = 0;
//  messages[0].len = 1;
//  messages[0].buf = &reg;
//  messages[1].addr = addr;
//  messages[1].flags = 0;
//  messages[1].len = 2;
//  messages[1].buf = buf;
//  buf[0] = (val >> 8);
//  buf[1] = val&0xff;
//  ioctl(fd,I2C_RDWR,&rbd);
//}
//void i2c_write_byte(int fd,unsigned char addr,unsigned char reg,
//                    unsigned char val) {
//  struct i2c_rdwr_ioctl_data rbd;
//  struct i2c_msg messages[2];
//  unsigned char buf;
//  rbd.nmsgs = 2;
//  rbd.msgs = messages;
//  messages[0].addr = addr;
//  messages[0].flags = 0;
//  messages[0].len = 1;
//  messages[0].buf = &reg;
//  messages[1].addr = addr;
//  messages[1].flags = I2C_M_RD;
//  messages[1].len = 1;
//  messages[1].buf = &buf;
//  buf = val;
//  ioctl(fd,I2C_RDWR,&rbd);
//}


/* Create the dset for devaiTracker */
static long init_airecord();
static long init_ai();
static long read_ai();
struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_airecord;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devaiTracker = {
	6,
	NULL,
	init_ai,
	init_airecord,
	NULL,
	read_ai,
	NULL
};
epicsExportAddress(dset,devaiTracker);


// structure for private information for each LV monitor
static struct my_dpvt_s {
  short AddrNo;  // see documentation in .db file
  short ChannelNo;
} private_data[30];
int iADCAdr=0;

/* init_ai for debug */
static long init_ai(struct aiRecord *prec)
{
#ifdef DEBUG_ON 
  printf("init_ai (init) called, not doing anything");
#endif
  return(0);
}

/* init_record */

static long init_airecord(struct aiRecord *prec)
{
  short AddrChanNo=0;
 #ifdef DEBUG_ON
  printf("(init_airecord called.\n");
#endif
   if ((prec->inp.type) == CONSTANT) {
#ifdef DEBUG_ON
     printf(" devaiTracker calling initialize constant link \n");
#endif
      if (recGblInitConstantLink(&prec->inp,DBF_SHORT,&AddrChanNo)){
	//      if (recGblInitConstantLink(&prec->inp,DBF_SHORT,&(private_data[iADCAdr]	\
  	//					      .AddrNo))){
#ifdef DEBUG_ON
       printf("devaiTracker called initialize constand linke, AddrNo is  %X\n", AddrChanNo);
#endif
       // decode AddrChanNo
       private_data[iADCAdr].ChannelNo = AddrChanNo & 0x00FF;
       private_data[iADCAdr].AddrNo = AddrChanNo >> 8;
#ifdef DEBUG_ON
       printf("Decoded: ChannelNo %X , AddrNo %X \n", private_data[iADCAdr].ChannelNo, private_data[iADCAdr].AddrNo);
#endif
      // don't do anything here?                                                
//      prec->udf = TRUE;  // haven't read anything yet 
      iADCAdr++;
      }		       
    }
   else
     printf("input type is not a constant");
  return(0);
}

static long read_ai(struct aiRecord *prec)
{
  // this gets called for every channel in .db list
  int ierr=-1;
  unsigned int state = 0;
  short AddrNo=((struct my_dpvt_s *)(prec->dpvt))->AddrNo;
  short ChannelNo = ((struct my_dpvt_s *)(prec->dpvt))->ChannelNo;
  unsigned int value;
#ifdef DEBUG_ON
  printf ("devaiTracker AddrNo is %X ChannelNo is %X \n",AddrNo, ChannelNo);
#endif
  // might want to lock with epicsMutexMustLock / epicsMutexUnlock.
  // there is a way to do a global read, but I'm not sure how to do it.
  ierr = read_LVChannel(AddrNo, ChannelNo, &value); 
  // decode the value. 
  // here we can override the error if the data is not too old (see mu2e tachreader)
  // for now just return an error
  if (ierr <0) { // error in reading the channel
    prec->udf = TRUE;
    return -1;
  }
  else {
    prec->val = value;
    prec->udf = FALSE;
    return 2;} // do not convert

}
