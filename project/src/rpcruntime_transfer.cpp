#include "rpcruntime_transfer.h"
#include "rpcruntime_decoded_function_call.h"
#include "rpcruntime_protocol_description.h"

#include <algorithm>
#include <cassert>
#include <sstream>

RPCRuntimeTransfer::RPCRuntimeTransfer(const RPCRunTimeProtocolDescription &protocol)
	: protocol(&protocol) {}

int RPCRuntimeTransfer::get_min_number_of_bytes() const {
	if (data.empty()) {
		return 1; //need at least 1 byte for the ID
	}
	//TODO: check in protocol how long function with ID data[0] is
	return protocol->get_parameter_size_bytes(data[0]);
}

bool RPCRuntimeTransfer::is_complete() const {
	return get_min_number_of_bytes() == static_cast<int>(data.size());
}

void RPCRuntimeTransfer::add_data(unsigned char byte) {
	data.push_back(byte);
}

RPCRuntimeDecodedFunctionCall RPCRuntimeTransfer::decode() const {
	std::vector<RPCRuntimeDecodedParam> decoded_parameters;
	std::stringstream ss;
	ss.str({reinterpret_cast<const char *>(data.data()), data.size()});
	unsigned char id;
	ss >> id;
	if (!ss) {
		throw std::runtime_error("No available data to decode");
	}
	if (id == 0) { //special case for hash
		static RPCRuntimeFunction rtfunction(0, 1, {}, {}, "get_hash",
											 "void get_hash(unsigned char hash_out[16], unsigned char start_command_id_out[1], uint16_t version_out[1]);");
		return {id, {}, rtfunction};
	}

	auto &function = protocol->get_function(data[0]);
	auto &parameter_descriptions = protocol->get_parameters(data[0]);
	for (auto &pd : parameter_descriptions) {
		decoded_parameters.emplace_back(pd);
		ss >> decoded_parameters.back();
	}

	assert(ss);
	return {id, std::move(decoded_parameters), function};
}

void RPCRuntimeTransfer::reset() {
	data.clear();
}
