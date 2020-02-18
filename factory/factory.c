/* PMSIS includes */
#include "pmsis.h"

void factory(void)
{
    printf("Entering main controller\n");
    
    printf("Hello word from factory app!\n");
	pmsis_exit(0);
}

int main(void)
{
    printf("\n\n\t *** PMSIS Factory App ***\n\n");
//    factory();
    pmsis_kickoff((void *) factory);
}

