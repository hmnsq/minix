/* The kernel call implemented in this file:
 *   m_type:	SYS_UPDATE
 *
 * The parameters for this kernel call are:
 *    m2_i1:	SYS_UPD_SRC_ENDPT 	(source process endpoint)
 *    m2_i2:	SYS_UPD_DST_ENDPT	(destination process endpoint)
 *    m2_i3:	SYS_UPD_FLAGS		(update flags)
 */

#ضمن "kernel/system.h"
#ضمن <string.h>
#ضمن <assert.h>

#إذا USE_UPDATE

#وصف_ڪريو DEBUG 0

#وصف_ڪريو proc_is_updatable(p) \
    (RTS_ISSET(p, RTS_NO_PRIV) || RTS_ISSET(p, RTS_SIG_PENDING) \
    || (RTS_ISSET(p, RTS_RECEIVING) && !RTS_ISSET(p, RTS_SENDING)))

ساكن صحيح inherit_priv_irq(بنية proc *src_rp, بنية proc *dst_rp);
ساكن صحيح inherit_priv_io(بنية proc *src_rp, بنية proc *dst_rp);
ساكن صحيح inherit_priv_mem(بنية proc *src_rp, بنية proc *dst_rp);
ساكن فراغ abort_proc_ipc_send(بنية proc *rp);
ساكن فراغ adjust_proc_جڳهه(بنية proc *rp, بنية proc *from_rp);
ساكن فراغ adjust_priv_جڳهه(بنية priv *privp, بنية priv
	*from_privp);
ساكن فراغ adjust_asyn_جدول(بنية priv *src_privp, بنية priv *dst_privp);
ساكن فراغ مٽا_سٽا_proc_جڳهه_اشارو(بنية proc **rpp, بنية proc
	*src_rp, بنية proc *dst_rp);
ساكن فراغ مٽا_سٽا_memreq(بنية proc *src_rp, بنية proc *dst_rp);

/*===========================================================================*
 *				do_update				     *
 *===========================================================================*/
