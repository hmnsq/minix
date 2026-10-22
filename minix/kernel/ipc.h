#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IPC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPC_H

/* This header file defines constants for MINIX inter-process communication.
 * These definitions are used in the file proc.c.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/com.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/ipcconst.h>

/* Masks and flags for system calls. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NON_BLOCKING    0x0080  /* do not block if target not ready */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FROM_KERNEL     0x0100  /* message from kernel on behalf of a process */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILLRECEIVE(src_e,dst_ptr,m_src_v,m_src_p) \
	((RTS_ISSET(dst_ptr, RTS_RECEIVING) && \
	!RTS_ISSET(dst_ptr, RTS_SENDING)) && \
	CANRECEIVE(dst_ptr->p_getfrom_e,src_e,dst_ptr,m_src_v,m_src_p))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CANRECEIVE(receive_e,src_e,dst_ptr,m_src_v,m_src_p) \
	(((receive_e) == ANY || (receive_e) == (src_e)) && \
	(priv(dst_ptr)->s_ipcf == ਨਿਰਾ || \
	allow_ipc_filtered_ਸੁਨੇਹਾ(dst_ptr,src_e,m_src_v,m_src_p)))

/* IPC status code macros. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPC_ਸਥਿਤੀ_GET(p)	((p)->p_reg.IPC_ਸਥਿਤੀ_REG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPC_ਸਥਿਤੀ_CLEAR(p)	((p)->p_reg.IPC_ਸਥਿਤੀ_REG = 0)

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPC_ਸਥਿਤੀ_ਜੋੜੋ(p, m)	ਕਰੋ { \
        ਜੇ(!((p)->p_misc_flags & MF_ਜਵਾਬ_PEND)) { \
            (p)->p_reg.IPC_ਸਥਿਤੀ_REG |= (m); \
        } \
    } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPC_ਸਥਿਤੀ_ਜੋੜੋ_CALL(p, call) \
    IPC_ਸਥਿਤੀ_ਜੋੜੋ(p, IPC_ਸਥਿਤੀ_CALL_ਮੰਜ਼ਿਲ(call))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPC_ਸਥਿਤੀ_ਜੋੜੋ_FLAGS(p, flags) \
    IPC_ਸਥਿਤੀ_ਜੋੜੋ(p, IPC_ਸਥਿਤੀ_FLAGS(flags))

#ਜੇ_ਅੰਤ /* IPC_H */
