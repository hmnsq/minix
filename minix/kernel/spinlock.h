#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା spinlock {
	atomic_t val;
} spinlock_t;

#ifndef CONFIG_SMP

#define SPINLOCK_DEFINE(name)
#define PRIVATE_SPINLOCK_DEFINE(name)
#define SPINLOCK_DECLARE(name)
#define spinlock_init(sl)
#define spinlock_lock(sl)
#define spinlock_unlock(sl)

#ଅନ୍ୟଥା

/* SMP */
#define SPINLOCK_DEFINE(name)	spinlock_t name;
#define PRIVATE_SPINLOCK_DEFINE(name)	PRIVATE SPINLOCK_DEFINE(name)
#define SPINLOCK_DECLARE(name)	ବାହ୍ୟ SPINLOCK_DEFINE(name)
#define spinlock_init(sl) କର { (sl)->val = 0; } ଯେତେବେଳେ (0)

#ଯଦି CONFIG_MAX_CPUS == 1
#define spinlock_lock(sl)
#define spinlock_unlock(sl)
#ଅନ୍ୟଥା
ଶୂନ୍ୟ arch_spinlock_lock(atomic_t * sl);
ଶୂନ୍ୟ arch_spinlock_unlock(atomic_t * sl);
#define spinlock_lock(sl)	arch_spinlock_lock((atomic_t*) sl)
#define spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#endif


#endif /* CONFIG_SMP */

#define BKL_LOCK()	spinlock_lock(&big_kernel_lock)
#define BKL_UNLOCK()	spinlock_unlock(&big_kernel_lock)

#endif /* __SPINLOCK_H__ */
