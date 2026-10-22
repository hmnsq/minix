#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IPC_FILTER_H
#ವ್ಯಾಖ್ಯಾನಿಸು IPC_FILTER_H

/* Declaration of the ipc filter structure. It provides a framework to
 * selectively allow/disallow ipc messages a process agrees to receive. To this
 * end, a single ipc filter can be specified at a given time for any recipient
 * to blacklist/whitelist a set of ipc messages identified by sender or message
 * type.
 */
#ಅನ್ತರ್ಭಾವ <minix/ipc_filter.h>

/* IPC filter types. */
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_NONE        0	/* no ipc filter */
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_BLACKLIST   1	/* blacklist filter type */
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_WHITELIST   2	/* whitelist filter type */

/* IPC filter element macros. */
EXTERN ಪೂರ್ಣಾಂಕ _ipcf_nr;
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_EL_CHECK(E) \
	((((E)->flags & IPCF_ಹೊಂದಿಕೆ_M_TYPE) || \
	((E)->flags & IPCF_ಹೊಂದಿಕೆ_M_ಮೂಲ)) && \
	(!((E)->flags & IPCF_ಹೊಂದಿಕೆ_M_ಮೂಲ) || \
	IPCF_IS_ANY_EP((E)->m_ಮೂಲ) || isokendpt((E)->m_ಮೂಲ, &_ipcf_nr)))
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_IS_USR_EP(EP) \
	(!(priv(proc_addr(_ENDPOINT_P((EP))))->s_flags & SYS_PROC))
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_IS_TSK_EP(EP) (iskerneln(_ENDPOINT_P((EP))))
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_IS_SYS_EP(EP) (!IPCF_IS_USR_EP(EP) && !IPCF_IS_TSK_EP(EP))
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_IS_ANY_EP(EP) \
	((EP) == ANY_USR || (EP) == ANY_SYS || (EP) == ANY_TSK)
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_EL_ಹೊಂದಿಕೆ_M_TYPE(E,M) \
	(!((E)->flags & IPCF_ಹೊಂದಿಕೆ_M_TYPE) || (E)->m_type == (M)->m_type)
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_EL_ಹೊಂದಿಕೆ_M_ಮೂಲ(E,M) \
	(!((E)->flags & IPCF_ಹೊಂದಿಕೆ_M_ಮೂಲ) || \
	(E)->m_ಮೂಲ == (M)->m_ಮೂಲ || \
	IPCF_EL_ಹೊಂದಿಕೆ_M_ಮೂಲ_ANY_EP((E)->m_ಮೂಲ,(M)->m_ಮೂಲ))
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_EL_ಹೊಂದಿಕೆ_M_ಮೂಲ_ANY_EP(ES,MS) \
	(((ES) == ANY_USR && IPCF_IS_USR_EP(MS)) || \
	((ES) == ANY_SYS && IPCF_IS_SYS_EP(MS)) || \
	((ES) == ANY_TSK && IPCF_IS_TSK_EP(MS)))
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_EL_ಹೊಂದಿಕೆ(E,M) \
	(IPCF_EL_ಹೊಂದಿಕೆ_M_TYPE(E,M) && IPCF_EL_ಹೊಂದಿಕೆ_M_ಮೂಲ(E,M))

ರಚನೆ ipc_filter_s {
  ಪೂರ್ಣಾಂಕ type;
  ಪೂರ್ಣಾಂಕ ಅಂಕೆ_elements;
  ಪೂರ್ಣಾಂಕ flags;
  ರಚನೆ ipc_filter_s *ಮುಂದಿನ;
  ipc_filter_el_t elements[IPCF_MAX_ELEMENTS];
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ipc_filter_s ipc_filter_t;

/* IPC filter pool. */
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_POOL_ಗಾತ್ರ          (2*NR_SYS_PROCS)
EXTERN ipc_filter_t ipc_filter_pool[IPCF_POOL_ಗಾತ್ರ];

/* IPC filter pool macros. */
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_POOL_FREE_ಸ್ಥಾನ(S) ((S)->type = IPCF_NONE)
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_POOL_IS_FREE_ಸ್ಥಾನ(S) ((S)->type == IPCF_NONE)
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_POOL_ALLOCATE_ಸ್ಥಾನ(T,S) \
	ಮಾಡು { \
		ಪೂರ್ಣಾಂಕ i; \
		*(S) = ಖಾಲಿ; \
		ಗಾಗಿ (i = 0; i < IPCF_POOL_ಗಾತ್ರ; i++) { \
			ಆದರೆ (IPCF_POOL_IS_FREE_ಸ್ಥಾನ(&ipc_filter_pool[i])) { \
				*(S) = &ipc_filter_pool[i]; \
				(*(S))->type = T; \
				ನಿಲ್ಲಿಸು; \
			} \
		} \
	} ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು IPCF_POOL_INIT(S) memset(&ipc_filter_pool,0,ಗಾತ್ರ(ipc_filter_pool))

#ಆದರೆ_ಅಂತ್ಯ /* !IPC_FILTER_H */
