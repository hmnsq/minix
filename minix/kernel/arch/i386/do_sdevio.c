/* The kernel call implemented in this file:
 *   m_type:	SYS_SDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_sdevio.request	(request input or output)
 *    m_lsys_krn_sys_sdevio.port	(port to read/ write)
 *    m_lsys_krn_sys_sdevio.vec_addr	(virtual address of buffer, or grant ID)
 *    m_lsys_krn_sys_sdevio.vec_size	(number of elements)
 *    m_lsys_krn_sys_sdevio.vec_endpt	(process where buffer is)
 *    m_lsys_krn_sys_sdevio.offset	(offset into the grant)
 */

#समावेश "kernel/system.h"
#समावेश <minix/devio.h>
#समावेश <minix/endpoint.h>

#समावेश "arch_proto.h"

#अगर USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
पूर्णांक do_sdevio(काष्ठा proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  पूर्णांक proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes गिनती = m_ptr->m_lsys_krn_sys_sdevio.vec_आकार;
  दीर्घ port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  पूर्णांक i, r, req_type, req_dir, आकार, nr_io_range;
  काष्ठा priv *privp;
  काष्ठा io_range *iorp;
  काष्ठा proc *destproc;
  पूर्णांक retval;

  /* Allow safe copies and accesses to SELF */
  अगर ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	अटल पूर्णांक first= 1;
	अगर (first)
	{
		first= 0;
		म_लिखो("do_sdevio: for %d, req %d\n",
			m_ptr->m_source, m_ptr->m_lsys_krn_sys_sdevio.request);
	}
  }

  /* Check if process endpoint is OK. 
   * A driver may directly provide a pointer to a buffer at the user-process
   * that initiated the device I/O. Kernel processes, of course, are denied.
   */
  अगर (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  अन्यथा
	अगर(!isokendpt(proc_nr_e, &proc_nr))
		वापस(EINVAL);
  अगर (iskerneln(proc_nr)) वापस(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  अगर((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     अगर((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, गिनती,
		req_dir == DIO_निवेश ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		शून्य)) != OK) {
	अगर(r == ENOTREADY) वापस r;
	म_लिखो("do_sdevio: verify_grant failed\n");
	वापस EPERM;
    }
	अगर(!isokendpt(newep, &proc_nr))
		वापस(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } अन्यथा {
     अगर(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	म_लिखो("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	वापस EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  चयन (req_type)
  {
	हाल _DIO_BYTE: आकार= 1; अवरोध;
	हाल _DIO_WORD: आकार= 2; अवरोध;
	हाल _DIO_LONG: आकार= 4; अवरोध;
	शेष: आकार= 4; अवरोध;	/* Be conservative */
  }

  privp= priv(caller);
  अगर (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	क्रम (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		अगर (port >= iorp->ior_base && port+आकार-1 <= iorp->ior_limit)
			अवरोध;
	}
	अगर (i >= nr_io_range)
	{
		म_लिखो(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		जाओ return_error;
	}
  }

  अगर (port & (आकार-1))
  {
	म_लिखो("do_devio: unaligned port 0x%x (size %d)\n", port, आकार);
	retval = EPERM;
	जाओ return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  अगर (req_dir == DIO_निवेश) { 
      चयन (req_type) {
      हाल _DIO_BYTE: phys_insb(port, vir_buf, गिनती); अवरोध; 
      हाल _DIO_WORD: phys_insw(port, vir_buf, गिनती); अवरोध; 
      शेष:
  		retval = EINVAL;
		जाओ return_error;
      } 
  } अन्यथा अगर (req_dir == _DIO_OUTPUT) { 
      चयन (req_type) {
      हाल _DIO_BYTE: phys_outsb(port, vir_buf, गिनती); अवरोध; 
      हाल _DIO_WORD: phys_outsw(port, vir_buf, गिनती); अवरोध; 
      शेष:
  		retval = EINVAL;
		जाओ return_error;
      } 
  }
  अन्यथा {
	  retval = EINVAL;
	  जाओ return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  वापस retval;
}

#पूर्ण_अगर /* USE_SDEVIO */
