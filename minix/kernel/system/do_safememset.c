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
#ଅନ୍ତର୍ଭାଵ <assert.h>

#ଅନ୍ତର୍ଭାଵ <minix/safecopies.h>

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_safememset(ସଂରଚନା proc *caller, message *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_bytes g_offset = m_ptr->SMS_OFFSET;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pattern = m_ptr->SMS_PATTERN;
	size_t len = (size_t)m_ptr->SMS_BYTES;

	ସଂରଚନା proc *dst_p;
	endpoint_t new_granter;
	ସ୍ଥିର vir_bytes v_offset;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

	ଯଦି (dst_endpt == NONE || caller_endpt == NONE)
		ଫେରନ୍ତୁ EFAULT;

	ଯଦି (!(dst_p = endpoint_lookup(dst_endpt)))
		ଫେରନ୍ତୁ EINVAL;

	ଯଦି (!(priv(dst_p) && priv(dst_p)->s_grant_table)) {
		printf("safememset: dst %d has no grant table\n", dst_endpt);
		ଫେରନ୍ତୁ EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_offset, &v_offset, &new_granter, ଖାଲି);

	ଯଦି (r != OK) {
		printf("safememset: grant %d verify failed %d", grantid, r);
		ଫେରନ୍ତୁ r;
	}

	ଫେରନ୍ତୁ vm_memset(caller, new_granter, v_offset, pattern, len);
}
