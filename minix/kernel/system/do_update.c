/* The kernel call implemented in this file:
 *   m_type:	SYS_UPDATE
 *
 * The parameters for this kernel call are:
 *    m2_i1:	SYS_UPD_SRC_ENDPT 	(source process endpoint)
 *    m2_i2:	SYS_UPD_DST_ENDPT	(destination process endpoint)
 *    m2_i3:	SYS_UPD_FLAGS		(update flags)
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>

#ଯଦି USE_UPDATE

#define DEBUG 0

#define proc_is_updatable(p) \
    (RTS_ISSET(p, RTS_NO_PRIV) || RTS_ISSET(p, RTS_SIG_PENDING) \
    || (RTS_ISSET(p, RTS_RECEIVING) && !RTS_ISSET(p, RTS_SENDING)))

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା inherit_priv_irq(ସଂରଚନା proc *src_rp, ସଂରଚନା proc *dst_rp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା inherit_priv_io(ସଂରଚନା proc *src_rp, ସଂରଚନା proc *dst_rp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା inherit_priv_mem(ସଂରଚନା proc *src_rp, ସଂରଚନା proc *dst_rp);
ସ୍ଥିର ଶୂନ୍ୟ abort_proc_ipc_send(ସଂରଚନା proc *rp);
ସ୍ଥିର ଶୂନ୍ୟ adjust_proc_slot(ସଂରଚନା proc *rp, ସଂରଚନା proc *from_rp);
ସ୍ଥିର ଶୂନ୍ୟ adjust_priv_slot(ସଂରଚନା priv *privp, ସଂରଚନା priv
	*from_privp);
ସ୍ଥିର ଶୂନ୍ୟ adjust_asyn_table(ସଂରଚନା priv *src_privp, ସଂରଚନା priv *dst_privp);
ସ୍ଥିର ଶୂନ୍ୟ swap_proc_slot_pointer(ସଂରଚନା proc **rpp, ସଂରଚନା proc
	*src_rp, ସଂରଚନା proc *dst_rp);
ସ୍ଥିର ଶୂନ୍ୟ swap_memreq(ସଂରଚନା proc *src_rp, ସଂରଚନା proc *dst_rp);

/*===========================================================================*
 *				do_update				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_update(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Handle sys_update(). Update a process into another by swapping their process
 * slots.
 */
  endpoint_t src_e, dst_e;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା src_p, dst_p, flags;
  ସଂରଚନା proc *src_rp, *dst_rp;
  ସଂରଚନା priv *src_privp, *dst_privp;
  ସଂରଚନା proc orig_src_proc;
  ସଂରଚନା proc orig_dst_proc;
  ସଂରଚନା priv orig_src_priv;
  ସଂରଚନା priv orig_dst_priv;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, r;

  /* Lookup slots for source and destination process. */
  flags = m_ptr->SYS_UPD_FLAGS;
  src_e = m_ptr->SYS_UPD_SRC_ENDPT;
  ଯଦି(!isokendpt(src_e, &src_p)) {
      ଫେରନ୍ତୁ EINVAL;
  }
  src_rp = proc_addr(src_p);
  src_privp = priv(src_rp);
  ଯଦି(!(src_privp->s_flags & SYS_PROC)) {
      ଫେରନ୍ତୁ EPERM;
  }

  dst_e = m_ptr->SYS_UPD_DST_ENDPT;
  ଯଦି(!isokendpt(dst_e, &dst_p)) {
      ଫେରନ୍ତୁ EINVAL;
  }
  dst_rp = proc_addr(dst_p);
  dst_privp = priv(dst_rp);
  ଯଦି(!(dst_privp->s_flags & SYS_PROC)) {
      ଫେରନ୍ତୁ EPERM;
  }

  ପ୍ରତିଜ୍ଞା(!proc_is_runnable(src_rp) && !proc_is_runnable(dst_rp));

  /* Check if processes are updatable. */
  ଯଦି(!proc_is_updatable(src_rp) || !proc_is_updatable(dst_rp)) {
      ଫେରନ୍ତୁ EBUSY;
  }

