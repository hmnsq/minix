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

#ଅନ୍ତର୍ଭାଵ <assert.h>

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ "kernel/vm.h"

#define MAX_INDIRECT_DEPTH 5	/* up to how many indirect grants to follow? */

#define MEM_ଶୀର୍ଷ 0xFFFFFFFFUL

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା safecopy(ସଂରଚନା proc *, endpoint_t, endpoint_t,
	cp_grant_id_t, size_t, vir_bytes, vir_bytes, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

#define HASGRANTTABLE(gr) \
	(priv(gr) && priv(gr)->s_grant_table)

ସଂରଚନା cp_sfinfo {		/* information for handling soft faults */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା try;		/* if nonzero, try copy only, stop on fault */
	endpoint_t endpt;	/* endpoint owning grant with CPF_TRY flag */
	vir_bytes addr;		/* address to write mark upon soft fault */
	cp_grant_id_t ମୂଲ୍ୟ;	/* grant ID to use as mark value to write */
};

/*===========================================================================*
 *				verify_grant				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା verify_grant(
  endpoint_t granter,		/* copyee */
  endpoint_t grantee,		/* copyer */
  cp_grant_id_t grant,		/* grant id */
  vir_bytes bytes,		/* copy size */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା access,			/* direction (read/write) */
  vir_bytes offset_in,		/* copy offset within grant */
  vir_bytes *offset_result,	/* copy offset within virtual address space */
  endpoint_t *e_granter,	/* new granter (magic grants) */
  ସଂରଚନା cp_sfinfo *sfinfo	/* storage for soft fault information */
)
{
	cp_grant_t g;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr;
	ଅଚଳ ସଂରଚନା proc *granter_proc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା grant_idx, grant_seq;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth = 0;

	କର {
		/* Get granter process slot (if valid), and check range of
		 * grant id.
		 */
		ଯଦି(!isokendpt(granter, &proc_nr) ) {
			printf(
			"grant verify failed: invalid granter %d\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) granter);
			ଫେରନ୍ତୁ(EINVAL);
		}
		ଯଦି(!GRANT_VALID(grant)) {
			printf(
			"grant verify failed: invalid grant %d\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) grant);
			ଫେରନ୍ତୁ(EINVAL);
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
		ଯଦି(priv(granter_proc)->s_grant_endpoint != granter_proc->p_endpoint) {
			ଯଦି(!access) {
				ଫେରନ୍ତୁ OK;
			}
			ଅନ୍ୟଥା ଯଦି(!HASGRANTTABLE(granter_proc) || grantee != priv(granter_proc)->s_grant_endpoint) {
				ଫେରନ୍ତୁ ENOTREADY;
			}
		}

		/* If there is no priv. structure, or no grant table in the
		 * priv. structure, or the grant table in the priv. structure
		 * is too small for the grant, return EPERM.
		 */
		ଯଦି(!HASGRANTTABLE(granter_proc)) {
			printf(
			"grant verify failed: granter %d has no grant table\n",
			granter);
			ଫେରନ୍ତୁ(EPERM);
		}

		grant_idx = GRANT_IDX(grant);
		grant_seq = GRANT_SEQ(grant);

		ଯଦି(priv(granter_proc)->s_grant_entries <= grant_idx) {
				printf(
				"verify_grant: grant verify failed in ep %d "
				"proc %d: grant 0x%x (#%d) out of range "
				"for table size %d\n",
					granter, proc_nr, grant, grant_idx,
					priv(granter_proc)->s_grant_entries);
			ଫେରନ୍ତୁ(EPERM);
		}

		/* Copy the grant entry corresponding to this ID's index to see
		 * what it looks like. If it fails, hide the fact that granter
		 * has (presumably) set an invalid grant table entry by
		 * returning EPERM, just like with an invalid grant id.
		 */
		ଯଦି(data_copy(granter, priv(granter_proc)->s_grant_table +
			ଆକାର(g) * grant_idx,
			KERNEL, (vir_bytes) &g, ଆକାର(g)) != OK) {
			printf(
			"verify_grant: grant verify: data_copy failed\n");
			ଫେରନ୍ତୁ EPERM;
		}

		/* Check validity: flags and sequence number. */
		ଯଦି((g.cp_flags & (CPF_USED | CPF_VALID)) !=
			(CPF_USED | CPF_VALID)) {
			printf("verify_grant: grant failed: invalid flags "
			    "(0x%x, 0x%lx)\n", grant, g.cp_flags);
			ଫେରନ୍ତୁ EPERM;
		}

		ଯଦି (g.cp_seq != grant_seq) {
			printf("verify_grant: grant failed: invalid sequence "
			    "(0x%x, %d vs %d)\n", grant, grant_seq, g.cp_seq);
			ଫେରନ୍ତୁ EPERM;
		}

		/* The given grant may be an indirect grant, that is, a grant
		 * that provides permission to use a grant given to the
		 * granter (i.e., for which it is the grantee). This can lead
		 * to a chain of indirect grants which must be followed back.
		 */
		ଯଦି((g.cp_flags & CPF_INDIRECT)) {
			/* Stop after a few iterations. There may be a loop. */
			ଯଦି (depth == MAX_INDIRECT_DEPTH) {
				printf(
					"verify grant: indirect grant verify "
					"failed: exceeded maximum depth\n");
				ଫେରନ୍ତୁ ELOOP;
			}
			depth++;

			/* Verify actual grantee. */
			ଯଦି(g.cp_u.cp_indirect.cp_who_to != grantee &&
				grantee != ANY &&
				g.cp_u.cp_indirect.cp_who_to != ANY) {
				printf(
					"verify_grant: indirect grant verify "
					"failed: bad grantee\n");
				ଫେରନ୍ତୁ EPERM;
			}

			/* Start over with new granter, grant, and grantee. */
			grantee = granter;
			granter = g.cp_u.cp_indirect.cp_who_from;
			grant = g.cp_u.cp_indirect.cp_grant;
		}
	} ଯେତେବେଳେ(g.cp_flags & CPF_INDIRECT);

	/* Check access of grant. */
	ଯଦି(((g.cp_flags & access) != access)) {
		printf(
	"verify_grant: grant verify failed: access invalid; want 0x%x, have 0x%x\n",
			access, g.cp_flags);
		ଫେରନ୍ତୁ EPERM;
	}

	ଯଦି((g.cp_flags & CPF_DIRECT)) {
		/* Don't fiddle around with grants that wrap, arithmetic
		 * below may be confused.
		 */
		ଯଦି(MEM_ଶୀର୍ଷ - g.cp_u.cp_direct.cp_len + 1 <
			g.cp_u.cp_direct.cp_start) {
			printf(
		"verify_grant: direct grant verify failed: len too long\n");
			ଫେରନ୍ତୁ EPERM;
		}

		/* Verify actual grantee. */
		ଯଦି(g.cp_u.cp_direct.cp_who_to != grantee && grantee != ANY
			&& g.cp_u.cp_direct.cp_who_to != ANY) {
			printf(
		"verify_grant: direct grant verify failed: bad grantee\n");
			ଫେରନ୍ତୁ EPERM;
		}

		/* Verify actual copy range. */
		ଯଦି((offset_in+bytes < offset_in) ||
		    offset_in+bytes > g.cp_u.cp_direct.cp_len) {
			printf(
		"verify_grant: direct grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_direct.cp_len,
				g.cp_u.cp_direct.cp_start,
				bytes, offset_in);
			ଫେରନ୍ତୁ EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*offset_result = g.cp_u.cp_direct.cp_start + offset_in;
		*e_granter = granter;
	} ଅନ୍ୟଥା ଯଦି(g.cp_flags & CPF_MAGIC) {
		/* Currently, it is hardcoded that only VFS and MIB may do
		 * magic grants.  TODO: this should be a system.conf flag.
		 */
		ଯଦି(granter != VFS_PROC_NR && granter != MIB_PROC_NR) {
			printf(
		"verify_grant: magic grant verify failed: granter (%d) "
		"not allowed\n", granter);
			ଫେରନ୍ତୁ EPERM;
		}

		/* Verify actual grantee. */
		ଯଦି(g.cp_u.cp_magic.cp_who_to != grantee && grantee != ANY
			&& g.cp_u.cp_direct.cp_who_to != ANY) {
			printf(
		"verify_grant: magic grant verify failed: bad grantee\n");
			ଫେରନ୍ତୁ EPERM;
		}

		/* Verify actual copy range. */
		ଯଦି((offset_in+bytes < offset_in) ||
		    offset_in+bytes > g.cp_u.cp_magic.cp_len) {
			printf(
		"verify_grant: magic grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_magic.cp_len,
				g.cp_u.cp_magic.cp_start,
				bytes, offset_in);
			ଫେରନ୍ତୁ EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*offset_result = g.cp_u.cp_magic.cp_start + offset_in;
		*e_granter = g.cp_u.cp_magic.cp_who_from;
	} ଅନ୍ୟଥା {
		printf(
		"verify_grant: grant verify failed: unknown grant type\n");
		ଫେରନ୍ତୁ EPERM;
	}

	/* If requested, store information regarding soft faults. */
	ଯଦି (sfinfo != ଖାଲି && (sfinfo->try = !!(g.cp_flags & CPF_TRY))) {
		sfinfo->endpt = granter;
		sfinfo->addr = priv(granter_proc)->s_grant_table +
		    ଆକାର(g) * grant_idx + offsetof(cp_grant_t, cp_faulted);
		sfinfo->ମୂଲ୍ୟ = grant;
	}

	ଫେରନ୍ତୁ OK;
}

/*===========================================================================*
 *				safecopy				     *
 *===========================================================================*/
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା safecopy(
  ସଂରଚନା proc * caller,
  endpoint_t granter,
  endpoint_t grantee,
  cp_grant_id_t grantid,
  size_t bytes,
  vir_bytes g_offset,
  vir_bytes addr,
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା access			/* CPF_READ for a copy from granter to grantee, CPF_WRITE
				 * for a copy from grantee to granter.
				 */
)
{
	ସ୍ଥିର ସଂରଚନା vir_addr v_src, v_dst;
	ସ୍ଥିର vir_bytes v_offset;
	endpoint_t new_granter, *src, *dst;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
	ସଂରଚନା cp_sfinfo sfinfo;

	ଯଦି(granter == NONE || grantee == NONE) {
		printf("safecopy: nonsense processes\n");
		ଫେରନ୍ତୁ EFAULT;
	}

	/* Decide who is src and who is dst. */
	ଯଦି(access & CPF_READ) {
		src = &granter;
		dst = &grantee;
	} ଅନ୍ୟଥା {
		src = &grantee;
		dst = &granter;
	}

	/* Verify permission exists. */
	ଯଦି((r=verify_grant(granter, grantee, grantid, bytes, access,
	    g_offset, &v_offset, &new_granter, &sfinfo)) != OK) {
		ଯଦି(r == ENOTREADY) ଫେରନ୍ତୁ r;
			printf(
		"grant %d verify to copy %d->%d by %d failed: err %d\n",
				grantid, *src, *dst, grantee, r);
		ଫେରନ୍ତୁ r;
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
	ଯଦି(access & CPF_READ) {
		v_src.offset = v_offset;
		v_dst.offset = (vir_bytes) addr;
	} ଅନ୍ୟଥା {
		v_src.offset = (vir_bytes) addr;
		v_dst.offset = v_offset;
	}

	/* Do the regular copy. */
	ଯଦି (sfinfo.try) {
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
		ଯଦି (r == EFAULT_SRC || r == EFAULT_DST) {
			/*
			 * Mark the magic grant as having experienced a soft
			 * fault during its lifetime.  The exact value does not
			 * matter, but we use the grant ID (including its
			 * sequence number) as a form of protection in the
			 * light of CPU concurrency.
			 */
			r = data_copy(KERNEL, (vir_bytes)&sfinfo.ମୂଲ୍ୟ,
			    sfinfo.endpt, sfinfo.addr, ଆକାର(sfinfo.ମୂଲ୍ୟ));
			/*
			 * Failure means the creator of the magic grant messed
			 * up, which can only be unintentional, so report..
			 */
			ଯଦି (r != OK)
				printf("Kernel: writing soft fault marker %d "
				    "into %d at 0x%lx failed (%d)\n",
				    sfinfo.ମୂଲ୍ୟ, sfinfo.endpt, sfinfo.addr,
				    r);

			ଫେରନ୍ତୁ EFAULT;
		}
		ଫେରନ୍ତୁ r;
	}
	ଫେରନ୍ତୁ virtual_copy_vmcheck(caller, &v_src, &v_dst, bytes);
}

/*===========================================================================*
 *				do_safecopy_to				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_safecopy_to(ସଂରଚନା proc * caller, message * m_ptr)
{
	ଫେରନ୍ତୁ safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_to, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.bytes, m_ptr->m_lsys_kern_safecopy.offset,
		(vir_bytes) m_ptr->m_lsys_kern_safecopy.address, CPF_WRITE);
}

/*===========================================================================*
 *				do_safecopy_from			     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_safecopy_from(ସଂରଚନା proc * caller, message * m_ptr)
{
	ଫେରନ୍ତୁ safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_to, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.bytes, m_ptr->m_lsys_kern_safecopy.offset,
		(vir_bytes) m_ptr->m_lsys_kern_safecopy.address, CPF_READ);
}

/*===========================================================================*
 *				do_vsafecopy				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_vsafecopy(ସଂରଚନା proc * caller, message * m_ptr)
{
	ସ୍ଥିର ସଂରଚନା vscp_vec vec[SCPVEC_NR];
	ସ୍ଥିର ସଂରଚନା vir_addr src, dst;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, i, els;
	size_t bytes;

	/* Set vector copy parameters. */
	src.proc_nr_e = caller->p_endpoint;
	ପ୍ରତିଜ୍ଞା(src.proc_nr_e != NONE);
	src.offset = (vir_bytes) m_ptr->m_lsys_kern_vsafecopy.vec_addr;
	dst.proc_nr_e = KERNEL;
	dst.offset = (vir_bytes) vec;

	/* No. of vector elements. */
	els = m_ptr->m_lsys_kern_vsafecopy.vec_size;
	bytes = els * ଆକାର(ସଂରଚନା vscp_vec);

	/* Obtain vector of copies. */
	ଯଦି((r=virtual_copy_vmcheck(caller, &src, &dst, bytes)) != OK)
		ଫେରନ୍ତୁ r;

	/* Perform safecopies. */
	ପାଇଁ(i = 0; i < els; i++) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା access;
		endpoint_t granter;
		ଯଦି(vec[i].v_from == SELF) {
			access = CPF_WRITE;
			granter = vec[i].v_to;
		} ଅନ୍ୟଥା ଯଦି(vec[i].v_to == SELF) {
			access = CPF_READ;
			granter = vec[i].v_from;
		} ଅନ୍ୟଥା {
			printf("vsafecopy: %d: element %d/%d: no SELF found\n",
				caller->p_endpoint, i, els);
			ଫେରନ୍ତୁ EINVAL;
		}

		/* Do safecopy for this element. */
		ଯଦି((r=safecopy(caller, granter, caller->p_endpoint,
			vec[i].v_gid,
			vec[i].v_bytes, vec[i].v_offset,
			vec[i].v_addr, access)) != OK) {
			ଫେରନ୍ତୁ r;
		}
	}

	ଫେରନ୍ତୁ OK;
}

