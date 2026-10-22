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

#define MAX_INDIRECT_DEPTH 5	/* up to how many indirect grants to follow? */

#define MEM_ತುದಿ 0xFFFFFFFFUL

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ safecopy(ಸಂರಚನಾ proc *, endpoint_t, endpoint_t,
	cp_grant_id_t, size_t, vir_bytes, vir_bytes, ಪೂರ್ಣಾಙ್ಕ);

#define HASGRANTTABLE(gr) \
	(priv(gr) && priv(gr)->s_grant_table)

ಸಂರಚನಾ cp_sfinfo {		/* information for handling soft faults */
	ಪೂರ್ಣಾಙ್ಕ try;		/* if nonzero, try copy only, stop on fault */
	endpoint_t endpt;	/* endpoint owning grant with CPF_TRY flag */
	vir_bytes addr;		/* address to write mark upon soft fault */
	cp_grant_id_t ಮೌಲ್ಯ;	/* grant ID to use as mark value to write */
};

/*===========================================================================*
 *				verify_grant				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ verify_grant(
  endpoint_t granter,		/* copyee */
  endpoint_t grantee,		/* copyer */
  cp_grant_id_t grant,		/* grant id */
  vir_bytes bytes,		/* copy size */
  ಪೂರ್ಣಾಙ್ಕ access,			/* direction (read/write) */
  vir_bytes offset_in,		/* copy offset within grant */
  vir_bytes *offset_result,	/* copy offset within virtual address space */
  endpoint_t *e_granter,	/* new granter (magic grants) */
  ಸಂರಚನಾ cp_sfinfo *sfinfo	/* storage for soft fault information */
)
{
	cp_grant_t g;
	ಪೂರ್ಣಾಙ್ಕ proc_nr;
	ಧ್ರುವ ಸಂರಚನಾ proc *granter_proc;
	ಪೂರ್ಣಾಙ್ಕ grant_idx, grant_seq;
	ಪೂರ್ಣಾಙ್ಕ depth = 0;

	ಕುರು {
		/* Get granter process slot (if valid), and check range of
		 * grant id.
		 */
		ಯದಿ(!isokendpt(granter, &proc_nr) ) {
			printf(
			"grant verify failed: invalid granter %d\n", (ಪೂರ್ಣಾಙ್ಕ) granter);
			ಪ್ರತ್ಯಾಗಮ(EINVAL);
		}
		ಯದಿ(!GRANT_VALID(grant)) {
			printf(
			"grant verify failed: invalid grant %d\n", (ಪೂರ್ಣಾಙ್ಕ) grant);
			ಪ್ರತ್ಯಾಗಮ(EINVAL);
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
		ಯದಿ(priv(granter_proc)->s_grant_endpoint != granter_proc->p_endpoint) {
			ಯದಿ(!access) {
				ಪ್ರತ್ಯಾಗಮ OK;
			}
			ಅನ್ಯಥಾ ಯದಿ(!HASGRANTTABLE(granter_proc) || grantee != priv(granter_proc)->s_grant_endpoint) {
				ಪ್ರತ್ಯಾಗಮ ENOTREADY;
			}
		}

		/* If there is no priv. structure, or no grant table in the
		 * priv. structure, or the grant table in the priv. structure
		 * is too small for the grant, return EPERM.
		 */
		ಯದಿ(!HASGRANTTABLE(granter_proc)) {
			printf(
			"grant verify failed: granter %d has no grant table\n",
			granter);
			ಪ್ರತ್ಯಾಗಮ(EPERM);
		}

		grant_idx = GRANT_IDX(grant);
		grant_seq = GRANT_SEQ(grant);

		ಯದಿ(priv(granter_proc)->s_grant_entries <= grant_idx) {
				printf(
				"verify_grant: grant verify failed in ep %d "
				"proc %d: grant 0x%x (#%d) out of range "
				"for table size %d\n",
					granter, proc_nr, grant, grant_idx,
					priv(granter_proc)->s_grant_entries);
			ಪ್ರತ್ಯಾಗಮ(EPERM);
		}

		/* Copy the grant entry corresponding to this ID's index to see
		 * what it looks like. If it fails, hide the fact that granter
		 * has (presumably) set an invalid grant table entry by
		 * returning EPERM, just like with an invalid grant id.
		 */
		ಯದಿ(data_copy(granter, priv(granter_proc)->s_grant_table +
			ಪರಿಮಾಣ(g) * grant_idx,
			KERNEL, (vir_bytes) &g, ಪರಿಮಾಣ(g)) != OK) {
			printf(
			"verify_grant: grant verify: data_copy failed\n");
			ಪ್ರತ್ಯಾಗಮ EPERM;
		}

		/* Check validity: flags and sequence number. */
		ಯದಿ((g.cp_flags & (CPF_USED | CPF_VALID)) !=
			(CPF_USED | CPF_VALID)) {
			printf("verify_grant: grant failed: invalid flags "
			    "(0x%x, 0x%lx)\n", grant, g.cp_flags);
			ಪ್ರತ್ಯಾಗಮ EPERM;
		}

		ಯದಿ (g.cp_seq != grant_seq) {
			printf("verify_grant: grant failed: invalid sequence "
			    "(0x%x, %d vs %d)\n", grant, grant_seq, g.cp_seq);
			ಪ್ರತ್ಯಾಗಮ EPERM;
		}

		/* The given grant may be an indirect grant, that is, a grant
		 * that provides permission to use a grant given to the
		 * granter (i.e., for which it is the grantee). This can lead
		 * to a chain of indirect grants which must be followed back.
		 */
		ಯದಿ((g.cp_flags & CPF_INDIRECT)) {
			/* Stop after a few iterations. There may be a loop. */
			ಯದಿ (depth == MAX_INDIRECT_DEPTH) {
				printf(
					"verify grant: indirect grant verify "
					"failed: exceeded maximum depth\n");
				ಪ್ರತ್ಯಾಗಮ ELOOP;
			}
			depth++;

			/* Verify actual grantee. */
			ಯದಿ(g.cp_u.cp_indirect.cp_who_to != grantee &&
				grantee != ANY &&
				g.cp_u.cp_indirect.cp_who_to != ANY) {
				printf(
					"verify_grant: indirect grant verify "
					"failed: bad grantee\n");
				ಪ್ರತ್ಯಾಗಮ EPERM;
			}

			/* Start over with new granter, grant, and grantee. */
			grantee = granter;
			granter = g.cp_u.cp_indirect.cp_who_from;
			grant = g.cp_u.cp_indirect.cp_grant;
		}
	} ಯಾವತ್(g.cp_flags & CPF_INDIRECT);

	/* Check access of grant. */
	ಯದಿ(((g.cp_flags & access) != access)) {
		printf(
	"verify_grant: grant verify failed: access invalid; want 0x%x, have 0x%x\n",
			access, g.cp_flags);
		ಪ್ರತ್ಯಾಗಮ EPERM;
	}

	ಯದಿ((g.cp_flags & CPF_DIRECT)) {
		/* Don't fiddle around with grants that wrap, arithmetic
		 * below may be confused.
		 */
		ಯದಿ(MEM_ತುದಿ - g.cp_u.cp_direct.cp_len + 1 <
			g.cp_u.cp_direct.cp_start) {
			printf(
		"verify_grant: direct grant verify failed: len too long\n");
			ಪ್ರತ್ಯಾಗಮ EPERM;
		}

		/* Verify actual grantee. */
		ಯದಿ(g.cp_u.cp_direct.cp_who_to != grantee && grantee != ANY
			&& g.cp_u.cp_direct.cp_who_to != ANY) {
			printf(
		"verify_grant: direct grant verify failed: bad grantee\n");
			ಪ್ರತ್ಯಾಗಮ EPERM;
		}

		/* Verify actual copy range. */
		ಯದಿ((offset_in+bytes < offset_in) ||
		    offset_in+bytes > g.cp_u.cp_direct.cp_len) {
			printf(
		"verify_grant: direct grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_direct.cp_len,
				g.cp_u.cp_direct.cp_start,
				bytes, offset_in);
			ಪ್ರತ್ಯಾಗಮ EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*offset_result = g.cp_u.cp_direct.cp_start + offset_in;
		*e_granter = granter;
	} ಅನ್ಯಥಾ ಯದಿ(g.cp_flags & CPF_MAGIC) {
		/* Currently, it is hardcoded that only VFS and MIB may do
		 * magic grants.  TODO: this should be a system.conf flag.
		 */
		ಯದಿ(granter != VFS_PROC_NR && granter != MIB_PROC_NR) {
			printf(
		"verify_grant: magic grant verify failed: granter (%d) "
		"not allowed\n", granter);
			ಪ್ರತ್ಯಾಗಮ EPERM;
		}

		/* Verify actual grantee. */
		ಯದಿ(g.cp_u.cp_magic.cp_who_to != grantee && grantee != ANY
			&& g.cp_u.cp_direct.cp_who_to != ANY) {
			printf(
		"verify_grant: magic grant verify failed: bad grantee\n");
			ಪ್ರತ್ಯಾಗಮ EPERM;
		}

		/* Verify actual copy range. */
		ಯದಿ((offset_in+bytes < offset_in) ||
		    offset_in+bytes > g.cp_u.cp_magic.cp_len) {
			printf(
		"verify_grant: magic grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_magic.cp_len,
				g.cp_u.cp_magic.cp_start,
				bytes, offset_in);
			ಪ್ರತ್ಯಾಗಮ EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*offset_result = g.cp_u.cp_magic.cp_start + offset_in;
		*e_granter = g.cp_u.cp_magic.cp_who_from;
	} ಅನ್ಯಥಾ {
		printf(
		"verify_grant: grant verify failed: unknown grant type\n");
		ಪ್ರತ್ಯಾಗಮ EPERM;
	}

	/* If requested, store information regarding soft faults. */
	ಯದಿ (sfinfo != ರಿಕ್ತ && (sfinfo->try = !!(g.cp_flags & CPF_TRY))) {
		sfinfo->endpt = granter;
		sfinfo->addr = priv(granter_proc)->s_grant_table +
		    ಪರಿಮಾಣ(g) * grant_idx + offsetof(cp_grant_t, cp_faulted);
		sfinfo->ಮೌಲ್ಯ = grant;
	}

	ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *				safecopy				     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ safecopy(
  ಸಂರಚನಾ proc * caller,
  endpoint_t granter,
  endpoint_t grantee,
  cp_grant_id_t grantid,
  size_t bytes,
  vir_bytes g_offset,
  vir_bytes addr,
  ಪೂರ್ಣಾಙ್ಕ access			/* CPF_READ for a copy from granter to grantee, CPF_WRITE
				 * for a copy from grantee to granter.
				 */
)
{
	ಸ್ಥಿರ ಸಂರಚನಾ vir_addr v_src, v_dst;
	ಸ್ಥಿರ vir_bytes v_offset;
	endpoint_t new_granter, *src, *dst;
	ಪೂರ್ಣಾಙ್ಕ r;
	ಸಂರಚನಾ cp_sfinfo sfinfo;

	ಯದಿ(granter == NONE || grantee == NONE) {
		printf("safecopy: nonsense processes\n");
		ಪ್ರತ್ಯಾಗಮ EFAULT;
	}

	/* Decide who is src and who is dst. */
	ಯದಿ(access & CPF_READ) {
		src = &granter;
		dst = &grantee;
	} ಅನ್ಯಥಾ {
		src = &grantee;
		dst = &granter;
	}

	/* Verify permission exists. */
	ಯದಿ((r=verify_grant(granter, grantee, grantid, bytes, access,
	    g_offset, &v_offset, &new_granter, &sfinfo)) != OK) {
		ಯದಿ(r == ENOTREADY) ಪ್ರತ್ಯಾಗಮ r;
			printf(
		"grant %d verify to copy %d->%d by %d failed: err %d\n",
				grantid, *src, *dst, grantee, r);
		ಪ್ರತ್ಯಾಗಮ r;
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
	ಯದಿ(access & CPF_READ) {
		v_src.offset = v_offset;
		v_dst.offset = (vir_bytes) addr;
	} ಅನ್ಯಥಾ {
		v_src.offset = (vir_bytes) addr;
		v_dst.offset = v_offset;
	}

	/* Do the regular copy. */
	ಯದಿ (sfinfo.try) {
		/*
		 * Try copying without transparently faulting in pages.
		 * TODO: while CPF_TRY is meant to protect against deadlocks on
		 * memory-mapped files in file systems, it seems that this case
		 * triggers faults a whole lot more often, resulting in extra
		 * overhead due to retried file system operations.  It might be
		 * a good idea to go through VM even in this case, and have VM
		 * fail (only) if the affected page belongs to a file mapping.
		 */
		r = virtual_copy(&v_src, &v_dst, bytes);
		ಯದಿ (r == EFAULT_SRC || r == EFAULT_DST) {
			/*
			 * Mark the magic grant as having experienced a soft
			 * fault during its lifetime.  The exact value does not
			 * matter, but we use the grant ID (including its
			 * sequence number) as a form of protection in the
			 * light of CPU concurrency.
			 */
			r = data_copy(KERNEL, (vir_bytes)&sfinfo.ಮೌಲ್ಯ,
			    sfinfo.endpt, sfinfo.addr, ಪರಿಮಾಣ(sfinfo.ಮೌಲ್ಯ));
			/*
			 * Failure means the creator of the magic grant messed
			 * up, which can only be unintentional, so report..
			 */
			ಯದಿ (r != OK)
				printf("Kernel: writing soft fault marker %d "
				    "into %d at 0x%lx failed (%d)\n",
				    sfinfo.ಮೌಲ್ಯ, sfinfo.endpt, sfinfo.addr,
				    r);

			ಪ್ರತ್ಯಾಗಮ EFAULT;
		}
		ಪ್ರತ್ಯಾಗಮ r;
	}
	ಪ್ರತ್ಯಾಗಮ virtual_copy_vmcheck(caller, &v_src, &v_dst, bytes);
}

/*===========================================================================*
 *				do_safecopy_to				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_safecopy_to(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
	ಪ್ರತ್ಯಾಗಮ safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_to, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.bytes, m_ptr->m_lsys_kern_safecopy.offset,
		(vir_bytes) m_ptr->m_lsys_kern_safecopy.address, CPF_WRITE);
}

/*===========================================================================*
 *				do_safecopy_from			     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_safecopy_from(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
	ಪ್ರತ್ಯಾಗಮ safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_to, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.bytes, m_ptr->m_lsys_kern_safecopy.offset,
		(vir_bytes) m_ptr->m_lsys_kern_safecopy.address, CPF_READ);
}

/*===========================================================================*
 *				do_vsafecopy				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_vsafecopy(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
	ಸ್ಥಿರ ಸಂರಚನಾ vscp_vec vec[SCPVEC_NR];
	ಸ್ಥಿರ ಸಂರಚನಾ vir_addr src, dst;
	ಪೂರ್ಣಾಙ್ಕ r, i, els;
	size_t bytes;

	/* Set vector copy parameters. */
	src.proc_nr_e = caller->p_endpoint;
	ಪ್ರತಿಜ್ಞಾ(src.proc_nr_e != NONE);
	src.offset = (vir_bytes) m_ptr->m_lsys_kern_vsafecopy.vec_addr;
	dst.proc_nr_e = KERNEL;
	dst.offset = (vir_bytes) vec;

	/* No. of vector elements. */
	els = m_ptr->m_lsys_kern_vsafecopy.vec_size;
	bytes = els * ಪರಿಮಾಣ(ಸಂರಚನಾ vscp_vec);

	/* Obtain vector of copies. */
	ಯದಿ((r=virtual_copy_vmcheck(caller, &src, &dst, bytes)) != OK)
		ಪ್ರತ್ಯಾಗಮ r;

	/* Perform safecopies. */
	ಪ್ರತಿ(i = 0; i < els; i++) {
		ಪೂರ್ಣಾಙ್ಕ access;
		endpoint_t granter;
		ಯದಿ(vec[i].v_from == SELF) {
			access = CPF_WRITE;
			granter = vec[i].v_to;
		} ಅನ್ಯಥಾ ಯದಿ(vec[i].v_to == SELF) {
			access = CPF_READ;
			granter = vec[i].v_from;
		} ಅನ್ಯಥಾ {
			printf("vsafecopy: %d: element %d/%d: no SELF found\n",
				caller->p_endpoint, i, els);
			ಪ್ರತ್ಯಾಗಮ EINVAL;
		}

		/* Do safecopy for this element. */
		ಯದಿ((r=safecopy(caller, granter, caller->p_endpoint,
			vec[i].v_gid,
			vec[i].v_bytes, vec[i].v_offset,
			vec[i].v_addr, access)) != OK) {
			ಪ್ರತ್ಯಾಗಮ r;
		}
	}

	ಪ್ರತ್ಯಾಗಮ OK;
}

