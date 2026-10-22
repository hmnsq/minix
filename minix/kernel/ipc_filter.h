#यदि_अपरिभाषितम् IPC_FILTER_H
#परिभाषय IPC_FILTER_H

/* Declaration of the ipc filter structure. It provides a framework to
 * selectively allow/disallow ipc messages a process agrees to receive. To this
 * end, a single ipc filter can be specified at a given time for any recipient
 * to blacklist/whitelist a set of ipc messages identified by sender or message
 * type.
 */
#अन्तर्भाव <minix/ipc_filter.h>

/* IPC filter types. */
#परिभाषय IPCF_NONE        0	/* no ipc filter */
#परिभाषय IPCF_BLACKLIST   1	/* blacklist filter type */
#परिभाषय IPCF_WHITELIST   2	/* whitelist filter type */

/* IPC filter element macros. */
EXTERN पूर्णाङ्क _ipcf_nr;
#परिभाषय IPCF_EL_CHECK(E) \
	((((E)->flags & IPCF_मेलनम्_M_TYPE) || \
	((E)->flags & IPCF_मेलनम्_M_SOURCE)) && \
	(!((E)->flags & IPCF_मेलनम्_M_SOURCE) || \
	IPCF_IS_ANY_EP((E)->m_source) || isokendpt((E)->m_source, &_ipcf_nr)))
#परिभाषय IPCF_IS_USR_EP(EP) \
	(!(priv(proc_addr(_ENDPOINT_P((EP))))->s_flags & SYS_PROC))
#परिभाषय IPCF_IS_TSK_EP(EP) (iskerneln(_ENDPOINT_P((EP))))
#परिभाषय IPCF_IS_SYS_EP(EP) (!IPCF_IS_USR_EP(EP) && !IPCF_IS_TSK_EP(EP))
#परिभाषय IPCF_IS_ANY_EP(EP) \
	((EP) == ANY_USR || (EP) == ANY_SYS || (EP) == ANY_TSK)
#परिभाषय IPCF_EL_मेलनम्_M_TYPE(E,M) \
	(!((E)->flags & IPCF_मेलनम्_M_TYPE) || (E)->m_type == (M)->m_type)
#परिभाषय IPCF_EL_मेलनम्_M_SOURCE(E,M) \
	(!((E)->flags & IPCF_मेलनम्_M_SOURCE) || \
	(E)->m_source == (M)->m_source || \
	IPCF_EL_मेलनम्_M_SOURCE_ANY_EP((E)->m_source,(M)->m_source))
#परिभाषय IPCF_EL_मेलनम्_M_SOURCE_ANY_EP(ES,MS) \
	(((ES) == ANY_USR && IPCF_IS_USR_EP(MS)) || \
	((ES) == ANY_SYS && IPCF_IS_SYS_EP(MS)) || \
	((ES) == ANY_TSK && IPCF_IS_TSK_EP(MS)))
#परिभाषय IPCF_EL_मेलनम्(E,M) \
	(IPCF_EL_मेलनम्_M_TYPE(E,M) && IPCF_EL_मेलनम्_M_SOURCE(E,M))

संरचना ipc_filter_s {
  पूर्णाङ्क type;
  पूर्णाङ्क अङ्कः_elements;
  पूर्णाङ्क flags;
  संरचना ipc_filter_s *अग्र;
  ipc_filter_el_t elements[IPCF_MAX_ELEMENTS];
};
प्रकारनिर्देश संरचना ipc_filter_s ipc_filter_t;

/* IPC filter pool. */
#परिभाषय IPCF_POOL_आकार          (2*NR_SYS_PROCS)
EXTERN ipc_filter_t ipc_filter_pool[IPCF_POOL_आकार];

/* IPC filter pool macros. */
#परिभाषय IPCF_POOL_FREE_SLOT(S) ((S)->type = IPCF_NONE)
#परिभाषय IPCF_POOL_IS_FREE_SLOT(S) ((S)->type == IPCF_NONE)
#परिभाषय IPCF_POOL_ALLOCATE_SLOT(T,S) \
	कुरु { \
		पूर्णाङ्क i; \
		*(S) = रिक्त; \
		प्रति (i = 0; i < IPCF_POOL_आकार; i++) { \
			यदि (IPCF_POOL_IS_FREE_SLOT(&ipc_filter_pool[i])) { \
				*(S) = &ipc_filter_pool[i]; \
				(*(S))->type = T; \
				भङ्ग; \
			} \
		} \
	} यावत्(0)
#परिभाषय IPCF_POOL_INIT(S) स्मृतिं_पूरय(&ipc_filter_pool,0,परिमाण(ipc_filter_pool))

#यदि_अन्तः /* !IPC_FILTER_H */
