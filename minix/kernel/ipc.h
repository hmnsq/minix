#اگر_تعریف_نشده IPC_H
#تعریف_کن IPC_H

/* This header file defines constants for MINIX inter-process communication.
 * These definitions are used in the file proc.c.
 */
#ضمن <minix/com.h>
#ضمن <minix/ipcconst.h>

/* Masks and flags for system calls. */
#تعریف_کن NON_BLOCKING    0x0080  /* do not block if target not ready */
#تعریف_کن FROM_KERNEL     0x0100  /* message from kernel on behalf of a process */

#تعریف_کن WILLRECEIVE(src_e,dst_ptr,m_src_v,m_src_p) \
	((RTS_ISSET(dst_ptr, RTS_RECEIVING) && \
	!RTS_ISSET(dst_ptr, RTS_SENDING)) && \
	CANRECEIVE(dst_ptr->p_getfrom_e,src_e,dst_ptr,m_src_v,m_src_p))

#تعریف_کن CANRECEIVE(receive_e,src_e,dst_ptr,m_src_v,m_src_p) \
	(((receive_e) == ANY || (receive_e) == (src_e)) && \
	(priv(dst_ptr)->s_ipcf == عدم || \
	allow_ipc_filtered_پیام(dst_ptr,src_e,m_src_v,m_src_p)))

/* IPC status code macros. */
#تعریف_کن IPC_وضعیت_GET(p)	((p)->p_reg.IPC_وضعیت_REG)
#تعریف_کن IPC_وضعیت_CLEAR(p)	((p)->p_reg.IPC_وضعیت_REG = 0)

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
#تعریف_کن IPC_وضعیت_جمع_کن(p, m)	افعل { \
        إذا(!((p)->p_misc_flags & MF_پاسخ_PEND)) { \
            (p)->p_reg.IPC_وضعیت_REG |= (m); \
        } \
    } طالما(0)
#تعریف_کن IPC_وضعیت_جمع_کن_CALL(p, call) \
    IPC_وضعیت_جمع_کن(p, IPC_وضعیت_CALL_مقصد(call))
#تعریف_کن IPC_وضعیت_جمع_کن_FLAGS(p, flags) \
    IPC_وضعیت_جمع_کن(p, IPC_وضعیت_FLAGS(flags))

#پایان_اگر /* IPC_H */
