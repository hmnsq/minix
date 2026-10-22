#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
ರಚನೆ minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
ರಚನೆ kinfo kinfo __section(".usermapped");		/* kernel information for services */
ರಚನೆ machine machine __section(".usermapped");	/* machine information for services */
ರಚನೆ kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
ರಚನೆ loadinfo loadinfo __section(".usermapped");	/* status of load average */
ರಚನೆ kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
ರಚನೆ arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
ರಚನೆ kclockinfo kclockinfo __section(".usermapped");	/* clock information */
