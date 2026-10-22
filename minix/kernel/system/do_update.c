/* The kernel call implemented in this file:
 *   m_type:	SYS_UPDATE
 *
 * The parameters for this kernel call are:
 *    m2_i1:	SYS_UPD_SRC_ENDPT 	(source process endpoint)
 *    m2_i2:	SYS_UPD_DST_ENDPT	(destination process endpoint)
 *    m2_i3:	SYS_UPD_FLAGS		(update flags)
 */

#شامل "kernel/system.h"
#شامل <string.h>
#شامل <assert.h>

#اگر USE_UPDATE

#تعریف_کرو DEBUG 0

#تعریف_کرو proc_is_updatable(p) \
    (RTS_ISSET(p, RTS_NO_PRIV) || RTS_ISSET(p, RTS_SIG_PENDING) \
    || (RTS_ISSET(p, RTS_RECEIVING) && !RTS_ISSET(p, RTS_SENDING)))

ساکن صحیح inherit_priv_irq(ساخت proc *src_rp, ساخت proc *dst_rp);
ساکن صحیح inherit_priv_io(ساخت proc *src_rp, ساخت proc *dst_rp);
ساکن صحیح inherit_priv_mem(ساخت proc *src_rp, ساخت proc *dst_rp);
ساکن خالی abort_proc_ipc_send(ساخت proc *rp);
ساکن خالی adjust_proc_خانہ(ساخت proc *rp, ساخت proc *from_rp);
ساکن خالی adjust_priv_خانہ(ساخت priv *privp, ساخت priv
	*from_privp);
ساکن خالی adjust_asyn_جدول(ساخت priv *src_privp, ساخت priv *dst_privp);
ساکن خالی تبادلہ_proc_خانہ_اشارہ_گر(ساخت proc **rpp, ساخت proc
	*src_rp, ساخت proc *dst_rp);
ساکن خالی تبادلہ_memreq(ساخت proc *src_rp, ساخت proc *dst_rp);

/*===========================================================================*
 *				do_update				     *
 *===========================================================================*/
