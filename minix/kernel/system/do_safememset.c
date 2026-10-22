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
ಪೂರ್ಣಾಂಕ do_safememset(ರಚನೆ proc *caller, ಸಂದೇಶ *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_ಬೈಟುಗಳು g_ಸ್ಥಾನಾಂತರ = m_ptr->SMS_ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ ನಮೂನೆ = m_ptr->SMS_ನಮೂನೆ;
	size_t len = (size_t)m_ptr->SMS_ಬೈಟುಗಳು;

	ರಚನೆ proc *dst_p;
	endpoint_t new_granter;
	ಸ್ಥಿರ vir_ಬೈಟುಗಳು v_ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ r;

	ಆದರೆ (dst_endpt == NONE || caller_endpt == NONE)
		ಹಿಂತಿರುಗಿ EFAULT;

	ಆದರೆ (!(dst_p = endpoint_lookup(dst_endpt)))
		ಹಿಂತಿರುಗಿ EINVAL;

	ಆದರೆ (!(priv(dst_p) && priv(dst_p)->s_grant_ಕೋಷ್ಟಕ)) {
		ಬರೆ("safememset: dst %d has no grant table\n", dst_endpt);
		ಹಿಂತಿರುಗಿ EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_ಸ್ಥಾನಾಂತರ, &v_ಸ್ಥಾನಾಂತರ, &new_granter, ಖಾಲಿ);

	ಆದರೆ (r != OK) {
		ಬರೆ("safememset: grant %d verify failed %d", grantid, r);
		ಹಿಂತಿರುಗಿ r;
	}

	ಹಿಂತಿರುಗಿ vm_memset(caller, new_granter, v_ಸ್ಥಾನಾಂತರ, ನಮೂನೆ, len);
}
