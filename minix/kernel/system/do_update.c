/* The kernel call implemented in this file:
 *   m_type:	SYS_UPDATE
 *
 * The parameters for this kernel call are:
 *    m2_i1:	SYS_UPD_SRC_ENDPT 	(source process endpoint)
 *    m2_i2:	SYS_UPD_DST_ENDPT	(destination process endpoint)
 *    m2_i3:	SYS_UPD_FLAGS		(update flags)
 */

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <assert.h>

#എങ്കിൽ USE_UPDATE

#define DEBUG 0

#define proc_is_updatable(p) \
    (RTS_ISSET(p, RTS_NO_PRIV) || RTS_ISSET(p, RTS_SIG_PENDING) \
    || (RTS_ISSET(p, RTS_RECEIVING) && !RTS_ISSET(p, RTS_SENDING)))

സ്ഥിതം പൂർണ്ണസംഖ്യ inherit_priv_irq(ഘടന proc *src_rp, ഘടന proc *dst_rp);
സ്ഥിതം പൂർണ്ണസംഖ്യ inherit_priv_io(ഘടന proc *src_rp, ഘടന proc *dst_rp);
സ്ഥിതം പൂർണ്ണസംഖ്യ inherit_priv_mem(ഘടന proc *src_rp, ഘടന proc *dst_rp);
സ്ഥിതം ശൂന്യം abort_proc_ipc_send(ഘടന proc *rp);
സ്ഥിതം ശൂന്യം adjust_proc_slot(ഘടന proc *rp, ഘടന proc *from_rp);
സ്ഥിതം ശൂന്യം adjust_priv_slot(ഘടന priv *privp, ഘടന priv
	*from_privp);
സ്ഥിതം ശൂന്യം adjust_asyn_table(ഘടന priv *src_privp, ഘടന priv *dst_privp);
സ്ഥിതം ശൂന്യം swap_proc_slot_pointer(ഘടന proc **rpp, ഘടന proc
	*src_rp, ഘടന proc *dst_rp);
സ്ഥിതം ശൂന്യം swap_memreq(ഘടന proc *src_rp, ഘടന proc *dst_rp);

/*===========================================================================*
 *				do_update				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_update(ഘടന proc * caller, message * m_ptr)
{
/* Handle sys_update(). Update a process into another by swapping their process
 * slots.
 */
  endpoint_t src_e, dst_e;
  പൂർണ്ണസംഖ്യ src_p, dst_p, flags;
  ഘടന proc *src_rp, *dst_rp;
  ഘടന priv *src_privp, *dst_privp;
  ഘടന proc orig_src_proc;
  ഘടന proc orig_dst_proc;
  ഘടന priv orig_src_priv;
  ഘടന priv orig_dst_priv;
  പൂർണ്ണസംഖ്യ i, r;

  /* Lookup slots for source and destination process. */
  flags = m_ptr->SYS_UPD_FLAGS;
  src_e = m_ptr->SYS_UPD_SRC_ENDPT;
  എങ്കിൽ(!isokendpt(src_e, &src_p)) {
      തിരികെ EINVAL;
  }
  src_rp = proc_addr(src_p);
  src_privp = priv(src_rp);
  എങ്കിൽ(!(src_privp->s_flags & SYS_PROC)) {
      തിരികെ EPERM;
  }

  dst_e = m_ptr->SYS_UPD_DST_ENDPT;
  എങ്കിൽ(!isokendpt(dst_e, &dst_p)) {
      തിരികെ EINVAL;
  }
  dst_rp = proc_addr(dst_p);
  dst_privp = priv(dst_rp);
  എങ്കിൽ(!(dst_privp->s_flags & SYS_PROC)) {
      തിരികെ EPERM;
  }

  പ്രതിജ്ഞാ(!proc_is_runnable(src_rp) && !proc_is_runnable(dst_rp));

  /* Check if processes are updatable. */
  എങ്കിൽ(!proc_is_updatable(src_rp) || !proc_is_updatable(dst_rp)) {
      തിരികെ EBUSY;
  }

#എങ്കിൽ DEBUG
  printf("do_update: updating %d (%s, %d, %d) into %d (%s, %d, %d)\n",
      src_rp->p_endpoint, src_rp->p_name, src_rp->p_nr, priv(src_rp)->s_proc_nr,
      dst_rp->p_endpoint, dst_rp->p_name, dst_rp->p_nr, priv(dst_rp)->s_proc_nr);

  proc_stacktrace(src_rp);
  proc_stacktrace(dst_rp);
  printf("do_update: curr ptproc %d\n", get_cpulocal_var(ptproc)->p_endpoint);
  printf("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", src_rp->p_endpoint, src_rp->p_rts_flags, priv(src_rp)->s_asyntab, priv(src_rp)->s_asynendpoint, priv(src_rp)->s_grant_table, priv(src_rp)->s_grant_endpoint);
  printf("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", dst_rp->p_endpoint, dst_rp->p_rts_flags, priv(dst_rp)->s_asyntab, priv(dst_rp)->s_asynendpoint, priv(dst_rp)->s_grant_table, priv(dst_rp)->s_grant_endpoint);
