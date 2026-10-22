#അന്തര്ഭാവ "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
ഘടന minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
ഘടന kinfo kinfo __section(".usermapped");		/* kernel information for services */
ഘടന machine machine __section(".usermapped");	/* machine information for services */
ഘടന kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
ഘടന loadinfo loadinfo __section(".usermapped");	/* status of load average */
ഘടന kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
ഘടന arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
ഘടന kclockinfo kclockinfo __section(".usermapped");	/* clock information */
