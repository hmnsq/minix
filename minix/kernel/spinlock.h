#که_نه_تعریف_شوی __SPINLOCK_H__
#تعریف_کړه __SPINLOCK_H__

#ضمن "kernel/kernel.h"

تعريفنوع بنية spinlock {
	atomic_t val;
} spinlock_t;

#که_نه_تعریف_شوی CONFIG_SMP

#تعریف_کړه SPINLOCK_DEFINE(name)
#تعریف_کړه شخصي_SPINLOCK_DEFINE(name)
#تعریف_کړه SPINLOCK_DECLARE(name)
#تعریف_کړه spinlock_init(sl)
#تعریف_کړه spinlock_قلف(sl)
#تعریف_کړه spinlock_unlock(sl)

#وإلا

/* SMP */
#تعریف_کړه SPINLOCK_DEFINE(name)	spinlock_t name;
#تعریف_کړه شخصي_SPINLOCK_DEFINE(name)	شخصي SPINLOCK_DEFINE(name)
#تعریف_کړه SPINLOCK_DECLARE(name)	خارجي SPINLOCK_DEFINE(name)
#تعریف_کړه spinlock_init(sl) افعل { (sl)->val = 0; } طالما (0)

#إذا CONFIG_MAX_CPUS == 1
#تعریف_کړه spinlock_قلف(sl)
#تعریف_کړه spinlock_unlock(sl)
#وإلا
فراغ arch_spinlock_قلف(atomic_t * sl);
فراغ arch_spinlock_unlock(atomic_t * sl);
#تعریف_کړه spinlock_قلف(sl)	arch_spinlock_قلف((atomic_t*) sl)
#تعریف_کړه spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#د_که_پای


#د_که_پای /* CONFIG_SMP */

#تعریف_کړه BKL_قلف()	spinlock_قلف(&big_kernel_قلف)
#تعریف_کړه BKL_UNLOCK()	spinlock_unlock(&big_kernel_قلف)

#د_که_پای /* __SPINLOCK_H__ */
