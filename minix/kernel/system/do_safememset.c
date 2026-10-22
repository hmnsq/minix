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
#অন্তর্ভাৱ <assert.h>

#অন্তর্ভাৱ <minix/safecopies.h>

#অন্তর্ভাৱ "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
পূর্ণাঙ্ক do_safememset(সংরচনা proc *caller, message *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_bytes g_offset = m_ptr->SMS_OFFSET;
	পূর্ণাঙ্ক pattern = m_ptr->SMS_PATTERN;
	size_t len = (size_t)m_ptr->SMS_BYTES;

	সংরচনা proc *dst_p;
	endpoint_t new_granter;
	স্থির vir_bytes v_offset;
	পূর্ণাঙ্ক r;

	যদি (dst_endpt == NONE || caller_endpt == NONE)
		ঘূৰাই EFAULT;

	যদি (!(dst_p = endpoint_lookup(dst_endpt)))
		ঘূৰাই EINVAL;

	যদি (!(priv(dst_p) && priv(dst_p)->s_grant_table)) {
		printf("safememset: dst %d has no grant table\n", dst_endpt);
		ঘূৰাই EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_offset, &v_offset, &new_granter, রিক্ত);

	যদি (r != OK) {
		printf("safememset: grant %d verify failed %d", grantid, r);
		ঘূৰাই r;
	}

	ঘূৰাই vm_memset(caller, new_granter, v_offset, pattern, len);
}
