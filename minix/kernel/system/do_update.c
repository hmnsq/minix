/* The kernel call implemented in this file:
 *   m_type:	SYS_UPDATE
 *
 * The parameters for this kernel call are:
 *    m2_i1:	SYS_UPD_SRC_ENDPT 	(source process endpoint)
 *    m2_i2:	SYS_UPD_DST_ENDPT	(destination process endpoint)
 *    m2_i3:	SYS_UPD_FLAGS		(update flags)
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <string.h>
#అన్తర్భావ <assert.h>

#ఒకవేళ USE_UPDATE

#నిర్వచించు DEBUG 0

#నిర్వచించు proc_is_updatable(p) \
    (RTS_ISSET(p, RTS_NO_PRIV) || RTS_ISSET(p, RTS_SIG_PENDING) \
    || (RTS_ISSET(p, RTS_RECEIVING) && !RTS_ISSET(p, RTS_SENDING)))

స్థిర పూర్ణాంకం inherit_priv_irq(నిర్మాణం proc *src_rp, నిర్మాణం proc *dst_rp);
స్థిర పూర్ణాంకం inherit_priv_io(నిర్మాణం proc *src_rp, నిర్మాణం proc *dst_rp);
స్థిర పూర్ణాంకం inherit_priv_mem(నిర్మాణం proc *src_rp, నిర్మాణం proc *dst_rp);
స్థిర శూన్య abort_proc_ipc_send(నిర్మాణం proc *rp);
స్థిర శూన్య adjust_proc_స్థానం(నిర్మాణం proc *rp, నిర్మాణం proc *from_rp);
స్థిర శూన్య adjust_priv_స్థానం(నిర్మాణం priv *privp, నిర్మాణం priv
	*from_privp);
స్థిర శూన్య adjust_asyn_పట్టిక(నిర్మాణం priv *src_privp, నిర్మాణం priv *dst_privp);
స్థిర శూన్య మార్పిడి_proc_స్థానం_సూచిక(నిర్మాణం proc **rpp, నిర్మాణం proc
	*src_rp, నిర్మాణం proc *dst_rp);
స్థిర శూన్య మార్పిడి_memreq(నిర్మాణం proc *src_rp, నిర్మాణం proc *dst_rp);

/*===========================================================================*
 *				do_update				     *
 *===========================================================================*/
