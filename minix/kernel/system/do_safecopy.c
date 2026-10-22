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

#ಅನ್ತರ್ಭಾವ <assert.h>

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಪರೋಕ್ಷ_ಆಳ 5	/* up to how many indirect grants to follow? */

#ವ್ಯಾಖ್ಯಾನಿಸು MEM_ತುದಿ 0xFFFFFFFFUL

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ safecopy(ರಚನೆ proc *, endpoint_t, endpoint_t,
	cp_grant_id_t, size_t, vir_ಬೈಟುಗಳು, vir_ಬೈಟುಗಳು, ಪೂರ್ಣಾಂಕ);

#ವ್ಯಾಖ್ಯಾನಿಸು HASGRANTTABLE(gr) \
	(priv(gr) && priv(gr)->s_grant_ಕೋಷ್ಟಕ)

ರಚನೆ cp_sfinfo {		/* information for handling soft faults */
	ಪೂರ್ಣಾಂಕ try;		/* if nonzero, try copy only, stop on fault */
	endpoint_t endpt;	/* endpoint owning grant with CPF_TRY flag */
	vir_ಬೈಟುಗಳು addr;		/* address to write mark upon soft fault */
	cp_grant_id_t ಮೌಲ್ಯ;	/* grant ID to use as mark value to write */
};

