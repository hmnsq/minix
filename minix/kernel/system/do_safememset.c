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
#అన్తర్భావ <assert.h>

#అన్తర్భావ <minix/safecopies.h>

#అన్తర్భావ "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
పూర్ణాంకం do_safememset(నిర్మాణం proc *caller, సందేశం *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_బైట్లు g_స్థానభ్రంశం = m_ptr->SMS_స్థానభ్రంశం;
	పూర్ణాంకం నమూనా = m_ptr->SMS_నమూనా;
	size_t len = (size_t)m_ptr->SMS_బైట్లు;

	నిర్మాణం proc *dst_p;
	endpoint_t new_granter;
	స్థిర vir_బైట్లు v_స్థానభ్రంశం;
	పూర్ణాంకం r;

	ఒకవేళ (dst_endpt == NONE || caller_endpt == NONE)
		తిరిగి EFAULT;

	ఒకవేళ (!(dst_p = endpoint_lookup(dst_endpt)))
		తిరిగి EINVAL;

	ఒకవేళ (!(priv(dst_p) && priv(dst_p)->s_grant_పట్టిక)) {
		రాయి("safememset: dst %d has no grant table\n", dst_endpt);
		తిరిగి EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_స్థానభ్రంశం, &v_స్థానభ్రంశం, &new_granter, ఖాళీ);

	ఒకవేళ (r != OK) {
		రాయి("safememset: grant %d verify failed %d", grantid, r);
		తిరిగి r;
	}

	తిరిగి vm_memset(caller, new_granter, v_స్థానభ్రంశం, నమూనా, len);
}
