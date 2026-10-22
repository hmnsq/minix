#شامل "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
ساختار minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
ساختار kinfo kinfo __section(".usermapped");		/* kernel information for services */
ساختار machine machine __section(".usermapped");	/* machine information for services */
ساختار kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
ساختار loadinfo loadinfo __section(".usermapped");	/* status of load average */
ساختار kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
ساختار arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
ساختار kclockinfo kclockinfo __section(".usermapped");	/* clock information */
