#جے_بنا_تعریف IPC_FILTER_H
#تعریف_کرو IPC_FILTER_H

/* Declaration of the ipc filter structure. It provides a framework to
 * selectively allow/disallow ipc messages a process agrees to receive. To this
 * end, a single ipc filter can be specified at a given time for any recipient
 * to blacklist/whitelist a set of ipc messages identified by sender or message
 * type.
 */
#ضمن <minix/ipc_filter.h>

/* IPC filter types. */
#تعریف_کرو IPCF_NONE        0	/* no ipc filter */
#تعریف_کرو IPCF_BLACKLIST   1	/* blacklist filter type */
#تعریف_کرو IPCF_WHITELIST   2	/* whitelist filter type */

/* IPC filter element macros. */
EXTERN صحيح _ipcf_nr;
#تعریف_کرو IPCF_EL_CHECK(E) \
	((((E)->flags & IPCF_میل_M_TYPE) || \
	((E)->flags & IPCF_میل_M_سروت)) && \
	(!((E)->flags & IPCF_میل_M_سروت) || \
	IPCF_IS_ANY_EP((E)->m_سروت) || isokendpt((E)->m_سروت, &_ipcf_nr)))
#تعریف_کرو IPCF_IS_USR_EP(EP) \
	(!(priv(proc_addr(_ENDPOINT_P((EP))))->s_flags & SYS_PROC))
#تعریف_کرو IPCF_IS_TSK_EP(EP) (iskerneln(_ENDPOINT_P((EP))))
#تعریف_کرو IPCF_IS_SYS_EP(EP) (!IPCF_IS_USR_EP(EP) && !IPCF_IS_TSK_EP(EP))
#تعریف_کرو IPCF_IS_ANY_EP(EP) \
	((EP) == ANY_USR || (EP) == ANY_SYS || (EP) == ANY_TSK)
#تعریف_کرو IPCF_EL_میل_M_TYPE(E,M) \
	(!((E)->flags & IPCF_میل_M_TYPE) || (E)->m_type == (M)->m_type)
#تعریف_کرو IPCF_EL_میل_M_سروت(E,M) \
	(!((E)->flags & IPCF_میل_M_سروت) || \
	(E)->m_سروت == (M)->m_سروت || \
	IPCF_EL_میل_M_سروت_ANY_EP((E)->m_سروت,(M)->m_سروت))
#تعریف_کرو IPCF_EL_میل_M_سروت_ANY_EP(ES,MS) \
	(((ES) == ANY_USR && IPCF_IS_USR_EP(MS)) || \
	((ES) == ANY_SYS && IPCF_IS_SYS_EP(MS)) || \
	((ES) == ANY_TSK && IPCF_IS_TSK_EP(MS)))
#تعریف_کرو IPCF_EL_میل(E,M) \
	(IPCF_EL_میل_M_TYPE(E,M) && IPCF_EL_میل_M_سروت(E,M))

بنية ipc_filter_s {
  صحيح type;
  صحيح عدد_elements;
  صحيح flags;
  بنية ipc_filter_s *اگلا;
  ipc_filter_el_t elements[IPCF_MAX_ELEMENTS];
};
تعريفنوع بنية ipc_filter_s ipc_filter_t;

/* IPC filter pool. */
#تعریف_کرو IPCF_POOL_ناپ          (2*NR_SYS_PROCS)
EXTERN ipc_filter_t ipc_filter_pool[IPCF_POOL_ناپ];

/* IPC filter pool macros. */
#تعریف_کرو IPCF_POOL_FREE_تھاں(S) ((S)->type = IPCF_NONE)
#تعریف_کرو IPCF_POOL_IS_FREE_تھاں(S) ((S)->type == IPCF_NONE)
#تعریف_کرو IPCF_POOL_ALLOCATE_تھاں(T,S) \
	افعل { \
		صحيح i; \
		*(S) = عدم; \
		لكل (i = 0; i < IPCF_POOL_ناپ; i++) { \
			إذا (IPCF_POOL_IS_FREE_تھاں(&ipc_filter_pool[i])) { \
				*(S) = &ipc_filter_pool[i]; \
				(*(S))->type = T; \
				اكسر; \
			} \
		} \
	} طالما(0)
#تعریف_کرو IPCF_POOL_INIT(S) memset(&ipc_filter_pool,0,حجم(ipc_filter_pool))

#جے_انت /* !IPC_FILTER_H */
