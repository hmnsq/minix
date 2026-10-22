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

#अन्तर्भाव <assert.h>

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव "kernel/vm.h"

#परिभाषय MAX_परोक्षम्_गभीरता 5	/* up to how many indirect grants to follow? */

#परिभाषय MEM_शिखरम् 0xFFFFFFFFUL

स्थिर पूर्णाङ्क safecopy(संरचना proc *, endpoint_t, endpoint_t,
	cp_grant_id_t, size_t, vir_अष्टकानि, vir_अष्टकानि, पूर्णाङ्क);

#परिभाषय HASGRANTTABLE(gr) \
	(priv(gr) && priv(gr)->s_grant_सारणी)

संरचना cp_sfinfo {		/* information for handling soft faults */
	पूर्णाङ्क try;		/* if nonzero, try copy only, stop on fault */
	endpoint_t endpt;	/* endpoint owning grant with CPF_TRY flag */
	vir_अष्टकानि addr;		/* address to write mark upon soft fault */
	cp_grant_id_t मानम्;	/* grant ID to use as mark value to write */
};

/*===========================================================================*
 *				verify_grant				     *
 *===========================================================================*/
पूर्णाङ्क verify_grant(
  endpoint_t granter,		/* copyee */
  endpoint_t grantee,		/* copyer */
  cp_grant_id_t grant,		/* grant id */
  vir_अष्टकानि अष्टकानि,		/* copy size */
  पूर्णाङ्क access,			/* direction (read/write) */
  vir_अष्टकानि विस्थापनम्_अन्तर्,		/* copy offset within grant */
  vir_अष्टकानि *विस्थापनम्_परिणाम,	/* copy offset within virtual address space */
  endpoint_t *e_granter,	/* new granter (magic grants) */
  संरचना cp_sfinfo *sfinfo	/* storage for soft fault information */
)
{
	cp_grant_t g;
	पूर्णाङ्क proc_nr;
	ध्रुव संरचना proc *granter_proc;
	पूर्णाङ्क grant_idx, grant_seq;
	पूर्णाङ्क गभीरता = 0;

	कुरु {
		/* Get granter process slot (if valid), and check range of
		 * grant id.
		 */
		यदि(!isokendpt(granter, &proc_nr) ) {
			प्रारूप_लिख(
			"grant verify failed: invalid granter %d\n", (पूर्णाङ्क) granter);
			प्रत्यागम(EINVAL);
		}
		यदि(!GRANT_VALID(grant)) {
			प्रारूप_लिख(
			"grant verify failed: invalid grant %d\n", (पूर्णाङ्क) grant);
			प्रत्यागम(EINVAL);
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
		यदि(priv(granter_proc)->s_grant_endpoint != granter_proc->p_endpoint) {
			यदि(!access) {
				प्रत्यागम OK;
			}
			अन्यथा यदि(!HASGRANTTABLE(granter_proc) || grantee != priv(granter_proc)->s_grant_endpoint) {
				प्रत्यागम ENOTREADY;
			}
		}

		/* If there is no priv. structure, or no grant table in the
		 * priv. structure, or the grant table in the priv. structure
		 * is too small for the grant, return EPERM.
		 */
		यदि(!HASGRANTTABLE(granter_proc)) {
			प्रारूप_लिख(
			"grant verify failed: granter %d has no grant table\n",
			granter);
			प्रत्यागम(EPERM);
		}

		grant_idx = GRANT_IDX(grant);
		grant_seq = GRANT_SEQ(grant);

		यदि(priv(granter_proc)->s_grant_entries <= grant_idx) {
				प्रारूप_लिख(
				"verify_grant: grant verify failed in ep %d "
				"proc %d: grant 0x%x (#%d) out of range "
				"for table size %d\n",
					granter, proc_nr, grant, grant_idx,
					priv(granter_proc)->s_grant_entries);
			प्रत्यागम(EPERM);
		}

		/* Copy the grant entry corresponding to this ID's index to see
		 * what it looks like. If it fails, hide the fact that granter
		 * has (presumably) set an invalid grant table entry by
		 * returning EPERM, just like with an invalid grant id.
		 */
		यदि(दत्त_copy(granter, priv(granter_proc)->s_grant_सारणी +
			परिमाण(g) * grant_idx,
			KERNEL, (vir_अष्टकानि) &g, परिमाण(g)) != OK) {
			प्रारूप_लिख(
			"verify_grant: grant verify: data_copy failed\n");
			प्रत्यागम EPERM;
		}

		/* Check validity: flags and sequence number. */
		यदि((g.cp_flags & (CPF_प्रयुक्तम् | CPF_VALID)) !=
			(CPF_प्रयुक्तम् | CPF_VALID)) {
			प्रारूप_लिख("verify_grant: grant failed: invalid flags "
			    "(0x%x, 0x%lx)\n", grant, g.cp_flags);
			प्रत्यागम EPERM;
		}

		यदि (g.cp_seq != grant_seq) {
			प्रारूप_लिख("verify_grant: grant failed: invalid sequence "
			    "(0x%x, %d vs %d)\n", grant, grant_seq, g.cp_seq);
			प्रत्यागम EPERM;
		}

		/* The given grant may be an indirect grant, that is, a grant
		 * that provides permission to use a grant given to the
		 * granter (i.e., for which it is the grantee). This can lead
		 * to a chain of indirect grants which must be followed back.
		 */
		यदि((g.cp_flags & CPF_परोक्षम्)) {
			/* Stop after a few iterations. There may be a loop. */
			यदि (गभीरता == MAX_परोक्षम्_गभीरता) {
				प्रारूप_लिख(
					"verify grant: indirect grant verify "
					"failed: exceeded maximum depth\n");
				प्रत्यागम ELOOP;
			}
			गभीरता++;

			/* Verify actual grantee. */
			यदि(g.cp_u.cp_परोक्षम्.cp_who_गन्तव्यम् != grantee &&
				grantee != ANY &&
				g.cp_u.cp_परोक्षम्.cp_who_गन्तव्यम् != ANY) {
				प्रारूप_लिख(
					"verify_grant: indirect grant verify "
					"failed: bad grantee\n");
				प्रत्यागम EPERM;
			}

			/* Start over with new granter, grant, and grantee. */
			grantee = granter;
			granter = g.cp_u.cp_परोक्षम्.cp_who_from;
			grant = g.cp_u.cp_परोक्षम्.cp_grant;
		}
	} यावत्(g.cp_flags & CPF_परोक्षम्);

	/* Check access of grant. */
	यदि(((g.cp_flags & access) != access)) {
		प्रारूप_लिख(
	"verify_grant: grant verify failed: access invalid; want 0x%x, have 0x%x\n",
			access, g.cp_flags);
		प्रत्यागम EPERM;
	}

	यदि((g.cp_flags & CPF_प्रत्यक्षम्)) {
		/* Don't fiddle around with grants that wrap, arithmetic
		 * below may be confused.
		 */
		यदि(MEM_शिखरम् - g.cp_u.cp_प्रत्यक्षम्.cp_len + 1 <
			g.cp_u.cp_प्रत्यक्षम्.cp_start) {
			प्रारूप_लिख(
		"verify_grant: direct grant verify failed: len too long\n");
			प्रत्यागम EPERM;
		}

		/* Verify actual grantee. */
		यदि(g.cp_u.cp_प्रत्यक्षम्.cp_who_गन्तव्यम् != grantee && grantee != ANY
			&& g.cp_u.cp_प्रत्यक्षम्.cp_who_गन्तव्यम् != ANY) {
			प्रारूप_लिख(
		"verify_grant: direct grant verify failed: bad grantee\n");
			प्रत्यागम EPERM;
		}

		/* Verify actual copy range. */
		यदि((विस्थापनम्_अन्तर्+अष्टकानि < विस्थापनम्_अन्तर्) ||
		    विस्थापनम्_अन्तर्+अष्टकानि > g.cp_u.cp_प्रत्यक्षम्.cp_len) {
			प्रारूप_लिख(
		"verify_grant: direct grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_प्रत्यक्षम्.cp_len,
				g.cp_u.cp_प्रत्यक्षम्.cp_start,
				अष्टकानि, विस्थापनम्_अन्तर्);
			प्रत्यागम EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*विस्थापनम्_परिणाम = g.cp_u.cp_प्रत्यक्षम्.cp_start + विस्थापनम्_अन्तर्;
		*e_granter = granter;
	} अन्यथा यदि(g.cp_flags & CPF_MAGIC) {
		/* Currently, it is hardcoded that only VFS and MIB may do
		 * magic grants.  TODO: this should be a system.conf flag.
		 */
		यदि(granter != VFS_PROC_NR && granter != MIB_PROC_NR) {
			प्रारूप_लिख(
		"verify_grant: magic grant verify failed: granter (%d) "
		"not allowed\n", granter);
			प्रत्यागम EPERM;
		}

		/* Verify actual grantee. */
		यदि(g.cp_u.cp_magic.cp_who_गन्तव्यम् != grantee && grantee != ANY
			&& g.cp_u.cp_प्रत्यक्षम्.cp_who_गन्तव्यम् != ANY) {
			प्रारूप_लिख(
		"verify_grant: magic grant verify failed: bad grantee\n");
			प्रत्यागम EPERM;
		}

		/* Verify actual copy range. */
		यदि((विस्थापनम्_अन्तर्+अष्टकानि < विस्थापनम्_अन्तर्) ||
		    विस्थापनम्_अन्तर्+अष्टकानि > g.cp_u.cp_magic.cp_len) {
			प्रारूप_लिख(
		"verify_grant: magic grant verify failed: bad size or range. "
		"granted %d bytes @ 0x%lx; wanted %d bytes @ 0x%lx\n",
				g.cp_u.cp_magic.cp_len,
				g.cp_u.cp_magic.cp_start,
				अष्टकानि, विस्थापनम्_अन्तर्);
			प्रत्यागम EPERM;
		}

		/* Verify successful - tell caller what address it is. */
		*विस्थापनम्_परिणाम = g.cp_u.cp_magic.cp_start + विस्थापनम्_अन्तर्;
		*e_granter = g.cp_u.cp_magic.cp_who_from;
	} अन्यथा {
		प्रारूप_लिख(
		"verify_grant: grant verify failed: unknown grant type\n");
		प्रत्यागम EPERM;
	}

	/* If requested, store information regarding soft faults. */
	यदि (sfinfo != रिक्त && (sfinfo->try = !!(g.cp_flags & CPF_TRY))) {
		sfinfo->endpt = granter;
		sfinfo->addr = priv(granter_proc)->s_grant_सारणी +
		    परिमाण(g) * grant_idx + offsetof(cp_grant_t, cp_faulted);
		sfinfo->मानम् = grant;
	}

	प्रत्यागम OK;
}

/*===========================================================================*
 *				safecopy				     *
 *===========================================================================*/
स्थिर पूर्णाङ्क safecopy(
  संरचना proc * caller,
  endpoint_t granter,
  endpoint_t grantee,
  cp_grant_id_t grantid,
  size_t अष्टकानि,
  vir_अष्टकानि g_विस्थापनम्,
  vir_अष्टकानि addr,
  पूर्णाङ्क access			/* CPF_READ for a copy from granter to grantee, CPF_WRITE
				 * for a copy from grantee to granter.
				 */
)
{
	स्थिर संरचना vir_addr v_src, v_dst;
	स्थिर vir_अष्टकानि v_विस्थापनम्;
	endpoint_t new_granter, *src, *dst;
	पूर्णाङ्क r;
	संरचना cp_sfinfo sfinfo;

	यदि(granter == NONE || grantee == NONE) {
		प्रारूप_लिख("safecopy: nonsense processes\n");
		प्रत्यागम EFAULT;
	}

	/* Decide who is src and who is dst. */
	यदि(access & CPF_READ) {
		src = &granter;
		dst = &grantee;
	} अन्यथा {
		src = &grantee;
		dst = &granter;
	}

	/* Verify permission exists. */
	यदि((r=verify_grant(granter, grantee, grantid, अष्टकानि, access,
	    g_विस्थापनम्, &v_विस्थापनम्, &new_granter, &sfinfo)) != OK) {
		यदि(r == ENOTREADY) प्रत्यागम r;
			प्रारूप_लिख(
		"grant %d verify to copy %d->%d by %d failed: err %d\n",
				grantid, *src, *dst, grantee, r);
		प्रत्यागम r;
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
	यदि(access & CPF_READ) {
		v_src.विस्थापनम् = v_विस्थापनम्;
		v_dst.विस्थापनम् = (vir_अष्टकानि) addr;
	} अन्यथा {
		v_src.विस्थापनम् = (vir_अष्टकानि) addr;
		v_dst.विस्थापनम् = v_विस्थापनम्;
	}

	/* Do the regular copy. */
	यदि (sfinfo.try) {
		/*
		 * Try copying without transparently faulting in pages.
		 * TODO: while CPF_TRY is meant to protect against deadlocks on
		 * memory-mapped files in file systems, it seems that this case
		 * triggers faults a whole lot more often, resulting in extra
		 * overhead due to retried file system operations.  It might be
		 * a good idea to go through VM even in this case, and have VM
		 * fail (only) if the affected page belongs to a file mapping.
		 */
		r = virtual_copy(&v_src, &v_dst, अष्टकानि);
		यदि (r == EFAULT_SRC || r == EFAULT_DST) {
			/*
			 * Mark the magic grant as having experienced a soft
			 * fault during its lifetime.  The exact value does not
			 * matter, but we use the grant ID (including its
			 * sequence number) as a form of protection in the
			 * light of CPU concurrency.
			 */
			r = दत्त_copy(KERNEL, (vir_अष्टकानि)&sfinfo.मानम्,
			    sfinfo.endpt, sfinfo.addr, परिमाण(sfinfo.मानम्));
			/*
			 * Failure means the creator of the magic grant messed
			 * up, which can only be unintentional, so report..
			 */
			यदि (r != OK)
				प्रारूप_लिख("Kernel: writing soft fault marker %d "
				    "into %d at 0x%lx failed (%d)\n",
				    sfinfo.मानम्, sfinfo.endpt, sfinfo.addr,
				    r);

			प्रत्यागम EFAULT;
		}
		प्रत्यागम r;
	}
	प्रत्यागम virtual_copy_vmcheck(caller, &v_src, &v_dst, अष्टकानि);
}

/*===========================================================================*
 *				do_safecopy_to				     *
 *===========================================================================*/
पूर्णाङ्क do_safecopy_गन्तव्यम्(संरचना proc * caller, सन्देशः * m_ptr)
{
	प्रत्यागम safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_गन्तव्यम्, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.अष्टकानि, m_ptr->m_lsys_kern_safecopy.विस्थापनम्,
		(vir_अष्टकानि) m_ptr->m_lsys_kern_safecopy.स्थानाङ्कः, CPF_WRITE);
}

/*===========================================================================*
 *				do_safecopy_from			     *
 *===========================================================================*/
पूर्णाङ्क do_safecopy_from(संरचना proc * caller, सन्देशः * m_ptr)
{
	प्रत्यागम safecopy(caller, m_ptr->m_lsys_kern_safecopy.from_गन्तव्यम्, caller->p_endpoint,
		(cp_grant_id_t) m_ptr->m_lsys_kern_safecopy.gid,
		m_ptr->m_lsys_kern_safecopy.अष्टकानि, m_ptr->m_lsys_kern_safecopy.विस्थापनम्,
		(vir_अष्टकानि) m_ptr->m_lsys_kern_safecopy.स्थानाङ्कः, CPF_READ);
}

/*===========================================================================*
 *				do_vsafecopy				     *
 *===========================================================================*/
पूर्णाङ्क do_vsafecopy(संरचना proc * caller, सन्देशः * m_ptr)
{
	स्थिर संरचना vscp_vec vec[SCPVEC_NR];
	स्थिर संरचना vir_addr src, dst;
	पूर्णाङ्क r, i, els;
	size_t अष्टकानि;

	/* Set vector copy parameters. */
	src.proc_nr_e = caller->p_endpoint;
	प्रतिज्ञा(src.proc_nr_e != NONE);
	src.विस्थापनम् = (vir_अष्टकानि) m_ptr->m_lsys_kern_vsafecopy.vec_addr;
	dst.proc_nr_e = KERNEL;
	dst.विस्थापनम् = (vir_अष्टकानि) vec;

	/* No. of vector elements. */
	els = m_ptr->m_lsys_kern_vsafecopy.vec_आकार;
	अष्टकानि = els * परिमाण(संरचना vscp_vec);

	/* Obtain vector of copies. */
	यदि((r=virtual_copy_vmcheck(caller, &src, &dst, अष्टकानि)) != OK)
		प्रत्यागम r;

	/* Perform safecopies. */
	प्रति(i = 0; i < els; i++) {
		पूर्णाङ्क access;
		endpoint_t granter;
		यदि(vec[i].v_from == SELF) {
			access = CPF_WRITE;
			granter = vec[i].v_गन्तव्यम्;
		} अन्यथा यदि(vec[i].v_गन्तव्यम् == SELF) {
			access = CPF_READ;
			granter = vec[i].v_from;
		} अन्यथा {
			प्रारूप_लिख("vsafecopy: %d: element %d/%d: no SELF found\n",
				caller->p_endpoint, i, els);
			प्रत्यागम EINVAL;
		}

		/* Do safecopy for this element. */
		यदि((r=safecopy(caller, granter, caller->p_endpoint,
			vec[i].v_gid,
			vec[i].v_अष्टकानि, vec[i].v_विस्थापनम्,
			vec[i].v_addr, access)) != OK) {
			प्रत्यागम r;
		}
	}

	प्रत्यागम OK;
}

