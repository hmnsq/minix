#समावेश "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
काष्ठा minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
काष्ठा kinfo kinfo __section(".usermapped");		/* kernel information for services */
काष्ठा machine machine __section(".usermapped");	/* machine information for services */
काष्ठा kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
काष्ठा loadinfo loadinfo __section(".usermapped");	/* status of load average */
काष्ठा kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
काष्ठा arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
काष्ठा kclockinfo kclockinfo __section(".usermapped");	/* clock information */
