#ضمن "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
بنية minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
بنية kinfo kinfo __section(".usermapped");		/* kernel information for services */
بنية machine machine __section(".usermapped");	/* machine information for services */
بنية kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
بنية loadinfo loadinfo __section(".usermapped");	/* status of load average */
بنية kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
بنية arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
بنية kclockinfo kclockinfo __section(".usermapped");	/* clock information */
