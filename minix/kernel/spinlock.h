#నిర్వచితం_కాకపోతే __SPINLOCK_H__
#నిర్వచించు __SPINLOCK_H__

#అన్తర్భావ "kernel/kernel.h"

రకనిర్వచనం నిర్మాణం spinlock {
	atomic_t val;
} spinlock_t;

#నిర్వచితం_కాకపోతే CONFIG_SMP

#నిర్వచించు SPINLOCK_DEFINE(name)
#నిర్వచించు వ్యక్తిగత_SPINLOCK_DEFINE(name)
#నిర్వచించు SPINLOCK_DECLARE(name)
#నిర్వచించు spinlock_init(sl)
#నిర్వచించు spinlock_తాళం(sl)
#నిర్వచించు spinlock_unlock(sl)

#లేకపోతే

/* SMP */
#నిర్వచించు SPINLOCK_DEFINE(name)	spinlock_t name;
#నిర్వచించు వ్యక్తిగత_SPINLOCK_DEFINE(name)	వ్యక్తిగత SPINLOCK_DEFINE(name)
#నిర్వచించు SPINLOCK_DECLARE(name)	బాహ్య SPINLOCK_DEFINE(name)
#నిర్వచించు spinlock_init(sl) చేయి { (sl)->val = 0; } అయితే (0)

#ఒకవేళ CONFIG_MAX_CPUS == 1
#నిర్వచించు spinlock_తాళం(sl)
#నిర్వచించు spinlock_unlock(sl)
#లేకపోతే
శూన్య arch_spinlock_తాళం(atomic_t * sl);
శూన్య arch_spinlock_unlock(atomic_t * sl);
#నిర్వచించు spinlock_తాళం(sl)	arch_spinlock_తాళం((atomic_t*) sl)
#నిర్వచించు spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#అయితే_ముగింపు


#అయితే_ముగింపు /* CONFIG_SMP */

#నిర్వచించు BKL_తాళం()	spinlock_తాళం(&big_kernel_తాళం)
#నిర్వచించు BKL_UNLOCK()	spinlock_unlock(&big_kernel_తాళం)

#అయితే_ముగింపు /* __SPINLOCK_H__ */
