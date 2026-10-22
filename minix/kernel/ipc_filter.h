#که_نه_تعریف_شوی IPC_FILTER_H
#تعریف_کړه IPC_FILTER_H

/* Declaration of the ipc filter structure. It provides a framework to
 * selectively allow/disallow ipc messages a process agrees to receive. To this
 * end, a single ipc filter can be specified at a given time for any recipient
 * to blacklist/whitelist a set of ipc messages identified by sender or message
 * type.
 */
#ضمن <minix/ipc_filter.h>

/* IPC filter types. */
#تعریف_کړه IPCF_NONE        0	/* no ipc filter */
#تعریف_کړه IPCF_BLACKLIST   1	/* blacklist filter type */
#تعریف_کړه IPCF_WHITELIST   2	/* whitelist filter type */

/* IPC filter element macros. */
EXTERN صحيح _ipcf_nr;
#تعریف_کړه IPCF_EL_CHECK(E) \
	((((E)->flags & IPCF_سمون_M_TYPE) || \
	((E)->flags & IPCF_سمون_M_سرچینه)) && \
	(!((E)->flags & IPCF_سمون_M_سرچینه) || \
	IPCF_IS_ANY_EP((E)->m_سرچینه) || isokendpt((E)->m_سرچینه, &_ipcf_nr)))
#تعریف_کړه IPCF_IS_USR_EP(EP) \
	(!(priv(proc_addr(_ENDPOINT_P((EP))))->s_flags & SYS_PROC))
#تعریف_کړه IPCF_IS_TSK_EP(EP) (iskerneln(_ENDPOINT_P((EP))))
#تعریف_کړه IPCF_IS_SYS_EP(EP) (!IPCF_IS_USR_EP(EP) && !IPCF_IS_TSK_EP(EP))
#تعریف_کړه IPCF_IS_ANY_EP(EP) \
	((EP) == ANY_USR || (EP) == ANY_SYS || (EP) == ANY_TSK)
#تعریف_کړه IPCF_EL_سمون_M_TYPE(E,M) \
	(!((E)->flags & IPCF_سمون_M_TYPE) || (E)->m_type == (M)->m_type)
#تعریف_کړه IPCF_EL_سمون_M_سرچینه(E,M) \
	(!((E)->flags & IPCF_سمون_M_سرچینه) || \
	(E)->m_سرچینه == (M)->m_سرچینه || \
	IPCF_EL_سمون_M_سرچینه_ANY_EP((E)->m_سرچینه,(M)->m_سرچینه))
#تعریف_کړه IPCF_EL_سمون_M_سرچینه_ANY_EP(ES,MS) \
	(((ES) == ANY_USR && IPCF_IS_USR_EP(MS)) || \
	((ES) == ANY_SYS && IPCF_IS_SYS_EP(MS)) || \
	((ES) == ANY_TSK && IPCF_IS_TSK_EP(MS)))
#تعریف_کړه IPCF_EL_سمون(E,M) \
	(IPCF_EL_سمون_M_TYPE(E,M) && IPCF_EL_سمون_M_سرچینه(E,M))

بنية ipc_filter_s {
  صحيح type;
  صحيح عدد_elements;
  صحيح flags;
  بنية ipc_filter_s *بل;
  ipc_filter_el_t elements[IPCF_MAX_ELEMENTS];
};
تعريفنوع بنية ipc_filter_s ipc_filter_t;

/* IPC filter pool. */
#تعریف_کړه IPCF_POOL_اندازه          (2*NR_SYS_PROCS)
EXTERN ipc_filter_t ipc_filter_pool[IPCF_POOL_اندازه];

/* IPC filter pool macros. */
#تعریف_کړه IPCF_POOL_FREE_خونه(S) ((S)->type = IPCF_NONE)
#تعریف_کړه IPCF_POOL_IS_FREE_خونه(S) ((S)->type == IPCF_NONE)
#تعریف_کړه IPCF_POOL_ALLOCATE_خونه(T,S) \
	افعل { \
		صحيح i; \
		*(S) = عدم; \
		لكل (i = 0; i < IPCF_POOL_اندازه; i++) { \
			إذا (IPCF_POOL_IS_FREE_خونه(&ipc_filter_pool[i])) { \
				*(S) = &ipc_filter_pool[i]; \
				(*(S))->type = T; \
				اكسر; \
			} \
		} \
	} طالما(0)
#تعریف_کړه IPCF_POOL_INIT(S) memset(&ipc_filter_pool,0,حجم(ipc_filter_pool))

#د_که_پای /* !IPC_FILTER_H */
