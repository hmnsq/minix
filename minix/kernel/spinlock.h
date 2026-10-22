#جے_بنا_تعریف __SPINLOCK_H__
#تعریف_کرو __SPINLOCK_H__

#ضمن "kernel/kernel.h"

تعريفنوع بنية spinlock {
	atomic_t val;
} spinlock_t;

#جے_بنا_تعریف CONFIG_SMP

#تعریف_کرو SPINLOCK_DEFINE(name)
#تعریف_کرو نجی_SPINLOCK_DEFINE(name)
#تعریف_کرو SPINLOCK_DECLARE(name)
#تعریف_کرو spinlock_init(sl)
#تعریف_کرو spinlock_جندرا(sl)
#تعریف_کرو spinlock_unlock(sl)

#وإلا

/* SMP */
#تعریف_کرو SPINLOCK_DEFINE(name)	spinlock_t name;
#تعریف_کرو نجی_SPINLOCK_DEFINE(name)	نجی SPINLOCK_DEFINE(name)
#تعریف_کرو SPINLOCK_DECLARE(name)	خارجي SPINLOCK_DEFINE(name)
#تعریف_کرو spinlock_init(sl) افعل { (sl)->val = 0; } طالما (0)

#إذا CONFIG_MAX_CPUS == 1
#تعریف_کرو spinlock_جندرا(sl)
#تعریف_کرو spinlock_unlock(sl)
#وإلا
فراغ arch_spinlock_جندرا(atomic_t * sl);
فراغ arch_spinlock_unlock(atomic_t * sl);
#تعریف_کرو spinlock_جندرا(sl)	arch_spinlock_جندرا((atomic_t*) sl)
#تعریف_کرو spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#جے_انت


#جے_انت /* CONFIG_SMP */

#تعریف_کرو BKL_جندرا()	spinlock_جندرا(&big_kernel_جندرا)
#تعریف_کرو BKL_UNLOCK()	spinlock_unlock(&big_kernel_جندرا)

#جے_انت /* __SPINLOCK_H__ */
