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

#ضمن <assert.h>

#ضمن "kernel/system.h"
#ضمن "kernel/vm.h"

#وصف_ڪريو MAX_اڻسڌو_اونهائي 5	/* up to how many indirect grants to follow? */

#وصف_ڪريو MEM_چوٽي 0xFFFFFFFFUL

ساكن صحيح safecopy(بنية proc *, endpoint_t, endpoint_t,
	cp_grant_id_t, size_t, vir_بائيٽون, vir_بائيٽون, صحيح);

#وصف_ڪريو HASGRANTTABLE(gr) \
	(priv(gr) && priv(gr)->s_grant_جدول)

بنية cp_sfinfo {		/* information for handling soft faults */
	صحيح try;		/* if nonzero, try copy only, stop on fault */
	endpoint_t endpt;	/* endpoint owning grant with CPF_TRY flag */
	vir_بائيٽون addr;		/* address to write mark upon soft fault */
	cp_grant_id_t قدر;	/* grant ID to use as mark value to write */
};

/*===========================================================================*
 *				verify_grant				     *
 *===========================================================================*/
صحيح verify_grant(
  endpoint_t granter,		/* copyee */
  endpoint_t grantee,		/* copyer */
  cp_grant_id_t grant,		/* grant id */
  vir_بائيٽون بائيٽون,		/* copy size */
  صحيح access,			/* direction (read/write) */
  vir_بائيٽون وٿي_اندريون,		/* copy offset within grant */
  vir_بائيٽون *وٿي_نتيجو,	/* copy offset within virtual address space */
  endpoint_t *e_granter,	/* new granter (magic grants) */
  بنية cp_sfinfo *sfinfo	/* storage for soft fault information */
)
{
	cp_grant_t g;
	صحيح proc_nr;
	ثابت بنية proc *granter_proc;
	صحيح grant_idx, grant_seq;
	صحيح اونهائي = 0;

	افعل {
		/* Get granter process slot (if valid), and check range of
		 * grant id.
		 */
		إذا(!isokendpt(granter, &proc_nr) ) {
			لکو(
			"grant verify failed: invalid granter %d\n", (صحيح) granter);
			أرجع(EINVAL);
		}
		إذا(!GRANT_VALID(grant)) {
			لکو(
			"grant verify failed: invalid grant %d\n", (صحيح) grant);
			أرجع(EINVAL);
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
		إذا(priv(granter_proc)->s_grant_endpoint != granter_proc->p_endpoint) {
			إذا(!access) {
				أرجع OK;
			}
			وإلا إذا(!HASGRANTTABLE(granter_proc) || grantee != priv(granter_proc)->s_grant_endpoint) {
				أرجع ENOTREADY;
			}
		}

		/* If there is no priv. structure, or no grant table in the
		 * priv. structure, or the grant table in the priv. structure
		 * is too small for the grant, return EPERM.
		 */
		إذا(!HASGRANTTABLE(granter_proc)) {
			لکو(
			"grant verify failed: granter %d has no grant table\n",
			granter);
			أرجع(EPERM);
		}

		grant_idx = GRANT_IDX(grant);
		grant_seq = GRANT_SEQ(grant);

		إذا(priv(granter_proc)->s_grant_entries <= grant_idx) {
				لکو(
				"verify_grant: grant verify failed in ep %d "
				"proc %d: grant 0x%x (#%d) out of range "
				"for table size %d\n",
					granter, proc_nr, grant, grant_idx,
					priv(granter_proc)->s_grant_entries);
			أرجع(EPERM);
		}

		/* Copy the grant entry corresponding to this ID's index to see
		 * what it looks like. If it fails, hide the fact that granter
		 * has (presumably) set an invalid grant table entry by
		 * returning EPERM, just like with an invalid grant id.
		 */
		إذا(معلومات_copy(granter, priv(granter_proc)->s_grant_جدول +
			حجم(g) * grant_idx,
			KERNEL, (vir_بائيٽون) &g, حجم(g)) != OK) {
			لکو(
			"verify_grant: grant verify: data_copy failed\n");
			أرجع EPERM;
		}

		/* Check validity: flags and sequence number. */
		إذا((g.cp_flags & (CPF_استعمال_ٿيل | CPF_VALID)) !=
			(CPF_استعمال_ٿيل | CPF_VALID)) {
			لکو("verify_grant: grant failed: invalid flags "
			    "(0x%x, 0x%lx)\n", grant, g.cp_flags);
			أرجع EPERM;
		}

		إذا (g.cp_seq != grant_seq) {
			لکو("verify_grant: grant failed: invalid sequence "
			    "(0x%x, %d vs %d)\n", grant, grant_seq, g.cp_seq);
			أرجع EPERM;
		}

		/* The given grant may be an indirect grant, that is, a grant
		 * that provides permission to use a grant given to the
		 * granter (i.e., for which it is the grantee). This can lead
		 * to a chain of indirect grants which must be followed back.
		 */
		إذا((g.cp_flags & CPF_اڻسڌو)) {
			/* Stop after a few iterations. There may be a loop. */
			إذا (اونهائي == MAX_اڻسڌو_اونهائي) {
				لکو(
					"verify grant: indirect grant verify "
					"failed: exceeded maximum depth\n");
				أرجع ELOOP;
			}
			اونهائي++;

			/* Verify actual grantee. */
			إذا(g.cp_u.cp_اڻسڌو.cp_who_منزل != grantee &&
				grantee != ANY &&
				g.cp_u.cp_اڻسڌو.cp_who_منزل != ANY) {
				لکو(
					"verify_grant: indirect grant verify "
					"failed: bad grantee\n");
				أرجع EPERM;
			}

			/* Start over with new granter, grant, and grantee. */
			grantee = granter;
			granter = g.cp_u.cp_اڻسڌو.cp_who_from;
			grant = g.cp_u.cp_اڻسڌو.cp_grant;
		}
	} طالما(g.cp_flags & CPF_اڻسڌو);

	/* Check access of grant. */
	إذا(((g.cp_flags & access) != access)) {
		لکو(
	"verify_grant: grant verify failed: access invalid; want 0x%x, have 0x%x\n",
			access, g.cp_flags);
		أرجع EPERM;
	}

	إذا((g.cp_flags & CPF_سڌو)) {
		/* Don't fiddle around with grants that wrap, arithmetic
		 * below may be confused.
		 */
		إذا(MEM_چوٽي - g.cp_u.cp_سڌو.cp_len + 1 <
			g.cp_u.cp_سڌو.cp_شروعات) {
			لکو(
		"verify_grant: direct grant verify failed: len too long\n");
			أرجع EPERM;
		}

		/* Verify actual grantee. */
		إذا(g.cp_u.cp_سڌو.cp_who_منزل != grantee && grantee != ANY
			&& g.cp_u.cp_سڌو.cp_who_منزل != ANY) {
			لکو(
		"verify_grant: direct grant verify failed: bad grantee\n");
			أرجع EPERM;
		}

		/* Verify actual copy range. */
		إذا((وٿي_اندريون+بائيٽون < وٿي_اندريون) ||
		    وٿي_اندريون+بائيٽون > g.cp_u.cp_سڌو.cp_len) {
			لکو(
		"verify_grant: direct grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_سڌو.cp_len,
				g.cp_u.cp_سڌو.cp_شروعات,
				بائيٽون, وٿي_اندريون);
			أرجع EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*وٿي_نتيجو = g.cp_u.cp_سڌو.cp_شروعات + وٿي_اندريون;
		*e_granter = granter;
	} وإلا إذا(g.cp_flags & CPF_MAGIC) {
		/* Currently, it is hardcoded that only VFS and MIB may do
		 * magic grants.  TODO: this should be a system.conf flag.
		 */
		إذا(granter != VFS_PROC_NR && granter != MIB_PROC_NR) {
			لکو(
		"verify_grant: magic grant verify failed: granter (%d) "
		"not allowed\n", granter);
			أرجع EPERM;
		}

		/* Verify actual grantee. */
		إذا(g.cp_u.cp_magic.cp_who_منزل != grantee && grantee != ANY
			&& g.cp_u.cp_سڌو.cp_who_منزل != ANY) {
			لکو(
		"verify_grant: magic grant verify failed: bad grantee\n");
			أرجع EPERM;
		}

		/* Verify actual copy range. */
		إذا((وٿي_اندريون+بائيٽون < وٿي_اندريون) ||
		    وٿي_اندريون+بائيٽون > g.cp_u.cp_magic.cp_len) {
			لکو(
		"verify_grant: magic grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_magic.cp_len,
				g.cp_u.cp_magic.cp_شروعات,
				بائيٽون, وٿي_اندريون);
			أرجع EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*وٿي_نتيجو = g.cp_u.cp_magic.cp_شروعات + وٿي_اندريون;
		*e_granter = g.cp_u.cp_magic.cp_who_from;
	} وإلا {
		لکو(
		"verify_grant: grant verify failed: unknown grant type\n");
		أرجع EPERM;
	}

	/* If requested, store information regarding soft faults. */
	إذا (sfinfo != عدم && (sfinfo->try = !!(g.cp_flags & CPF_TRY))) {
		sfinfo->endpt = granter;
		sfinfo->addr = priv(granter_proc)->s_grant_جدول +
		    حجم(g) * grant_idx + offsetof(cp_grant_t, cp_faulted);
		sfinfo->قدر = grant;
	}

	أرجع OK;
}

/*===========================================================================*
 *				safecopy				     *
 *===========================================================================*/
ساكن صحيح safecopy(
  بنية proc * caller,
  endpoint_t granter,
  endpoint_t grantee,
  cp_grant_id_t grantid,
  size_t بائيٽون,
  vir_بائيٽون g_وٿي,
  vir_بائيٽون addr,
  صحيح access			/* CPF_READ for a copy from granter to grantee, CPF_WRITE
				 * for a copy from grantee to granter.
				 */
)
{
	ساكن بنية vir_addr v_src, v_dst;
	ساكن vir_بائيٽون v_وٿي;
	endpoint_t new_granter, *src, *dst;
	صحيح r;
	بنية cp_sfinfo sfinfo;

	إذا(granter == NONE || grantee == NONE) {
		لکو("safecopy: nonsense processes\n");
		أرجع EFAULT;
	}

	/* Decide who is src and who is dst. */
	إذا(access & CPF_READ) {
		src = &granter;
		dst = &grantee;
	} وإلا {
		src = &grantee;
		dst = &granter;
	}

	/* Verify permission exists. */
	إذا((r=verify_grant(granter, grantee, grantid, بائيٽون, access,
	    g_وٿي, &v_وٿي, &new_granter, &sfinfo)) != OK) {
		إذا(r == ENOTREADY) أرجع r;
			لکو(
		"grant %d verify to copy %d->%d by %d failed: err %d\n",
				grantid, *src, *dst, grantee, r);
		أرجع r;
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
	إذا(access & CPF_READ) {
		v_src.وٿي = v_وٿي;
		v_dst.وٿي = (vir_بائيٽون) addr;
	} وإلا {
		v_src.وٿي = (vir_بائيٽون) addr;
		v_dst.وٿي = v_وٿي;
	}

	/* Do the regular copy. */
	إذا (sfinfo.try) {
		/*
		 * Try copying without transparently faulting in pages.
		 * TODO: while CPF_TRY is meant to protect against deadlocks on
		 * memory-mapped files in file systems, it seems that this case
		 * triggers faults a whole lot more often, resulting in extra
		 * overhead due to retried file system operations.  It might be
		 * a good idea to go through VM even in this case, and have VM
		 * fail (only) if the affected page belongs to a file mapping.
		 */
		r = virtual_copy(&v_src, &v_dst, بائيٽون);
		إذا (r == EFAULT_SRC || r == EFAULT_DST) {
			/*
			 * Mark the magic grant as having experienced a soft
			 * fault during its lifetime.  The exact value does not
			 * matter, but we use the grant ID (including its
			 * sequence number) as a form of protection in the
			 * light of CPU concurrency.
			 */
			r = معلومات_copy(KERNEL, (vir_بائيٽون)&sfinfo.قدر,
			    sfinfo.endpt, sfinfo.addr, حجم(sfinfo.قدر));
			/*
			 * Failure means the creator of the magic grant messed
			 * up, which can only be unintentional, so report..
			 */
			إذا (r != OK)
				لکو("Kernel: writing soft fault marker %d "
				    "into %d at 0x%lx failed (%d)\n",
				    sfinfo.قدر, sfinfo.endpt, sfinfo.addr,
				    r);

			أرجع EFAULT;
		}
		أرجع r;
	}
	أرجع virtual_copy_vmcheck(caller, &v_src, &v_dst, بائيٽون);
}

/*===========================================================================*
 *				do_safecopy_to				     *
 *===========================================================================*/
صحيح do_safecopy_منزل(بنية proc * caller, پيغام * m_ptr)
{
	أرجع safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_منزل, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.بائيٽون, m_ptr->m_lsys_kern_safecopy.وٿي,
		(vir_بائيٽون) m_ptr->m_lsys_kern_safecopy.پتو, CPF_WRITE);
}

/*===========================================================================*
 *				do_safecopy_from			     *
 *===========================================================================*/
صحيح do_safecopy_from(بنية proc * caller, پيغام * m_ptr)
{
	أرجع safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_منزل, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.بائيٽون, m_ptr->m_lsys_kern_safecopy.وٿي,
		(vir_بائيٽون) m_ptr->m_lsys_kern_safecopy.پتو, CPF_READ);
}

/*===========================================================================*
 *				do_vsafecopy				     *
 *===========================================================================*/
صحيح do_vsafecopy(بنية proc * caller, پيغام * m_ptr)
{
	ساكن بنية vscp_vec vec[SCPVEC_NR];
	ساكن بنية vir_addr src, dst;
	صحيح r, i, els;
	size_t بائيٽون;

	/* Set vector copy parameters. */
	src.proc_nr_e = caller->p_endpoint;
	أكد(src.proc_nr_e != NONE);
	src.وٿي = (vir_بائيٽون) m_ptr->m_lsys_kern_vsafecopy.vec_addr;
	dst.proc_nr_e = KERNEL;
	dst.وٿي = (vir_بائيٽون) vec;

	/* No. of vector elements. */
	els = m_ptr->m_lsys_kern_vsafecopy.vec_سائيز;
	بائيٽون = els * حجم(بنية vscp_vec);

	/* Obtain vector of copies. */
	إذا((r=virtual_copy_vmcheck(caller, &src, &dst, بائيٽون)) != OK)
		أرجع r;

	/* Perform safecopies. */
	لكل(i = 0; i < els; i++) {
		صحيح access;
		endpoint_t granter;
		إذا(vec[i].v_from == SELF) {
			access = CPF_WRITE;
			granter = vec[i].v_منزل;
		} وإلا إذا(vec[i].v_منزل == SELF) {
			access = CPF_READ;
			granter = vec[i].v_from;
		} وإلا {
			لکو("vsafecopy: %d: element %d/%d: no SELF found\n",
				caller->p_endpoint, i, els);
			أرجع EINVAL;
		}

		/* Do safecopy for this element. */
		إذا((r=safecopy(caller, granter, caller->p_endpoint,
			vec[i].v_gid,
			vec[i].v_بائيٽون, vec[i].v_وٿي,
			vec[i].v_addr, access)) != OK) {
			أرجع r;
		}
	}

	أرجع OK;
}

