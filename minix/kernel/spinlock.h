#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#അന്തര്ഭാവ "kernel/kernel.h"

തരനിർവചനം ഘടന spinlock {
	atomic_t val;
} spinlock_t;

#ifndef CONFIG_SMP

#define SPINLOCK_DEFINE(name)
#define PRIVATE_SPINLOCK_DEFINE(name)
#define SPINLOCK_DECLARE(name)
#define spinlock_init(sl)
#define spinlock_lock(sl)
#define spinlock_unlock(sl)

#അല്ലെങ്കിൽ

/* SMP */
#define SPINLOCK_DEFINE(name)	spinlock_t name;
#define PRIVATE_SPINLOCK_DEFINE(name)	PRIVATE SPINLOCK_DEFINE(name)
#define SPINLOCK_DECLARE(name)	ബാഹ്യ SPINLOCK_DEFINE(name)
#define spinlock_init(sl) ചെയ്യുക { (sl)->val = 0; } ആകുമ്പോൾ (0)

#എങ്കിൽ CONFIG_MAX_CPUS == 1
#define spinlock_lock(sl)
#define spinlock_unlock(sl)
#അല്ലെങ്കിൽ
ശൂന്യം arch_spinlock_lock(atomic_t * sl);
ശൂന്യം arch_spinlock_unlock(atomic_t * sl);
#define spinlock_lock(sl)	arch_spinlock_lock((atomic_t*) sl)
#define spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#endif


#endif /* CONFIG_SMP */

#define BKL_LOCK()	spinlock_lock(&big_kernel_lock)
#define BKL_UNLOCK()	spinlock_unlock(&big_kernel_lock)

#endif /* __SPINLOCK_H__ */
