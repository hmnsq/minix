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
#समावेश <assert.h>

#समावेश <minix/safecopies.h>

#समावेश "kernel/system.h"

/*===========================================================================*
 *                              do_safememset                                *
 *===========================================================================*/
पूर्णांक do_safememset(काष्ठा proc *caller, message *m_ptr) {
	/* Implementation of the do_safememset() kernel call */

	/* Extract parameters */
	endpoint_t dst_endpt = m_ptr->SMS_DST;
	endpoint_t caller_endpt = caller->p_endpoint;
	cp_grant_id_t grantid = m_ptr->SMS_GID;
	vir_bytes g_offset = m_ptr->SMS_OFFSET;
	पूर्णांक pattern = m_ptr->SMS_PATTERN;
	माप_प्रकार len = (माप_प्रकार)m_ptr->SMS_BYTES;

	काष्ठा proc *dst_p;
	endpoint_t new_granter;
	अटल vir_bytes v_offset;
	पूर्णांक r;

	अगर (dst_endpt == NONE || caller_endpt == NONE)
		वापस EFAULT;

	अगर (!(dst_p = endpoint_lookup(dst_endpt)))
		वापस EINVAL;

	अगर (!(priv(dst_p) && priv(dst_p)->s_grant_table)) {
		म_लिखो("safememset: dst %d has no grant table\n", dst_endpt);
		वापस EINVAL;
	}

	/* Verify permission exists, memset always requires CPF_WRITE */
	r = verify_grant(dst_endpt, caller_endpt, grantid, len, CPF_WRITE,
			 g_offset, &v_offset, &new_granter, शून्य);

	अगर (r != OK) {
		म_लिखो("safememset: grant %d verify failed %d", grantid, r);
		वापस r;
	}

	वापस vm_memset(caller, new_granter, v_offset, pattern, len);
}
