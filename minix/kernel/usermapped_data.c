#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
ಸಂರಚನಾ minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
ಸಂರಚನಾ kinfo kinfo __section(".usermapped");		/* kernel information for services */
ಸಂರಚನಾ machine machine __section(".usermapped");	/* machine information for services */
ಸಂರಚನಾ kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
ಸಂರಚನಾ loadinfo loadinfo __section(".usermapped");	/* status of load average */
ಸಂರಚನಾ kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
ಸಂರಚನಾ arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
ಸಂರಚನಾ kclockinfo kclockinfo __section(".usermapped");	/* clock information */