పూర్ణాంకం do_update(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Handle sys_update(). Update a process into another by swapping their process
 * slots.
 */
  endpoint_t src_e, dst_e;
  పూర్ణాంకం src_p, dst_p, flags;
  నిర్మాణం proc *src_rp, *dst_rp;
  నిర్మాణం priv *src_privp, *dst_privp;
  నిర్మాణం proc orig_src_proc;
  నిర్మాణం proc orig_dst_proc;
  నిర్మాణం priv orig_src_priv;
  నిర్మాణం priv orig_dst_priv;
  పూర్ణాంకం i, r;

  /* Lookup slots for source and destination process. */
  flags = m_ptr->SYS_UPD_FLAGS;
  src_e = m_ptr->SYS_UPD_SRC_ENDPT;
  ఒకవేళ(!isokendpt(src_e, &src_p)) {
      తిరిగి EINVAL;
  }
  src_rp = proc_addr(src_p);
  src_privp = priv(src_rp);
  ఒకవేళ(!(src_privp->s_flags & SYS_PROC)) {
      తిరిగి EPERM;
  }

  dst_e = m_ptr->SYS_UPD_DST_ENDPT;
  ఒకవేళ(!isokendpt(dst_e, &dst_p)) {
      తిరిగి EINVAL;
  }
  dst_rp = proc_addr(dst_p);
  dst_privp = priv(dst_rp);
  ఒకవేళ(!(dst_privp->s_flags & SYS_PROC)) {
      తిరిగి EPERM;
  }

  ప్రతిజ్ఞా(!proc_is_runnable(src_rp) && !proc_is_runnable(dst_rp));

  /* Check if processes are updatable. */
  ఒకవేళ(!proc_is_updatable(src_rp) || !proc_is_updatable(dst_rp)) {
      తిరిగి EBUSY;
  }

#ఒకవేళ DEBUG
  రాయి("do_update: updating %d (%s, %d, %d) into %d (%s, %d, %d)\n",
      src_rp->p_endpoint, src_rp->p_name, src_rp->p_nr, priv(src_rp)->s_proc_nr,
      dst_rp->p_endpoint, dst_rp->p_name, dst_rp->p_nr, priv(dst_rp)->s_proc_nr);

  proc_stacktrace(src_rp);
  proc_stacktrace(dst_rp);
  రాయి("do_update: curr ptproc %d\n", get_cpulocal_var(ptproc)->p_endpoint);
  రాయి("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", src_rp->p_endpoint, src_rp->p_rts_flags, priv(src_rp)->s_asyntab, priv(src_rp)->s_asynendpoint, priv(src_rp)->s_grant_పట్టిక, priv(src_rp)->s_grant_endpoint);
  రాయి("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", dst_rp->p_endpoint, dst_rp->p_rts_flags, priv(dst_rp)->s_asyntab, priv(dst_rp)->s_asynendpoint, priv(dst_rp)->s_grant_పట్టిక, priv(dst_rp)->s_grant_endpoint);
#అయితే_ముగింపు

  /* Let destination inherit allowed IRQ, I/O ranges, and memory ranges. */
  r = inherit_priv_irq(src_rp, dst_rp);
  ఒకవేళ(r != OK) {
      తిరిగి r;
  }
  r = inherit_priv_io(src_rp, dst_rp);
  ఒకవేళ(r != OK) {
      తిరిగి r;
  }
  r = inherit_priv_mem(src_rp, dst_rp);
  ఒకవేళ(r != OK) {
      తిరిగి r;
  }

  /* Let destination inherit the target mask from source. */
  కొరకు (i=0; i < NR_SYS_PROCS; i++) {
      ఒకవేళ (get_sys_బిట్టు(priv(src_rp)->s_ipc_గమ్యం, i)) {
          సమితి_sendto_బిట్టు(dst_rp, i);
      }
  }

  /* Save existing data. */
  orig_src_proc = *src_rp;
  orig_src_priv = *(priv(src_rp));
  orig_dst_proc = *dst_rp;
  orig_dst_priv = *(priv(dst_rp));

  /* Adjust asyn tables. */
  adjust_asyn_పట్టిక(priv(src_rp), priv(dst_rp));
  adjust_asyn_పట్టిక(priv(dst_rp), priv(src_rp));

  /* Abort any pending send() on rollback. */
  ఒకవేళ(flags & SYS_UPD_ROLLBACK) {
      abort_proc_ipc_send(src_rp);
  }

  /* Swap slots. */
  *src_rp = orig_dst_proc;
  *src_privp = orig_dst_priv;
  *dst_rp = orig_src_proc;
  *dst_privp = orig_src_priv;

  /* Adjust process slots. */
  adjust_proc_స్థానం(src_rp, &orig_src_proc);
  adjust_proc_స్థానం(dst_rp, &orig_dst_proc);

  /* Adjust privilege slots. */
  adjust_priv_స్థానం(priv(src_rp), &orig_src_priv);
  adjust_priv_స్థానం(priv(dst_rp), &orig_dst_priv);

  /* Swap global process slot addresses. */
  మార్పిడి_proc_స్థానం_సూచిక(get_cpulocal_var_ptr(ptproc), src_rp, dst_rp);

  /* Swap VM request entries. */
  మార్పిడి_memreq(src_rp, dst_rp);

#ఒకవేళ DEBUG
  రాయి("do_update: updated %d (%s, %d, %d) into %d (%s, %d, %d)\n",
      src_rp->p_endpoint, src_rp->p_name, src_rp->p_nr, priv(src_rp)->s_proc_nr,
      dst_rp->p_endpoint, dst_rp->p_name, dst_rp->p_nr, priv(dst_rp)->s_proc_nr);

  proc_stacktrace(src_rp);
  proc_stacktrace(dst_rp);
  రాయి("do_update: curr ptproc %d\n", get_cpulocal_var(ptproc)->p_endpoint);
  రాయి("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", src_rp->p_endpoint, src_rp->p_rts_flags, priv(src_rp)->s_asyntab, priv(src_rp)->s_asynendpoint, priv(src_rp)->s_grant_పట్టిక, priv(src_rp)->s_grant_endpoint);
  రాయి("do_update: endpoint %d rts flags %x asyn tab %08x asyn endpoint %d grant tab %08x grant endpoint %d\n", dst_rp->p_endpoint, dst_rp->p_rts_flags, priv(dst_rp)->s_asyntab, priv(dst_rp)->s_asynendpoint, priv(dst_rp)->s_grant_పట్టిక, priv(dst_rp)->s_grant_endpoint);
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_SMP
  బిట్లు_fill(src_rp->p_stale_tlb, CONFIG_MAX_CPUS);
  బిట్లు_fill(dst_rp->p_stale_tlb, CONFIG_MAX_CPUS);
#అయితే_ముగింపు

  తిరిగి OK;
}

