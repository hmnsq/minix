#अगर_अघोषित IPC_FILTER_H
#घोषणा IPC_FILTER_H

/* Declaration of the ipc filter structure. It provides a framework to
 * selectively allow/disallow ipc messages a process agrees to receive. To this
 * end, a single ipc filter can be specified at a given time for any recipient
 * to blacklist/whitelist a set of ipc messages identified by sender or message
 * type.
 */
#समावेश <minix/ipc_filter.h>

/* IPC filter types. */
#घोषणा IPCF_NONE        0	/* no ipc filter */
#घोषणा IPCF_BLACKLIST   1	/* blacklist filter type */
#घोषणा IPCF_WHITELIST   2	/* whitelist filter type */

/* IPC filter element macros. */
EXTERN पूर्णांक _ipcf_nr;
#घोषणा IPCF_EL_CHECK(E) \
	((((E)->flags & IPCF_MATCH_M_TYPE) || \
	((E)->flags & IPCF_MATCH_M_SOURCE)) && \
	(!((E)->flags & IPCF_MATCH_M_SOURCE) || \
	IPCF_IS_ANY_EP((E)->m_source) || isokendpt((E)->m_source, &_ipcf_nr)))
#घोषणा IPCF_IS_USR_EP(EP) \
	(!(priv(proc_addr(_ENDPOINT_P((EP))))->s_flags & SYS_PROC))
#घोषणा IPCF_IS_TSK_EP(EP) (iskerneln(_ENDPOINT_P((EP))))
#घोषणा IPCF_IS_SYS_EP(EP) (!IPCF_IS_USR_EP(EP) && !IPCF_IS_TSK_EP(EP))
#घोषणा IPCF_IS_ANY_EP(EP) \
	((EP) == ANY_USR || (EP) == ANY_SYS || (EP) == ANY_TSK)
#घोषणा IPCF_EL_MATCH_M_TYPE(E,M) \
	(!((E)->flags & IPCF_MATCH_M_TYPE) || (E)->m_type == (M)->m_type)
#घोषणा IPCF_EL_MATCH_M_SOURCE(E,M) \
	(!((E)->flags & IPCF_MATCH_M_SOURCE) || \
	(E)->m_source == (M)->m_source || \
	IPCF_EL_MATCH_M_SOURCE_ANY_EP((E)->m_source,(M)->m_source))
#घोषणा IPCF_EL_MATCH_M_SOURCE_ANY_EP(ES,MS) \
	(((ES) == ANY_USR && IPCF_IS_USR_EP(MS)) || \
	((ES) == ANY_SYS && IPCF_IS_SYS_EP(MS)) || \
	((ES) == ANY_TSK && IPCF_IS_TSK_EP(MS)))
#घोषणा IPCF_EL_MATCH(E,M) \
	(IPCF_EL_MATCH_M_TYPE(E,M) && IPCF_EL_MATCH_M_SOURCE(E,M))

काष्ठा ipc_filter_s {
  पूर्णांक type;
  पूर्णांक अंक_elements;
  पूर्णांक flags;
  काष्ठा ipc_filter_s *अग्र;
  ipc_filter_el_t elements[IPCF_MAX_ELEMENTS];
};
प्रकार काष्ठा ipc_filter_s ipc_filter_t;

/* IPC filter pool. */
#घोषणा IPCF_POOL_आकार          (2*NR_SYS_PROCS)
EXTERN ipc_filter_t ipc_filter_pool[IPCF_POOL_आकार];

/* IPC filter pool macros. */
#घोषणा IPCF_POOL_FREE_SLOT(S) ((S)->type = IPCF_NONE)
#घोषणा IPCF_POOL_IS_FREE_SLOT(S) ((S)->type == IPCF_NONE)
#घोषणा IPCF_POOL_ALLOCATE_SLOT(T,S) \
	करो { \
		पूर्णांक i; \
		*(S) = शून्य; \
		क्रम (i = 0; i < IPCF_POOL_आकार; i++) { \
			अगर (IPCF_POOL_IS_FREE_SLOT(&ipc_filter_pool[i])) { \
				*(S) = &ipc_filter_pool[i]; \
				(*(S))->type = T; \
				अवरोध; \
			} \
		} \
	} जबतक(0)
#घोषणा IPCF_POOL_INIT(S) स_रखो(&ipc_filter_pool,0,माप(ipc_filter_pool))

#पूर्ण_अगर /* !IPC_FILTER_H */
