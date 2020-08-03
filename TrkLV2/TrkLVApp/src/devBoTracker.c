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
#include "boRecord.h"
#include "link.h"
#include "epicsExport.h"

/* Create the dset for devBiTracker */
static long init_borecord();
static long init_bo();
static long write_bo();
struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_borecord;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	write_bo;
	DEVSUPFUN	special_linconv;
} devBoTracker = {
	6,
	NULL,
	init_bo,
	init_borecord,
	NULL,
	write_bo,
	NULL
};
epicsExportAddress(dset,devBoTracker);

// Bo Record has 1 output, gpio number

// structure for private information for each LV monitor
static struct my_dpvt_s {
  short gpioNo;  // see documentation in .db file
  char gpioFN[40];  
  unsigned int istate;
} private_data[30];
int iStatusChan=0;

/* init_bo for debug */
static long init_bo(struct boRecord *prec)
{
#ifdef DEBUG_ON 
  //  printf("init_bo (init) called, prec->rval is %d\n", prec->rval);
#endif
  return(0);
}

/* init_record */

static long init_borecord(struct boRecord *prec)
{
  int istat=-1;

#ifdef DEBUG_ON
  printf("devBoTracker (init_record) called.\n");
#endif
    if ((prec->out.type) == CONSTANT) {
#ifdef DEBUG_ON
    printf(" devBoTracker calling initialize constant link \n");
#endif
    if (recGblInitConstantLink(&prec->out,DBF_SHORT,&(private_data[iStatusChan].gpioNo))){
#ifdef DEBUG_ON
      printf("devBoTracker called initialize constand linke, gpioNo is  %d\n", private_data[iStatusChan].gpioNo);
#endif
      // check the initial value 
#ifdef SIMUL_ON
    sprintf(private_data[iStatusChan].gpioFN,"/Users/odell/BBB/gpio%d/value",private_data[iStatusChan].gpioNo);
#else
    sprintf(private_data[iStatusChan].gpioFN,"/sys/class/gpio/gpio%d/value",private_data[iStatusChan].gpioNo);
#endif
      FILE *fr = fopen(private_data[iStatusChan].gpioFN,"r");
      if (fr) {
        istat = fscanf(fr,"%d",&(private_data[iStatusChan].istate));
#ifdef DEBUG_ON
        printf(" devBoTracker read value % d istat is %d\n ", private_data[iStatusChan].istate, istat);
#endif
        fclose(fr); }
      prec->val = private_data[iStatusChan].istate;
      prec->rval = private_data[iStatusChan].istate;
#ifdef DEBUG_ON
      printf(" devBoTracker prec->val % d prec->rval %d\n ", prec->val, prec->rval);
#endif

      //      private_data[iStatusChan].istate = state;
      prec->udf = FALSE;
      prec->dpvt=&(private_data[iStatusChan]);

      iStatusChan++;
    }
        
   }
  return(0);
}

static long write_bo(struct boRecord *prec)
{
  // this gets called for every channel in .db list ?

  int istat=-1;
  unsigned int state = 0;
  short gpioNo=((struct my_dpvt_s *)(prec->dpvt))->gpioNo;
  char* gpioFN=((struct my_dpvt_s *)(prec->dpvt))->gpioFN;
#ifdef DEBUG_ON
   printf ("devBoTracker gpio number is %d \n",gpioNo);
#endif
   // could move the following preprossessor commands to the .h file
    // check the current value 
    FILE *fr = fopen(gpioFN,"r");
    if (fr) {
        istat = fscanf(fr,"%d",&state);
#ifdef DEBUG_ON
	printf(" devBoTracker read value % d\n ", state);
#endif
        fclose(fr); }
    else
      printf ("Could not open file %s for reading \n", gpioFN);
    // do we need to write a new value?
#ifdef DEBUG_ON
      printf(" devBoTracker about to set newstate prec->val % d prec->rval %d\n ", prec->val, prec->rval);
#endif
    unsigned int newstate = prec->val;
    if (newstate != state) {
#ifdef DEBUG_ON
      printf ("From write_bo, current state is %d, newstate is %d \n",state,newstate);
#endif
	// State has changed so open the file for writing
      FILE *fw = fopen(gpioFN,"w+");
      if (fw) {
	istat = fprintf(fw,"%d",newstate);
	istat = fprintf(fw,"\n");
        if (istat)
	  printf (" devBoTracker, successfully wrote %d to file %s \n", newstate, gpioFN);
	else{
	  printf(" error writing newstate to file \n");
	  fclose(fw); /* need to close file regardless */
	  prec->udf=TRUE;
	  return -1; /* error */
	    }
        fclose(fw);}
      else {
	printf ("Could not open file %s for writing \n", gpioFN);
   	// indicate undefined value
	prec->udf = TRUE;
        return -1; /* error */
      }
    }

    else {
#ifdef DEBUG_ON
      printf(" newstate and state are the same \n");
#endif
    }
   
    // everything good.
      prec->udf = FALSE;
      return(2);				/* do not convert */
}
