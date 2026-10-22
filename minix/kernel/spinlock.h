#اگر_تعریف_نشده __SPINLOCK_H__
#تعریف_کن __SPINLOCK_H__

#ضمن "kernel/kernel.h"

تعريفنوع بنية spinlock {
	atomic_t val;
} spinlock_t;

#اگر_تعریف_نشده CONFIG_SMP

#تعریف_کن SPINLOCK_DEFINE(name)
#تعریف_کن خصوصی_SPINLOCK_DEFINE(name)
#تعریف_کن SPINLOCK_DECLARE(name)
#تعریف_کن spinlock_init(sl)
#تعریف_کن spinlock_قفل(sl)
#تعریف_کن spinlock_unlock(sl)

#وإلا

/* SMP */
#تعریف_کن SPINLOCK_DEFINE(name)	spinlock_t name;
#تعریف_کن خصوصی_SPINLOCK_DEFINE(name)	خصوصی SPINLOCK_DEFINE(name)
#تعریف_کن SPINLOCK_DECLARE(name)	خارجي SPINLOCK_DEFINE(name)
#تعریف_کن spinlock_init(sl) افعل { (sl)->val = 0; } طالما (0)

#إذا CONFIG_MAX_CPUS == 1
#تعریف_کن spinlock_قفل(sl)
#تعریف_کن spinlock_unlock(sl)
#وإلا
فراغ arch_spinlock_قفل(atomic_t * sl);
فراغ arch_spinlock_unlock(atomic_t * sl);
#تعریف_کن spinlock_قفل(sl)	arch_spinlock_قفل((atomic_t*) sl)
#تعریف_کن spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#پایان_اگر


#پایان_اگر /* CONFIG_SMP */

#تعریف_کن BKL_قفل()	spinlock_قفل(&big_kernel_قفل)
#تعریف_کن BKL_UNLOCK()	spinlock_unlock(&big_kernel_قفل)

#پایان_اگر /* __SPINLOCK_H__ */
