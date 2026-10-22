#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
ਬਣਤਰ minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
ਬਣਤਰ kinfo kinfo __section(".usermapped");		/* kernel information for services */
ਬਣਤਰ machine machine __section(".usermapped");	/* machine information for services */
ਬਣਤਰ kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
ਬਣਤਰ loadinfo loadinfo __section(".usermapped");	/* status of load average */
ਬਣਤਰ kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
ਬਣਤਰ arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
ਬਣਤਰ kclockinfo kclockinfo __section(".usermapped");	/* clock information */
