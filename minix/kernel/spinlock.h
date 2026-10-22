#यदि_अपरिभाषितम् __SPINLOCK_H__
#परिभाषय __SPINLOCK_H__

#अन्तर्भाव "kernel/kernel.h"

प्रकारनिर्देश संरचना spinlock {
	atomic_t val;
} spinlock_t;

#यदि_अपरिभाषितम् CONFIG_SMP

#परिभाषय SPINLOCK_DEFINE(name)
#परिभाषय वैयक्तिकम्_SPINLOCK_DEFINE(name)
#परिभाषय SPINLOCK_DECLARE(name)
#परिभाषय spinlock_init(sl)
#परिभाषय spinlock_तालकम्(sl)
#परिभाषय spinlock_unlock(sl)

#अन्यथा

/* SMP */
#परिभाषय SPINLOCK_DEFINE(name)	spinlock_t name;
#परिभाषय वैयक्तिकम्_SPINLOCK_DEFINE(name)	वैयक्तिकम् SPINLOCK_DEFINE(name)
#परिभाषय SPINLOCK_DECLARE(name)	बाह्य SPINLOCK_DEFINE(name)
#परिभाषय spinlock_init(sl) कुरु { (sl)->val = 0; } यावत् (0)

#यदि CONFIG_MAX_CPUS == 1
#परिभाषय spinlock_तालकम्(sl)
#परिभाषय spinlock_unlock(sl)
#अन्यथा
शून्य arch_spinlock_तालकम्(atomic_t * sl);
शून्य arch_spinlock_unlock(atomic_t * sl);
#परिभाषय spinlock_तालकम्(sl)	arch_spinlock_तालकम्((atomic_t*) sl)
#परिभाषय spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#यदि_अन्तः


#यदि_अन्तः /* CONFIG_SMP */

#परिभाषय BKL_तालकम्()	spinlock_तालकम्(&big_kernel_तालकम्)
#परिभाषय BKL_UNLOCK()	spinlock_unlock(&big_kernel_तालकम्)

#यदि_अन्तः /* __SPINLOCK_H__ */
