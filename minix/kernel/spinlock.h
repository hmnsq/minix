#جيڪڏهن_وصف_نه_ٿيل __SPINLOCK_H__
#وصف_ڪريو __SPINLOCK_H__

#ضمن "kernel/kernel.h"

تعريفنوع بنية spinlock {
	atomic_t val;
} spinlock_t;

#جيڪڏهن_وصف_نه_ٿيل CONFIG_SMP

#وصف_ڪريو SPINLOCK_DEFINE(name)
#وصف_ڪريو ذاتي_SPINLOCK_DEFINE(name)
#وصف_ڪريو SPINLOCK_DECLARE(name)
#وصف_ڪريو spinlock_init(sl)
#وصف_ڪريو spinlock_تالو(sl)
#وصف_ڪريو spinlock_unlock(sl)

#وإلا

/* SMP */
#وصف_ڪريو SPINLOCK_DEFINE(name)	spinlock_t name;
#وصف_ڪريو ذاتي_SPINLOCK_DEFINE(name)	ذاتي SPINLOCK_DEFINE(name)
#وصف_ڪريو SPINLOCK_DECLARE(name)	خارجي SPINLOCK_DEFINE(name)
#وصف_ڪريو spinlock_init(sl) افعل { (sl)->val = 0; } طالما (0)

#إذا CONFIG_MAX_CPUS == 1
#وصف_ڪريو spinlock_تالو(sl)
#وصف_ڪريو spinlock_unlock(sl)
#وإلا
فراغ arch_spinlock_تالو(atomic_t * sl);
فراغ arch_spinlock_unlock(atomic_t * sl);
#وصف_ڪريو spinlock_تالو(sl)	arch_spinlock_تالو((atomic_t*) sl)
#وصف_ڪريو spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#جيڪڏهن_پورو


#جيڪڏهن_پورو /* CONFIG_SMP */

#وصف_ڪريو BKL_تالو()	spinlock_تالو(&big_kernel_تالو)
#وصف_ڪريو BKL_UNLOCK()	spinlock_unlock(&big_kernel_تالو)

#جيڪڏهن_پورو /* __SPINLOCK_H__ */
