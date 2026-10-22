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
#ضمن <assert.h>

#ضمن <minix/safecopies.h>

#ضمن "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
صحيح do_safememset(بنية proc *caller, پیغام *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_بایټونه g_دننه_واټن = m_ptr->SMS_دننه_واټن;
	صحيح نمونه = m_ptr->SMS_نمونه;
	size_t len = (size_t)m_ptr->SMS_بایټونه;

	بنية proc *dst_p;
	endpoint_t new_granter;
	ساكن vir_بایټونه v_دننه_واټن;
	صحيح r;

	إذا (dst_endpt == NONE || caller_endpt == NONE)
		أرجع EFAULT;

	إذا (!(dst_p = endpoint_lookup(dst_endpt)))
		أرجع EINVAL;

	إذا (!(priv(dst_p) && priv(dst_p)->s_grant_جدول)) {
		چاپ_کړه("safememset: dst %d has no grant table\n", dst_endpt);
		أرجع EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_دننه_واټن, &v_دننه_واټن, &new_granter, عدم);

	إذا (r != OK) {
		چاپ_کړه("safememset: grant %d verify failed %d", grantid, r);
		أرجع r;
	}

	أرجع vm_memset(caller, new_granter, v_دننه_واټن, نمونه, len);
}
