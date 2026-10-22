#অন্তর্ভাৱ "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
সংরচনা minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
সংরচনা kinfo kinfo __section(".usermapped");		/* kernel information for services */
সংরচনা machine machine __section(".usermapped");	/* machine information for services */
সংরচনা kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
সংরচনা loadinfo loadinfo __section(".usermapped");	/* status of load average */
সংরচনা kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
সংরচনা arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
সংরচনা kclockinfo kclockinfo __section(".usermapped");	/* clock information */
