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
صحیح do_safememset(ساخت proc *caller, پیغام *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_بائٹیں g_انحراف = m_ptr->SMS_انحراف;
	صحیح نمونہ = m_ptr->SMS_نمونہ;
	size_t len = (size_t)m_ptr->SMS_بائٹیں;

	ساخت proc *dst_p;
	endpoint_t new_granter;
	ساکن vir_بائٹیں v_انحراف;
	صحیح r;

	اگر (dst_endpt == NONE || caller_endpt == NONE)
		واپس EFAULT;

	اگر (!(dst_p = endpoint_lookup(dst_endpt)))
		واپس EINVAL;

	اگر (!(priv(dst_p) && priv(dst_p)->s_grant_جدول)) {
		لکھو("safememset: dst %d has no grant table\n", dst_endpt);
		واپس EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_انحراف, &v_انحراف, &new_granter, تہی);

	اگر (r != OK) {
		لکھو("safememset: grant %d verify failed %d", grantid, r);
		واپس r;
	}

	واپس vm_memset(caller, new_granter, v_انحراف, نمونہ, len);
}
