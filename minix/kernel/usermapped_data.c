#અન્તર્ભાવ "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
સંરચના minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
સંરચના kinfo kinfo __section(".usermapped");		/* kernel information for services */
સંરચના machine machine __section(".usermapped");	/* machine information for services */
સંરચના kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
સંરચના loadinfo loadinfo __section(".usermapped");	/* status of load average */
સંરચના kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
સંરચના arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
સંરચના kclockinfo kclockinfo __section(".usermapped");	/* clock information */
