#include "rpcruntime_encoder.h"
#include "rpcruntime_encoded_function_call.h"
#include "rpcruntime_protocol_description.h"

RPCRuntimeEncoder::RPCRuntimeEncoder(const RPCRunTimeProtocolDescription &description)
	: description(&description) {}

RPCRuntimeEncodedFunctionCall RPCRuntimeEncoder::encode(int id) {
	if (id == 0) { //special case for get hash function
		return RPCRuntimeEncodedFunctionCall::create_hash_request();
	}
	for (auto &function : description->get_functions()) {
		if (function.get_request_id() == id) {
			return {function};
		}
	}
	throw std::runtime_error("unknown function name");
}

RPCRuntimeEncodedFunctionCall RPCRuntimeEncoder::encode(const std::string &function_name) {
	for (auto &function : description->get_functions()) {
		if (function.get_function_name() == function_name) {
			return {function};
		}
	}
	throw std::runtime_error("unknown function name");
}
