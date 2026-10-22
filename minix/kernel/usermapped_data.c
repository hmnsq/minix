#अन्तर्भाव "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
संरचना minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
संरचना kinfo kinfo __section(".usermapped");		/* kernel information for services */
संरचना machine machine __section(".usermapped");	/* machine information for services */
संरचना kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
संरचना loadinfo loadinfo __section(".usermapped");	/* status of load average */
संरचना kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
संरचना arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
संरचना kclockinfo kclockinfo __section(".usermapped");	/* clock information */
