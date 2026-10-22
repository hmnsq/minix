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

#شامل <assert.h>

#شامل "kernel/system.h"
#شامل "kernel/vm.h"

#تعریف_کرو MAX_بالواسطہ_گہرائی 5	/* up to how many indirect grants to follow? */

#تعریف_کرو MEM_چوٹی 0xFFFFFFFFUL

ساکن صحیح safecopy(ساخت proc *, endpoint_t, endpoint_t,
	cp_grant_id_t, size_t, vir_بائٹیں, vir_بائٹیں, صحیح);

#تعریف_کرو HASGRANTTABLE(gr) \
	(priv(gr) && priv(gr)->s_grant_جدول)

ساخت cp_sfinfo {		/* information for handling soft faults */
	صحیح try;		/* if nonzero, try copy only, stop on fault */
	endpoint_t endpt;	/* endpoint owning grant with CPF_TRY flag */
	vir_بائٹیں addr;		/* address to write mark upon soft fault */
	cp_grant_id_t قدر;	/* grant ID to use as mark value to write */
};

/*===========================================================================*
 *				verify_grant				     *
 *===========================================================================*/
صحیح verify_grant(
  endpoint_t granter,		/* copyee */
  endpoint_t grantee,		/* copyer */
  cp_grant_id_t grant,		/* grant id */
  vir_بائٹیں بائٹیں,		/* copy size */
  صحیح access,			/* direction (read/write) */
  vir_بائٹیں انحراف_اندر,		/* copy offset within grant */
  vir_بائٹیں *انحراف_نتیجہ,	/* copy offset within virtual address space */
  endpoint_t *e_granter,	/* new granter (magic grants) */
  ساخت cp_sfinfo *sfinfo	/* storage for soft fault information */
)
{
	cp_grant_t g;
	صحیح proc_nr;
	ثابت ساخت proc *granter_proc;
	صحیح grant_idx, grant_seq;
	صحیح گہرائی = 0;

	کرو {
		/* Get granter process slot (if valid), and check range of
		 * grant id.
		 */
		اگر(!isokendpt(granter, &proc_nr) ) {
			لکھو(
			"grant verify failed: invalid granter %d\n", (صحیح) granter);
			واپس(EINVAL);
		}
		اگر(!GRANT_VALID(grant)) {
			لکھو(
			"grant verify failed: invalid grant %d\n", (صحیح) grant);
			واپس(EINVAL);
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
		اگر(priv(granter_proc)->s_grant_endpoint != granter_proc->p_endpoint) {
			اگر(!access) {
				واپس OK;
			}
			ورنہ اگر(!HASGRANTTABLE(granter_proc) || grantee != priv(granter_proc)->s_grant_endpoint) {
				واپس ENOTREADY;
			}
		}

		/* If there is no priv. structure, or no grant table in the
		 * priv. structure, or the grant table in the priv. structure
		 * is too small for the grant, return EPERM.
		 */
		اگر(!HASGRANTTABLE(granter_proc)) {
			لکھو(
			"grant verify failed: granter %d has no grant table\n",
			granter);
			واپس(EPERM);
		}

		grant_idx = GRANT_IDX(grant);
		grant_seq = GRANT_SEQ(grant);

		اگر(priv(granter_proc)->s_grant_entries <= grant_idx) {
				لکھو(
				"verify_grant: grant verify failed in ep %d "
				"proc %d: grant 0x%x (#%d) out of range "
				"for table size %d\n",
					granter, proc_nr, grant, grant_idx,
					priv(granter_proc)->s_grant_entries);
			واپس(EPERM);
		}

		/* Copy the grant entry corresponding to this ID's index to see
		 * what it looks like. If it fails, hide the fact that granter
		 * has (presumably) set an invalid grant table entry by
		 * returning EPERM, just like with an invalid grant id.
		 */
		اگر(ڈیٹا_copy(granter, priv(granter_proc)->s_grant_جدول +
			حجم(g) * grant_idx,
			KERNEL, (vir_بائٹیں) &g, حجم(g)) != OK) {
			لکھو(
			"verify_grant: grant verify: data_copy failed\n");
			واپس EPERM;
		}

		/* Check validity: flags and sequence number. */
		اگر((g.cp_flags & (CPF_مستعمل | CPF_VALID)) !=
			(CPF_مستعمل | CPF_VALID)) {
			لکھو("verify_grant: grant failed: invalid flags "
			    "(0x%x, 0x%lx)\n", grant, g.cp_flags);
			واپس EPERM;
		}

		اگر (g.cp_seq != grant_seq) {
			لکھو("verify_grant: grant failed: invalid sequence "
			    "(0x%x, %d vs %d)\n", grant, grant_seq, g.cp_seq);
			واپس EPERM;
		}

		/* The given grant may be an indirect grant, that is, a grant
		 * that provides permission to use a grant given to the
		 * granter (i.e., for which it is the grantee). This can lead
		 * to a chain of indirect grants which must be followed back.
		 */
		اگر((g.cp_flags & CPF_بالواسطہ)) {
			/* Stop after a few iterations. There may be a loop. */
			اگر (گہرائی == MAX_بالواسطہ_گہرائی) {
				لکھو(
					"verify grant: indirect grant verify "
					"failed: exceeded maximum depth\n");
				واپس ELOOP;
			}
			گہرائی++;

			/* Verify actual grantee. */
			اگر(g.cp_u.cp_بالواسطہ.cp_who_مقصد != grantee &&
				grantee != ANY &&
				g.cp_u.cp_بالواسطہ.cp_who_مقصد != ANY) {
				لکھو(
					"verify_grant: indirect grant verify "
					"failed: bad grantee\n");
				واپس EPERM;
			}

			/* Start over with new granter, grant, and grantee. */
			grantee = granter;
			granter = g.cp_u.cp_بالواسطہ.cp_who_from;
			grant = g.cp_u.cp_بالواسطہ.cp_grant;
		}
	} جب(g.cp_flags & CPF_بالواسطہ);

	/* Check access of grant. */
	اگر(((g.cp_flags & access) != access)) {
		لکھو(
	"verify_grant: grant verify failed: access invalid; want 0x%x, have 0x%x\n",
			access, g.cp_flags);
		واپس EPERM;
	}

	اگر((g.cp_flags & CPF_براہ_راست)) {
		/* Don't fiddle around with grants that wrap, arithmetic
		 * below may be confused.
		 */
		اگر(MEM_چوٹی - g.cp_u.cp_براہ_راست.cp_len + 1 <
			g.cp_u.cp_براہ_راست.cp_آغاز) {
			لکھو(
		"verify_grant: direct grant verify failed: len too long\n");
			واپس EPERM;
		}

		/* Verify actual grantee. */
		اگر(g.cp_u.cp_براہ_راست.cp_who_مقصد != grantee && grantee != ANY
			&& g.cp_u.cp_براہ_راست.cp_who_مقصد != ANY) {
			لکھو(
		"verify_grant: direct grant verify failed: bad grantee\n");
			واپس EPERM;
		}

		/* Verify actual copy range. */
		اگر((انحراف_اندر+بائٹیں < انحراف_اندر) ||
		    انحراف_اندر+بائٹیں > g.cp_u.cp_براہ_راست.cp_len) {
			لکھو(
		"verify_grant: direct grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_براہ_راست.cp_len,
				g.cp_u.cp_براہ_راست.cp_آغاز,
				بائٹیں, انحراف_اندر);
			واپس EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*انحراف_نتیجہ = g.cp_u.cp_براہ_راست.cp_آغاز + انحراف_اندر;
		*e_granter = granter;
	} ورنہ اگر(g.cp_flags & CPF_MAGIC) {
		/* Currently, it is hardcoded that only VFS and MIB may do
		 * magic grants.  TODO: this should be a system.conf flag.
		 */
		اگر(granter != VFS_PROC_NR && granter != MIB_PROC_NR) {
			لکھو(
		"verify_grant: magic grant verify failed: granter (%d) "
		"not allowed\n", granter);
			واپس EPERM;
		}

		/* Verify actual grantee. */
		اگر(g.cp_u.cp_magic.cp_who_مقصد != grantee && grantee != ANY
			&& g.cp_u.cp_براہ_راست.cp_who_مقصد != ANY) {
			لکھو(
		"verify_grant: magic grant verify failed: bad grantee\n");
			واپس EPERM;
		}

		/* Verify actual copy range. */
		اگر((انحراف_اندر+بائٹیں < انحراف_اندر) ||
		    انحراف_اندر+بائٹیں > g.cp_u.cp_magic.cp_len) {
			لکھو(
		"verify_grant: magic grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_magic.cp_len,
				g.cp_u.cp_magic.cp_آغاز,
				بائٹیں, انحراف_اندر);
			واپس EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*انحراف_نتیجہ = g.cp_u.cp_magic.cp_آغاز + انحراف_اندر;
		*e_granter = g.cp_u.cp_magic.cp_who_from;
	} ورنہ {
		لکھو(
		"verify_grant: grant verify failed: unknown grant type\n");
		واپس EPERM;
	}

	/* If requested, store information regarding soft faults. */
	اگر (sfinfo != تہی && (sfinfo->try = !!(g.cp_flags & CPF_TRY))) {
		sfinfo->endpt = granter;
		sfinfo->addr = priv(granter_proc)->s_grant_جدول +
		    حجم(g) * grant_idx + offsetof(cp_grant_t, cp_faulted);
		sfinfo->قدر = grant;
	}

	واپس OK;
}

/*===========================================================================*
 *				safecopy				     *
 *===========================================================================*/
ساکن صحیح safecopy(
  ساخت proc * caller,
  endpoint_t granter,
  endpoint_t grantee,
  cp_grant_id_t grantid,
  size_t بائٹیں,
  vir_بائٹیں g_انحراف,
  vir_بائٹیں addr,
  صحیح access			/* CPF_READ for a copy from granter to grantee, CPF_WRITE
				 * for a copy from grantee to granter.
				 */
)
{
	ساکن ساخت vir_addr v_src, v_dst;
	ساکن vir_بائٹیں v_انحراف;
	endpoint_t new_granter, *src, *dst;
	صحیح r;
	ساخت cp_sfinfo sfinfo;

	اگر(granter == NONE || grantee == NONE) {
		لکھو("safecopy: nonsense processes\n");
		واپس EFAULT;
	}

	/* Decide who is src and who is dst. */
	اگر(access & CPF_READ) {
		src = &granter;
		dst = &grantee;
	} ورنہ {
		src = &grantee;
		dst = &granter;
	}

	/* Verify permission exists. */
	اگر((r=verify_grant(granter, grantee, grantid, بائٹیں, access,
	    g_انحراف, &v_انحراف, &new_granter, &sfinfo)) != OK) {
		اگر(r == ENOTREADY) واپس r;
			لکھو(
		"grant %d verify to copy %d->%d by %d failed: err %d\n",
				grantid, *src, *dst, grantee, r);
		واپس r;
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
	اگر(access & CPF_READ) {
		v_src.انحراف = v_انحراف;
		v_dst.انحراف = (vir_بائٹیں) addr;
	} ورنہ {
		v_src.انحراف = (vir_بائٹیں) addr;
		v_dst.انحراف = v_انحراف;
	}

	/* Do the regular copy. */
	اگر (sfinfo.try) {
		/*
		 * Try copying without transparently faulting in pages.
		 * TODO: while CPF_TRY is meant to protect against deadlocks on
		 * memory-mapped files in file systems, it seems that this case
		 * triggers faults a whole lot more often, resulting in extra
		 * overhead due to retried file system operations.  It might be
		 * a good idea to go through VM even in this case, and have VM
		 * fail (only) if the affected page belongs to a file mapping.
		 */
		r = virtual_copy(&v_src, &v_dst, بائٹیں);
		اگر (r == EFAULT_SRC || r == EFAULT_DST) {
			/*
			 * Mark the magic grant as having experienced a soft
			 * fault during its lifetime.  The exact value does not
			 * matter, but we use the grant ID (including its
			 * sequence number) as a form of protection in the
			 * light of CPU concurrency.
			 */
			r = ڈیٹا_copy(KERNEL, (vir_بائٹیں)&sfinfo.قدر,
			    sfinfo.endpt, sfinfo.addr, حجم(sfinfo.قدر));
			/*
			 * Failure means the creator of the magic grant messed
			 * up, which can only be unintentional, so report..
			 */
			اگر (r != OK)
				لکھو("Kernel: writing soft fault marker %d "
				    "into %d at 0x%lx failed (%d)\n",
				    sfinfo.قدر, sfinfo.endpt, sfinfo.addr,
				    r);

			واپس EFAULT;
		}
		واپس r;
	}
	واپس virtual_copy_vmcheck(caller, &v_src, &v_dst, بائٹیں);
}

/*===========================================================================*
 *				do_safecopy_to				     *
 *===========================================================================*/
صحیح do_safecopy_مقصد(ساخت proc * caller, پیغام * m_ptr)
{
	واپس safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_مقصد, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.بائٹیں, m_ptr->m_lsys_kern_safecopy.انحراف,
		(vir_بائٹیں) m_ptr->m_lsys_kern_safecopy.پتہ, CPF_WRITE);
}

/*===========================================================================*
 *				do_safecopy_from			     *
 *===========================================================================*/
صحیح do_safecopy_from(ساخت proc * caller, پیغام * m_ptr)
{
	واپس safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_مقصد, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.بائٹیں, m_ptr->m_lsys_kern_safecopy.انحراف,
		(vir_بائٹیں) m_ptr->m_lsys_kern_safecopy.پتہ, CPF_READ);
}

/*===========================================================================*
 *				do_vsafecopy				     *
 *===========================================================================*/
صحیح do_vsafecopy(ساخت proc * caller, پیغام * m_ptr)
{
	ساکن ساخت vscp_vec vec[SCPVEC_NR];
	ساکن ساخت vir_addr src, dst;
	صحیح r, i, els;
	size_t بائٹیں;

	/* Set vector copy parameters. */
	src.proc_nr_e = caller->p_endpoint;
	أكد(src.proc_nr_e != NONE);
	src.انحراف = (vir_بائٹیں) m_ptr->m_lsys_kern_vsafecopy.vec_addr;
	dst.proc_nr_e = KERNEL;
	dst.انحراف = (vir_بائٹیں) vec;

	/* No. of vector elements. */
	els = m_ptr->m_lsys_kern_vsafecopy.vec_سائز;
	بائٹیں = els * حجم(ساخت vscp_vec);

	/* Obtain vector of copies. */
	اگر((r=virtual_copy_vmcheck(caller, &src, &dst, بائٹیں)) != OK)
		واپس r;

	/* Perform safecopies. */
	برائے(i = 0; i < els; i++) {
		صحیح access;
		endpoint_t granter;
		اگر(vec[i].v_from == SELF) {
			access = CPF_WRITE;
			granter = vec[i].v_مقصد;
		} ورنہ اگر(vec[i].v_مقصد == SELF) {
			access = CPF_READ;
			granter = vec[i].v_from;
		} ورنہ {
			لکھو("vsafecopy: %d: element %d/%d: no SELF found\n",
				caller->p_endpoint, i, els);
			واپس EINVAL;
		}

		/* Do safecopy for this element. */
		اگر((r=safecopy(caller, granter, caller->p_endpoint,
			vec[i].v_gid,
			vec[i].v_بائٹیں, vec[i].v_انحراف,
			vec[i].v_addr, access)) != OK) {
			واپس r;
		}
	}

	واپس OK;
}

