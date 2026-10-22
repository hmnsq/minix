#అన్తర్భావ "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
నిర్మాణం minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
నిర్మాణం kinfo kinfo __section(".usermapped");		/* kernel information for services */
నిర్మాణం machine machine __section(".usermapped");	/* machine information for services */
నిర్మాణం kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
నిర్మాణం loadinfo loadinfo __section(".usermapped");	/* status of load average */
నిర్మాణం kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
నిర్మాణం arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
నిర్మాణం kclockinfo kclockinfo __section(".usermapped");	/* clock information */