/*===========================================================================*
 *				verify_grant				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ verify_grant(
  endpoint_t granter,		/* copyee */
  endpoint_t grantee,		/* copyer */
  cp_grant_id_t grant,		/* grant id */
  vir_ಬೈಟುಗಳು ಬೈಟುಗಳು,		/* copy size */
  ಪೂರ್ಣಾಂಕ access,			/* direction (read/write) */
  vir_ಬೈಟುಗಳು ಸ್ಥಾನಾಂತರ_ಒಳ,		/* copy offset within grant */
  vir_ಬೈಟುಗಳು *ಸ್ಥಾನಾಂತರ_ಫಲಿತಾಂಶ,	/* copy offset within virtual address space */
  endpoint_t *e_granter,	/* new granter (magic grants) */
  ರಚನೆ cp_sfinfo *sfinfo	/* storage for soft fault information */
)
{
	cp_grant_t g;
	ಪೂರ್ಣಾಂಕ proc_nr;
	ಸ್ಥಿರಾಂಕ ರಚನೆ proc *granter_proc;
	ಪೂರ್ಣಾಂಕ grant_idx, grant_seq;
	ಪೂರ್ಣಾಂಕ ಆಳ = 0;

	ಮಾಡು {
		/* Get granter process slot (if valid), and check range of
		 * grant id.
		 */
		ಆದರೆ(!isokendpt(granter, &proc_nr) ) {
			ಬರೆ(
			"grant verify failed: invalid granter %d\n", (ಪೂರ್ಣಾಂಕ) granter);
			ಹಿಂತಿರುಗಿ(EINVAL);
		}
		ಆದರೆ(!GRANT_VALID(grant)) {
			ಬರೆ(
			"grant verify failed: invalid grant %d\n", (ಪೂರ್ಣಾಂಕ) grant);
			ಹಿಂತಿರುಗಿ(EINVAL);
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
		ಆದರೆ(priv(granter_proc)->s_grant_endpoint != granter_proc->p_endpoint) {
			ಆದರೆ(!access) {
				ಹಿಂತಿರುಗಿ OK;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!HASGRANTTABLE(granter_proc) || grantee != priv(granter_proc)->s_grant_endpoint) {
				ಹಿಂತಿರುಗಿ ENOTREADY;
			}
		}

		/* If there is no priv. structure, or no grant table in the
		 * priv. structure, or the grant table in the priv. structure
		 * is too small for the grant, return EPERM.
		 */
		ಆದರೆ(!HASGRANTTABLE(granter_proc)) {
			ಬರೆ(
			"grant verify failed: granter %d has no grant table\n",
			granter);
			ಹಿಂತಿರುಗಿ(EPERM);
		}

		grant_idx = GRANT_IDX(grant);
		grant_seq = GRANT_SEQ(grant);

		ಆದರೆ(priv(granter_proc)->s_grant_entries <= grant_idx) {
				ಬರೆ(
				"verify_grant: grant verify failed in ep %d "
				"proc %d: grant 0x%x (#%d) out of range "
				"for table size %d\n",
					granter, proc_nr, grant, grant_idx,
					priv(granter_proc)->s_grant_entries);
			ಹಿಂತಿರುಗಿ(EPERM);
		}

		/* Copy the grant entry corresponding to this ID's index to see
		 * what it looks like. If it fails, hide the fact that granter
		 * has (presumably) set an invalid grant table entry by
		 * returning EPERM, just like with an invalid grant id.
		 */
		ಆದರೆ(ದತ್ತಾಂಶ_copy(granter, priv(granter_proc)->s_grant_ಕೋಷ್ಟಕ +
			ಗಾತ್ರ(g) * grant_idx,
			KERNEL, (vir_ಬೈಟುಗಳು) &g, ಗಾತ್ರ(g)) != OK) {
			ಬರೆ(
			"verify_grant: grant verify: data_copy failed\n");
			ಹಿಂತಿರುಗಿ EPERM;
		}

		/* Check validity: flags and sequence number. */
		ಆದರೆ((g.cp_flags & (CPF_ಬಳಸಿದ | CPF_VALID)) !=
			(CPF_ಬಳಸಿದ | CPF_VALID)) {
			ಬರೆ("verify_grant: grant failed: invalid flags "
			    "(0x%x, 0x%lx)\n", grant, g.cp_flags);
			ಹಿಂತಿರುಗಿ EPERM;
		}

		ಆದರೆ (g.cp_seq != grant_seq) {
			ಬರೆ("verify_grant: grant failed: invalid sequence "
			    "(0x%x, %d vs %d)\n", grant, grant_seq, g.cp_seq);
			ಹಿಂತಿರುಗಿ EPERM;
		}

		/* The given grant may be an indirect grant, that is, a grant
		 * that provides permission to use a grant given to the
		 * granter (i.e., for which it is the grantee). This can lead
		 * to a chain of indirect grants which must be followed back.
		 */
		ಆದರೆ((g.cp_flags & CPF_ಪರೋಕ್ಷ)) {
			/* Stop after a few iterations. There may be a loop. */
			ಆದರೆ (ಆಳ == MAX_ಪರೋಕ್ಷ_ಆಳ) {
				ಬರೆ(
					"verify grant: indirect grant verify "
					"failed: exceeded maximum depth\n");
				ಹಿಂತಿರುಗಿ ELOOP;
			}
			ಆಳ++;

			/* Verify actual grantee. */
			ಆದರೆ(g.cp_u.cp_ಪರೋಕ್ಷ.cp_who_ಗಮ್ಯ != grantee &&
				grantee != ANY &&
				g.cp_u.cp_ಪರೋಕ್ಷ.cp_who_ಗಮ್ಯ != ANY) {
				ಬರೆ(
					"verify_grant: indirect grant verify "
					"failed: bad grantee\n");
				ಹಿಂತಿರುಗಿ EPERM;
			}

			/* Start over with new granter, grant, and grantee. */
			grantee = granter;
			granter = g.cp_u.cp_ಪರೋಕ್ಷ.cp_who_from;
			grant = g.cp_u.cp_ಪರೋಕ್ಷ.cp_grant;
		}
	} ಆಗ(g.cp_flags & CPF_ಪರೋಕ್ಷ);

	/* Check access of grant. */
	ಆದರೆ(((g.cp_flags & access) != access)) {
		ಬರೆ(
	"verify_grant: grant verify failed: access invalid; want 0x%x, have 0x%x\n",
			access, g.cp_flags);
		ಹಿಂತಿರುಗಿ EPERM;
	}

	ಆದರೆ((g.cp_flags & CPF_ನೇರ)) {
		/* Don't fiddle around with grants that wrap, arithmetic
		 * below may be confused.
		 */
		ಆದರೆ(MEM_ತುದಿ - g.cp_u.cp_ನೇರ.cp_len + 1 <
			g.cp_u.cp_ನೇರ.cp_ಆರಂಭ) {
			ಬರೆ(
		"verify_grant: direct grant verify failed: len too long\n");
			ಹಿಂತಿರುಗಿ EPERM;
		}

		/* Verify actual grantee. */
		ಆದರೆ(g.cp_u.cp_ನೇರ.cp_who_ಗಮ್ಯ != grantee && grantee != ANY
			&& g.cp_u.cp_ನೇರ.cp_who_ಗಮ್ಯ != ANY) {
			ಬರೆ(
		"verify_grant: direct grant verify failed: bad grantee\n");
			ಹಿಂತಿರುಗಿ EPERM;
		}

		/* Verify actual copy range. */
		ಆದರೆ((ಸ್ಥಾನಾಂತರ_ಒಳ+ಬೈಟುಗಳು < ಸ್ಥಾನಾಂತರ_ಒಳ) ||
		    ಸ್ಥಾನಾಂತರ_ಒಳ+ಬೈಟುಗಳು > g.cp_u.cp_ನೇರ.cp_len) {
			ಬರೆ(
		"verify_grant: direct grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_ನೇರ.cp_len,
				g.cp_u.cp_ನೇರ.cp_ಆರಂಭ,
				ಬೈಟುಗಳು, ಸ್ಥಾನಾಂತರ_ಒಳ);
			ಹಿಂತಿರುಗಿ EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*ಸ್ಥಾನಾಂತರ_ಫಲಿತಾಂಶ = g.cp_u.cp_ನೇರ.cp_ಆರಂಭ + ಸ್ಥಾನಾಂತರ_ಒಳ;
		*e_granter = granter;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(g.cp_flags & CPF_MAGIC) {
		/* Currently, it is hardcoded that only VFS and MIB may do
		 * magic grants.  TODO: this should be a system.conf flag.
		 */
		ಆದರೆ(granter != VFS_PROC_NR && granter != MIB_PROC_NR) {
			ಬರೆ(
		"verify_grant: magic grant verify failed: granter (%d) "
		"not allowed\n", granter);
			ಹಿಂತಿರುಗಿ EPERM;
		}

		/* Verify actual grantee. */
		ಆದರೆ(g.cp_u.cp_magic.cp_who_ಗಮ್ಯ != grantee && grantee != ANY
			&& g.cp_u.cp_ನೇರ.cp_who_ಗಮ್ಯ != ANY) {
			ಬರೆ(
		"verify_grant: magic grant verify failed: bad grantee\n");
			ಹಿಂತಿರುಗಿ EPERM;
		}

		/* Verify actual copy range. */
		ಆದರೆ((ಸ್ಥಾನಾಂತರ_ಒಳ+ಬೈಟುಗಳು < ಸ್ಥಾನಾಂತರ_ಒಳ) ||
		    ಸ್ಥಾನಾಂತರ_ಒಳ+ಬೈಟುಗಳು > g.cp_u.cp_magic.cp_len) {
			ಬರೆ(
		"verify_grant: magic grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_magic.cp_len,
				g.cp_u.cp_magic.cp_ಆರಂಭ,
				ಬೈಟುಗಳು, ಸ್ಥಾನಾಂತರ_ಒಳ);
			ಹಿಂತಿರುಗಿ EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*ಸ್ಥಾನಾಂತರ_ಫಲಿತಾಂಶ = g.cp_u.cp_magic.cp_ಆರಂಭ + ಸ್ಥಾನಾಂತರ_ಒಳ;
		*e_granter = g.cp_u.cp_magic.cp_who_from;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಬರೆ(
		"verify_grant: grant verify failed: unknown grant type\n");
		ಹಿಂತಿರುಗಿ EPERM;
	}

	/* If requested, store information regarding soft faults. */
	ಆದರೆ (sfinfo != ಖಾಲಿ && (sfinfo->try = !!(g.cp_flags & CPF_TRY))) {
		sfinfo->endpt = granter;
		sfinfo->addr = priv(granter_proc)->s_grant_ಕೋಷ್ಟಕ +
		    ಗಾತ್ರ(g) * grant_idx + offsetof(cp_grant_t, cp_faulted);
		sfinfo->ಮೌಲ್ಯ = grant;
	}

	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				safecopy				     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ safecopy(
  ರಚನೆ proc * caller,
  endpoint_t granter,
  endpoint_t grantee,
  cp_grant_id_t grantid,
  size_t ಬೈಟುಗಳು,
  vir_ಬೈಟುಗಳು g_ಸ್ಥಾನಾಂತರ,
  vir_ಬೈಟುಗಳು addr,
  ಪೂರ್ಣಾಂಕ access			/* CPF_READ for a copy from granter to grantee, CPF_WRITE
				 * for a copy from grantee to granter.
				 */
)
{
	ಸ್ಥಿರ ರಚನೆ vir_addr v_src, v_dst;
	ಸ್ಥಿರ vir_ಬೈಟುಗಳು v_ಸ್ಥಾನಾಂತರ;
	endpoint_t new_granter, *src, *dst;
	ಪೂರ್ಣಾಂಕ r;
	ರಚನೆ cp_sfinfo sfinfo;

	ಆದರೆ(granter == NONE || grantee == NONE) {
		ಬರೆ("safecopy: nonsense processes\n");
		ಹಿಂತಿರುಗಿ EFAULT;
	}

	/* Decide who is src and who is dst. */
	ಆದರೆ(access & CPF_READ) {
		src = &granter;
		dst = &grantee;
	} ಇಲ್ಲದಿದ್ದರೆ {
		src = &grantee;
		dst = &granter;
	}

	/* Verify permission exists. */
	ಆದರೆ((r=verify_grant(granter, grantee, grantid, ಬೈಟುಗಳು, access,
	    g_ಸ್ಥಾನಾಂತರ, &v_ಸ್ಥಾನಾಂತರ, &new_granter, &sfinfo)) != OK) {
		ಆದರೆ(r == ENOTREADY) ಹಿಂತಿರುಗಿ r;
			ಬರೆ(
		"grant %d verify to copy %d->%d by %d failed: err %d\n",
				grantid, *src, *dst, grantee, r);
		ಹಿಂತಿರುಗಿ r;
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
	ಆದರೆ(access & CPF_READ) {
		v_src.ಸ್ಥಾನಾಂತರ = v_ಸ್ಥಾನಾಂತರ;
		v_dst.ಸ್ಥಾನಾಂತರ = (vir_ಬೈಟುಗಳು) addr;
	} ಇಲ್ಲದಿದ್ದರೆ {
		v_src.ಸ್ಥಾನಾಂತರ = (vir_ಬೈಟುಗಳು) addr;
		v_dst.ಸ್ಥಾನಾಂತರ = v_ಸ್ಥಾನಾಂತರ;
	}

	/* Do the regular copy. */
	ಆದರೆ (sfinfo.try) {
		/*
		 * Try copying without transparently faulting in pages.
		 * TODO: while CPF_TRY is meant to protect against deadlocks on
		 * memory-mapped files in file systems, it seems that this case
		 * triggers faults a whole lot more often, resulting in extra
		 * overhead due to retried file system operations.  It might be
		 * a good idea to go through VM even in this case, and have VM
		 * fail (only) if the affected page belongs to a file mapping.
		 */
		r = virtual_copy(&v_src, &v_dst, ಬೈಟುಗಳು);
		ಆದರೆ (r == EFAULT_SRC || r == EFAULT_DST) {
			/*
			 * Mark the magic grant as having experienced a soft
			 * fault during its lifetime.  The exact value does not
			 * matter, but we use the grant ID (including its
			 * sequence number) as a form of protection in the
			 * light of CPU concurrency.
			 */
			r = ದತ್ತಾಂಶ_copy(KERNEL, (vir_ಬೈಟುಗಳು)&sfinfo.ಮೌಲ್ಯ,
			    sfinfo.endpt, sfinfo.addr, ಗಾತ್ರ(sfinfo.ಮೌಲ್ಯ));
			/*
			 * Failure means the creator of the magic grant messed
			 * up, which can only be unintentional, so report..
			 */
			ಆದರೆ (r != OK)
				ಬರೆ("Kernel: writing soft fault marker %d "
				    "into %d at 0x%lx failed (%d)\n",
				    sfinfo.ಮೌಲ್ಯ, sfinfo.endpt, sfinfo.addr,
				    r);

			ಹಿಂತಿರುಗಿ EFAULT;
		}
		ಹಿಂತಿರುಗಿ r;
	}
	ಹಿಂತಿರುಗಿ virtual_copy_vmcheck(caller, &v_src, &v_dst, ಬೈಟುಗಳು);
}

/*===========================================================================*
 *				do_safecopy_to				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_safecopy_ಗಮ್ಯ(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
	ಹಿಂತಿರುಗಿ safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_ಗಮ್ಯ, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.ಬೈಟುಗಳು, m_ptr->m_lsys_kern_safecopy.ಸ್ಥಾನಾಂತರ,
		(vir_ಬೈಟುಗಳು) m_ptr->m_lsys_kern_safecopy.ವಿಳಾಸ, CPF_WRITE);
}

/*===========================================================================*
 *				do_safecopy_from			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_safecopy_from(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
	ಹಿಂತಿರುಗಿ safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_ಗಮ್ಯ, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.ಬೈಟುಗಳು, m_ptr->m_lsys_kern_safecopy.ಸ್ಥಾನಾಂತರ,
		(vir_ಬೈಟುಗಳು) m_ptr->m_lsys_kern_safecopy.ವಿಳಾಸ, CPF_READ);
}

/*===========================================================================*
 *				do_vsafecopy				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_vsafecopy(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
	ಸ್ಥಿರ ರಚನೆ vscp_vec vec[SCPVEC_NR];
	ಸ್ಥಿರ ರಚನೆ vir_addr src, dst;
	ಪೂರ್ಣಾಂಕ r, i, els;
	size_t ಬೈಟುಗಳು;

	/* Set vector copy parameters. */
	src.proc_nr_e = caller->p_endpoint;
	ಪ್ರತಿಜ್ಞಾ(src.proc_nr_e != NONE);
	src.ಸ್ಥಾನಾಂತರ = (vir_ಬೈಟುಗಳು) m_ptr->m_lsys_kern_vsafecopy.vec_addr;
	dst.proc_nr_e = KERNEL;
	dst.ಸ್ಥಾನಾಂತರ = (vir_ಬೈಟುಗಳು) vec;

	/* No. of vector elements. */
	els = m_ptr->m_lsys_kern_vsafecopy.vec_ಗಾತ್ರ;
	ಬೈಟುಗಳು = els * ಗಾತ್ರ(ರಚನೆ vscp_vec);

	/* Obtain vector of copies. */
	ಆದರೆ((r=virtual_copy_vmcheck(caller, &src, &dst, ಬೈಟುಗಳು)) != OK)
		ಹಿಂತಿರುಗಿ r;

	/* Perform safecopies. */
	ಗಾಗಿ(i = 0; i < els; i++) {
		ಪೂರ್ಣಾಂಕ access;
		endpoint_t granter;
		ಆದರೆ(vec[i].v_from == SELF) {
			access = CPF_WRITE;
			granter = vec[i].v_ಗಮ್ಯ;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(vec[i].v_ಗಮ್ಯ == SELF) {
			access = CPF_READ;
			granter = vec[i].v_from;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಬರೆ("vsafecopy: %d: element %d/%d: no SELF found\n",
				caller->p_endpoint, i, els);
			ಹಿಂತಿರುಗಿ EINVAL;
		}

		/* Do safecopy for this element. */
		ಆದರೆ((r=safecopy(caller, granter, caller->p_endpoint,
			vec[i].v_gid,
			vec[i].v_ಬೈಟುಗಳು, vec[i].v_ಸ್ಥಾನಾಂತರ,
			vec[i].v_addr, access)) != OK) {
			ಹಿಂತಿರುಗಿ r;
		}
	}

	ಹಿಂತಿರುಗಿ OK;
}