#endif

  /* Let destination inherit allowed IRQ, I/O ranges, and memory ranges. */
  r = inherit_priv_irq(src_rp, dst_rp);
  എങ്കിൽ(r != OK) {
      തിരികെ r;
  }
  r = inherit_priv_io(src_rp, dst_rp);
  എങ്കിൽ(r != OK) {
      തിരികെ r;
  }
  r = inherit_priv_mem(src_rp, dst_rp);
  എങ്കിൽ(r != OK) {
      തിരികെ r;
  }

  /* Let destination inherit the target mask from source. */
  വേണ്ടി (i=0; i < NR_SYS_PROCS; i++) {
      എങ്കിൽ (get_sys_bit(priv(src_rp)->s_ipc_to, i)) {
          set_sendto_bit(dst_rp, i);
      }
  }

  /* Save existing data. */
  orig_src_proc = *src_rp;
  orig_src_priv = *(priv(src_rp));
  orig_dst_proc = *dst_rp;
  orig_dst_priv = *(priv(dst_rp));

  /* Adjust asyn tables. */
  adjust_asyn_table(priv(src_rp), priv(dst_rp));
  adjust_asyn_table(priv(dst_rp), priv(src_rp));

  /* Abort any pending send() on rollback. */
  എങ്കിൽ(flags & SYS_UPD_ROLLBACK) {
      abort_proc_ipc_send(src_rp);
  }

  /* Swap slots. */
  *src_rp = orig_dst_proc;
  *src_privp = orig_dst_priv;
  *dst_rp = orig_src_proc;
  *dst_privp = orig_src_priv;

  /* Adjust process slots. */
  adjust_proc_slot(src_rp, &orig_src_proc);
  adjust_proc_slot(dst_rp, &orig_dst_proc);

  /* Adjust privilege slots. */
  adjust_priv_slot(priv(src_rp), &orig_src_priv);
  adjust_priv_slot(priv(dst_rp), &orig_dst_priv);

  /* Swap global process slot addresses. */
  swap_proc_slot_pointer(get_cpulocal_var_ptr(ptproc), src_rp, dst_rp);

  /* Swap VM request entries. */
  swap_memreq(src_rp, dst_rp);

#എങ്കിൽ DEBUG
  printf("do_update: updated %d (%s, %d, %d) into %d (%s, %d, %d)\n",
      src_rp->p_endpoint, src_rp->p_name, src_rp->p_nr, priv(src_rp)->s_proc_nr,
      dst_rp->p_endpoint, dst_rp->p_name, dst_rp->p_nr, priv(dst_rp)->s_proc_nr);

  proc_stacktrace(src_rp);
  proc_stacktrace(dst_rp);
  printf("do_update: curr ptproc %d\n", get_cpulocal_var(ptproc)->p_endpoint);
  printf("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", src_rp->p_endpoint, src_rp->p_rts_flags, priv(src_rp)->s_asyntab, priv(src_rp)->s_asynendpoint, priv(src_rp)->s_grant_table, priv(src_rp)->s_grant_endpoint);
  printf("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", dst_rp->p_endpoint, dst_rp->p_rts_flags, priv(dst_rp)->s_asyntab, priv(dst_rp)->s_asynendpoint, priv(dst_rp)->s_grant_table, priv(dst_rp)->s_grant_endpoint);
#endif

#ifdef CONFIG_SMP
  bits_fill(src_rp->p_stale_tlb, CONFIG_MAX_CPUS);
  bits_fill(dst_rp->p_stale_tlb, CONFIG_MAX_CPUS);
#endif

  തിരികെ OK;
}

/*===========================================================================*
 *			     inherit_priv_irq 				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ inherit_priv_irq(ഘടന proc *src_rp, ഘടന proc *dst_rp)
{
  പൂർണ്ണസംഖ്യ i, r;
  വേണ്ടി (i= 0; i<priv(src_rp)->s_nr_irq; i++) {
      r = priv_add_irq(dst_rp, priv(src_rp)->s_irq_tab[i]); 
      എങ്കിൽ(r != OK) {
          തിരികെ r;
      }
  }

  തിരികെ OK;
}

/*===========================================================================*
 *			     inherit_priv_io 				     *
 *===========================================================================*/
 പൂർണ്ണസംഖ്യ inherit_priv_io(ഘടന proc *src_rp, ഘടന proc *dst_rp)
{
  പൂർണ്ണസംഖ്യ i, r;
  വേണ്ടി (i= 0; i<priv(src_rp)->s_nr_io_range; i++) {
      r = priv_add_io(dst_rp, &(priv(src_rp)->s_io_tab[i])); 
      എങ്കിൽ(r != OK) {
          തിരികെ r;
      }
  }

  തിരികെ OK;
}

