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
#شامل <assert.h>

#شامل <minix/safecopies.h>

#شامل "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
صحیح do_safememset(ساختار proc *caller, message *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_bytes g_offset = m_ptr->SMS_OFFSET;
	صحیح pattern = m_ptr->SMS_PATTERN;
	size_t len = (size_t)m_ptr->SMS_BYTES;

	ساختار proc *dst_p;
	endpoint_t new_granter;
	ایستا vir_bytes v_offset;
	صحیح r;

	اگر (dst_endpt == NONE || caller_endpt == NONE)
		بازگشت EFAULT;

	اگر (!(dst_p = endpoint_lookup(dst_endpt)))
		بازگشت EINVAL;

	اگر (!(priv(dst_p) && priv(dst_p)->s_grant_table)) {
		printf("safememset: dst %d has no grant table\n", dst_endpt);
		بازگشت EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_offset, &v_offset, &new_granter, پوچ);

	اگر (r != OK) {
		printf("safememset: grant %d verify failed %d", grantid, r);
		بازگشت r;
	}

	بازگشت vm_memset(caller, new_granter, v_offset, pattern, len);
}
