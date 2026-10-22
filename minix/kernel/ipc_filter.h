#جيڪڏهن_وصف_نه_ٿيل IPC_FILTER_H
#وصف_ڪريو IPC_FILTER_H

/* Declaration of the ipc filter structure. It provides a framework to
 * selectively allow/disallow ipc messages a process agrees to receive. To this
 * end, a single ipc filter can be specified at a given time for any recipient
 * to blacklist/whitelist a set of ipc messages identified by sender or message
 * type.
 */
#ضمن <minix/ipc_filter.h>

/* IPC filter types. */
#وصف_ڪريو IPCF_NONE        0	/* no ipc filter */
#وصف_ڪريو IPCF_BLACKLIST   1	/* blacklist filter type */
#وصف_ڪريو IPCF_WHITELIST   2	/* whitelist filter type */

/* IPC filter element macros. */
EXTERN صحيح _ipcf_nr;
#وصف_ڪريو IPCF_EL_CHECK(E) \
	((((E)->flags & IPCF_ميلاپ_M_TYPE) || \
	((E)->flags & IPCF_ميلاپ_M_ذريعو)) && \
	(!((E)->flags & IPCF_ميلاپ_M_ذريعو) || \
	IPCF_IS_ANY_EP((E)->m_ذريعو) || isokendpt((E)->m_ذريعو, &_ipcf_nr)))
#وصف_ڪريو IPCF_IS_USR_EP(EP) \
	(!(priv(proc_addr(_ENDPOINT_P((EP))))->s_flags & SYS_PROC))
#وصف_ڪريو IPCF_IS_TSK_EP(EP) (iskerneln(_ENDPOINT_P((EP))))
#وصف_ڪريو IPCF_IS_SYS_EP(EP) (!IPCF_IS_USR_EP(EP) && !IPCF_IS_TSK_EP(EP))
#وصف_ڪريو IPCF_IS_ANY_EP(EP) \
	((EP) == ANY_USR || (EP) == ANY_SYS || (EP) == ANY_TSK)
#وصف_ڪريو IPCF_EL_ميلاپ_M_TYPE(E,M) \
	(!((E)->flags & IPCF_ميلاپ_M_TYPE) || (E)->m_type == (M)->m_type)
#وصف_ڪريو IPCF_EL_ميلاپ_M_ذريعو(E,M) \
	(!((E)->flags & IPCF_ميلاپ_M_ذريعو) || \
	(E)->m_ذريعو == (M)->m_ذريعو || \
	IPCF_EL_ميلاپ_M_ذريعو_ANY_EP((E)->m_ذريعو,(M)->m_ذريعو))
#وصف_ڪريو IPCF_EL_ميلاپ_M_ذريعو_ANY_EP(ES,MS) \
	(((ES) == ANY_USR && IPCF_IS_USR_EP(MS)) || \
	((ES) == ANY_SYS && IPCF_IS_SYS_EP(MS)) || \
	((ES) == ANY_TSK && IPCF_IS_TSK_EP(MS)))
#وصف_ڪريو IPCF_EL_ميلاپ(E,M) \
	(IPCF_EL_ميلاپ_M_TYPE(E,M) && IPCF_EL_ميلاپ_M_ذريعو(E,M))

بنية ipc_filter_s {
  صحيح type;
  صحيح انگ_elements;
  صحيح flags;
  بنية ipc_filter_s *ايندڙ;
  ipc_filter_el_t elements[IPCF_MAX_ELEMENTS];
};
تعريفنوع بنية ipc_filter_s ipc_filter_t;

/* IPC filter pool. */
#وصف_ڪريو IPCF_POOL_سائيز          (2*NR_SYS_PROCS)
EXTERN ipc_filter_t ipc_filter_pool[IPCF_POOL_سائيز];

/* IPC filter pool macros. */
#وصف_ڪريو IPCF_POOL_FREE_جڳهه(S) ((S)->type = IPCF_NONE)
#وصف_ڪريو IPCF_POOL_IS_FREE_جڳهه(S) ((S)->type == IPCF_NONE)
#وصف_ڪريو IPCF_POOL_ALLOCATE_جڳهه(T,S) \
	افعل { \
		صحيح i; \
		*(S) = عدم; \
		لكل (i = 0; i < IPCF_POOL_سائيز; i++) { \
			إذا (IPCF_POOL_IS_FREE_جڳهه(&ipc_filter_pool[i])) { \
				*(S) = &ipc_filter_pool[i]; \
				(*(S))->type = T; \
				اكسر; \
			} \
		} \
	} طالما(0)
#وصف_ڪريو IPCF_POOL_INIT(S) memset(&ipc_filter_pool,0,حجم(ipc_filter_pool))

#جيڪڏهن_پورو /* !IPC_FILTER_H */
