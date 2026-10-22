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
#அந்தர்பாவ <assert.h>

#அந்தர்பாவ <minix/safecopies.h>

#அந்தர்பாவ "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
முழு do_safememset(கட்டமைப்பு proc *caller, message *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_bytes g_offset = m_ptr->SMS_OFFSET;
	முழு pattern = m_ptr->SMS_PATTERN;
	size_t len = (size_t)m_ptr->SMS_BYTES;

	கட்டமைப்பு proc *dst_p;
	endpoint_t new_granter;
	நிலையான vir_bytes v_offset;
	முழு r;

	எனில் (dst_endpt == NONE || caller_endpt == NONE)
		திரும்பு EFAULT;

	எனில் (!(dst_p = endpoint_lookup(dst_endpt)))
		திரும்பு EINVAL;

	எனில் (!(priv(dst_p) && priv(dst_p)->s_grant_table)) {
		printf("safememset: dst %d has no grant table\n", dst_endpt);
		திரும்பு EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_offset, &v_offset, &new_granter, வெற்று);

	எனில் (r != OK) {
		printf("safememset: grant %d verify failed %d", grantid, r);
		திரும்பு r;
	}

	திரும்பு vm_memset(caller, new_granter, v_offset, pattern, len);
}
