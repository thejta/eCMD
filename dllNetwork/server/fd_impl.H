#ifndef _FD_IMPL_H
#define _FD_IMPL_H
//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG
#include <unistd.h>

ssize_t fd_write(int i_fd, const void *i_ptr, size_t i_len);
ssize_t fd_read(int i_fd, void *o_ptr, size_t i_len);
#endif // _FD_IMPL_H
