#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __SPINLOCK_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __SPINLOCK_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ spinlock {
	atomic_t val;
} spinlock_t;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_SMP

#ವ್ಯಾಖ್ಯಾನಿಸು SPINLOCK_DEFINE(name)
#ವ್ಯಾಖ್ಯಾನಿಸು ಖಾಸಗಿ_SPINLOCK_DEFINE(name)
#ವ್ಯಾಖ್ಯಾನಿಸು SPINLOCK_DECLARE(name)
#ವ್ಯಾಖ್ಯಾನಿಸು spinlock_init(sl)
#ವ್ಯಾಖ್ಯಾನಿಸು spinlock_ಬೀಗ(sl)
#ವ್ಯಾಖ್ಯಾನಿಸು spinlock_unlock(sl)

#ಇಲ್ಲದಿದ್ದರೆ

/* SMP */
#ವ್ಯಾಖ್ಯಾನಿಸು SPINLOCK_DEFINE(name)	spinlock_t name;
#ವ್ಯಾಖ್ಯಾನಿಸು ಖಾಸಗಿ_SPINLOCK_DEFINE(name)	ಖಾಸಗಿ_2 SPINLOCK_DEFINE(name)
#ವ್ಯಾಖ್ಯಾನಿಸು SPINLOCK_DECLARE(name)	ಬಾಹ್ಯ SPINLOCK_DEFINE(name)
#ವ್ಯಾಖ್ಯಾನಿಸು spinlock_init(sl) ಮಾಡು { (sl)->val = 0; } ಆಗ (0)

#ಆದರೆ CONFIG_MAX_CPUS == 1
#ವ್ಯಾಖ್ಯಾನಿಸು spinlock_ಬೀಗ(sl)
#ವ್ಯಾಖ್ಯಾನಿಸು spinlock_unlock(sl)
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ arch_spinlock_ಬೀಗ(atomic_t * sl);
ಶೂನ್ಯ arch_spinlock_unlock(atomic_t * sl);
#ವ್ಯಾಖ್ಯಾನಿಸು spinlock_ಬೀಗ(sl)	arch_spinlock_ಬೀಗ((atomic_t*) sl)
#ವ್ಯಾಖ್ಯಾನಿಸು spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_SMP */

#ವ್ಯಾಖ್ಯಾನಿಸು BKL_ಬೀಗ()	spinlock_ಬೀಗ(&big_kernel_ಬೀಗ)
#ವ್ಯಾಖ್ಯಾನಿಸು BKL_UNLOCK()	spinlock_unlock(&big_kernel_ಬೀಗ)

#ಆದರೆ_ಅಂತ್ಯ /* __SPINLOCK_H__ */
