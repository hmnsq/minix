/* The kernel call implemented in this file:
 *   m_type:	SYS_SAFECOPYFROM or SYS_SAFECOPYTO or SYS_VSAFECOPY
 *
 * The parameters for this kernel call are:
 *    	m_lsys_kern_safecopy.from_to	other endpoint
 *    	m_lsys_kern_safecopy.gid	grant id
 *    	m_lsys_kern_safecopy.offset	offset within granted space
 *	m_lsys_kern_safecopy.address	address in own address space
 *    	m_lsys_kern_safecopy.bytes	bytes to be copied
 *
 * For the vectored variant (do_vsafecopy):
 *      m_lsys_kern_vsafecopy.vec_addr   address of vector
 *      m_lsys_kern_vsafecopy.vec_size   number of significant elements in vector
 */

#అన్తర్భావ <assert.h>

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ "kernel/vm.h"

#నిర్వచించు MAX_పరోక్ష_లోతు 5	/* up to how many indirect grants to follow? */

#నిర్వచించు MEM_శిఖరం 0xFFFFFFFFUL

స్థిర పూర్ణాంకం safecopy(నిర్మాణం proc *, endpoint_t, endpoint_t,
	cp_grant_id_t, size_t, vir_బైట్లు, vir_బైట్లు, పూర్ణాంకం);

#నిర్వచించు HASGRANTTABLE(gr) \
	(priv(gr) && priv(gr)->s_grant_పట్టిక)

నిర్మాణం cp_sfinfo {		/* information for handling soft faults */
	పూర్ణాంకం try;		/* if nonzero, try copy only, stop on fault */
	endpoint_t endpt;	/* endpoint owning grant with CPF_TRY flag */
	vir_బైట్లు addr;		/* address to write mark upon soft fault */
	cp_grant_id_t విలువ;	/* grant ID to use as mark value to write */
};

/*===========================================================================*
 *				verify_grant				     *
 *===========================================================================*/
