//#define DEBUG_ON
#define SIMUL_ON

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "alarm.h"
#include "cvtTable.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "recSup.h"
#include "devSup.h"
#include "aiRecord.h"
#include "biRecord.h"
#include "link.h"
#include "epicsExport.h"

/* Create the dset for devBiTracker */
static long init_record();
static long init_bi();
static long read_bi();
struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read_bi;
	DEVSUPFUN	special_linconv;
} devBiTracker = {
	6,
	NULL,
	init_bi,
	init_record,
	NULL,
	read_bi,
	NULL
};
epicsExportAddress(dset,devBiTracker);

// Bi Record has 1 input, gpio number

// structure for private information for each LV monitor
static struct my_dpvt_s {
  short gpioNo;  // see documentation in .db file
} private_data[30];
int iMonChan=0;

/* init_bi for debug */
static long init_bi(int after)
{
#ifdef DEBUG_ON 
  printf("init_bi (init) called, pass=%d\n", after);
#endif
  return(0);
}

/* init_record */

static long init_record(struct biRecord *prec)
{
#ifdef DEBUG_ON
  printf("devBiTracker (init_record) called.\n");
#endif
    if ((prec->inp.type) == CONSTANT) {
#ifdef DEBUG_ON
    printf(" calling initialize constant link \n");
#endif
    if (recGblInitConstantLink(&prec->inp,DBF_SHORT,&(private_data[iMonChan].gpioNo))){
#ifdef DEBUG_ON
      printf("called initialize constand linke, gpioNo is  %d\n", private_data[iMonChan].gpioNo);
#endif
      prec->udf = FALSE;
      prec->dpvt=&(private_data[iMonChan]);
      iMonChan++;
    }
        
   }
  return(0);
}

static long read_bi(struct biRecord *prec)
{
  // this gets called for every channel in .db list ?
  char gpioFN[40];
  int istat=-1;
  unsigned int state = 0;
  short gpioNo=((struct my_dpvt_s *)(prec->dpvt))->gpioNo;  
#ifdef DEBUG_ON
   printf ("gpio number is %d \n",gpioNo);
#endif
   // could move the following preprossessor commands to the .h file
#ifdef SIMUL_ON
    sprintf(gpioFN,"/Users/odell/BBB/gpio%d/value",gpioNo);
#else
    sprintf(gpioFN,"/sys/class/gpio/gpio%d/value",gpioNo);
#endif   
    FILE *f = fopen(gpioFN,"r");
      if (f) {
        istat = fscanf(f,"%d",&state);
        fclose(f);
      }
      else
	printf ("Could not open file\n");
#ifdef DEBUG_ON
      printf(" From read_bi, filename is: '%s', istat = %d, state = %d \n", gpioFN,istat, state);
#endif
      if (istat == 1) {
 // set the value of the variable
      if (state == 0 && prec->val != 0)
        printf("LV monitor bit is zero for %s\n", prec->name);
        prec->val = state;
        prec->udf = FALSE;
        return 2;  /* 2: succesful read, do not convert */    
       }
       else {
   	// indicate undefined value
        prec->udf = TRUE;
        return -1; /* error */
     }
  return(2);				/* do not convert */
}
