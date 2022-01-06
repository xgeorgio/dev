//----------------------------------------------------
//  
//   File: Multi-Layer Perceptron ANN executor, v1.7
//
//----------------------------------------------------
//                Harris Georgiou, 2000.
//----------------------------------------------------


#include "mlp_imp2.h"

int __cdecl main( void )
{
    TNet	BP_net;
    FILE	*inpfile=NULL, *netcfg=NULL;
    TCount	npat = 0;


    printf("Multi-Layer Perceptron ANN executor, v1.7 - Universal version\n");
	printf("Build: R000711-Win32-PPro , Harris Georgiou (c) 2000.\n");
    printf("-------------------------------------------------------------\n\n");

    openfiles(&netcfg,&inpfile);
    printf("files opened!\n");

	quickload_net(netcfg,&BP_net);

    printf("Start execution.............\n\n");
    check_testset(&BP_net, inpfile, ivecs);
	printf("\n");

    printf("Closing all files...");
    closefiles(netcfg,inpfile);
    printf(" done!\n");

    printf("De-allocating memory...");
    done_net(&BP_net);
    printf(" done!\n\n");

    return(0);
}


