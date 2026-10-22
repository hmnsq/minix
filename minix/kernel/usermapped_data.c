#அந்தர்பாவ "kernel/kernel.h"

/* This is the user-visible struct that has pointers to other bits of data. */
கட்டமைப்பு minix_kerninfo minix_kerninfo __section(".usermapped");

/* Kernel information structures. */
கட்டமைப்பு kinfo kinfo __section(".usermapped");		/* kernel information for services */
கட்டமைப்பு machine machine __section(".usermapped");	/* machine information for services */
கட்டமைப்பு kmessages kmessages __section(".usermapped");	/* diagnostic messages in kernel */
கட்டமைப்பு loadinfo loadinfo __section(".usermapped");	/* status of load average */
கட்டமைப்பு kuserinfo kuserinfo __section(".usermapped");
	/* kernel information for users */
கட்டமைப்பு arm_frclock arm_frclock __section(".usermapped");
	/* ARM free running timer information */
கட்டமைப்பு kclockinfo kclockinfo __section(".usermapped");	/* clock information */
