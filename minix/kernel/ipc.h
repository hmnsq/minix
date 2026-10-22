#اگر_غیر_تعریف_شدہ IPC_H
#تعریف_کرو IPC_H

/* This header file defines constants for MINIX inter-process communication.
 * These definitions are used in the file proc.c.
 */
#شامل <minix/com.h>
#شامل <minix/ipcconst.h>

/* Masks and flags for system calls. */
#تعریف_کرو NON_BLOCKING    0x0080  /* do not block if target not ready */
#تعریف_کرو FROM_KERNEL     0x0100  /* message from kernel on behalf of a process */

#تعریف_کرو WILLRECEIVE(src_e,dst_ptr,m_src_v,m_src_p) \
	((RTS_ISSET(dst_ptr, RTS_RECEIVING) && \
	!RTS_ISSET(dst_ptr, RTS_SENDING)) && \
	CANRECEIVE(dst_ptr->p_getfrom_e,src_e,dst_ptr,m_src_v,m_src_p))

#تعریف_کرو CANRECEIVE(receive_e,src_e,dst_ptr,m_src_v,m_src_p) \
	(((receive_e) == ANY || (receive_e) == (src_e)) && \
	(priv(dst_ptr)->s_ipcf == تہی || \
	allow_ipc_filtered_پیغام(dst_ptr,src_e,m_src_v,m_src_p)))

/* IPC status code macros. */
#تعریف_کرو IPC_حالت_GET(p)	((p)->p_reg.IPC_حالت_REG)
#تعریف_کرو IPC_حالت_CLEAR(p)	((p)->p_reg.IPC_حالت_REG = 0)

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
#تعریف_کرو IPC_حالت_جوڑو(p, m)	کرو { \
        اگر(!((p)->p_misc_flags & MF_جواب_PEND)) { \
            (p)->p_reg.IPC_حالت_REG |= (m); \
        } \
    } جب(0)
#تعریف_کرو IPC_حالت_جوڑو_CALL(p, call) \
    IPC_حالت_جوڑو(p, IPC_حالت_CALL_مقصد(call))
#تعریف_کرو IPC_حالت_جوڑو_FLAGS(p, flags) \
    IPC_حالت_جوڑو(p, IPC_حالت_FLAGS(flags))

#اگر_ختم /* IPC_H */
