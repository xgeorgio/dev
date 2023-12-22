/*
  +----------------------------------+
  |                                  |
  |   BPM - BackProp ANN Trainer     |
  |   --------------------------     |
  |                                  |
  |    Harris Georgiou (c) 2000      |
  |                                  |
  +----------------------------------+
*/

//................................................

#include "bpm_cnst.h"
#include "bpm_imp2.h"


int __cdecl main( void )
{
    TNet	BP_net, aux_net;
    TWeight	LCOEFF, MCOEFF;
    FILE	*patfile, *valfile, *testfile;
    char    patfname[30], valfname[30], testfname[30];
    TCount	npat = 0, nval = 0, ntest = 0, maxiters;
    TValue  goodvalptg;
    Tmembuf trn_buf, val_buf, tst_buf;
    int     res;


    printf("%s\n%s\n",VERSION_TAG1,VERSION_TAG2);
    printf("--------------------------------------------------------\n\n");

    printf("Opening patterns files...\n");
    openfiles(patfname,&npat,valfname,&nval,testfname,&ntest);
    res=initfiles(patfname,&patfile,&trn_buf,npat,valfname,&valfile,&val_buf,nval,testfname,&testfile,&tst_buf,ntest);
    if (res)  fatal("Error: unable to read input files\n");
    
    printf("\nCreating network topology...\n");
    create_net(&BP_net,&aux_net,&LCOEFF,&MCOEFF,&maxiters,&goodvalptg,&trn_buf,&val_buf,&tst_buf);

    printf("Initializing file buffers...\n");
    initialize_buffers(&trn_buf,&val_buf,&tst_buf);
    printf("trn_buf: nvec=%d, ivecsz=%d, ovecsz=%d\n",trn_buf.nvec,trn_buf.ivecsz,trn_buf.ovecsz);
    printf("val_buf: nvec=%d, ivecsz=%d, ovecsz=%d\n",val_buf.nvec,val_buf.ivecsz,val_buf.ovecsz);
    printf("tst_buf: nvec=%d, ivecsz=%d, ovecsz=%d\n",tst_buf.nvec,tst_buf.ivecsz,tst_buf.ovecsz);

    printf("\nStart learning (stop at: val%=%5.1f%% or epoch>%d) .............\n\n",goodvalptg*100.0,maxiters);
    teach_net(&BP_net,&aux_net,&trn_buf,npat,&val_buf,nval,LCOEFF,MCOEFF,maxiters,goodvalptg);

    printf("\nFinished training. Start testing.............\n");
    test_net(&BP_net,&tst_buf,ntest,LCOEFF,MCOEFF);

    printf("\nSaving optimal network into file...\n");
	quicksave_net(&aux_net,DEFSAVNAME);
    
    printf("Closing patterns files...\n");
    closefiles(&patfile,&valfile,&testfile);
	//printf("De-allocating file buffers...\n");
    close_buffers(&trn_buf,&val_buf,&tst_buf);

    printf("De-allocating network structure...\n");
    done_net(&BP_net,&aux_net);

    printf("\nProcess terminated successfully. Exit to system.\n");

    return(0);
}


