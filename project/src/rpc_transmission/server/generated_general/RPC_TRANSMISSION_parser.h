/* This file has been automatically generated by RPC-Generator
   https://github.com/Crystal-Photonics/RPC-Generator
   You should not modify this file manually. */

#include "RPC_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Receives a pointer to a (partly) received message and it's size.
   Returns a result and a size. If size equals RPC_TRANSMISSION_SUCCESS then size is the
   size that the message is supposed to have. If result equals RPC_TRANSMISSION_COMMAND_INCOMPLETE
   then more bytes are required to determine the size of the message. In this case
   size is the expected number of bytes required to determine the correct size.*/


RPC_SIZE_RESULT RPC_TRANSMISSION_get_request_size(const void *buffer, size_t size_bytes);

/* This function parses RPC requests, calls the original function and sends an
   answer. */
void RPC_TRANSMISSION_parse_request(const void *buffer, size_t size_bytes);


#ifdef __cplusplus
}
#endif /* __cplusplus */
