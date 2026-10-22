#अगर_अघोषित __GLO_X86_H__
#घोषणा __GLO_X86_H__

#समावेश "kernel/kernel.h"
#समावेश "arch_proto.h"

EXTERN पूर्णांक cpu_has_tsc;	/* signal whether this cpu has time stamp register. This
			   feature was introduced by Pentium */

EXTERN काष्ठा tss_s tss[CONFIG_MAX_CPUS];

EXTERN पूर्णांक i386_paging_enabled;

#पूर्ण_अगर /* __GLO_X86_H__ */