/*===========================================================================*
 *			     inherit_priv_irq 				     *
 *===========================================================================*/
పూర్ణాంకం inherit_priv_irq(నిర్మాణం proc *src_rp, నిర్మాణం proc *dst_rp)
{
  పూర్ణాంకం i, r;
  కొరకు (i= 0; i<priv(src_rp)->s_nr_irq; i++) {
      r = priv_కూడు_irq(dst_rp, priv(src_rp)->s_irq_tab[i]); 
      ఒకవేళ(r != OK) {
          తిరిగి r;
      }
  }

  తిరిగి OK;
}

/*===========================================================================*
 *			     inherit_priv_io 				     *
 *===========================================================================*/
 పూర్ణాంకం inherit_priv_io(నిర్మాణం proc *src_rp, నిర్మాణం proc *dst_rp)
{
  పూర్ణాంకం i, r;
  కొరకు (i= 0; i<priv(src_rp)->s_nr_io_range; i++) {
      r = priv_కూడు_io(dst_rp, &(priv(src_rp)->s_io_tab[i])); 
      ఒకవేళ(r != OK) {
          తిరిగి r;
      }
  }

  తిరిగి OK;
}

/*===========================================================================*
 *			     inherit_priv_mem 				     *
 *===========================================================================*/
పూర్ణాంకం inherit_priv_mem(నిర్మాణం proc *src_rp, నిర్మాణం proc *dst_rp)
{
  పూర్ణాంకం i, r;
  కొరకు (i= 0; i<priv(src_rp)->s_nr_mem_range; i++) {
      r = priv_కూడు_mem(dst_rp, &(priv(src_rp)->s_mem_tab[i])); 
      ఒకవేళ(r != OK) {
          తిరిగి r;
      }
  }

  తిరిగి OK;
}

/*===========================================================================*
 *			    abort_proc_ipc_send				     *
 *===========================================================================*/
శూన్య abort_proc_ipc_send(నిర్మాణం proc *rp)
{
  ఒకవేళ(RTS_ISSET(rp, RTS_SENDING)) {
      నిర్మాణం proc **xpp;
      RTS_UNSET(rp, RTS_SENDING);
      rp->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
      xpp = &(proc_addr(_ENDPOINT_P(rp->p_sendto_e))->p_caller_q);
      అయితే (*xpp) {
          ఒకవేళ(*xpp == rp) {
              *xpp = rp->p_q_link;
              rp->p_q_link = ఖాళీ;
              ఆపు;
          }
          xpp = &(*xpp)->p_q_link;
      }
  }
}

/*===========================================================================*
 *			     adjust_proc_slot				     *
 *===========================================================================*/
