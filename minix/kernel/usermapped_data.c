#شامل "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
ساخت minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
ساخت kinfo kinfo __section(".usermapped");		/* kernel information for services */
ساخت machine machine __section(".usermapped");	/* machine information for services */
ساخت kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
ساخت loadinfo loadinfo __section(".usermapped");	/* status of load average */
ساخت kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
ساخت arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
ساخت kclockinfo kclockinfo __section(".usermapped");	/* clock information */
