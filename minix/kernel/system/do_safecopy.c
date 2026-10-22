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

static 整数 safecopy(struct proc *, endpoint_t, endpoint_t,
	cp_grant_id_t, size_t, vir_bytes, vir_bytes, 整数);

#define HASGRANTTABLE(gr) \
	(priv(gr) && priv(gr)->s_grant_table)

struct cp_sfinfo {		/* information for handling soft faults */
	整数 try;		/* if nonzero, try copy only, stop on fault */
	endpoint_t endpt;	/* endpoint owning grant with CPF_TRY flag */
	vir_bytes addr;		/* address to write mark upon soft fault */
	cp_grant_id_t value;	/* grant ID to use as mark value to write */
};

/*===========================================================================*
 *				verify_grant				     *
 *===========================================================================*/
整数 verify_grant(
  endpoint_t granter,		/* copyee */
  endpoint_t grantee,		/* copyer */
  cp_grant_id_t grant,		/* grant id */
  vir_bytes bytes,		/* copy size */
  整数 access,			/* direction (read/write) */
  vir_bytes offset_in,		/* copy offset within grant */
  vir_bytes *offset_result,	/* copy offset within virtual address space */
  endpoint_t *e_granter,	/* new granter (magic grants) */
  struct cp_sfinfo *sfinfo	/* storage for soft fault information */
)
{
	cp_grant_t g;
	整数 proc_nr;
	const struct proc *granter_proc;
	整数 grant_idx, grant_seq;
	整数 depth = 0;

	do {
		/* Get granter process slot (if valid), and check range of
		 * grant id.
		 */
		如果(!isokendpt(granter, &proc_nr) ) {
			printf(
			"grant verify failed: invalid granter %d\n", (整数) granter);
			返回(EINVAL);
		}
		如果(!GRANT_VALID(grant)) {
			printf(
			"grant verify failed: invalid grant %d\n", (整数) grant);
			返回(EINVAL);
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
		如果(priv(granter_proc)->s_grant_endpoint != granter_proc->p_endpoint) {
			如果(!access) {
				返回 OK;
			}
			否则 如果(!HASGRANTTABLE(granter_proc) || grantee != priv(granter_proc)->s_grant_endpoint) {
				返回 ENOTREADY;
			}
		}

		/* If there is no priv. structure, or no grant table in the
		 * priv. structure, or the grant table in the priv. structure
		 * is too small for the grant, return EPERM.
		 */
		如果(!HASGRANTTABLE(granter_proc)) {
			printf(
			"grant verify failed: granter %d has no grant table\n",
			granter);
			返回(EPERM);
		}

		grant_idx = GRANT_IDX(grant);
		grant_seq = GRANT_SEQ(grant);

		如果(priv(granter_proc)->s_grant_entries <= grant_idx) {
				printf(
				"verify_grant: grant verify failed in ep %d "
				"proc %d: grant 0x%x (#%d) out of range "
				"for table size %d\n",
					granter, proc_nr, grant, grant_idx,
					priv(granter_proc)->s_grant_entries);
			返回(EPERM);
		}

		/* Copy the grant entry corresponding to this ID's index to see
		 * what it looks like. If it fails, hide the fact that granter
		 * has (presumably) set an invalid grant table entry by
		 * returning EPERM, just like with an invalid grant id.
		 */
		如果(data_copy(granter, priv(granter_proc)->s_grant_table +
			sizeof(g) * grant_idx,
			KERNEL, (vir_bytes) &g, sizeof(g)) != OK) {
			printf(
			"verify_grant: grant verify: data_copy failed\n");
			返回 EPERM;
		}

		/* Check validity: flags and sequence number. */
		如果((g.cp_flags & (CPF_USED | CPF_VALID)) !=
			(CPF_USED | CPF_VALID)) {
			printf("verify_grant: grant failed: invalid flags "
			    "(0x%x, 0x%lx)\n", grant, g.cp_flags);
			返回 EPERM;
		}

		如果 (g.cp_seq != grant_seq) {
			printf("verify_grant: grant failed: invalid sequence "
			    "(0x%x, %d vs %d)\n", grant, grant_seq, g.cp_seq);
			返回 EPERM;
		}

		/* The given grant may be an indirect grant, that is, a grant
		 * that provides permission to use a grant given to the
		 * granter (i.e., for which it is the grantee). This can lead
		 * to a chain of indirect grants which must be followed back.
		 */
		如果((g.cp_flags & CPF_INDIRECT)) {
			/* Stop after a few iterations. There may be a loop. */
			如果 (depth == MAX_INDIRECT_DEPTH) {
				printf(
					"verify grant: indirect grant verify "
					"failed: exceeded maximum depth\n");
				返回 ELOOP;
			}
			depth++;

			/* Verify actual grantee. */
			如果(g.cp_u.cp_indirect.cp_who_to != grantee &&
				grantee != ANY &&
				g.cp_u.cp_indirect.cp_who_to != ANY) {
				printf(
					"verify_grant: indirect grant verify "
					"failed: bad grantee\n");
				返回 EPERM;
			}

			/* Start over with new granter, grant, and grantee. */
			grantee = granter;
			granter = g.cp_u.cp_indirect.cp_who_from;
			grant = g.cp_u.cp_indirect.cp_grant;
		}
	} 当(g.cp_flags & CPF_INDIRECT);

	/* Check access of grant. */
	如果(((g.cp_flags & access) != access)) {
		printf(
	"verify_grant: grant verify failed: access invalid; want 0x%x, have 0x%x\n",
			access, g.cp_flags);
		返回 EPERM;
	}

	如果((g.cp_flags & CPF_DIRECT)) {
		/* Don't fiddle around with grants that wrap, arithmetic
		 * below may be confused.
		 */
		如果(MEM_TOP - g.cp_u.cp_direct.cp_len + 1 <
			g.cp_u.cp_direct.cp_start) {
			printf(
		"verify_grant: direct grant verify failed: len too long\n");
			返回 EPERM;
		}

		/* Verify actual grantee. */
		如果(g.cp_u.cp_direct.cp_who_to != grantee && grantee != ANY
			&& g.cp_u.cp_direct.cp_who_to != ANY) {
			printf(
		"verify_grant: direct grant verify failed: bad grantee\n");
			返回 EPERM;
		}

		/* Verify actual copy range. */
		如果((offset_in+bytes < offset_in) ||
		    offset_in+bytes > g.cp_u.cp_direct.cp_len) {
			printf(
		"verify_grant: direct grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_direct.cp_len,
				g.cp_u.cp_direct.cp_start,
				bytes, offset_in);
			返回 EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*offset_result = g.cp_u.cp_direct.cp_start + offset_in;
		*e_granter = granter;
	} 否则 如果(g.cp_flags & CPF_MAGIC) {
		/* Currently, it is hardcoded that only VFS and MIB may do
		 * magic grants.  TODO: this should be a system.conf flag.
		 */
		如果(granter != VFS_PROC_NR && granter != MIB_PROC_NR) {
			printf(
		"verify_grant: magic grant verify failed: granter (%d) "
		"not allowed\n", granter);
			返回 EPERM;
		}

		/* Verify actual grantee. */
		如果(g.cp_u.cp_magic.cp_who_to != grantee && grantee != ANY
			&& g.cp_u.cp_direct.cp_who_to != ANY) {
			printf(
		"verify_grant: magic grant verify failed: bad grantee\n");
			返回 EPERM;
		}

		/* Verify actual copy range. */
		如果((offset_in+bytes < offset_in) ||
		    offset_in+bytes > g.cp_u.cp_magic.cp_len) {
			printf(
		"verify_grant: magic grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_magic.cp_len,
				g.cp_u.cp_magic.cp_start,
				bytes, offset_in);
			返回 EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*offset_result = g.cp_u.cp_magic.cp_start + offset_in;
		*e_granter = g.cp_u.cp_magic.cp_who_from;
	} 否则 {
		printf(
		"verify_grant: grant verify failed: unknown grant type\n");
		返回 EPERM;
	}

	/* If requested, store information regarding soft faults. */
	如果 (sfinfo != NULL && (sfinfo->try = !!(g.cp_flags & CPF_TRY))) {
		sfinfo->endpt = granter;
		sfinfo->addr = priv(granter_proc)->s_grant_table +
		    sizeof(g) * grant_idx + offsetof(cp_grant_t, cp_faulted);
		sfinfo->value = grant;
	}

	返回 OK;
}

/*===========================================================================*
 *				safecopy				     *
 *===========================================================================*/
static 整数 safecopy(
  struct proc * caller,
  endpoint_t granter,
  endpoint_t grantee,
  cp_grant_id_t grantid,
  size_t bytes,
  vir_bytes g_offset,
  vir_bytes addr,
  整数 access			/* CPF_READ for a copy from granter to grantee, CPF_WRITE
				 * for a copy from grantee to granter.
				 */
)
{
	static struct vir_addr v_src, v_dst;
	static vir_bytes v_offset;
	endpoint_t new_granter, *src, *dst;
	整数 r;
	struct cp_sfinfo sfinfo;

	如果(granter == NONE || grantee == NONE) {
		printf("safecopy: nonsense processes\n");
		返回 EFAULT;
	}

	/* Decide who is src and who is dst. */
	如果(access & CPF_READ) {
		src = &granter;
		dst = &grantee;
	} 否则 {
		src = &grantee;
		dst = &granter;
	}

	/* Verify permission exists. */
	如果((r=verify_grant(granter, grantee, grantid, bytes, access,
	    g_offset, &v_offset, &new_granter, &sfinfo)) != OK) {
		如果(r == ENOTREADY) 返回 r;
			printf(
		"grant %d verify to copy %d->%d by %d failed: err %d\n",
				grantid, *src, *dst, grantee, r);
		返回 r;
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
	如果(access & CPF_READ) {
		v_src.offset = v_offset;
		v_dst.offset = (vir_bytes) addr;
	} 否则 {
		v_src.offset = (vir_bytes) addr;
		v_dst.offset = v_offset;
	}

	/* Do the regular copy. */
	如果 (sfinfo.try) {
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
		如果 (r == EFAULT_SRC || r == EFAULT_DST) {
			/*
			 * Mark the magic grant as having experienced a soft
			 * fault during its lifetime.  The exact value does not
			 * matter, but we use the grant ID (including its
			 * sequence number) as a form of protection in the
			 * light of CPU concurrency.
			 */
			r = data_copy(KERNEL, (vir_bytes)&sfinfo.value,
			    sfinfo.endpt, sfinfo.addr, sizeof(sfinfo.value));
			/*
			 * Failure means the creator of the magic grant messed
			 * up, which can only be unintentional, so report..
			 */
			如果 (r != OK)
				printf("Kernel: writing soft fault marker %d "
				    "into %d at 0x%lx failed (%d)\n",
				    sfinfo.value, sfinfo.endpt, sfinfo.addr,
				    r);

			返回 EFAULT;
		}
		返回 r;
	}
	返回 virtual_copy_vmcheck(caller, &v_src, &v_dst, bytes);
}

/*===========================================================================*
 *				do_safecopy_to				     *
 *===========================================================================*/
整数 do_safecopy_to(struct proc * caller, message * m_ptr)
{
	返回 safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_to, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.bytes, m_ptr->m_lsys_kern_safecopy.offset,
		(vir_bytes) m_ptr->m_lsys_kern_safecopy.address, CPF_WRITE);
}

/*===========================================================================*
 *				do_safecopy_from			     *
 *===========================================================================*/
整数 do_safecopy_from(struct proc * caller, message * m_ptr)
{
	返回 safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_to, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.bytes, m_ptr->m_lsys_kern_safecopy.offset,
		(vir_bytes) m_ptr->m_lsys_kern_safecopy.address, CPF_READ);
}

/*===========================================================================*
 *				do_vsafecopy				     *
 *===========================================================================*/
整数 do_vsafecopy(struct proc * caller, message * m_ptr)
{
	static struct vscp_vec vec[SCPVEC_NR];
	static struct vir_addr src, dst;
	整数 r, i, els;
	size_t bytes;

	/* Set vector copy parameters. */
	src.proc_nr_e = caller->p_endpoint;
	assert(src.proc_nr_e != NONE);
	src.offset = (vir_bytes) m_ptr->m_lsys_kern_vsafecopy.vec_addr;
	dst.proc_nr_e = KERNEL;
	dst.offset = (vir_bytes) vec;

	/* No. of vector elements. */
	els = m_ptr->m_lsys_kern_vsafecopy.vec_size;
	bytes = els * sizeof(struct vscp_vec);

	/* Obtain vector of copies. */
	如果((r=virtual_copy_vmcheck(caller, &src, &dst, bytes)) != OK)
		返回 r;

	/* Perform safecopies. */
	为(i = 0; i < els; i++) {
		整数 access;
		endpoint_t granter;
		如果(vec[i].v_from == SELF) {
			access = CPF_WRITE;
			granter = vec[i].v_to;
		} 否则 如果(vec[i].v_to == SELF) {
			access = CPF_READ;
			granter = vec[i].v_from;
		} 否则 {
			printf("vsafecopy: %d: element %d/%d: no SELF found\n",
				caller->p_endpoint, i, els);
			返回 EINVAL;
		}

		/* Do safecopy for this element. */
		如果((r=safecopy(caller, granter, caller->p_endpoint,
			vec[i].v_gid,
			vec[i].v_bytes, vec[i].v_offset,
			vec[i].v_addr, access)) != OK) {
			返回 r;
		}
	}

	返回 OK;
}

