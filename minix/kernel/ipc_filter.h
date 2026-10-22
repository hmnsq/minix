#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IPC_FILTER_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPC_FILTER_H

/* Declaration of the ipc filter structure. It provides a framework to
 * selectively allow/disallow ipc messages a process agrees to receive. To this
 * end, a single ipc filter can be specified at a given time for any recipient
 * to blacklist/whitelist a set of ipc messages identified by sender or message
 * type.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/ipc_filter.h>

/* IPC filter types. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_NONE        0	/* no ipc filter */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_BLACKLIST   1	/* blacklist filter type */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_WHITELIST   2	/* whitelist filter type */

/* IPC filter element macros. */
EXTERN ਪੂਰਨ _ipcf_nr;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_EL_CHECK(E) \
	((((E)->flags & IPCF_ਮੇਲ_M_TYPE) || \
	((E)->flags & IPCF_ਮੇਲ_M_ਸਰੋਤ)) && \
	(!((E)->flags & IPCF_ਮੇਲ_M_ਸਰੋਤ) || \
	IPCF_IS_ANY_EP((E)->m_ਸਰੋਤ) || isokendpt((E)->m_ਸਰੋਤ, &_ipcf_nr)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_IS_USR_EP(EP) \
	(!(priv(proc_addr(_ENDPOINT_P((EP))))->s_flags & SYS_PROC))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_IS_TSK_EP(EP) (iskerneln(_ENDPOINT_P((EP))))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_IS_SYS_EP(EP) (!IPCF_IS_USR_EP(EP) && !IPCF_IS_TSK_EP(EP))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_IS_ANY_EP(EP) \
	((EP) == ANY_USR || (EP) == ANY_SYS || (EP) == ANY_TSK)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_EL_ਮੇਲ_M_TYPE(E,M) \
	(!((E)->flags & IPCF_ਮੇਲ_M_TYPE) || (E)->m_type == (M)->m_type)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_EL_ਮੇਲ_M_ਸਰੋਤ(E,M) \
	(!((E)->flags & IPCF_ਮੇਲ_M_ਸਰੋਤ) || \
	(E)->m_ਸਰੋਤ == (M)->m_ਸਰੋਤ || \
	IPCF_EL_ਮੇਲ_M_ਸਰੋਤ_ANY_EP((E)->m_ਸਰੋਤ,(M)->m_ਸਰੋਤ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_EL_ਮੇਲ_M_ਸਰੋਤ_ANY_EP(ES,MS) \
	(((ES) == ANY_USR && IPCF_IS_USR_EP(MS)) || \
	((ES) == ANY_SYS && IPCF_IS_SYS_EP(MS)) || \
	((ES) == ANY_TSK && IPCF_IS_TSK_EP(MS)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_EL_ਮੇਲ(E,M) \
	(IPCF_EL_ਮੇਲ_M_TYPE(E,M) && IPCF_EL_ਮੇਲ_M_ਸਰੋਤ(E,M))

ਬਣਤਰ ipc_filter_s {
  ਪੂਰਨ type;
  ਪੂਰਨ ਅੰਕ_elements;
  ਪੂਰਨ flags;
  ਬਣਤਰ ipc_filter_s *ਅਗਲਾ;
  ipc_filter_el_t elements[IPCF_MAX_ELEMENTS];
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ipc_filter_s ipc_filter_t;

/* IPC filter pool. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_POOL_ਮਾਪ          (2*NR_SYS_PROCS)
EXTERN ipc_filter_t ipc_filter_pool[IPCF_POOL_ਮਾਪ];

/* IPC filter pool macros. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_POOL_FREE_ਥਾਂ(S) ((S)->type = IPCF_NONE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_POOL_IS_FREE_ਥਾਂ(S) ((S)->type == IPCF_NONE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_POOL_ALLOCATE_ਥਾਂ(T,S) \
	ਕਰੋ { \
		ਪੂਰਨ i; \
		*(S) = ਨਿਰਾ; \
		ਲਈ (i = 0; i < IPCF_POOL_ਮਾਪ; i++) { \
			ਜੇ (IPCF_POOL_IS_FREE_ਥਾਂ(&ipc_filter_pool[i])) { \
				*(S) = &ipc_filter_pool[i]; \
				(*(S))->type = T; \
				ਰੋਕੋ; \
			} \
		} \
	} ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCF_POOL_INIT(S) memset(&ipc_filter_pool,0,ਆਕਾਰ(ipc_filter_pool))

#ਜੇ_ਅੰਤ /* !IPC_FILTER_H */