/*===========================================================================*
 *			     inherit_priv_mem 				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ inherit_priv_mem(ഘടന proc *src_rp, ഘടന proc *dst_rp)
{
  പൂർണ്ണസംഖ്യ i, r;
  വേണ്ടി (i= 0; i<priv(src_rp)->s_nr_mem_range; i++) {
      r = priv_add_mem(dst_rp, &(priv(src_rp)->s_mem_tab[i])); 
      എങ്കിൽ(r != OK) {
          തിരികെ r;
      }
  }

  തിരികെ OK;
}

/*===========================================================================*
 *			    abort_proc_ipc_send				     *
 *===========================================================================*/
ശൂന്യം abort_proc_ipc_send(ഘടന proc *rp)
{
  എങ്കിൽ(RTS_ISSET(rp, RTS_SENDING)) {
      ഘടന proc **xpp;
      RTS_UNSET(rp, RTS_SENDING);
      rp->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
      xpp = &(proc_addr(_ENDPOINT_P(rp->p_sendto_e))->p_caller_q);
      ആകുമ്പോൾ (*xpp) {
          എങ്കിൽ(*xpp == rp) {
              *xpp = rp->p_q_link;
              rp->p_q_link = ശൂന്യ;
              നിർത്തുക;
          }
          xpp = &(*xpp)->p_q_link;
      }
  }
}

/*===========================================================================*
 *			     adjust_proc_slot				     *
 *===========================================================================*/
സ്ഥിതം ശൂന്യം adjust_proc_slot(ഘടന proc *rp, ഘടന proc *from_rp)
{
  /* Preserve endpoints, slot numbers, priv structure, and IPC. */
  rp->p_endpoint = from_rp->p_endpoint;
  rp->p_nr = from_rp->p_nr;
  rp->p_priv = from_rp->p_priv;
  priv(rp)->s_proc_nr = from_rp->p_nr;

  rp->p_caller_q = from_rp->p_caller_q;

  /* preserve scheduling */
  rp->p_scheduler = from_rp->p_scheduler;
#ifdef CONFIG_SMP
  rp->p_cpu = from_rp->p_cpu;
  memcpy(rp->p_cpu_mask, from_rp->p_cpu_mask,
		  വലുപ്പം(bitchunk_t) * BITMAP_CHUNKS(CONFIG_MAX_CPUS));
#endif
}

/*===========================================================================*
 *			     adjust_asyn_table				     *
 *===========================================================================*/
സ്ഥിതം ശൂന്യം adjust_asyn_table(ഘടന priv *src_privp, ഘടന priv *dst_privp)
{
  /* Transfer the asyn table if source's table belongs to the destination. */
  endpoint_t src_e = proc_addr(src_privp->s_proc_nr)->p_endpoint;
  endpoint_t dst_e = proc_addr(dst_privp->s_proc_nr)->p_endpoint;

  എങ്കിൽ(src_privp->s_asynsize > 0 && dst_privp->s_asynsize > 0 && src_privp->s_asynendpoint == dst_e) {
      എങ്കിൽ(data_copy(src_e, src_privp->s_asyntab, dst_e, dst_privp->s_asyntab,
	  src_privp->s_asynsize*വലുപ്പം(asynmsg_t)) != OK) {
	  printf("Warning: unable to transfer asyn table from ep %d to ep %d\n",
	      src_e, dst_e);
      }
      അല്ലെങ്കിൽ {
          dst_privp->s_asynsize = src_privp->s_asynsize;
      }
  }
}

/*===========================================================================*
 *			     adjust_priv_slot				     *
 *===========================================================================*/
സ്ഥിതം ശൂന്യം adjust_priv_slot(ഘടന priv *privp, ഘടന priv *from_privp)
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
സ്ഥിതം ശൂന്യം swap_proc_slot_pointer(ഘടന proc **rpp, ഘടന proc *src_rp,
    ഘടന proc *dst_rp)
{
  എങ്കിൽ(*rpp == src_rp) {
      *rpp = dst_rp;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(*rpp == dst_rp) {
      *rpp = src_rp;
  }
}

/*===========================================================================*
 *				swap_memreq				     *
 *===========================================================================*/
സ്ഥിതം ശൂന്യം swap_memreq(ഘടന proc *src_rp, ഘടന proc *dst_rp)
{
/* If either the source or the destination process is part of the VM request
 * chain, but not both, then swap the process pointers in the chain.
 */
  ഘടന proc **rpp;

  എങ്കിൽ (RTS_ISSET(src_rp, RTS_VMREQUEST) == RTS_ISSET(dst_rp, RTS_VMREQUEST))
	തിരികെ; /* nothing to do */

  വേണ്ടി (rpp = &vmrequest; *rpp != ശൂന്യ;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	എങ്കിൽ (*rpp == src_rp) {
		dst_rp->p_vmrequest.nextrequestor =
		    src_rp->p_vmrequest.nextrequestor;
		*rpp = dst_rp;
		നിർത്തുക;
	} അല്ലെങ്കിൽ എങ്കിൽ (*rpp == dst_rp) {
		src_rp->p_vmrequest.nextrequestor =
		    dst_rp->p_vmrequest.nextrequestor;
		*rpp = src_rp;
		നിർത്തുക;
	}
  }
}

#endif /* USE_UPDATE */