స్థిర శూన్య adjust_proc_స్థానం(నిర్మాణం proc *rp, నిర్మాణం proc *from_rp)
{
  /* Preserve endpoints, slot numbers, priv structure, and IPC. */
  rp->p_endpoint = from_rp->p_endpoint;
  rp->p_nr = from_rp->p_nr;
  rp->p_priv = from_rp->p_priv;
  priv(rp)->s_proc_nr = from_rp->p_nr;

  rp->p_caller_q = from_rp->p_caller_q;

  /* preserve scheduling */
  rp->p_scheduler = from_rp->p_scheduler;
#నిర్వచితమైతే CONFIG_SMP
  rp->p_cpu = from_rp->p_cpu;
  జ్ఞాపకం_నకలు(rp->p_cpu_ముసుగు, from_rp->p_cpu_ముసుగు,
		  పరిమాణం(bitchunk_t) * BITMAP_CHUNKS(CONFIG_MAX_CPUS));
#అయితే_ముగింపు
}

/*===========================================================================*
 *			     adjust_asyn_table				     *
 *===========================================================================*/
స్థిర శూన్య adjust_asyn_పట్టిక(నిర్మాణం priv *src_privp, నిర్మాణం priv *dst_privp)
{
  /* Transfer the asyn table if source's table belongs to the destination. */
  endpoint_t src_e = proc_addr(src_privp->s_proc_nr)->p_endpoint;
  endpoint_t dst_e = proc_addr(dst_privp->s_proc_nr)->p_endpoint;

  ఒకవేళ(src_privp->s_asynsize > 0 && dst_privp->s_asynsize > 0 && src_privp->s_asynendpoint == dst_e) {
      ఒకవేళ(దత్తాంశం_copy(src_e, src_privp->s_asyntab, dst_e, dst_privp->s_asyntab,
	  src_privp->s_asynsize*పరిమాణం(asynmsg_t)) != OK) {
	  రాయి("Warning: unable to transfer asyn table from ep %d to ep %d\n",
	      src_e, dst_e);
      }
      లేకపోతే {
          dst_privp->s_asynsize = src_privp->s_asynsize;
      }
  }
}

/*===========================================================================*
 *			     adjust_priv_slot				     *
 *===========================================================================*/
స్థిర శూన్య adjust_priv_స్థానం(నిర్మాణం priv *privp, నిర్మాణం priv *from_privp)
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
స్థిర శూన్య మార్పిడి_proc_స్థానం_సూచిక(నిర్మాణం proc **rpp, నిర్మాణం proc *src_rp,
    నిర్మాణం proc *dst_rp)
{
  ఒకవేళ(*rpp == src_rp) {
      *rpp = dst_rp;
  }
  లేకపోతే ఒకవేళ(*rpp == dst_rp) {
      *rpp = src_rp;
  }
}

/*===========================================================================*
 *				swap_memreq				     *
 *===========================================================================*/
స్థిర శూన్య మార్పిడి_memreq(నిర్మాణం proc *src_rp, నిర్మాణం proc *dst_rp)
{
/* If either the source or the destination process is part of the VM request
 * chain, but not both, then swap the process pointers in the chain.
 */
  నిర్మాణం proc **rpp;

  ఒకవేళ (RTS_ISSET(src_rp, RTS_VMREQUEST) == RTS_ISSET(dst_rp, RTS_VMREQUEST))
	తిరిగి; /* nothing to do */

  కొరకు (rpp = &vmrequest; *rpp != ఖాళీ;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	ఒకవేళ (*rpp == src_rp) {
		dst_rp->p_vmrequest.nextrequestor =
		    src_rp->p_vmrequest.nextrequestor;
		*rpp = dst_rp;
		ఆపు;
	} లేకపోతే ఒకవేళ (*rpp == dst_rp) {
		src_rp->p_vmrequest.nextrequestor =
		    dst_rp->p_vmrequest.nextrequestor;
		*rpp = src_rp;
		ఆపు;
	}
  }
}

#అయితే_ముగింపు /* USE_UPDATE */

