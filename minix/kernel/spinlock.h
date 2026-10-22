#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#અન્તર્ભાવ "kernel/kernel.h"

પ્રકારવ્યાખ્યા સંરચના spinlock {
	atomic_t val;
} spinlock_t;

#ifndef CONFIG_SMP

#define SPINLOCK_DEFINE(name)
#define PRIVATE_SPINLOCK_DEFINE(name)
#define SPINLOCK_DECLARE(name)
#define spinlock_init(sl)
#define spinlock_lock(sl)
#define spinlock_unlock(sl)

#નહીંતર

/* SMP */
#define SPINLOCK_DEFINE(name)	spinlock_t name;
#define PRIVATE_SPINLOCK_DEFINE(name)	PRIVATE SPINLOCK_DEFINE(name)
#define SPINLOCK_DECLARE(name)	બાહ્ય SPINLOCK_DEFINE(name)
#define spinlock_init(sl) કરો { (sl)->val = 0; } જ્યારે (0)

#જો CONFIG_MAX_CPUS == 1
#define spinlock_lock(sl)
#define spinlock_unlock(sl)
#નહીંતર
શૂન્ય arch_spinlock_lock(atomic_t * sl);
શૂન્ય arch_spinlock_unlock(atomic_t * sl);
#define spinlock_lock(sl)	arch_spinlock_lock((atomic_t*) sl)
#define spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#endif


#endif /* CONFIG_SMP */

#define BKL_LOCK()	spinlock_lock(&big_kernel_lock)
#define BKL_UNLOCK()	spinlock_unlock(&big_kernel_lock)

#endif /* __SPINLOCK_H__ */
