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
#अन्तर्भाव <assert.h>

#अन्तर्भाव <minix/safecopies.h>

#अन्तर्भाव "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
पूर्णाङ्क do_safememset(संरचना proc *caller, message *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_bytes g_offset = m_ptr->SMS_OFFSET;
	पूर्णाङ्क pattern = m_ptr->SMS_PATTERN;
	size_t len = (size_t)m_ptr->SMS_BYTES;

	संरचना proc *dst_p;
	endpoint_t new_granter;
	स्थिर vir_bytes v_offset;
	पूर्णाङ्क r;

	यदि (dst_endpt == NONE || caller_endpt == NONE)
		प्रत्यागम EFAULT;

	यदि (!(dst_p = endpoint_lookup(dst_endpt)))
		प्रत्यागम EINVAL;

	यदि (!(priv(dst_p) && priv(dst_p)->s_grant_table)) {
		printf("safememset: dst %d has no grant table\n", dst_endpt);
		प्रत्यागम EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_offset, &v_offset, &new_granter, रिक्त);

	यदि (r != OK) {
		printf("safememset: grant %d verify failed %d", grantid, r);
		प्रत्यागम r;
	}

	प्रत्यागम vm_memset(caller, new_granter, v_offset, pattern, len);
}
