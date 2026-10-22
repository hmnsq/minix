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
#ಅನ್ತರ್ಭಾವ <assert.h>

#ಅನ್ತರ್ಭಾವ <minix/safecopies.h>

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_safememset(ಸಂರಚನಾ proc *caller, message *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_bytes g_offset = m_ptr->SMS_OFFSET;
	ಪೂರ್ಣಾಙ್ಕ pattern = m_ptr->SMS_PATTERN;
	size_t len = (size_t)m_ptr->SMS_BYTES;

	ಸಂರಚನಾ proc *dst_p;
	endpoint_t new_granter;
	ಸ್ಥಿರ vir_bytes v_offset;
	ಪೂರ್ಣಾಙ್ಕ r;

	ಯದಿ (dst_endpt == NONE || caller_endpt == NONE)
		ಪ್ರತ್ಯಾಗಮ EFAULT;

	ಯದಿ (!(dst_p = endpoint_lookup(dst_endpt)))
		ಪ್ರತ್ಯಾಗಮ EINVAL;

	ಯದಿ (!(priv(dst_p) && priv(dst_p)->s_grant_table)) {
		printf("safememset: dst %d has no grant table\n", dst_endpt);
		ಪ್ರತ್ಯಾಗಮ EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_offset, &v_offset, &new_granter, ರಿಕ್ತ);

	ಯದಿ (r != OK) {
		printf("safememset: grant %d verify failed %d", grantid, r);
		ಪ್ರತ್ಯಾಗಮ r;
	}

	ಪ್ರತ್ಯಾಗಮ vm_memset(caller, new_granter, v_offset, pattern, len);
}
