#اگر_غیر_تعریف_شدہ __SPINLOCK_H__
#تعریف_کرو __SPINLOCK_H__

#شامل "kernel/kernel.h"

قسمتعریف ساخت spinlock {
	atomic_t val;
} spinlock_t;

#اگر_غیر_تعریف_شدہ CONFIG_SMP

#تعریف_کرو SPINLOCK_DEFINE(name)
#تعریف_کرو نجی_SPINLOCK_DEFINE(name)
#تعریف_کرو SPINLOCK_DECLARE(name)
#تعریف_کرو spinlock_init(sl)
#تعریف_کرو spinlock_تالا(sl)
#تعریف_کرو spinlock_unlock(sl)

#ورنہ

/* SMP */
#تعریف_کرو SPINLOCK_DEFINE(name)	spinlock_t name;
#تعریف_کرو نجی_SPINLOCK_DEFINE(name)	نجی_2 SPINLOCK_DEFINE(name)
#تعریف_کرو SPINLOCK_DECLARE(name)	خارجی SPINLOCK_DEFINE(name)
#تعریف_کرو spinlock_init(sl) کرو { (sl)->val = 0; } جب (0)

#اگر CONFIG_MAX_CPUS == 1
#تعریف_کرو spinlock_تالا(sl)
#تعریف_کرو spinlock_unlock(sl)
#ورنہ
خالی arch_spinlock_تالا(atomic_t * sl);
خالی arch_spinlock_unlock(atomic_t * sl);
#تعریف_کرو spinlock_تالا(sl)	arch_spinlock_تالا((atomic_t*) sl)
#تعریف_کرو spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#اگر_ختم


#اگر_ختم /* CONFIG_SMP */

#تعریف_کرو BKL_تالا()	spinlock_تالا(&big_kernel_تالا)
#تعریف_کرو BKL_UNLOCK()	spinlock_unlock(&big_kernel_تالا)

#اگر_ختم /* __SPINLOCK_H__ */