పూర్ణాంకం verify_grant(
  endpoint_t granter,		/* copyee */
  endpoint_t grantee,		/* copyer */
  cp_grant_id_t grant,		/* grant id */
  vir_బైట్లు బైట్లు,		/* copy size */
  పూర్ణాంకం access,			/* direction (read/write) */
  vir_బైట్లు స్థానభ్రంశం_లోపల,		/* copy offset within grant */
  vir_బైట్లు *స్థానభ్రంశం_ఫలితం,	/* copy offset within virtual address space */
  endpoint_t *e_granter,	/* new granter (magic grants) */
  నిర్మాణం cp_sfinfo *sfinfo	/* storage for soft fault information */
)
{
	cp_grant_t g;
	పూర్ణాంకం proc_nr;
	స్థిరరాశి నిర్మాణం proc *granter_proc;
	పూర్ణాంకం grant_idx, grant_seq;
	పూర్ణాంకం లోతు = 0;

	చేయి {
		/* Get granter process slot (if valid), and check range of
		 * grant id.
		 */
		ఒకవేళ(!isokendpt(granter, &proc_nr) ) {
			రాయి(
			"grant verify failed: invalid granter %d\n", (పూర్ణాంకం) granter);
			తిరిగి(EINVAL);
		}
		ఒకవేళ(!GRANT_VALID(grant)) {
			రాయి(
			"grant verify failed: invalid grant %d\n", (పూర్ణాంకం) grant);
			తిరిగి(EINVAL);
		}
		granter_proc = proc_addr(proc_nr);

		/* If the granter has a temporary grant table, always allow
		 * requests with unspecified access and return ENOTREADY if
		 * no grant table is present or if the grantee's endpoint is not
		 * the endpoint the table belongs to. When ENOTREADY is returned
		 * the same verify_grant() request will be replayed again in a
		 * while until the grant table is final. This is necessary to
		 * avoid races at live update time.
		 */
		ఒకవేళ(priv(granter_proc)->s_grant_endpoint != granter_proc->p_endpoint) {
			ఒకవేళ(!access) {
				తిరిగి OK;
			}
			లేకపోతే ఒకవేళ(!HASGRANTTABLE(granter_proc) || grantee != priv(granter_proc)->s_grant_endpoint) {
				తిరిగి ENOTREADY;
			}
		}

		/* If there is no priv. structure, or no grant table in the
		 * priv. structure, or the grant table in the priv. structure
		 * is too small for the grant, return EPERM.
		 */
		ఒకవేళ(!HASGRANTTABLE(granter_proc)) {
			రాయి(
			"grant verify failed: granter %d has no grant table\n",
			granter);
			తిరిగి(EPERM);
		}

		grant_idx = GRANT_IDX(grant);
		grant_seq = GRANT_SEQ(grant);

		ఒకవేళ(priv(granter_proc)->s_grant_entries <= grant_idx) {
				రాయి(
				"verify_grant: grant verify failed in ep %d "
				"proc %d: grant 0x%x (#%d) out of range "
				"for table size %d\n",
					granter, proc_nr, grant, grant_idx,
					priv(granter_proc)->s_grant_entries);
			తిరిగి(EPERM);
		}

		/* Copy the grant entry corresponding to this ID's index to see
		 * what it looks like. If it fails, hide the fact that granter
		 * has (presumably) set an invalid grant table entry by
		 * returning EPERM, just like with an invalid grant id.
		 */
		ఒకవేళ(దత్తాంశం_copy(granter, priv(granter_proc)->s_grant_పట్టిక +
			పరిమాణం(g) * grant_idx,
			KERNEL, (vir_బైట్లు) &g, పరిమాణం(g)) != OK) {
			రాయి(
			"verify_grant: grant verify: data_copy failed\n");
			తిరిగి EPERM;
		}

		/* Check validity: flags and sequence number. */
		ఒకవేళ((g.cp_flags & (CPF_వాడిన | CPF_VALID)) !=
			(CPF_వాడిన | CPF_VALID)) {
			రాయి("verify_grant: grant failed: invalid flags "
			    "(0x%x, 0x%lx)\n", grant, g.cp_flags);
			తిరిగి EPERM;
		}

		ఒకవేళ (g.cp_seq != grant_seq) {
			రాయి("verify_grant: grant failed: invalid sequence "
			    "(0x%x, %d vs %d)\n", grant, grant_seq, g.cp_seq);
			తిరిగి EPERM;
		}

		/* The given grant may be an indirect grant, that is, a grant
		 * that provides permission to use a grant given to the
		 * granter (i.e., for which it is the grantee). This can lead
		 * to a chain of indirect grants which must be followed back.
		 */
		ఒకవేళ((g.cp_flags & CPF_పరోక్ష)) {
			/* Stop after a few iterations. There may be a loop. */
			ఒకవేళ (లోతు == MAX_పరోక్ష_లోతు) {
				రాయి(
					"verify grant: indirect grant verify "
					"failed: exceeded maximum depth\n");
				తిరిగి ELOOP;
			}
			లోతు++;

			/* Verify actual grantee. */
			ఒకవేళ(g.cp_u.cp_పరోక్ష.cp_who_గమ్యం != grantee &&
				grantee != ANY &&
				g.cp_u.cp_పరోక్ష.cp_who_గమ్యం != ANY) {
				రాయి(
					"verify_grant: indirect grant verify "
					"failed: bad grantee\n");
				తిరిగి EPERM;
			}

			/* Start over with new granter, grant, and grantee. */
			grantee = granter;
			granter = g.cp_u.cp_పరోక్ష.cp_who_from;
			grant = g.cp_u.cp_పరోక్ష.cp_grant;
		}
	} అయితే(g.cp_flags & CPF_పరోక్ష);

	/* Check access of grant. */
	ఒకవేళ(((g.cp_flags & access) != access)) {
		రాయి(
	"verify_grant: grant verify failed: access invalid; want 0x%x, have 0x%x\n",
			access, g.cp_flags);
		తిరిగి EPERM;
	}

	ఒకవేళ((g.cp_flags & CPF_ప్రత్యక్ష)) {
		/* Don't fiddle around with grants that wrap, arithmetic
		 * below may be confused.
		 */
		ఒకవేళ(MEM_శిఖరం - g.cp_u.cp_ప్రత్యక్ష.cp_len + 1 <
			g.cp_u.cp_ప్రత్యక్ష.cp_ప్రారంభం) {
			రాయి(
		"verify_grant: direct grant verify failed: len too long\n");
			తిరిగి EPERM;
		}

		/* Verify actual grantee. */
		ఒకవేళ(g.cp_u.cp_ప్రత్యక్ష.cp_who_గమ్యం != grantee && grantee != ANY
			&& g.cp_u.cp_ప్రత్యక్ష.cp_who_గమ్యం != ANY) {
			రాయి(
		"verify_grant: direct grant verify failed: bad grantee\n");
			తిరిగి EPERM;
		}

		/* Verify actual copy range. */
		ఒకవేళ((స్థానభ్రంశం_లోపల+బైట్లు < స్థానభ్రంశం_లోపల) ||
		    స్థానభ్రంశం_లోపల+బైట్లు > g.cp_u.cp_ప్రత్యక్ష.cp_len) {
			రాయి(
		"verify_grant: direct grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_ప్రత్యక్ష.cp_len,
				g.cp_u.cp_ప్రత్యక్ష.cp_ప్రారంభం,
				బైట్లు, స్థానభ్రంశం_లోపల);
			తిరిగి EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*స్థానభ్రంశం_ఫలితం = g.cp_u.cp_ప్రత్యక్ష.cp_ప్రారంభం + స్థానభ్రంశం_లోపల;
		*e_granter = granter;
	} లేకపోతే ఒకవేళ(g.cp_flags & CPF_MAGIC) {
		/* Currently, it is hardcoded that only VFS and MIB may do
		 * magic grants.  TODO: this should be a system.conf flag.
		 */
		ఒకవేళ(granter != VFS_PROC_NR && granter != MIB_PROC_NR) {
			రాయి(
		"verify_grant: magic grant verify failed: granter (%d) "
		"not allowed\n", granter);
			తిరిగి EPERM;
		}

		/* Verify actual grantee. */
		ఒకవేళ(g.cp_u.cp_magic.cp_who_గమ్యం != grantee && grantee != ANY
			&& g.cp_u.cp_ప్రత్యక్ష.cp_who_గమ్యం != ANY) {
			రాయి(
		"verify_grant: magic grant verify failed: bad grantee\n");
			తిరిగి EPERM;
		}

		/* Verify actual copy range. */
		ఒకవేళ((స్థానభ్రంశం_లోపల+బైట్లు < స్థానభ్రంశం_లోపల) ||
		    స్థానభ్రంశం_లోపల+బైట్లు > g.cp_u.cp_magic.cp_len) {
			రాయి(
		"verify_grant: magic grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_magic.cp_len,
				g.cp_u.cp_magic.cp_ప్రారంభం,
				బైట్లు, స్థానభ్రంశం_లోపల);
			తిరిగి EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*స్థానభ్రంశం_ఫలితం = g.cp_u.cp_magic.cp_ప్రారంభం + స్థానభ్రంశం_లోపల;
		*e_granter = g.cp_u.cp_magic.cp_who_from;
	} లేకపోతే {
		రాయి(
		"verify_grant: grant verify failed: unknown grant type\n");
		తిరిగి EPERM;
	}

	/* If requested, store information regarding soft faults. */
	ఒకవేళ (sfinfo != ఖాళీ && (sfinfo->try = !!(g.cp_flags & CPF_TRY))) {
		sfinfo->endpt = granter;
		sfinfo->addr = priv(granter_proc)->s_grant_పట్టిక +
		    పరిమాణం(g) * grant_idx + offsetof(cp_grant_t, cp_faulted);
		sfinfo->విలువ = grant;
	}

	తిరిగి OK;
}

/*===========================================================================*
 *				safecopy				     *
 *===========================================================================*/
స్థిర పూర్ణాంకం safecopy(
  నిర్మాణం proc * caller,
  endpoint_t granter,
  endpoint_t grantee,
  cp_grant_id_t grantid,
  size_t బైట్లు,
  vir_బైట్లు g_స్థానభ్రంశం,
  vir_బైట్లు addr,
  పూర్ణాంకం access			/* CPF_READ for a copy from granter to grantee, CPF_WRITE
				 * for a copy from grantee to granter.
				 */
)
{
	స్థిర నిర్మాణం vir_addr v_src, v_dst;
	స్థిర vir_బైట్లు v_స్థానభ్రంశం;
	endpoint_t new_granter, *src, *dst;
	పూర్ణాంకం r;
	నిర్మాణం cp_sfinfo sfinfo;

	ఒకవేళ(granter == NONE || grantee == NONE) {
		రాయి("safecopy: nonsense processes\n");
		తిరిగి EFAULT;
	}

	/* Decide who is src and who is dst. */
	ఒకవేళ(access & CPF_READ) {
		src = &granter;
		dst = &grantee;
	} లేకపోతే {
		src = &grantee;
		dst = &granter;
	}

	/* Verify permission exists. */
	ఒకవేళ((r=verify_grant(granter, grantee, grantid, బైట్లు, access,
	    g_స్థానభ్రంశం, &v_స్థానభ్రంశం, &new_granter, &sfinfo)) != OK) {
		ఒకవేళ(r == ENOTREADY) తిరిగి r;
			రాయి(
		"grant %d verify to copy %d->%d by %d failed: err %d\n",
				grantid, *src, *dst, grantee, r);
		తిరిగి r;
	}

	/* verify_grant() can redirect the grantee to someone else,
	 * meaning the source or destination changes.
	 */
	granter = new_granter;

	/* Now it's a regular copy. */
	v_src.proc_nr_e = *src;
	v_dst.proc_nr_e = *dst;

	/* Now the offset in virtual addressing is known in 'offset'.
	 * Depending on the access, this is the source or destination
	 * address.
	 */
	ఒకవేళ(access & CPF_READ) {
		v_src.స్థానభ్రంశం = v_స్థానభ్రంశం;
		v_dst.స్థానభ్రంశం = (vir_బైట్లు) addr;
	} లేకపోతే {
		v_src.స్థానభ్రంశం = (vir_బైట్లు) addr;
		v_dst.స్థానభ్రంశం = v_స్థానభ్రంశం;
	}

	/* Do the regular copy. */
	ఒకవేళ (sfinfo.try) {
		/*
		 * Try copying without transparently faulting in pages.
		 * TODO: while CPF_TRY is meant to protect against deadlocks on
		 * memory-mapped files in file systems, it seems that this case
		 * triggers faults a whole lot more often, resulting in extra
		 * overhead due to retried file system operations.  It might be
		 * a good idea to go through VM even in this case, and have VM
		 * fail (only) if the affected page belongs to a file mapping.
		 */
		r = virtual_copy(&v_src, &v_dst, బైట్లు);
		ఒకవేళ (r == EFAULT_SRC || r == EFAULT_DST) {
			/*
			 * Mark the magic grant as having experienced a soft
			 * fault during its lifetime.  The exact value does not
			 * matter, but we use the grant ID (including its
			 * sequence number) as a form of protection in the
			 * light of CPU concurrency.
			 */
			r = దత్తాంశం_copy(KERNEL, (vir_బైట్లు)&sfinfo.విలువ,
			    sfinfo.endpt, sfinfo.addr, పరిమాణం(sfinfo.విలువ));
			/*
			 * Failure means the creator of the magic grant messed
			 * up, which can only be unintentional, so report..
			 */
			ఒకవేళ (r != OK)
				రాయి("Kernel: writing soft fault marker %d "
				    "into %d at 0x%lx failed (%d)\n",
				    sfinfo.విలువ, sfinfo.endpt, sfinfo.addr,
				    r);

			తిరిగి EFAULT;
		}
		తిరిగి r;
	}
	తిరిగి virtual_copy_vmcheck(caller, &v_src, &v_dst, బైట్లు);
}

/*===========================================================================*
 *				do_safecopy_to				     *
 *===========================================================================*/
పూర్ణాంకం do_safecopy_గమ్యం(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
	తిరిగి safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_గమ్యం, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.బైట్లు, m_ptr->m_lsys_kern_safecopy.స్థానభ్రంశం,
		(vir_బైట్లు) m_ptr->m_lsys_kern_safecopy.చిరునామా, CPF_WRITE);
}

/*===========================================================================*
 *				do_safecopy_from			     *
 *===========================================================================*/
పూర్ణాంకం do_safecopy_from(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
	తిరిగి safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_గమ్యం, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.బైట్లు, m_ptr->m_lsys_kern_safecopy.స్థానభ్రంశం,
		(vir_బైట్లు) m_ptr->m_lsys_kern_safecopy.చిరునామా, CPF_READ);
}