#ଯଦି DEBUG
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
  ଯଦି(r != OK) {
      ଫେରନ୍ତୁ r;
  }
  r = inherit_priv_io(src_rp, dst_rp);
  ଯଦି(r != OK) {
      ଫେରନ୍ତୁ r;
  }
  r = inherit_priv_mem(src_rp, dst_rp);
  ଯଦି(r != OK) {
      ଫେରନ୍ତୁ r;
  }

  /* Let destination inherit the target mask from source. */
  ପାଇଁ (i=0; i < NR_SYS_PROCS; i++) {
      ଯଦି (get_sys_bit(priv(src_rp)->s_ipc_to, i)) {
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
  ଯଦି(flags & SYS_UPD_ROLLBACK) {
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

#ଯଦି DEBUG
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

  ଫେରନ୍ତୁ OK;
}

/*===========================================================================*
 *			     inherit_priv_irq 				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା inherit_priv_irq(ସଂରଚନା proc *src_rp, ସଂରଚନା proc *dst_rp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, r;
  ପାଇଁ (i= 0; i<priv(src_rp)->s_nr_irq; i++) {
      r = priv_add_irq(dst_rp, priv(src_rp)->s_irq_tab[i]); 
      ଯଦି(r != OK) {
          ଫେରନ୍ତୁ r;
      }
  }

  ଫେରନ୍ତୁ OK;
}

/*===========================================================================*
 *			     inherit_priv_io 				     *
 *===========================================================================*/
 ପୂର୍ଣ୍ଣସଂଖ୍ୟା inherit_priv_io(ସଂରଚନା proc *src_rp, ସଂରଚନା proc *dst_rp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, r;
  ପାଇଁ (i= 0; i<priv(src_rp)->s_nr_io_range; i++) {
      r = priv_add_io(dst_rp, &(priv(src_rp)->s_io_tab[i])); 
      ଯଦି(r != OK) {
          ଫେରନ୍ତୁ r;
      }
  }

  ଫେରନ୍ତୁ OK;
}

/*===========================================================================*
 *			     inherit_priv_mem 				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା inherit_priv_mem(ସଂରଚନା proc *src_rp, ସଂରଚନା proc *dst_rp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, r;
  ପାଇଁ (i= 0; i<priv(src_rp)->s_nr_mem_range; i++) {
      r = priv_add_mem(dst_rp, &(priv(src_rp)->s_mem_tab[i])); 
      ଯଦି(r != OK) {
          ଫେରନ୍ତୁ r;
      }
  }

  ଫେରନ୍ତୁ OK;
}

/*===========================================================================*
 *			    abort_proc_ipc_send				     *
 *===========================================================================*/
ଶୂନ୍ୟ abort_proc_ipc_send(ସଂରଚନା proc *rp)
{
  ଯଦି(RTS_ISSET(rp, RTS_SENDING)) {
      ସଂରଚନା proc **xpp;
      RTS_UNSET(rp, RTS_SENDING);
      rp->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
      xpp = &(proc_addr(_ENDPOINT_P(rp->p_sendto_e))->p_caller_q);
      ଯେତେବେଳେ (*xpp) {
          ଯଦି(*xpp == rp) {
              *xpp = rp->p_q_link;
              rp->p_q_link = ଖାଲି;
              ଭାଙ୍ଗ;
          }
          xpp = &(*xpp)->p_q_link;
      }
  }
}

/*===========================================================================*
 *			     adjust_proc_slot				     *
 *===========================================================================*/
ସ୍ଥିର ଶୂନ୍ୟ adjust_proc_slot(ସଂରଚନା proc *rp, ସଂରଚନା proc *from_rp)
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
		  ଆକାର(bitchunk_t) * BITMAP_CHUNKS(CONFIG_MAX_CPUS));
#endif
}

/*===========================================================================*
 *			     adjust_asyn_table				     *
 *===========================================================================*/
ସ୍ଥିର ଶୂନ୍ୟ adjust_asyn_table(ସଂରଚନା priv *src_privp, ସଂରଚନା priv *dst_privp)
{
  /* Transfer the asyn table if source's table belongs to the destination. */
  endpoint_t src_e = proc_addr(src_privp->s_proc_nr)->p_endpoint;
  endpoint_t dst_e = proc_addr(dst_privp->s_proc_nr)->p_endpoint;

  ଯଦି(src_privp->s_asynsize > 0 && dst_privp->s_asynsize > 0 && src_privp->s_asynendpoint == dst_e) {
      ଯଦି(data_copy(src_e, src_privp->s_asyntab, dst_e, dst_privp->s_asyntab,
	  src_privp->s_asynsize*ଆକାର(asynmsg_t)) != OK) {
	  printf("Warning: unable to transfer asyn table from ep %d to ep %d\n",
	      src_e, dst_e);
      }
      ଅନ୍ୟଥା {
          dst_privp->s_asynsize = src_privp->s_asynsize;
      }
  }
}

/*===========================================================================*
 *			     adjust_priv_slot				     *
 *===========================================================================*/
ସ୍ଥିର ଶୂନ୍ୟ adjust_priv_slot(ସଂରଚନା priv *privp, ସଂରଚନା priv *from_privp)
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
ସ୍ଥିର ଶୂନ୍ୟ swap_proc_slot_pointer(ସଂରଚନା proc **rpp, ସଂରଚନା proc *src_rp,
    ସଂରଚନା proc *dst_rp)
{
  ଯଦି(*rpp == src_rp) {
      *rpp = dst_rp;
  }
  ଅନ୍ୟଥା ଯଦି(*rpp == dst_rp) {
      *rpp = src_rp;
  }
}

/*===========================================================================*
 *				swap_memreq				     *
 *===========================================================================*/
ସ୍ଥିର ଶୂନ୍ୟ swap_memreq(ସଂରଚନା proc *src_rp, ସଂରଚନା proc *dst_rp)
{
/* If either the source or the destination process is part of the VM request
 * chain, but not both, then swap the process pointers in the chain.
 */
  ସଂରଚନା proc **rpp;

  ଯଦି (RTS_ISSET(src_rp, RTS_VMREQUEST) == RTS_ISSET(dst_rp, RTS_VMREQUEST))
	ଫେରନ୍ତୁ; /* nothing to do */

  ପାଇଁ (rpp = &vmrequest; *rpp != ଖାଲି;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	ଯଦି (*rpp == src_rp) {
		dst_rp->p_vmrequest.nextrequestor =
		    src_rp->p_vmrequest.nextrequestor;
		*rpp = dst_rp;
		ଭାଙ୍ଗ;
	} ଅନ୍ୟଥା ଯଦି (*rpp == dst_rp) {
		src_rp->p_vmrequest.nextrequestor =
		    dst_rp->p_vmrequest.nextrequestor;
		*rpp = src_rp;
		ଭାଙ୍ଗ;
	}
  }
}

#endif /* USE_UPDATE */

