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

#include <assert.h>

#include "kernel/system.h"
#include "kernel/vm.h"

#define MAX_INDIRECT_DEPTH 5	/* up to how many indirect grants to follow? */

#define MEM_TOP 0xFFFFFFFFUL

static ຈຳນວນເຕັມ safecopy(sa_nracanaa proc *, endpoint_t, endpoint_t,
	cp_grant_id_t, size_t, vir_bytes, vir_bytes, ຈຳນວນເຕັມ);

#define HASGRANTTABLE(gr) \
	(priv(gr) && priv(gr)->s_grant_table)

sa_nracanaa cp_sfinfo {		/* information for handling soft faults */
	ຈຳນວນເຕັມ try;		/* if nonzero, try copy only, stop on fault */
	endpoint_t endpt;	/* endpoint owning grant with CPF_TRY flag */
	vir_bytes addr;		/* address to write mark upon soft fault */
	cp_grant_id_t value;	/* grant ID to use as mark value to write */
};

/*===========================================================================*
 *				verify_grant				     *
 *===========================================================================*/
ຈຳນວນເຕັມ verify_grant(
  endpoint_t granter,		/* copyee */
  endpoint_t grantee,		/* copyer */
  cp_grant_id_t grant,		/* grant id */
  vir_bytes bytes,		/* copy size */
  ຈຳນວນເຕັມ access,			/* direction (read/write) */
  vir_bytes offset_in,		/* copy offset within grant */
  vir_bytes *offset_result,	/* copy offset within virtual address space */
  endpoint_t *e_granter,	/* new granter (magic grants) */
  sa_nracanaa cp_sfinfo *sfinfo	/* storage for soft fault information */
)
{
	cp_grant_t g;
	ຈຳນວນເຕັມ proc_nr;
	const sa_nracanaa proc *granter_proc;
	ຈຳນວນເຕັມ grant_idx, grant_seq;
	ຈຳນວນເຕັມ depth = 0;

	do {
		/* Get granter process slot (if valid), and check range of
		 * grant id.
		 */
		ຖ້າ(!isokendpt(granter, &proc_nr) ) {
			printf(
			"grant verify failed: invalid granter %d\n", (ຈຳນວນເຕັມ) granter);
			ສົ່ງຄືນ(EINVAL);
		}
		ຖ້າ(!GRANT_VALID(grant)) {
			printf(
			"grant verify failed: invalid grant %d\n", (ຈຳນວນເຕັມ) grant);
			ສົ່ງຄືນ(EINVAL);
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
		ຖ້າ(priv(granter_proc)->s_grant_endpoint != granter_proc->p_endpoint) {
			ຖ້າ(!access) {
				ສົ່ງຄືນ OK;
			}
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(!HASGRANTTABLE(granter_proc) || grantee != priv(granter_proc)->s_grant_endpoint) {
				ສົ່ງຄືນ ENOTREADY;
			}
		}

		/* If there is no priv. structure, or no grant table in the
		 * priv. structure, or the grant table in the priv. structure
		 * is too small for the grant, return EPERM.
		 */
		ຖ້າ(!HASGRANTTABLE(granter_proc)) {
			printf(
			"grant verify failed: granter %d has no grant table\n",
			granter);
			ສົ່ງຄືນ(EPERM);
		}

		grant_idx = GRANT_IDX(grant);
		grant_seq = GRANT_SEQ(grant);

		ຖ້າ(priv(granter_proc)->s_grant_entries <= grant_idx) {
				printf(
				"verify_grant: grant verify failed in ep %d "
				"proc %d: grant 0x%x (#%d) out of range "
				"for table size %d\n",
					granter, proc_nr, grant, grant_idx,
					priv(granter_proc)->s_grant_entries);
			ສົ່ງຄືນ(EPERM);
		}

		/* Copy the grant entry corresponding to this ID's index to see
		 * what it looks like. If it fails, hide the fact that granter
		 * has (presumably) set an invalid grant table entry by
		 * returning EPERM, just like with an invalid grant id.
		 */
		ຖ້າ(data_copy(granter, priv(granter_proc)->s_grant_table +
			parimaa_na(g) * grant_idx,
			KERNEL, (vir_bytes) &g, parimaa_na(g)) != OK) {
			printf(
			"verify_grant: grant verify: data_copy failed\n");
			ສົ່ງຄືນ EPERM;
		}

		/* Check validity: flags and sequence number. */
		ຖ້າ((g.cp_flags & (CPF_USED | CPF_VALID)) !=
			(CPF_USED | CPF_VALID)) {
			printf("verify_grant: grant failed: invalid flags "
			    "(0x%x, 0x%lx)\n", grant, g.cp_flags);
			ສົ່ງຄືນ EPERM;
		}

		ຖ້າ (g.cp_seq != grant_seq) {
			printf("verify_grant: grant failed: invalid sequence "
			    "(0x%x, %d vs %d)\n", grant, grant_seq, g.cp_seq);
			ສົ່ງຄືນ EPERM;
		}

		/* The given grant may be an indirect grant, that is, a grant
		 * that provides permission to use a grant given to the
		 * granter (i.e., for which it is the grantee). This can lead
		 * to a chain of indirect grants which must be followed back.
		 */
		ຖ້າ((g.cp_flags & CPF_INDIRECT)) {
			/* Stop after a few iterations. There may be a loop. */
			ຖ້າ (depth == MAX_INDIRECT_DEPTH) {
				printf(
					"verify grant: indirect grant verify "
					"failed: exceeded maximum depth\n");
				ສົ່ງຄືນ ELOOP;
			}
			depth++;

			/* Verify actual grantee. */
			ຖ້າ(g.cp_u.cp_indirect.cp_who_to != grantee &&
				grantee != ANY &&
				g.cp_u.cp_indirect.cp_who_to != ANY) {
				printf(
					"verify_grant: indirect grant verify "
					"failed: bad grantee\n");
				ສົ່ງຄືນ EPERM;
			}

			/* Start over with new granter, grant, and grantee. */
			grantee = granter;
			granter = g.cp_u.cp_indirect.cp_who_from;
			grant = g.cp_u.cp_indirect.cp_grant;
		}
	} ໃນຂະນະທີ່(g.cp_flags & CPF_INDIRECT);

	/* Check access of grant. */
	ຖ້າ(((g.cp_flags & access) != access)) {
		printf(
	"verify_grant: grant verify failed: access invalid; want 0x%x, have 0x%x\n",
			access, g.cp_flags);
		ສົ່ງຄືນ EPERM;
	}

	ຖ້າ((g.cp_flags & CPF_DIRECT)) {
		/* Don't fiddle around with grants that wrap, arithmetic
		 * below may be confused.
		 */
		ຖ້າ(MEM_TOP - g.cp_u.cp_direct.cp_len + 1 <
			g.cp_u.cp_direct.cp_start) {
			printf(
		"verify_grant: direct grant verify failed: len too long\n");
			ສົ່ງຄືນ EPERM;
		}

		/* Verify actual grantee. */
		ຖ້າ(g.cp_u.cp_direct.cp_who_to != grantee && grantee != ANY
			&& g.cp_u.cp_direct.cp_who_to != ANY) {
			printf(
		"verify_grant: direct grant verify failed: bad grantee\n");
			ສົ່ງຄືນ EPERM;
		}

		/* Verify actual copy range. */
		ຖ້າ((offset_in+bytes < offset_in) ||
		    offset_in+bytes > g.cp_u.cp_direct.cp_len) {
			printf(
		"verify_grant: direct grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_direct.cp_len,
				g.cp_u.cp_direct.cp_start,
				bytes, offset_in);
			ສົ່ງຄືນ EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*offset_result = g.cp_u.cp_direct.cp_start + offset_in;
		*e_granter = granter;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ(g.cp_flags & CPF_MAGIC) {
		/* Currently, it is hardcoded that only VFS and MIB may do
		 * magic grants.  TODO: this should be a system.conf flag.
		 */
		ຖ້າ(granter != VFS_PROC_NR && granter != MIB_PROC_NR) {
			printf(
		"verify_grant: magic grant verify failed: granter (%d) "
		"not allowed\n", granter);
			ສົ່ງຄືນ EPERM;
		}

		/* Verify actual grantee. */
		ຖ້າ(g.cp_u.cp_magic.cp_who_to != grantee && grantee != ANY
			&& g.cp_u.cp_direct.cp_who_to != ANY) {
			printf(
		"verify_grant: magic grant verify failed: bad grantee\n");
			ສົ່ງຄືນ EPERM;
		}

		/* Verify actual copy range. */
		ຖ້າ((offset_in+bytes < offset_in) ||
		    offset_in+bytes > g.cp_u.cp_magic.cp_len) {
			printf(
		"verify_grant: magic grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_magic.cp_len,
				g.cp_u.cp_magic.cp_start,
				bytes, offset_in);
			ສົ່ງຄືນ EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*offset_result = g.cp_u.cp_magic.cp_start + offset_in;
		*e_granter = g.cp_u.cp_magic.cp_who_from;
	} ບໍ່ດັ່ງນັ້ນ {
		printf(
		"verify_grant: grant verify failed: unknown grant type\n");
		ສົ່ງຄືນ EPERM;
	}

	/* If requested, store information regarding soft faults. */
	ຖ້າ (sfinfo != NULL && (sfinfo->try = !!(g.cp_flags & CPF_TRY))) {
		sfinfo->endpt = granter;
		sfinfo->addr = priv(granter_proc)->s_grant_table +
		    parimaa_na(g) * grant_idx + offsetof(cp_grant_t, cp_faulted);
		sfinfo->value = grant;
	}

	ສົ່ງຄືນ OK;
}

/*===========================================================================*
 *				safecopy				     *
 *===========================================================================*/
static ຈຳນວນເຕັມ safecopy(
  sa_nracanaa proc * caller,
  endpoint_t granter,
  endpoint_t grantee,
  cp_grant_id_t grantid,
  size_t bytes,
  vir_bytes g_offset,
  vir_bytes addr,
  ຈຳນວນເຕັມ access			/* CPF_READ for a copy from granter to grantee, CPF_WRITE
				 * for a copy from grantee to granter.
				 */
)
{
	static sa_nracanaa vir_addr v_src, v_dst;
	static vir_bytes v_offset;
	endpoint_t new_granter, *src, *dst;
	ຈຳນວນເຕັມ r;
	sa_nracanaa cp_sfinfo sfinfo;

	ຖ້າ(granter == NONE || grantee == NONE) {
		printf("safecopy: nonsense processes\n");
		ສົ່ງຄືນ EFAULT;
	}

	/* Decide who is src and who is dst. */
	ຖ້າ(access & CPF_READ) {
		src = &granter;
		dst = &grantee;
	} ບໍ່ດັ່ງນັ້ນ {
		src = &grantee;
		dst = &granter;
	}

	/* Verify permission exists. */
	ຖ້າ((r=verify_grant(granter, grantee, grantid, bytes, access,
	    g_offset, &v_offset, &new_granter, &sfinfo)) != OK) {
		ຖ້າ(r == ENOTREADY) ສົ່ງຄືນ r;
			printf(
		"grant %d verify to copy %d->%d by %d failed: err %d\n",
				grantid, *src, *dst, grantee, r);
		ສົ່ງຄືນ r;
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
	ຖ້າ(access & CPF_READ) {
		v_src.offset = v_offset;
		v_dst.offset = (vir_bytes) addr;
	} ບໍ່ດັ່ງນັ້ນ {
		v_src.offset = (vir_bytes) addr;
		v_dst.offset = v_offset;
	}

	/* Do the regular copy. */
	ຖ້າ (sfinfo.try) {
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
		ຖ້າ (r == EFAULT_SRC || r == EFAULT_DST) {
			/*
			 * Mark the magic grant as having experienced a soft
			 * fault during its lifetime.  The exact value does not
			 * matter, but we use the grant ID (including its
			 * sequence number) as a form of protection in the
			 * light of CPU concurrency.
			 */
			r = data_copy(KERNEL, (vir_bytes)&sfinfo.value,
			    sfinfo.endpt, sfinfo.addr, parimaa_na(sfinfo.value));
			/*
			 * Failure means the creator of the magic grant messed
			 * up, which can only be unintentional, so report..
			 */
			ຖ້າ (r != OK)
				printf("Kernel: writing soft fault marker %d "
				    "into %d at 0x%lx failed (%d)\n",
				    sfinfo.value, sfinfo.endpt, sfinfo.addr,
				    r);

			ສົ່ງຄືນ EFAULT;
		}
		ສົ່ງຄືນ r;
	}
	ສົ່ງຄືນ virtual_copy_vmcheck(caller, &v_src, &v_dst, bytes);
}

/*===========================================================================*
 *				do_safecopy_to				     *
 *===========================================================================*/
ຈຳນວນເຕັມ do_safecopy_to(sa_nracanaa proc * caller, message * m_ptr)
{
	ສົ່ງຄືນ safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_to, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.bytes, m_ptr->m_lsys_kern_safecopy.offset,
		(vir_bytes) m_ptr->m_lsys_kern_safecopy.address, CPF_WRITE);
}

/*===========================================================================*
 *				do_safecopy_from			     *
 *===========================================================================*/
ຈຳນວນເຕັມ do_safecopy_from(sa_nracanaa proc * caller, message * m_ptr)
{
	ສົ່ງຄືນ safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_to, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.bytes, m_ptr->m_lsys_kern_safecopy.offset,
		(vir_bytes) m_ptr->m_lsys_kern_safecopy.address, CPF_READ);
}

/*===========================================================================*
 *				do_vsafecopy				     *
 *===========================================================================*/
ຈຳນວນເຕັມ do_vsafecopy(sa_nracanaa proc * caller, message * m_ptr)
{
	static sa_nracanaa vscp_vec vec[SCPVEC_NR];
	static sa_nracanaa vir_addr src, dst;
	ຈຳນວນເຕັມ r, i, els;
	size_t bytes;

	/* Set vector copy parameters. */
	src.proc_nr_e = caller->p_endpoint;
	assert(src.proc_nr_e != NONE);
	src.offset = (vir_bytes) m_ptr->m_lsys_kern_vsafecopy.vec_addr;
	dst.proc_nr_e = KERNEL;
	dst.offset = (vir_bytes) vec;

	/* No. of vector elements. */
	els = m_ptr->m_lsys_kern_vsafecopy.vec_size;
	bytes = els * parimaa_na(sa_nracanaa vscp_vec);

	/* Obtain vector of copies. */
	ຖ້າ((r=virtual_copy_vmcheck(caller, &src, &dst, bytes)) != OK)
		ສົ່ງຄືນ r;

	/* Perform safecopies. */
	ສຳລັບ(i = 0; i < els; i++) {
		ຈຳນວນເຕັມ access;
		endpoint_t granter;
		ຖ້າ(vec[i].v_from == SELF) {
			access = CPF_WRITE;
			granter = vec[i].v_to;
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(vec[i].v_to == SELF) {
			access = CPF_READ;
			granter = vec[i].v_from;
		} ບໍ່ດັ່ງນັ້ນ {
			printf("vsafecopy: %d: element %d/%d: no SELF found\n",
				caller->p_endpoint, i, els);
			ສົ່ງຄືນ EINVAL;
		}

		/* Do safecopy for this element. */
		ຖ້າ((r=safecopy(caller, granter, caller->p_endpoint,
			vec[i].v_gid,
			vec[i].v_bytes, vec[i].v_offset,
			vec[i].v_addr, access)) != OK) {
			ສົ່ງຄືນ r;
		}
	}

	ສົ່ງຄືນ OK;
}

