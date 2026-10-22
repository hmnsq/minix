#অন্তর্ভাব "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
কাঠামো minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
কাঠামো kinfo kinfo __section(".usermapped");		/* kernel information for services */
কাঠামো machine machine __section(".usermapped");	/* machine information for services */
কাঠামো kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
কাঠামো loadinfo loadinfo __section(".usermapped");	/* status of load average */
কাঠামো kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
কাঠামো arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
কাঠামো kclockinfo kclockinfo __section(".usermapped");	/* clock information */
