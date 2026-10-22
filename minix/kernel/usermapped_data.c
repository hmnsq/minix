#include "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
sa_nracanaa minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
sa_nracanaa kinfo kinfo __section(".usermapped");		/* kernel information for services */
sa_nracanaa machine machine __section(".usermapped");	/* machine information for services */
sa_nracanaa kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
sa_nracanaa loadinfo loadinfo __section(".usermapped");	/* status of load average */
sa_nracanaa kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
sa_nracanaa arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
sa_nracanaa kclockinfo kclockinfo __section(".usermapped");	/* clock information */
