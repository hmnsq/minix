#अगर_अघोषित __SPINLOCK_H__
#घोषणा __SPINLOCK_H__

#समावेश "kernel/kernel.h"

प्रकार काष्ठा spinlock {
	atomic_t val;
} spinlock_t;

#अगर_अघोषित CONFIG_SMP

#घोषणा SPINLOCK_DEFINE(name)
#घोषणा PRIVATE_SPINLOCK_DEFINE(name)
#घोषणा SPINLOCK_DECLARE(name)
#घोषणा spinlock_init(sl)
#घोषणा spinlock_lock(sl)
#घोषणा spinlock_unlock(sl)

#अन्यथा

/* SMP */
#घोषणा SPINLOCK_DEFINE(name)	spinlock_t name;
#घोषणा PRIVATE_SPINLOCK_DEFINE(name)	PRIVATE SPINLOCK_DEFINE(name)
#घोषणा SPINLOCK_DECLARE(name)	बाह्य SPINLOCK_DEFINE(name)
#घोषणा spinlock_init(sl) करो { (sl)->val = 0; } जबतक (0)

#अगर CONFIG_MAX_CPUS == 1
#घोषणा spinlock_lock(sl)
#घोषणा spinlock_unlock(sl)
#अन्यथा
व्योम arch_spinlock_lock(atomic_t * sl);
व्योम arch_spinlock_unlock(atomic_t * sl);
#घोषणा spinlock_lock(sl)	arch_spinlock_lock((atomic_t*) sl)
#घोषणा spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#पूर्ण_अगर


#पूर्ण_अगर /* CONFIG_SMP */

#घोषणा BKL_LOCK()	spinlock_lock(&big_kernel_lock)
#घोषणा BKL_UNLOCK()	spinlock_unlock(&big_kernel_lock)

#पूर्ण_अगर /* __SPINLOCK_H__ */
