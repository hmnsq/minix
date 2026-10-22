#నిర్వచితం_కాకపోతే IPC_H
#నిర్వచించు IPC_H

/* This header file defines constants for MINIX inter-process communication.
 * These definitions are used in the file proc.c.
 */
#అన్తర్భావ <minix/com.h>
#అన్తర్భావ <minix/ipcconst.h>

/* Masks and flags for system calls. */
#నిర్వచించు NON_BLOCKING    0x0080  /* do not block if target not ready */
#నిర్వచించు FROM_KERNEL     0x0100  /* message from kernel on behalf of a process */

#నిర్వచించు WILLRECEIVE(src_e,dst_ptr,m_src_v,m_src_p) \
	((RTS_ISSET(dst_ptr, RTS_RECEIVING) && \
	!RTS_ISSET(dst_ptr, RTS_SENDING)) && \
	CANRECEIVE(dst_ptr->p_getfrom_e,src_e,dst_ptr,m_src_v,m_src_p))

#నిర్వచించు CANRECEIVE(receive_e,src_e,dst_ptr,m_src_v,m_src_p) \
	(((receive_e) == ANY || (receive_e) == (src_e)) && \
	(priv(dst_ptr)->s_ipcf == ఖాళీ || \
	allow_ipc_filtered_సందేశం(dst_ptr,src_e,m_src_v,m_src_p)))

/* IPC status code macros. */
#నిర్వచించు IPC_స్థితి_GET(p)	((p)->p_reg.IPC_స్థితి_REG)
#నిర్వచించు IPC_స్థితి_CLEAR(p)	((p)->p_reg.IPC_స్థితి_REG = 0)

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
#నిర్వచించు IPC_స్థితి_కూడు(p, m)	చేయి { \
        ఒకవేళ(!((p)->p_misc_flags & MF_జవాబు_PEND)) { \
            (p)->p_reg.IPC_స్థితి_REG |= (m); \
        } \
    } అయితే(0)
#నిర్వచించు IPC_స్థితి_కూడు_CALL(p, call) \
    IPC_స్థితి_కూడు(p, IPC_స్థితి_CALL_గమ్యం(call))
#నిర్వచించు IPC_స్థితి_కూడు_FLAGS(p, flags) \
    IPC_స్థితి_కూడు(p, IPC_స్థితి_FLAGS(flags))

#అయితే_ముగింపు /* IPC_H */