صحیح do_update(ساخت proc * caller, پیغام * m_ptr)
{
/* Handle sys_update(). Update a process into another by swapping their process
 * slots.
 */
  endpoint_t src_e, dst_e;
  صحیح src_p, dst_p, flags;
  ساخت proc *src_rp, *dst_rp;
  ساخت priv *src_privp, *dst_privp;
  ساخت proc orig_src_proc;
  ساخت proc orig_dst_proc;
  ساخت priv orig_src_priv;
  ساخت priv orig_dst_priv;
  صحیح i, r;

  /* Lookup slots for source and destination process. */
  flags = m_ptr->SYS_UPD_FLAGS;
  src_e = m_ptr->SYS_UPD_SRC_ENDPT;
  اگر(!isokendpt(src_e, &src_p)) {
      واپس EINVAL;
  }
  src_rp = proc_addr(src_p);
  src_privp = priv(src_rp);
  اگر(!(src_privp->s_flags & SYS_PROC)) {
      واپس EPERM;
  }

  dst_e = m_ptr->SYS_UPD_DST_ENDPT;
  اگر(!isokendpt(dst_e, &dst_p)) {
      واپس EINVAL;
  }
  dst_rp = proc_addr(dst_p);
  dst_privp = priv(dst_rp);
  اگر(!(dst_privp->s_flags & SYS_PROC)) {
      واپس EPERM;
  }

  أكد(!proc_is_runnable(src_rp) && !proc_is_runnable(dst_rp));

  /* Check if processes are updatable. */
  اگر(!proc_is_updatable(src_rp) || !proc_is_updatable(dst_rp)) {
      واپس EBUSY;
  }

#اگر DEBUG
  لکھو("do_update: updating %d (%s, %d, %d) into %d (%s, %d, %d)\n",
      src_rp->p_endpoint, src_rp->p_name, src_rp->p_nr, priv(src_rp)->s_proc_nr,
      dst_rp->p_endpoint, dst_rp->p_name, dst_rp->p_nr, priv(dst_rp)->s_proc_nr);

  proc_stacktrace(src_rp);
  proc_stacktrace(dst_rp);
  لکھو("do_update: curr ptproc %d\n", get_cpulocal_var(ptproc)->p_endpoint);
  لکھو("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", src_rp->p_endpoint, src_rp->p_rts_flags, priv(src_rp)->s_asyntab, priv(src_rp)->s_asynendpoint, priv(src_rp)->s_grant_جدول, priv(src_rp)->s_grant_endpoint);
  لکھو("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", dst_rp->p_endpoint, dst_rp->p_rts_flags, priv(dst_rp)->s_asyntab, priv(dst_rp)->s_asynendpoint, priv(dst_rp)->s_grant_جدول, priv(dst_rp)->s_grant_endpoint);
#اگر_ختم

  /* Let destination inherit allowed IRQ, I/O ranges, and memory ranges. */
  r = inherit_priv_irq(src_rp, dst_rp);
  اگر(r != OK) {
      واپس r;
  }
  r = inherit_priv_io(src_rp, dst_rp);
  اگر(r != OK) {
      واپس r;
  }
  r = inherit_priv_mem(src_rp, dst_rp);
  اگر(r != OK) {
      واپس r;
  }

  /* Let destination inherit the target mask from source. */
  برائے (i=0; i < NR_SYS_PROCS; i++) {
      اگر (get_sys_ثنائی_ہندسہ(priv(src_rp)->s_ipc_مقصد, i)) {
          سیٹ_sendto_ثنائی_ہندسہ(dst_rp, i);
      }
  }

  /* Save existing data. */
  orig_src_proc = *src_rp;
  orig_src_priv = *(priv(src_rp));
  orig_dst_proc = *dst_rp;
  orig_dst_priv = *(priv(dst_rp));

  /* Adjust asyn tables. */
  adjust_asyn_جدول(priv(src_rp), priv(dst_rp));
  adjust_asyn_جدول(priv(dst_rp), priv(src_rp));

  /* Abort any pending send() on rollback. */
  اگر(flags & SYS_UPD_ROLLBACK) {
      abort_proc_ipc_send(src_rp);
  }

  /* Swap slots. */
  *src_rp = orig_dst_proc;
  *src_privp = orig_dst_priv;
  *dst_rp = orig_src_proc;
  *dst_privp = orig_src_priv;

  /* Adjust process slots. */
  adjust_proc_خانہ(src_rp, &orig_src_proc);
  adjust_proc_خانہ(dst_rp, &orig_dst_proc);

  /* Adjust privilege slots. */
  adjust_priv_خانہ(priv(src_rp), &orig_src_priv);
  adjust_priv_خانہ(priv(dst_rp), &orig_dst_priv);

  /* Swap global process slot addresses. */
  تبادلہ_proc_خانہ_اشارہ_گر(get_cpulocal_var_ptr(ptproc), src_rp, dst_rp);

  /* Swap VM request entries. */
  تبادلہ_memreq(src_rp, dst_rp);

#اگر DEBUG
  لکھو("do_update: updated %d (%s, %d, %d) into %d (%s, %d, %d)\n",
      src_rp->p_endpoint, src_rp->p_name, src_rp->p_nr, priv(src_rp)->s_proc_nr,
      dst_rp->p_endpoint, dst_rp->p_name, dst_rp->p_nr, priv(dst_rp)->s_proc_nr);

  proc_stacktrace(src_rp);
  proc_stacktrace(dst_rp);
  لکھو("do_update: curr ptproc %d\n", get_cpulocal_var(ptproc)->p_endpoint);
  لکھو("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", src_rp->p_endpoint, src_rp->p_rts_flags, priv(src_rp)->s_asyntab, priv(src_rp)->s_asynendpoint, priv(src_rp)->s_grant_جدول, priv(src_rp)->s_grant_endpoint);
  لکھو("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", dst_rp->p_endpoint, dst_rp->p_rts_flags, priv(dst_rp)->s_asyntab, priv(dst_rp)->s_asynendpoint, priv(dst_rp)->s_grant_جدول, priv(dst_rp)->s_grant_endpoint);
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_SMP
  بٹ_fill(src_rp->p_stale_tlb, CONFIG_MAX_CPUS);
  بٹ_fill(dst_rp->p_stale_tlb, CONFIG_MAX_CPUS);
#اگر_ختم

  واپس OK;
}

/*===========================================================================*
 *			     inherit_priv_irq 				     *
 *===========================================================================*/
صحیح inherit_priv_irq(ساخت proc *src_rp, ساخت proc *dst_rp)
{
  صحیح i, r;
  برائے (i= 0; i<priv(src_rp)->s_nr_irq; i++) {
      r = priv_جوڑو_irq(dst_rp, priv(src_rp)->s_irq_tab[i]); 
      اگر(r != OK) {
          واپس r;
      }
  }

  واپس OK;
}

/*===========================================================================*
 *			     inherit_priv_io 				     *
 *===========================================================================*/
 صحیح inherit_priv_io(ساخت proc *src_rp, ساخت proc *dst_rp)
{
  صحیح i, r;
  برائے (i= 0; i<priv(src_rp)->s_nr_io_range; i++) {
      r = priv_جوڑو_io(dst_rp, &(priv(src_rp)->s_io_tab[i])); 
      اگر(r != OK) {
          واپس r;
      }
  }

  واپس OK;
}

/*===========================================================================*
 *			     inherit_priv_mem 				     *
 *===========================================================================*/
صحیح inherit_priv_mem(ساخت proc *src_rp, ساخت proc *dst_rp)
{
  صحیح i, r;
  برائے (i= 0; i<priv(src_rp)->s_nr_mem_range; i++) {
      r = priv_جوڑو_mem(dst_rp, &(priv(src_rp)->s_mem_tab[i])); 
      اگر(r != OK) {
          واپس r;
      }
  }

  واپس OK;
}

/*===========================================================================*
 *			    abort_proc_ipc_send				     *
 *===========================================================================*/