/*===========================================================================*
 *				do_vsafecopy				     *
 *===========================================================================*/
పూర్ణాంకం do_vsafecopy(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
	స్థిర నిర్మాణం vscp_vec vec[SCPVEC_NR];
	స్థిర నిర్మాణం vir_addr src, dst;
	పూర్ణాంకం r, i, els;
	size_t బైట్లు;

	/* Set vector copy parameters. */
	src.proc_nr_e = caller->p_endpoint;
	ప్రతిజ్ఞా(src.proc_nr_e != NONE);
	src.స్థానభ్రంశం = (vir_బైట్లు) m_ptr->m_lsys_kern_vsafecopy.vec_addr;
	dst.proc_nr_e = KERNEL;
	dst.స్థానభ్రంశం = (vir_బైట్లు) vec;

	/* No. of vector elements. */
	els = m_ptr->m_lsys_kern_vsafecopy.vec_కొలత;
	బైట్లు = els * పరిమాణం(నిర్మాణం vscp_vec);

	/* Obtain vector of copies. */
	ఒకవేళ((r=virtual_copy_vmcheck(caller, &src, &dst, బైట్లు)) != OK)
		తిరిగి r;

	/* Perform safecopies. */
	కొరకు(i = 0; i < els; i++) {
		పూర్ణాంకం access;
		endpoint_t granter;
		ఒకవేళ(vec[i].v_from == SELF) {
			access = CPF_WRITE;
			granter = vec[i].v_గమ్యం;
		} లేకపోతే ఒకవేళ(vec[i].v_గమ్యం == SELF) {
			access = CPF_READ;
			granter = vec[i].v_from;
		} లేకపోతే {
			రాయి("vsafecopy: %d: element %d/%d: no SELF found\n",
				caller->p_endpoint, i, els);
			తిరిగి EINVAL;
		}

		/* Do safecopy for this element. */
		ఒకవేళ((r=safecopy(caller, granter, caller->p_endpoint,
			vec[i].v_gid,
			vec[i].v_బైట్లు, vec[i].v_స్థానభ్రంశం,
			vec[i].v_addr, access)) != OK) {
			తిరిగి r;
		}
	}

	తిరిగి OK;
}

