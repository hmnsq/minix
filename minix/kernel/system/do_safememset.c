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
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/safecopies.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
ਪੂਰਨ do_safememset(ਬਣਤਰ proc *caller, ਸੁਨੇਹਾ *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_ਬਾਈਟਾਂ g_ਵਿੱਥ = m_ptr->SMS_ਵਿੱਥ;
	ਪੂਰਨ ਨਮੂਨਾ = m_ptr->SMS_ਨਮੂਨਾ;
	size_t len = (size_t)m_ptr->SMS_ਬਾਈਟਾਂ;

	ਬਣਤਰ proc *dst_p;
	endpoint_t new_granter;
	ਸਥਿਰ vir_ਬਾਈਟਾਂ v_ਵਿੱਥ;
	ਪੂਰਨ r;

	ਜੇ (dst_endpt == NONE || caller_endpt == NONE)
		ਵਾਪਸ EFAULT;

	ਜੇ (!(dst_p = endpoint_lookup(dst_endpt)))
		ਵਾਪਸ EINVAL;

	ਜੇ (!(priv(dst_p) && priv(dst_p)->s_grant_ਸਾਰਣੀ)) {
		ਲਿਖੋ("safememset: dst %d has no grant table\n", dst_endpt);
		ਵਾਪਸ EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_ਵਿੱਥ, &v_ਵਿੱਥ, &new_granter, ਨਿਰਾ);

	ਜੇ (r != OK) {
		ਲਿਖੋ("safememset: grant %d verify failed %d", grantid, r);
		ਵਾਪਸ r;
	}

	ਵਾਪਸ vm_memset(caller, new_granter, v_ਵਿੱਥ, ਨਮੂਨਾ, len);
}