خالی abort_proc_ipc_send(ساخت proc *rp)
{
  اگر(RTS_ISSET(rp, RTS_SENDING)) {
      ساخت proc **xpp;
      RTS_UNSET(rp, RTS_SENDING);
      rp->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
      xpp = &(proc_addr(_ENDPOINT_P(rp->p_sendto_e))->p_caller_q);
      جب (*xpp) {
          اگر(*xpp == rp) {
              *xpp = rp->p_q_link;
              rp->p_q_link = تہی;
              توڑو;
          }
          xpp = &(*xpp)->p_q_link;
      }
  }
}

/*===========================================================================*
 *			     adjust_proc_slot				     *
 *===========================================================================*/
ساکن خالی adjust_proc_خانہ(ساخت proc *rp, ساخت proc *from_rp)
{
  /* Preserve endpoints, slot numbers, priv structure, and IPC. */
  rp->p_endpoint = from_rp->p_endpoint;
  rp->p_nr = from_rp->p_nr;
  rp->p_priv = from_rp->p_priv;
  priv(rp)->s_proc_nr = from_rp->p_nr;

  rp->p_caller_q = from_rp->p_caller_q;

  /* preserve scheduling */
  rp->p_scheduler = from_rp->p_scheduler;
#اگر_تعریف_شدہ CONFIG_SMP
  rp->p_cpu = from_rp->p_cpu;
  حافظہ_نقل(rp->p_cpu_نقاب, from_rp->p_cpu_نقاب,
		  حجم(bitchunk_t) * BITMAP_CHUNKS(CONFIG_MAX_CPUS));
#اگر_ختم
}

/*===========================================================================*
 *			     adjust_asyn_table				     *
 *===========================================================================*/
ساکن خالی adjust_asyn_جدول(ساخت priv *src_privp, ساخت priv *dst_privp)
{
  /* Transfer the asyn table if source's table belongs to the destination. */
  endpoint_t src_e = proc_addr(src_privp->s_proc_nr)->p_endpoint;
  endpoint_t dst_e = proc_addr(dst_privp->s_proc_nr)->p_endpoint;

  اگر(src_privp->s_asynsize > 0 && dst_privp->s_asynsize > 0 && src_privp->s_asynendpoint == dst_e) {
      اگر(ڈیٹا_copy(src_e, src_privp->s_asyntab, dst_e, dst_privp->s_asyntab,
	  src_privp->s_asynsize*حجم(asynmsg_t)) != OK) {
	  لکھو("Warning: unable to transfer asyn table from ep %d to ep %d\n",
	      src_e, dst_e);
      }
      ورنہ {
          dst_privp->s_asynsize = src_privp->s_asynsize;
      }
  }
}

/*===========================================================================*
 *			     adjust_priv_slot				     *
 *===========================================================================*/
ساکن خالی adjust_priv_خانہ(ساخت priv *privp, ساخت priv *from_privp)
{
  /* Preserve privilege ids and non-privilege stuff in the priv structure. */
  privp->s_id = from_privp->s_id;
  privp->s_asyn_pending = from_privp->s_asyn_pending;
  privp->s_notify_pending = from_privp->s_notify_pending;
  privp->s_int_pending = from_privp->s_int_pending;
  privp->s_sig_pending = from_privp->s_sig_pending;
  privp->s_alarm_timer = from_privp->s_alarm_timer;
  privp->s_diag_sig = from_privp->s_diag_sig;
}

/*===========================================================================*
 *			   swap_proc_slot_pointer			     *
 *===========================================================================*/
ساکن خالی تبادلہ_proc_خانہ_اشارہ_گر(ساخت proc **rpp, ساخت proc *src_rp,
    ساخت proc *dst_rp)
{
  اگر(*rpp == src_rp) {
      *rpp = dst_rp;
  }
  ورنہ اگر(*rpp == dst_rp) {
      *rpp = src_rp;
  }
}

/*===========================================================================*
 *				swap_memreq				     *
 *===========================================================================*/
ساکن خالی تبادلہ_memreq(ساخت proc *src_rp, ساخت proc *dst_rp)
{
/* If either the source or the destination process is part of the VM request
 * chain, but not both, then swap the process pointers in the chain.
 */
  ساخت proc **rpp;

  اگر (RTS_ISSET(src_rp, RTS_VMREQUEST) == RTS_ISSET(dst_rp, RTS_VMREQUEST))
	واپس; /* nothing to do */

  برائے (rpp = &vmrequest; *rpp != تہی;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	اگر (*rpp == src_rp) {
		dst_rp->p_vmrequest.nextrequestor =
		    src_rp->p_vmrequest.nextrequestor;
		*rpp = dst_rp;
		توڑو;
	} ورنہ اگر (*rpp == dst_rp) {
		src_rp->p_vmrequest.nextrequestor =
		    dst_rp->p_vmrequest.nextrequestor;
		*rpp = src_rp;
		توڑو;
	}
  }
}

#اگر_ختم /* USE_UPDATE */

