#अगर_अघोषित IPC_H
#घोषणा IPC_H

/* This header file defines constants for MINIX inter-process communication.
 * These definitions are used in the file proc.c.
 */
#समावेश <minix/com.h>
#समावेश <minix/ipcconst.h>

/* Masks and flags for system calls. */
#घोषणा NON_BLOCKING    0x0080  /* do not block if target not ready */
#घोषणा FROM_KERNEL     0x0100  /* message from kernel on behalf of a process */

#घोषणा WILLRECEIVE(src_e,dst_ptr,m_src_v,m_src_p) \
	((RTS_ISSET(dst_ptr, RTS_RECEIVING) && \
	!RTS_ISSET(dst_ptr, RTS_SENDING)) && \
	CANRECEIVE(dst_ptr->p_getfrom_e,src_e,dst_ptr,m_src_v,m_src_p))

#घोषणा CANRECEIVE(receive_e,src_e,dst_ptr,m_src_v,m_src_p) \
	(((receive_e) == ANY || (receive_e) == (src_e)) && \
	(priv(dst_ptr)->s_ipcf == शून्य || \
	allow_ipc_filtered_संदेश(dst_ptr,src_e,m_src_v,m_src_p)))

/* IPC status code macros. */
#घोषणा IPC_STATUS_GET(p)	((p)->p_reg.IPC_STATUS_REG)
#घोषणा IPC_STATUS_CLEAR(p)	((p)->p_reg.IPC_STATUS_REG = 0)

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
#घोषणा IPC_STATUS_ADD(p, m)	करो { \
        अगर(!((p)->p_misc_flags & MF_REPLY_PEND)) { \
            (p)->p_reg.IPC_STATUS_REG |= (m); \
        } \
    } जबतक(0)
#घोषणा IPC_STATUS_ADD_CALL(p, call) \
    IPC_STATUS_ADD(p, IPC_STATUS_CALL_TO(call))
#घोषणा IPC_STATUS_ADD_FLAGS(p, flags) \
    IPC_STATUS_ADD(p, IPC_STATUS_FLAGS(flags))

#पूर्ण_अगर /* IPC_H */
