#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
ସଂରଚନା minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
ସଂରଚନା kinfo kinfo __section(".usermapped");		/* kernel information for services */
ସଂରଚନା machine machine __section(".usermapped");	/* machine information for services */
ସଂରଚନା kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
ସଂରଚନା loadinfo loadinfo __section(".usermapped");	/* status of load average */
ସଂରଚନା kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
ସଂରଚନା arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
ସଂରଚନା kclockinfo kclockinfo __section(".usermapped");	/* clock information */