صحيح do_update(بنية proc * caller, پيغام * m_ptr)
{
/* Handle sys_update(). Update a process into another by swapping their process
 * slots.
 */
  endpoint_t src_e, dst_e;
  صحيح src_p, dst_p, flags;
  بنية proc *src_rp, *dst_rp;
  بنية priv *src_privp, *dst_privp;
  بنية proc orig_src_proc;
  بنية proc orig_dst_proc;
  بنية priv orig_src_priv;
  بنية priv orig_dst_priv;
  صحيح i, r;

  /* Lookup slots for source and destination process. */
  flags = m_ptr->SYS_UPD_FLAGS;
  src_e = m_ptr->SYS_UPD_SRC_ENDPT;
  إذا(!isokendpt(src_e, &src_p)) {
      أرجع EINVAL;
  }
  src_rp = proc_addr(src_p);
  src_privp = priv(src_rp);
  إذا(!(src_privp->s_flags & SYS_PROC)) {
      أرجع EPERM;
  }

  dst_e = m_ptr->SYS_UPD_DST_ENDPT;
  إذا(!isokendpt(dst_e, &dst_p)) {
      أرجع EINVAL;
  }
  dst_rp = proc_addr(dst_p);
  dst_privp = priv(dst_rp);
  إذا(!(dst_privp->s_flags & SYS_PROC)) {
      أرجع EPERM;
  }

  أكد(!proc_is_runnable(src_rp) && !proc_is_runnable(dst_rp));

  /* Check if processes are updatable. */
  إذا(!proc_is_updatable(src_rp) || !proc_is_updatable(dst_rp)) {
      أرجع EBUSY;
  }

#إذا DEBUG
  لکو("do_update: updating %d (%s, %d, %d) into %d (%s, %d, %d)\n",
      src_rp->p_endpoint, src_rp->p_name, src_rp->p_nr, priv(src_rp)->s_proc_nr,
      dst_rp->p_endpoint, dst_rp->p_name, dst_rp->p_nr, priv(dst_rp)->s_proc_nr);

  proc_stacktrace(src_rp);
  proc_stacktrace(dst_rp);
  لکو("do_update: curr ptproc %d\n", get_cpulocal_var(ptproc)->p_endpoint);
  لکو("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", src_rp->p_endpoint, src_rp->p_rts_flags, priv(src_rp)->s_asyntab, priv(src_rp)->s_asynendpoint, priv(src_rp)->s_grant_جدول, priv(src_rp)->s_grant_endpoint);
  لکو("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", dst_rp->p_endpoint, dst_rp->p_rts_flags, priv(dst_rp)->s_asyntab, priv(dst_rp)->s_asynendpoint, priv(dst_rp)->s_grant_جدول, priv(dst_rp)->s_grant_endpoint);
#جيڪڏهن_پورو

  /* Let destination inherit allowed IRQ, I/O ranges, and memory ranges. */
  r = inherit_priv_irq(src_rp, dst_rp);
  إذا(r != OK) {
      أرجع r;
  }
  r = inherit_priv_io(src_rp, dst_rp);
  إذا(r != OK) {
      أرجع r;
  }
  r = inherit_priv_mem(src_rp, dst_rp);
  إذا(r != OK) {
      أرجع r;
  }

  /* Let destination inherit the target mask from source. */
  لكل (i=0; i < NR_SYS_PROCS; i++) {
      إذا (get_sys_ٻٽو_انگ(priv(src_rp)->s_ipc_منزل, i)) {
          سيٽ_sendto_ٻٽو_انگ(dst_rp, i);
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
  إذا(flags & SYS_UPD_ROLLBACK) {
      abort_proc_ipc_send(src_rp);
  }

  /* Swap slots. */
  *src_rp = orig_dst_proc;
  *src_privp = orig_dst_priv;
  *dst_rp = orig_src_proc;
  *dst_privp = orig_src_priv;

  /* Adjust process slots. */
  adjust_proc_جڳهه(src_rp, &orig_src_proc);
  adjust_proc_جڳهه(dst_rp, &orig_dst_proc);

  /* Adjust privilege slots. */
  adjust_priv_جڳهه(priv(src_rp), &orig_src_priv);
  adjust_priv_جڳهه(priv(dst_rp), &orig_dst_priv);

  /* Swap global process slot addresses. */
  مٽا_سٽا_proc_جڳهه_اشارو(get_cpulocal_var_ptr(ptproc), src_rp, dst_rp);

  /* Swap VM request entries. */
  مٽا_سٽا_memreq(src_rp, dst_rp);

#إذا DEBUG
  لکو("do_update: updated %d (%s, %d, %d) into %d (%s, %d, %d)\n",
      src_rp->p_endpoint, src_rp->p_name, src_rp->p_nr, priv(src_rp)->s_proc_nr,
      dst_rp->p_endpoint, dst_rp->p_name, dst_rp->p_nr, priv(dst_rp)->s_proc_nr);

  proc_stacktrace(src_rp);
  proc_stacktrace(dst_rp);
  لکو("do_update: curr ptproc %d\n", get_cpulocal_var(ptproc)->p_endpoint);
  لکو("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", src_rp->p_endpoint, src_rp->p_rts_flags, priv(src_rp)->s_asyntab, priv(src_rp)->s_asynendpoint, priv(src_rp)->s_grant_جدول, priv(src_rp)->s_grant_endpoint);
  لکو("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", dst_rp->p_endpoint, dst_rp->p_rts_flags, priv(dst_rp)->s_asyntab, priv(dst_rp)->s_asynendpoint, priv(dst_rp)->s_grant_جدول, priv(dst_rp)->s_grant_endpoint);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
  بٽ_fill(src_rp->p_stale_tlb, CONFIG_MAX_CPUS);
  بٽ_fill(dst_rp->p_stale_tlb, CONFIG_MAX_CPUS);
#جيڪڏهن_پورو

  أرجع OK;
}

/*===========================================================================*
 *			     inherit_priv_irq 				     *
 *===========================================================================*/
صحيح inherit_priv_irq(بنية proc *src_rp, بنية proc *dst_rp)
{
  صحيح i, r;
  لكل (i= 0; i<priv(src_rp)->s_nr_irq; i++) {
      r = priv_جوڙيو_irq(dst_rp, priv(src_rp)->s_irq_tab[i]); 
      إذا(r != OK) {
          أرجع r;
      }
  }

  أرجع OK;
}

/*===========================================================================*
 *			     inherit_priv_io 				     *
 *===========================================================================*/
 صحيح inherit_priv_io(بنية proc *src_rp, بنية proc *dst_rp)
{
  صحيح i, r;
  لكل (i= 0; i<priv(src_rp)->s_nr_io_range; i++) {
      r = priv_جوڙيو_io(dst_rp, &(priv(src_rp)->s_io_tab[i])); 
      إذا(r != OK) {
          أرجع r;
      }
  }

  أرجع OK;
}

/*===========================================================================*
 *			     inherit_priv_mem 				     *
 *===========================================================================*/
صحيح inherit_priv_mem(بنية proc *src_rp, بنية proc *dst_rp)
{
  صحيح i, r;
  لكل (i= 0; i<priv(src_rp)->s_nr_mem_range; i++) {
      r = priv_جوڙيو_mem(dst_rp, &(priv(src_rp)->s_mem_tab[i])); 
      إذا(r != OK) {
          أرجع r;
      }
  }

  أرجع OK;
}

/*===========================================================================*
 *			    abort_proc_ipc_send				     *
 *===========================================================================*/
فراغ abort_proc_ipc_send(بنية proc *rp)
{
  إذا(RTS_ISSET(rp, RTS_SENDING)) {
      بنية proc **xpp;
      RTS_UNSET(rp, RTS_SENDING);
      rp->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
      xpp = &(proc_addr(_ENDPOINT_P(rp->p_sendto_e))->p_caller_q);
      طالما (*xpp) {
          إذا(*xpp == rp) {
              *xpp = rp->p_q_link;
              rp->p_q_link = عدم;
              اكسر;
          }
          xpp = &(*xpp)->p_q_link;
      }
  }
}

/*===========================================================================*
 *			     adjust_proc_slot				     *
 *===========================================================================*/
ساكن فراغ adjust_proc_جڳهه(بنية proc *rp, بنية proc *from_rp)
{
  /* Preserve endpoints, slot numbers, priv structure, and IPC. */
  rp->p_endpoint = from_rp->p_endpoint;
  rp->p_nr = from_rp->p_nr;
  rp->p_priv = from_rp->p_priv;
  priv(rp)->s_proc_nr = from_rp->p_nr;

  rp->p_caller_q = from_rp->p_caller_q;

  /* preserve scheduling */
  rp->p_scheduler = from_rp->p_scheduler;
#جيڪڏهن_وصف_ٿيل CONFIG_SMP
  rp->p_cpu = from_rp->p_cpu;
  يادگيري_نقل(rp->p_cpu_نقاب, from_rp->p_cpu_نقاب,
		  حجم(bitchunk_t) * BITMAP_CHUNKS(CONFIG_MAX_CPUS));
#جيڪڏهن_پورو
}

/*===========================================================================*
 *			     adjust_asyn_table				     *
 *===========================================================================*/
ساكن فراغ adjust_asyn_جدول(بنية priv *src_privp, بنية priv *dst_privp)
{
  /* Transfer the asyn table if source's table belongs to the destination. */
  endpoint_t src_e = proc_addr(src_privp->s_proc_nr)->p_endpoint;
  endpoint_t dst_e = proc_addr(dst_privp->s_proc_nr)->p_endpoint;

  إذا(src_privp->s_asynsize > 0 && dst_privp->s_asynsize > 0 && src_privp->s_asynendpoint == dst_e) {
      إذا(معلومات_copy(src_e, src_privp->s_asyntab, dst_e, dst_privp->s_asyntab,
	  src_privp->s_asynsize*حجم(asynmsg_t)) != OK) {
	  لکو("Warning: unable to transfer asyn table from ep %d to ep %d\n",
	      src_e, dst_e);
      }
      وإلا {
          dst_privp->s_asynsize = src_privp->s_asynsize;
      }
  }
}

/*===========================================================================*
 *			     adjust_priv_slot				     *
 *===========================================================================*/
ساكن فراغ adjust_priv_جڳهه(بنية priv *privp, بنية priv *from_privp)
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
ساكن فراغ مٽا_سٽا_proc_جڳهه_اشارو(بنية proc **rpp, بنية proc *src_rp,
    بنية proc *dst_rp)
{
  إذا(*rpp == src_rp) {
      *rpp = dst_rp;
  }
  وإلا إذا(*rpp == dst_rp) {
      *rpp = src_rp;
  }
}

/*===========================================================================*
 *				swap_memreq				     *
 *===========================================================================*/
ساكن فراغ مٽا_سٽا_memreq(بنية proc *src_rp, بنية proc *dst_rp)
{
/* If either the source or the destination process is part of the VM request
 * chain, but not both, then swap the process pointers in the chain.
 */
  بنية proc **rpp;

  إذا (RTS_ISSET(src_rp, RTS_VMREQUEST) == RTS_ISSET(dst_rp, RTS_VMREQUEST))
	أرجع; /* nothing to do */

  لكل (rpp = &vmrequest; *rpp != عدم;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	إذا (*rpp == src_rp) {
		dst_rp->p_vmrequest.nextrequestor =
		    src_rp->p_vmrequest.nextrequestor;
		*rpp = dst_rp;
		اكسر;
	} وإلا إذا (*rpp == dst_rp) {
		src_rp->p_vmrequest.nextrequestor =
		    dst_rp->p_vmrequest.nextrequestor;
		*rpp = src_rp;
		اكسر;
	}
  }
}

#جيڪڏهن_پورو /* USE_UPDATE */

