#నిర్వచితం_కాకపోతే IPC_FILTER_H
#నిర్వచించు IPC_FILTER_H

/* Declaration of the ipc filter structure. It provides a framework to
 * selectively allow/disallow ipc messages a process agrees to receive. To this
 * end, a single ipc filter can be specified at a given time for any recipient
 * to blacklist/whitelist a set of ipc messages identified by sender or message
 * type.
 */
#అన్తర్భావ <minix/ipc_filter.h>

/* IPC filter types. */
#నిర్వచించు IPCF_NONE        0	/* no ipc filter */
#నిర్వచించు IPCF_BLACKLIST   1	/* blacklist filter type */
#నిర్వచించు IPCF_WHITELIST   2	/* whitelist filter type */

/* IPC filter element macros. */
EXTERN పూర్ణాంకం _ipcf_nr;
#నిర్వచించు IPCF_EL_CHECK(E) \
	((((E)->flags & IPCF_సరిపోలిక_M_TYPE) || \
	((E)->flags & IPCF_సరిపోలిక_M_ఆధారం)) && \
	(!((E)->flags & IPCF_సరిపోలిక_M_ఆధారం) || \
	IPCF_IS_ANY_EP((E)->m_ఆధారం) || isokendpt((E)->m_ఆధారం, &_ipcf_nr)))
#నిర్వచించు IPCF_IS_USR_EP(EP) \
	(!(priv(proc_addr(_ENDPOINT_P((EP))))->s_flags & SYS_PROC))
#నిర్వచించు IPCF_IS_TSK_EP(EP) (iskerneln(_ENDPOINT_P((EP))))
#నిర్వచించు IPCF_IS_SYS_EP(EP) (!IPCF_IS_USR_EP(EP) && !IPCF_IS_TSK_EP(EP))
#నిర్వచించు IPCF_IS_ANY_EP(EP) \
	((EP) == ANY_USR || (EP) == ANY_SYS || (EP) == ANY_TSK)
#నిర్వచించు IPCF_EL_సరిపోలిక_M_TYPE(E,M) \
	(!((E)->flags & IPCF_సరిపోలిక_M_TYPE) || (E)->m_type == (M)->m_type)
#నిర్వచించు IPCF_EL_సరిపోలిక_M_ఆధారం(E,M) \
	(!((E)->flags & IPCF_సరిపోలిక_M_ఆధారం) || \
	(E)->m_ఆధారం == (M)->m_ఆధారం || \
	IPCF_EL_సరిపోలిక_M_ఆధారం_ANY_EP((E)->m_ఆధారం,(M)->m_ఆధారం))
#నిర్వచించు IPCF_EL_సరిపోలిక_M_ఆధారం_ANY_EP(ES,MS) \
	(((ES) == ANY_USR && IPCF_IS_USR_EP(MS)) || \
	((ES) == ANY_SYS && IPCF_IS_SYS_EP(MS)) || \
	((ES) == ANY_TSK && IPCF_IS_TSK_EP(MS)))
#నిర్వచించు IPCF_EL_సరిపోలిక(E,M) \
	(IPCF_EL_సరిపోలిక_M_TYPE(E,M) && IPCF_EL_సరిపోలిక_M_ఆధారం(E,M))

నిర్మాణం ipc_filter_s {
  పూర్ణాంకం type;
  పూర్ణాంకం అంకె_elements;
  పూర్ణాంకం flags;
  నిర్మాణం ipc_filter_s *తదుపరి;
  ipc_filter_el_t elements[IPCF_MAX_ELEMENTS];
};
రకనిర్వచనం నిర్మాణం ipc_filter_s ipc_filter_t;

/* IPC filter pool. */
#నిర్వచించు IPCF_POOL_కొలత          (2*NR_SYS_PROCS)
EXTERN ipc_filter_t ipc_filter_pool[IPCF_POOL_కొలత];

/* IPC filter pool macros. */
#నిర్వచించు IPCF_POOL_FREE_స్థానం(S) ((S)->type = IPCF_NONE)
#నిర్వచించు IPCF_POOL_IS_FREE_స్థానం(S) ((S)->type == IPCF_NONE)
#నిర్వచించు IPCF_POOL_ALLOCATE_స్థానం(T,S) \
	చేయి { \
		పూర్ణాంకం i; \
		*(S) = ఖాళీ; \
		కొరకు (i = 0; i < IPCF_POOL_కొలత; i++) { \
			ఒకవేళ (IPCF_POOL_IS_FREE_స్థానం(&ipc_filter_pool[i])) { \
				*(S) = &ipc_filter_pool[i]; \
				(*(S))->type = T; \
				ఆపు; \
			} \
		} \
	} అయితే(0)
#నిర్వచించు IPCF_POOL_INIT(S) memset(&ipc_filter_pool,0,పరిమాణం(ipc_filter_pool))

#అయితే_ముగింపు /* !IPC_FILTER_H */
