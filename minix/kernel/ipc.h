#यदि_अपरिभाषितम् IPC_H
#परिभाषय IPC_H

/* This header file defines constants for MINIX inter-process communication.
 * These definitions are used in the file proc.c.
 */
#अन्तर्भाव <minix/com.h>
#अन्तर्भाव <minix/ipcconst.h>

/* Masks and flags for system calls. */
#परिभाषय NON_BLOCKING    0x0080  /* do not block if target not ready */
#परिभाषय FROM_KERNEL     0x0100  /* message from kernel on behalf of a process */

#परिभाषय WILLRECEIVE(src_e,dst_ptr,m_src_v,m_src_p) \
	((RTS_ISSET(dst_ptr, RTS_RECEIVING) && \
	!RTS_ISSET(dst_ptr, RTS_SENDING)) && \
	CANRECEIVE(dst_ptr->p_getfrom_e,src_e,dst_ptr,m_src_v,m_src_p))

#परिभाषय CANRECEIVE(receive_e,src_e,dst_ptr,m_src_v,m_src_p) \
	(((receive_e) == ANY || (receive_e) == (src_e)) && \
	(priv(dst_ptr)->s_ipcf == रिक्त || \
	allow_ipc_filtered_सन्देशः(dst_ptr,src_e,m_src_v,m_src_p)))

/* IPC status code macros. */
#परिभाषय IPC_स्थितिः_GET(p)	((p)->p_reg.IPC_स्थितिः_REG)
#परिभाषय IPC_स्थितिः_CLEAR(p)	((p)->p_reg.IPC_स्थितिः_REG = 0)

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
#परिभाषय IPC_स्थितिः_सङ्कलय(p, m)	कुरु { \
        यदि(!((p)->p_misc_flags & MF_प्रत्युत्तरम्_PEND)) { \
            (p)->p_reg.IPC_स्थितिः_REG |= (m); \
        } \
    } यावत्(0)
#परिभाषय IPC_स्थितिः_सङ्कलय_CALL(p, call) \
    IPC_स्थितिः_सङ्कलय(p, IPC_स्थितिः_CALL_गन्तव्यम्(call))
#परिभाषय IPC_स्थितिः_सङ्कलय_FLAGS(p, flags) \
    IPC_स्थितिः_सङ्कलय(p, IPC_स्थितिः_FLAGS(flags))

#यदि_अन्तः /* IPC_H */
