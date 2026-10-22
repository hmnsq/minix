#ifndef IPC_FILTER_H
#define IPC_FILTER_H

/* Declaration of the ipc filter structure. It provides a framework to
 * selectively allow/disallow ipc messages a process agrees to receive. To this
 * end, a single ipc filter can be specified at a given time for any recipient
 * to blacklist/whitelist a set of ipc messages identified by sender or message
 * type.
 */
#அந்தர்பாவ <minix/ipc_filter.h>

/* IPC filter types. */
#define IPCF_NONE        0	/* no ipc filter */
#define IPCF_BLACKLIST   1	/* blacklist filter type */
#define IPCF_WHITELIST   2	/* whitelist filter type */

/* IPC filter element macros. */
EXTERN முழு _ipcf_nr;
#define IPCF_EL_CHECK(E) \
	((((E)->flags & IPCF_MATCH_M_TYPE) || \
	((E)->flags & IPCF_MATCH_M_SOURCE)) && \
	(!((E)->flags & IPCF_MATCH_M_SOURCE) || \
	IPCF_IS_ANY_EP((E)->m_source) || isokendpt((E)->m_source, &_ipcf_nr)))
#define IPCF_IS_USR_EP(EP) \
	(!(priv(proc_addr(_ENDPOINT_P((EP))))->s_flags & SYS_PROC))
#define IPCF_IS_TSK_EP(EP) (iskerneln(_ENDPOINT_P((EP))))
#define IPCF_IS_SYS_EP(EP) (!IPCF_IS_USR_EP(EP) && !IPCF_IS_TSK_EP(EP))
#define IPCF_IS_ANY_EP(EP) \
	((EP) == ANY_USR || (EP) == ANY_SYS || (EP) == ANY_TSK)
#define IPCF_EL_MATCH_M_TYPE(E,M) \
	(!((E)->flags & IPCF_MATCH_M_TYPE) || (E)->m_type == (M)->m_type)
#define IPCF_EL_MATCH_M_SOURCE(E,M) \
	(!((E)->flags & IPCF_MATCH_M_SOURCE) || \
	(E)->m_source == (M)->m_source || \
	IPCF_EL_MATCH_M_SOURCE_ANY_EP((E)->m_source,(M)->m_source))
#define IPCF_EL_MATCH_M_SOURCE_ANY_EP(ES,MS) \
	(((ES) == ANY_USR && IPCF_IS_USR_EP(MS)) || \
	((ES) == ANY_SYS && IPCF_IS_SYS_EP(MS)) || \
	((ES) == ANY_TSK && IPCF_IS_TSK_EP(MS)))
#define IPCF_EL_MATCH(E,M) \
	(IPCF_EL_MATCH_M_TYPE(E,M) && IPCF_EL_MATCH_M_SOURCE(E,M))

கட்டமைப்பு ipc_filter_s {
  முழு type;
  முழு இலக்கம்_elements;
  முழு flags;
  கட்டமைப்பு ipc_filter_s *next;
  ipc_filter_el_t elements[IPCF_MAX_ELEMENTS];
};
வகைவரை கட்டமைப்பு ipc_filter_s ipc_filter_t;

/* IPC filter pool. */
#define IPCF_POOL_SIZE          (2*NR_SYS_PROCS)
EXTERN ipc_filter_t ipc_filter_pool[IPCF_POOL_SIZE];

/* IPC filter pool macros. */
#define IPCF_POOL_FREE_SLOT(S) ((S)->type = IPCF_NONE)
#define IPCF_POOL_IS_FREE_SLOT(S) ((S)->type == IPCF_NONE)
#define IPCF_POOL_ALLOCATE_SLOT(T,S) \
	செய் { \
		முழு i; \
		*(S) = வெற்று; \
		க்கு (i = 0; i < IPCF_POOL_SIZE; i++) { \
			எனில் (IPCF_POOL_IS_FREE_SLOT(&ipc_filter_pool[i])) { \
				*(S) = &ipc_filter_pool[i]; \
				(*(S))->type = T; \
				நிறுத்து; \
			} \
		} \
	} போது(0)
#define IPCF_POOL_INIT(S) memset(&ipc_filter_pool,0,அளவு(ipc_filter_pool))

#endif /* !IPC_FILTER_H */
