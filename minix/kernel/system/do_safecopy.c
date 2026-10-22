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

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਅਸਿੱਧਾ_ਡੂੰਘਾਈ 5	/* up to how many indirect grants to follow? */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MEM_ਸਿਖਰ 0xFFFFFFFFUL

ਸਥਿਰ ਪੂਰਨ safecopy(ਬਣਤਰ proc *, endpoint_t, endpoint_t,
	cp_grant_id_t, size_t, vir_ਬਾਈਟਾਂ, vir_ਬਾਈਟਾਂ, ਪੂਰਨ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASGRANTTABLE(gr) \
	(priv(gr) && priv(gr)->s_grant_ਸਾਰਣੀ)

ਬਣਤਰ cp_sfinfo {		/* information for handling soft faults */
	ਪੂਰਨ try;		/* if nonzero, try copy only, stop on fault */
	endpoint_t endpt;	/* endpoint owning grant with CPF_TRY flag */
	vir_ਬਾਈਟਾਂ addr;		/* address to write mark upon soft fault */
	cp_grant_id_t ਮੁੱਲ;	/* grant ID to use as mark value to write */
};

/*===========================================================================*
 *				verify_grant				     *
 *===========================================================================*/
ਪੂਰਨ verify_grant(
  endpoint_t granter,		/* copyee */
  endpoint_t grantee,		/* copyer */
  cp_grant_id_t grant,		/* grant id */
  vir_ਬਾਈਟਾਂ ਬਾਈਟਾਂ,		/* copy size */
  ਪੂਰਨ access,			/* direction (read/write) */
  vir_ਬਾਈਟਾਂ ਵਿੱਥ_ਵਿੱਚ,		/* copy offset within grant */
  vir_ਬਾਈਟਾਂ *ਵਿੱਥ_ਨਤੀਜਾ,	/* copy offset within virtual address space */
  endpoint_t *e_granter,	/* new granter (magic grants) */
  ਬਣਤਰ cp_sfinfo *sfinfo	/* storage for soft fault information */
)
{
	cp_grant_t g;
	ਪੂਰਨ proc_nr;
	ਅਚਲ ਬਣਤਰ proc *granter_proc;
	ਪੂਰਨ grant_idx, grant_seq;
	ਪੂਰਨ ਡੂੰਘਾਈ = 0;

	ਕਰੋ {
		/* Get granter process slot (if valid), and check range of
		 * grant id.
		 */
		ਜੇ(!isokendpt(granter, &proc_nr) ) {
			ਲਿਖੋ(
			"grant verify failed: invalid granter %d\n", (ਪੂਰਨ) granter);
			ਵਾਪਸ(EINVAL);
		}
		ਜੇ(!GRANT_VALID(grant)) {
			ਲਿਖੋ(
			"grant verify failed: invalid grant %d\n", (ਪੂਰਨ) grant);
			ਵਾਪਸ(EINVAL);
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
		ਜੇ(priv(granter_proc)->s_grant_endpoint != granter_proc->p_endpoint) {
			ਜੇ(!access) {
				ਵਾਪਸ OK;
			}
			ਨਹੀਂਤਾਂ ਜੇ(!HASGRANTTABLE(granter_proc) || grantee != priv(granter_proc)->s_grant_endpoint) {
				ਵਾਪਸ ENOTREADY;
			}
		}

		/* If there is no priv. structure, or no grant table in the
		 * priv. structure, or the grant table in the priv. structure
		 * is too small for the grant, return EPERM.
		 */
		ਜੇ(!HASGRANTTABLE(granter_proc)) {
			ਲਿਖੋ(
			"grant verify failed: granter %d has no grant table\n",
			granter);
			ਵਾਪਸ(EPERM);
		}

		grant_idx = GRANT_IDX(grant);
		grant_seq = GRANT_SEQ(grant);

		ਜੇ(priv(granter_proc)->s_grant_entries <= grant_idx) {
				ਲਿਖੋ(
				"verify_grant: grant verify failed in ep %d "
				"proc %d: grant 0x%x (#%d) out of range "
				"for table size %d\n",
					granter, proc_nr, grant, grant_idx,
					priv(granter_proc)->s_grant_entries);
			ਵਾਪਸ(EPERM);
		}

		/* Copy the grant entry corresponding to this ID's index to see
		 * what it looks like. If it fails, hide the fact that granter
		 * has (presumably) set an invalid grant table entry by
		 * returning EPERM, just like with an invalid grant id.
		 */
		ਜੇ(ਅੰਕੜੇ_copy(granter, priv(granter_proc)->s_grant_ਸਾਰਣੀ +
			ਆਕਾਰ(g) * grant_idx,
			KERNEL, (vir_ਬਾਈਟਾਂ) &g, ਆਕਾਰ(g)) != OK) {
			ਲਿਖੋ(
			"verify_grant: grant verify: data_copy failed\n");
			ਵਾਪਸ EPERM;
		}

		/* Check validity: flags and sequence number. */
		ਜੇ((g.cp_flags & (CPF_ਵਰਤਿਆ | CPF_VALID)) !=
			(CPF_ਵਰਤਿਆ | CPF_VALID)) {
			ਲਿਖੋ("verify_grant: grant failed: invalid flags "
			    "(0x%x, 0x%lx)\n", grant, g.cp_flags);
			ਵਾਪਸ EPERM;
		}

		ਜੇ (g.cp_seq != grant_seq) {
			ਲਿਖੋ("verify_grant: grant failed: invalid sequence "
			    "(0x%x, %d vs %d)\n", grant, grant_seq, g.cp_seq);
			ਵਾਪਸ EPERM;
		}

		/* The given grant may be an indirect grant, that is, a grant
		 * that provides permission to use a grant given to the
		 * granter (i.e., for which it is the grantee). This can lead
		 * to a chain of indirect grants which must be followed back.
		 */
		ਜੇ((g.cp_flags & CPF_ਅਸਿੱਧਾ)) {
			/* Stop after a few iterations. There may be a loop. */
			ਜੇ (ਡੂੰਘਾਈ == MAX_ਅਸਿੱਧਾ_ਡੂੰਘਾਈ) {
				ਲਿਖੋ(
					"verify grant: indirect grant verify "
					"failed: exceeded maximum depth\n");
				ਵਾਪਸ ELOOP;
			}
			ਡੂੰਘਾਈ++;

			/* Verify actual grantee. */
			ਜੇ(g.cp_u.cp_ਅਸਿੱਧਾ.cp_who_ਮੰਜ਼ਿਲ != grantee &&
				grantee != ANY &&
				g.cp_u.cp_ਅਸਿੱਧਾ.cp_who_ਮੰਜ਼ਿਲ != ANY) {
				ਲਿਖੋ(
					"verify_grant: indirect grant verify "
					"failed: bad grantee\n");
				ਵਾਪਸ EPERM;
			}

			/* Start over with new granter, grant, and grantee. */
			grantee = granter;
			granter = g.cp_u.cp_ਅਸਿੱਧਾ.cp_who_from;
			grant = g.cp_u.cp_ਅਸਿੱਧਾ.cp_grant;
		}
	} ਜਦੋਂ(g.cp_flags & CPF_ਅਸਿੱਧਾ);

	/* Check access of grant. */
	ਜੇ(((g.cp_flags & access) != access)) {
		ਲਿਖੋ(
	"verify_grant: grant verify failed: access invalid; want 0x%x, have 0x%x\n",
			access, g.cp_flags);
		ਵਾਪਸ EPERM;
	}

	ਜੇ((g.cp_flags & CPF_ਸਿੱਧਾ)) {
		/* Don't fiddle around with grants that wrap, arithmetic
		 * below may be confused.
		 */
		ਜੇ(MEM_ਸਿਖਰ - g.cp_u.cp_ਸਿੱਧਾ.cp_len + 1 <
			g.cp_u.cp_ਸਿੱਧਾ.cp_ਸ਼ੁਰੂ) {
			ਲਿਖੋ(
		"verify_grant: direct grant verify failed: len too long\n");
			ਵਾਪਸ EPERM;
		}

		/* Verify actual grantee. */
		ਜੇ(g.cp_u.cp_ਸਿੱਧਾ.cp_who_ਮੰਜ਼ਿਲ != grantee && grantee != ANY
			&& g.cp_u.cp_ਸਿੱਧਾ.cp_who_ਮੰਜ਼ਿਲ != ANY) {
			ਲਿਖੋ(
		"verify_grant: direct grant verify failed: bad grantee\n");
			ਵਾਪਸ EPERM;
		}

		/* Verify actual copy range. */
		ਜੇ((ਵਿੱਥ_ਵਿੱਚ+ਬਾਈਟਾਂ < ਵਿੱਥ_ਵਿੱਚ) ||
		    ਵਿੱਥ_ਵਿੱਚ+ਬਾਈਟਾਂ > g.cp_u.cp_ਸਿੱਧਾ.cp_len) {
			ਲਿਖੋ(
		"verify_grant: direct grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_ਸਿੱਧਾ.cp_len,
				g.cp_u.cp_ਸਿੱਧਾ.cp_ਸ਼ੁਰੂ,
				ਬਾਈਟਾਂ, ਵਿੱਥ_ਵਿੱਚ);
			ਵਾਪਸ EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*ਵਿੱਥ_ਨਤੀਜਾ = g.cp_u.cp_ਸਿੱਧਾ.cp_ਸ਼ੁਰੂ + ਵਿੱਥ_ਵਿੱਚ;
		*e_granter = granter;
	} ਨਹੀਂਤਾਂ ਜੇ(g.cp_flags & CPF_MAGIC) {
		/* Currently, it is hardcoded that only VFS and MIB may do
		 * magic grants.  TODO: this should be a system.conf flag.
		 */
		ਜੇ(granter != VFS_PROC_NR && granter != MIB_PROC_NR) {
			ਲਿਖੋ(
		"verify_grant: magic grant verify failed: granter (%d) "
		"not allowed\n", granter);
			ਵਾਪਸ EPERM;
		}

		/* Verify actual grantee. */
		ਜੇ(g.cp_u.cp_magic.cp_who_ਮੰਜ਼ਿਲ != grantee && grantee != ANY
			&& g.cp_u.cp_ਸਿੱਧਾ.cp_who_ਮੰਜ਼ਿਲ != ANY) {
			ਲਿਖੋ(
		"verify_grant: magic grant verify failed: bad grantee\n");
			ਵਾਪਸ EPERM;
		}

		/* Verify actual copy range. */
		ਜੇ((ਵਿੱਥ_ਵਿੱਚ+ਬਾਈਟਾਂ < ਵਿੱਥ_ਵਿੱਚ) ||
		    ਵਿੱਥ_ਵਿੱਚ+ਬਾਈਟਾਂ > g.cp_u.cp_magic.cp_len) {
			ਲਿਖੋ(
		"verify_grant: magic grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_magic.cp_len,
				g.cp_u.cp_magic.cp_ਸ਼ੁਰੂ,
				ਬਾਈਟਾਂ, ਵਿੱਥ_ਵਿੱਚ);
			ਵਾਪਸ EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*ਵਿੱਥ_ਨਤੀਜਾ = g.cp_u.cp_magic.cp_ਸ਼ੁਰੂ + ਵਿੱਥ_ਵਿੱਚ;
		*e_granter = g.cp_u.cp_magic.cp_who_from;
	} ਨਹੀਂਤਾਂ {
		ਲਿਖੋ(
		"verify_grant: grant verify failed: unknown grant type\n");
		ਵਾਪਸ EPERM;
	}

	/* If requested, store information regarding soft faults. */
	ਜੇ (sfinfo != ਨਿਰਾ && (sfinfo->try = !!(g.cp_flags & CPF_TRY))) {
		sfinfo->endpt = granter;
		sfinfo->addr = priv(granter_proc)->s_grant_ਸਾਰਣੀ +
		    ਆਕਾਰ(g) * grant_idx + offsetof(cp_grant_t, cp_faulted);
		sfinfo->ਮੁੱਲ = grant;
	}

	ਵਾਪਸ OK;
}

/*===========================================================================*
 *				safecopy				     *
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ safecopy(
  ਬਣਤਰ proc * caller,
  endpoint_t granter,
  endpoint_t grantee,
  cp_grant_id_t grantid,
  size_t ਬਾਈਟਾਂ,
  vir_ਬਾਈਟਾਂ g_ਵਿੱਥ,
  vir_ਬਾਈਟਾਂ addr,
  ਪੂਰਨ access			/* CPF_READ for a copy from granter to grantee, CPF_WRITE
				 * for a copy from grantee to granter.
				 */
)
{
	ਸਥਿਰ ਬਣਤਰ vir_addr v_src, v_dst;
	ਸਥਿਰ vir_ਬਾਈਟਾਂ v_ਵਿੱਥ;
	endpoint_t new_granter, *src, *dst;
	ਪੂਰਨ r;
	ਬਣਤਰ cp_sfinfo sfinfo;

	ਜੇ(granter == NONE || grantee == NONE) {
		ਲਿਖੋ("safecopy: nonsense processes\n");
		ਵਾਪਸ EFAULT;
	}

	/* Decide who is src and who is dst. */
	ਜੇ(access & CPF_READ) {
		src = &granter;
		dst = &grantee;
	} ਨਹੀਂਤਾਂ {
		src = &grantee;
		dst = &granter;
	}

	/* Verify permission exists. */
	ਜੇ((r=verify_grant(granter, grantee, grantid, ਬਾਈਟਾਂ, access,
	    g_ਵਿੱਥ, &v_ਵਿੱਥ, &new_granter, &sfinfo)) != OK) {
		ਜੇ(r == ENOTREADY) ਵਾਪਸ r;
			ਲਿਖੋ(
		"grant %d verify to copy %d->%d by %d failed: err %d\n",
				grantid, *src, *dst, grantee, r);
		ਵਾਪਸ r;
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
	ਜੇ(access & CPF_READ) {
		v_src.ਵਿੱਥ = v_ਵਿੱਥ;
		v_dst.ਵਿੱਥ = (vir_ਬਾਈਟਾਂ) addr;
	} ਨਹੀਂਤਾਂ {
		v_src.ਵਿੱਥ = (vir_ਬਾਈਟਾਂ) addr;
		v_dst.ਵਿੱਥ = v_ਵਿੱਥ;
	}

	/* Do the regular copy. */
	ਜੇ (sfinfo.try) {
		/*
		 * Try copying without transparently faulting in pages.
		 * TODO: while CPF_TRY is meant to protect against deadlocks on
		 * memory-mapped files in file systems, it seems that this case
		 * triggers faults a whole lot more often, resulting in extra
		 * overhead due to retried file system operations.  It might be
		 * a good idea to go through VM even in this case, and have VM
		 * fail (only) if the affected page belongs to a file mapping.
		 */
		r = virtual_copy(&v_src, &v_dst, ਬਾਈਟਾਂ);
		ਜੇ (r == EFAULT_SRC || r == EFAULT_DST) {
			/*
			 * Mark the magic grant as having experienced a soft
			 * fault during its lifetime.  The exact value does not
			 * matter, but we use the grant ID (including its
			 * sequence number) as a form of protection in the
			 * light of CPU concurrency.
			 */
			r = ਅੰਕੜੇ_copy(KERNEL, (vir_ਬਾਈਟਾਂ)&sfinfo.ਮੁੱਲ,
			    sfinfo.endpt, sfinfo.addr, ਆਕਾਰ(sfinfo.ਮੁੱਲ));
			/*
			 * Failure means the creator of the magic grant messed
			 * up, which can only be unintentional, so report..
			 */
			ਜੇ (r != OK)
				ਲਿਖੋ("Kernel: writing soft fault marker %d "
				    "into %d at 0x%lx failed (%d)\n",
				    sfinfo.ਮੁੱਲ, sfinfo.endpt, sfinfo.addr,
				    r);

			ਵਾਪਸ EFAULT;
		}
		ਵਾਪਸ r;
	}
	ਵਾਪਸ virtual_copy_vmcheck(caller, &v_src, &v_dst, ਬਾਈਟਾਂ);
}

/*===========================================================================*
 *				do_safecopy_to				     *
 *===========================================================================*/
ਪੂਰਨ do_safecopy_ਮੰਜ਼ਿਲ(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
	ਵਾਪਸ safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_ਮੰਜ਼ਿਲ, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.ਬਾਈਟਾਂ, m_ptr->m_lsys_kern_safecopy.ਵਿੱਥ,
		(vir_ਬਾਈਟਾਂ) m_ptr->m_lsys_kern_safecopy.ਪਤਾ, CPF_WRITE);
}

/*===========================================================================*
 *				do_safecopy_from			     *
 *===========================================================================*/
ਪੂਰਨ do_safecopy_from(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
	ਵਾਪਸ safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_ਮੰਜ਼ਿਲ, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.ਬਾਈਟਾਂ, m_ptr->m_lsys_kern_safecopy.ਵਿੱਥ,
		(vir_ਬਾਈਟਾਂ) m_ptr->m_lsys_kern_safecopy.ਪਤਾ, CPF_READ);
}

/*===========================================================================*
 *				do_vsafecopy				     *
 *===========================================================================*/
ਪੂਰਨ do_vsafecopy(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
	ਸਥਿਰ ਬਣਤਰ vscp_vec vec[SCPVEC_NR];
	ਸਥਿਰ ਬਣਤਰ vir_addr src, dst;
	ਪੂਰਨ r, i, els;
	size_t ਬਾਈਟਾਂ;

	/* Set vector copy parameters. */
	src.proc_nr_e = caller->p_endpoint;
	ਪ੍ਰਤਿਜ੍ਞਾ(src.proc_nr_e != NONE);
	src.ਵਿੱਥ = (vir_ਬਾਈਟਾਂ) m_ptr->m_lsys_kern_vsafecopy.vec_addr;
	dst.proc_nr_e = KERNEL;
	dst.ਵਿੱਥ = (vir_ਬਾਈਟਾਂ) vec;

	/* No. of vector elements. */
	els = m_ptr->m_lsys_kern_vsafecopy.vec_ਮਾਪ;
	ਬਾਈਟਾਂ = els * ਆਕਾਰ(ਬਣਤਰ vscp_vec);

	/* Obtain vector of copies. */
	ਜੇ((r=virtual_copy_vmcheck(caller, &src, &dst, ਬਾਈਟਾਂ)) != OK)
		ਵਾਪਸ r;

	/* Perform safecopies. */
	ਲਈ(i = 0; i < els; i++) {
		ਪੂਰਨ access;
		endpoint_t granter;
		ਜੇ(vec[i].v_from == SELF) {
			access = CPF_WRITE;
			granter = vec[i].v_ਮੰਜ਼ਿਲ;
		} ਨਹੀਂਤਾਂ ਜੇ(vec[i].v_ਮੰਜ਼ਿਲ == SELF) {
			access = CPF_READ;
			granter = vec[i].v_from;
		} ਨਹੀਂਤਾਂ {
			ਲਿਖੋ("vsafecopy: %d: element %d/%d: no SELF found\n",
				caller->p_endpoint, i, els);
			ਵਾਪਸ EINVAL;
		}

		/* Do safecopy for this element. */
		ਜੇ((r=safecopy(caller, granter, caller->p_endpoint,
			vec[i].v_gid,
			vec[i].v_ਬਾਈਟਾਂ, vec[i].v_ਵਿੱਥ,
			vec[i].v_addr, access)) != OK) {
			ਵਾਪਸ r;
		}
	}

	ਵਾਪਸ OK;
}

