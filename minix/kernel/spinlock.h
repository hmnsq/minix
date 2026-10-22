#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __SPINLOCK_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __SPINLOCK_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ spinlock {
	atomic_t val;
} spinlock_t;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPINLOCK_DEFINE(name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿੱਜੀ_SPINLOCK_DEFINE(name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPINLOCK_DECLARE(name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ spinlock_init(sl)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ spinlock_ਤਾਲਾ(sl)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ spinlock_unlock(sl)

#ਨਹੀਂਤਾਂ

/* SMP */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPINLOCK_DEFINE(name)	spinlock_t name;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿੱਜੀ_SPINLOCK_DEFINE(name)	ਨਿੱਜੀ SPINLOCK_DEFINE(name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPINLOCK_DECLARE(name)	ਬਾਹਰੀ SPINLOCK_DEFINE(name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ spinlock_init(sl) ਕਰੋ { (sl)->val = 0; } ਜਦੋਂ (0)

#ਜੇ CONFIG_MAX_CPUS == 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ spinlock_ਤਾਲਾ(sl)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ spinlock_unlock(sl)
#ਨਹੀਂਤਾਂ
ਖਾਲੀ arch_spinlock_ਤਾਲਾ(atomic_t * sl);
ਖਾਲੀ arch_spinlock_unlock(atomic_t * sl);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ spinlock_ਤਾਲਾ(sl)	arch_spinlock_ਤਾਲਾ((atomic_t*) sl)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#ਜੇ_ਅੰਤ


#ਜੇ_ਅੰਤ /* CONFIG_SMP */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BKL_ਤਾਲਾ()	spinlock_ਤਾਲਾ(&big_kernel_ਤਾਲਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BKL_UNLOCK()	spinlock_unlock(&big_kernel_ਤਾਲਾ)

#ਜੇ_ਅੰਤ /* __SPINLOCK_H__ */
