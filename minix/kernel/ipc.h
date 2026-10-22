#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IPC_H
#ವ್ಯಾಖ್ಯಾನಿಸು IPC_H

/* This header file defines constants for MINIX inter-process communication.
 * These definitions are used in the file proc.c.
 */
#ಅನ್ತರ್ಭಾವ <minix/com.h>
#ಅನ್ತರ್ಭಾವ <minix/ipcconst.h>

/* Masks and flags for system calls. */
#ವ್ಯಾಖ್ಯಾನಿಸು NON_BLOCKING    0x0080  /* do not block if target not ready */
#ವ್ಯಾಖ್ಯಾನಿಸು FROM_KERNEL     0x0100  /* message from kernel on behalf of a process */

#ವ್ಯಾಖ್ಯಾನಿಸು WILLRECEIVE(src_e,dst_ptr,m_src_v,m_src_p) \
	((RTS_ISSET(dst_ptr, RTS_RECEIVING) && \
	!RTS_ISSET(dst_ptr, RTS_SENDING)) && \
	CANRECEIVE(dst_ptr->p_getfrom_e,src_e,dst_ptr,m_src_v,m_src_p))

#ವ್ಯಾಖ್ಯಾನಿಸು CANRECEIVE(receive_e,src_e,dst_ptr,m_src_v,m_src_p) \
	(((receive_e) == ANY || (receive_e) == (src_e)) && \
	(priv(dst_ptr)->s_ipcf == ಖಾಲಿ || \
	allow_ipc_filtered_ಸಂದೇಶ(dst_ptr,src_e,m_src_v,m_src_p)))

/* IPC status code macros. */
#ವ್ಯಾಖ್ಯಾನಿಸು IPC_ಸ್ಥಿತಿ_GET(p)	((p)->p_reg.IPC_ಸ್ಥಿತಿ_REG)
#ವ್ಯಾಖ್ಯಾನಿಸು IPC_ಸ್ಥಿತಿ_CLEAR(p)	((p)->p_reg.IPC_ಸ್ಥಿತಿ_REG = 0)

/*
 * XXX: the following check is used to set the status code only on RECEIVE.
 * SENDREC is not currently atomic for user processes. A process can return
 * from SENDREC in a different context than the original when a Posix signal
 * handler gets executed. For this reason, it is not safe to manipulate
 * the context (i.e. registers) when a process is blocked on a SENDREC.
 * Unfortunately, avoiding setting the status code for SENDREC doesn't solve
 * the problem entirely because in rare situations it is still necessary to
 * override retreg dynamically (and possibly in a different context).
 * A possible reliable solution is to improve our Posix signal handling
 * implementation and guarantee SENDREC atomicity w.r.t. the process context.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು(p, m)	ಮಾಡು { \
        ಆದರೆ(!((p)->p_misc_flags & MF_ಉತ್ತರ_PEND)) { \
            (p)->p_reg.IPC_ಸ್ಥಿತಿ_REG |= (m); \
        } \
    } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು_CALL(p, call) \
    IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು(p, IPC_ಸ್ಥಿತಿ_CALL_ಗಮ್ಯ(call))
#ವ್ಯಾಖ್ಯಾನಿಸು IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು_FLAGS(p, flags) \
    IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು(p, IPC_ಸ್ಥಿತಿ_FLAGS(flags))

#ಆದರೆ_ಅಂತ್ಯ /* IPC_H */
