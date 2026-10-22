/* The kernel call implemented in this file:
 *   m_type:	SYS_SAFEMEMSET
 *
 * The parameters for this kernel call are:
 *	SMS_DST		dst endpoint
 *	SMS_GID		grant id
 *	SMS_OFFSET	offset within grant
 *	SMS_PATTERN     memset pattern byte
 *	SMS_BYTES	bytes from offset
 */
#അന്തര്ഭാവ <assert.h>

#അന്തര്ഭാവ <minix/safecopies.h>

#അന്തര്ഭാവ "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_safememset(ഘടന proc *caller, message *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_bytes g_offset = m_ptr->SMS_OFFSET;
	പൂർണ്ണസംഖ്യ pattern = m_ptr->SMS_PATTERN;
	size_t len = (size_t)m_ptr->SMS_BYTES;

	ഘടന proc *dst_p;
	endpoint_t new_granter;
	സ്ഥിതം vir_bytes v_offset;
	പൂർണ്ണസംഖ്യ r;

	എങ്കിൽ (dst_endpt == NONE || caller_endpt == NONE)
		തിരികെ EFAULT;

	എങ്കിൽ (!(dst_p = endpoint_lookup(dst_endpt)))
		തിരികെ EINVAL;

	എങ്കിൽ (!(priv(dst_p) && priv(dst_p)->s_grant_table)) {
		printf("safememset: dst %d has no grant table\n", dst_endpt);
		തിരികെ EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_offset, &v_offset, &new_granter, ശൂന്യ);

	എങ്കിൽ (r != OK) {
		printf("safememset: grant %d verify failed %d", grantid, r);
		തിരികെ r;
	}

	തിരികെ vm_memset(caller, new_granter, v_offset, pattern, len);
}
