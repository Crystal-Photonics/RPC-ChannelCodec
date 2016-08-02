/*
 * channel_codec_test.cpp
 *
 *  Created on: 30.03.2015
 *      Author: ak
 */



#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "errorlogger/generic_eeprom_errorlogger.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

#include "channel_codec/channel_codec.h"
#include "channel_codec/channel_codec_test.h"


uint16_t TXDataLength;

#define DISABLE_TESTS 1

#if CHANNEL_BLOCKLENGTH == 8
#elif CHANNEL_BLOCKLENGTH == 16
#else
#error CHANNEL_BLOCKLENGTH has to be defined with either 8 or 16
#endif

extern "C"
{
	#include "CppUTestExt/MockSupport_c.h"
	#include "channel_codec/phylayer.h"

	void ChannelCodec_errorHandler(channelCodecErrorNum_t errNum){
		(void)errNum;
	}

	RPC_TRANSMISSION_RESULT phyPushDataBuffer(channel_codec_instance_index_t instance_index, const char *buffer, size_t length){
		/*printf("eunistonePushDataBuffer\n");*/
#if 0
		printf("\n");
		for (uint16_t i=0;i<length;i++){
			printf("%02X ",(unsigned char)buffer[i]);
		}
		printf("\n");
#endif

		mock_c()->actualCall("eunistonePushDataBuffer")
						->withIntParameters("length", length);
		TXDataLength = length;
		return RPC_TRANSMISSION_SUCCESS;
	}

	void RPC_TRANSMISSION_parse_answer(const void *buffer, size_t size){
		(void)buffer;
		uint8_t *buf = (uint8_t*)buffer;
#if 0
		printf("RPC_parse_answer:\n");
		for (uint16_t i=0;i<size;i++){
			printf("%02X ",(unsigned char)buf[i]);
		}
		printf("\n");
#endif
		mock_c()->actualCall("RPC_parse_answer")
						->withIntParameters("firstByte", (unsigned char)buf[0])
						->withIntParameters("length", size);

		//RPC_TRANSMISSION_RESULT result = RPC_TRANSMISSION_SUCCESS;
		//return result;
	}

	void RPC_TRANSMISSION_parse_request(const void *buffer, size_t size){
		(void)buffer;
		uint8_t *buf = (uint8_t*)buffer;
#if 0
		printf("RPC_parse_answer:\n");
		for (uint16_t i=0;i<size;i++){
			printf("%02X ",(unsigned char)buf[i]);
		}
		printf("\n");
#endif
		mock_c()->actualCall("RPC_parse_request")
						->withIntParameters("firstByte", (unsigned char)buf[0])
						->withIntParameters("length", size);

		//RPC_TRANSMISSION_RESULT result = RPC_TRANSMISSION_SUCCESS;
		//return result;
	}
#if 0
	RPC_TRANSMISSION_SIZE_RESULT RPC_get_answer_length(const void *buffer, size_t size){
		RPC_TRANSMISSION_SIZE_RESULT result;
		uint8_t *buf = (uint8_t*)buffer;

		result.result = RPC_TRANSMISSION_COMMAND_INCOMPLETE;
		result.size = 0;
		if(buf[0] == 0){
			if (size < 5){
				result.result = RPC_TRANSMISSION_COMMAND_INCOMPLETE;
			}else{
				result.result = RPC_TRANSMISSION_SUCCESS;
				result.size = 10;
			}
		}else if (buf[0]==1){
			result.result = RPC_TRANSMISSION_SUCCESS;
			result.size = 50;
		}
		return result;
	}
#endif
	uint16_t getMsgLength(uint16_t payLoadLength, uint8_t blocklength){
		uint16_t result=payLoadLength;

		if (blocklength==8){
			result+=(payLoadLength/7)+1;
			if ((payLoadLength%7) == 0)
				result--;
		}else if(blocklength==16){
			result+=(payLoadLength/15)+1;
			if ((payLoadLength%15) == 0)
				result--;
		}
		result += 5;
		return result;

	}

	uint8_t getByteSequence(uint8_t firstByte, uint16_t index){
		uint8_t byte;
		if(index){
			byte = index | 0x80;
		}else{
			byte = firstByte;
		}
		return byte;
	}

#if 0
	void unistExecRDDSRES_cb_t(int64_t bdAdress,char *name, int8_t nameLength, uint32_t CoD){
		genMutexLock(&mutexMockCrtitcalSection);
		memcpy(MockBuffer,name,nameLength);
		MockBuffer[nameLength] = 0;
#if 0
		printf("unistExecRDDSRES_cb %s\n",MockBuffer);
#endif
		mock_c()->actualCall("unistExecRDDSRES_cb")
						->withIntParameters("param1a", (bdAdress >> 24) & 0xFFFFFF)
						->withIntParameters("param1b", bdAdress & 0xFFFFFF)
						->withIntParameters("param3", CoD);
		genMutexUnlock(&mutexMockCrtitcalSection);
	}
#endif


}


TEST_GROUP(channel_codec)
{
	void setup()
	{
		channel_init();
		channel_init_instance(channel_codec_instance_uart,
				&RPC_TRANSMISSION_parse_request,
				&RPC_TRANSMISSION_parse_answer,
				&RPC_TRANSMISSION_get_request_size,
				&RPC_TRANSMISSION_get_answer_length,
				&RPC_TRANSMISSION_Parser_init);
	}

	void teardown()
	{
		mock().clear();
		TXDataLength = 0;

	}

	void checkPreambel(channel_codec_instance_index_t instance_index){
		CHECK_EQUAL(0xFF,(unsigned char)instances[instance_index].txState.buffer[0]);
		CHECK_EQUAL(0xFF,(unsigned char)instances[instance_index].txState.buffer[1]);
		CHECK_EQUAL(0xFF,(unsigned char)instances[instance_index].txState.buffer[2]);
	}

};
#if 1 && DISABLE_TESTS
TEST(channel_codec, Initialization)
{
	channel_init();
	for (uint8_t i = 0; i<channel_codec_instance_COUNT;i++){
		CHECK_EQUAL(0,instances[i].txState.writePointer);
		CHECK_EQUAL(0,instances[i].txState.bitMaskPositionInBuffer);
		CHECK_EQUAL(0,instances[i].txState.indexInBlock);
	}

}
#endif

#if 1 && DISABLE_TESTS
TEST(channel_codec, channel_start_message_from_RPC)
{
	channel_start_message_from_RPC(channel_codec_instance_uart,10);
	checkPreambel(channel_codec_instance_uart);
	CHECK_EQUAL(3,instances[channel_codec_instance_uart].txState.writePointer);
}
#endif

#if 1 && DISABLE_TESTS
TEST(channel_codec, channel_push_byte_from_RPC)
{
	const channel_codec_instance_index_t ii = channel_codec_instance_uart;

	channel_start_message_from_RPC(ii,10);
	CHECK_EQUAL(3,instances[ii].txState.writePointer);

	channel_push_byte_from_RPC(ii,0x81);
	CHECK_EQUAL(1,instances[ii].txState.indexInBlock);
	CHECK_EQUAL(3,instances[ii].txState.bitMaskPositionInBuffer);
	channel_push_byte_from_RPC(ii,0x82);
	channel_push_byte_from_RPC(ii,0x81);
	channel_push_byte_from_RPC(ii,0x82);
	channel_push_byte_from_RPC(ii,0x81);
	channel_push_byte_from_RPC(ii,0x82);
	channel_push_byte_from_RPC(ii,0x81);
	checkPreambel(ii);
#if CHANNEL_BLOCKLENGTH == 8
	CHECK_EQUAL(0,instances[ii].txState.indexInBlock);
	CHECK_EQUAL(0,instances[ii].txState.bitMaskPositionInBuffer);
	CHECK_EQUAL(0x55,(unsigned char)instances[ii].txState.buffer[3]);
#endif




	CHECK_EQUAL(0x80,(unsigned char)instances[ii].txState.buffer[4]);
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].txState.buffer[5]);
	CHECK_EQUAL(0x80,(unsigned char)instances[ii].txState.buffer[6]);
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].txState.buffer[7]);
	CHECK_EQUAL(0x80,(unsigned char)instances[ii].txState.buffer[8]);
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].txState.buffer[9]);
	CHECK_EQUAL(0x80,(unsigned char)instances[ii].txState.buffer[10]);

	channel_push_byte_from_RPC(ii,0x81);
	channel_push_byte_from_RPC(ii,0x82);
	channel_push_byte_from_RPC(ii,0x81);
	channel_push_byte_from_RPC(ii,0x82);
	channel_push_byte_from_RPC(ii,0x81);
	channel_push_byte_from_RPC(ii,0x82);
	channel_push_byte_from_RPC(ii,0x81);
#if CHANNEL_BLOCKLENGTH == 16
	CHECK_EQUAL(0,instances[ii].txState.indexInBlock);
	CHECK_EQUAL(0,instances[ii].txState.bitMaskPositionInBuffer);
	CHECK_EQUAL(15,(unsigned char)instances[ii].txState.buffer[3]);
	CHECK_EQUAL(0x81,(unsigned char)instances[ii].txState.buffer[11]);
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].txState.buffer[12]);
	CHECK_EQUAL(0x81,(unsigned char)instances[ii].txState.buffer[13]);
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].txState.buffer[14]);
	CHECK_EQUAL(0x81,(unsigned char)instances[ii].txState.buffer[15]);
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].txState.buffer[16]);
	CHECK_EQUAL(0x81,(unsigned char)instances[ii].txState.buffer[17]);
	CHECK_EQUAL(18,instances[ii].txState.writePointer);
#else
	CHECK_EQUAL(0x55,(unsigned char)instances[ii].txState.buffer[11]);
	CHECK_EQUAL(0x80,(unsigned char)instances[ii].txState.buffer[12]);
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].txState.buffer[13]);
	CHECK_EQUAL(0x80,(unsigned char)instances[ii].txState.buffer[14]);
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].txState.buffer[15]);
	CHECK_EQUAL(0x80,(unsigned char)instances[ii].txState.buffer[16]);
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].txState.buffer[17]);
	CHECK_EQUAL(0x80,(unsigned char)instances[ii].txState.buffer[18]);
	CHECK_EQUAL(19,instances[ii].txState.writePointer);
#endif







}
#endif


#if 1 && DISABLE_TESTS
TEST(channel_codec, channel_commit_from_RPC)
{
	const channel_codec_instance_index_t ii = channel_codec_instance_uart;
	RPC_TRANSMISSION_RESULT result;

	uint8_t sendbuffer[16] = {0x10,0x18,0x81,0x82, 0x81,0x82,0x81,0x81,   0x81,0x82,0x81,0x82, 0x81,0x82,0x81,0x81};
	uint16_t msglengh=getMsgLength(16, CHANNEL_BLOCKLENGTH);

	mock().expectOneCall("eunistonePushDataBuffer").withParameter("length", msglengh);

	channel_start_message_from_RPC(ii,16);
	CHECK_EQUAL(3,instances[ii].txState.writePointer);


	for (int i=0;i<16;i++){
		channel_push_byte_from_RPC(ii,sendbuffer[i]);
#if 0
		printf("%02X ",(unsigned char)sendbuffer[i]);
#endif
	}

#if 0
	printf("\n");
	for (int i=0;i<22;i++){
		printf("%02X ",(unsigned char)channel_tx_buffer[i]);
	}
	printf("\n");
#endif
	result = channel_commit_from_RPC(ii);
#if CHANNEL_BLOCKLENGTH==8
	CHECK_EQUAL(0xB6,(unsigned char)instances[ii].txState.buffer[22]);
	CHECK_EQUAL(0xBD,(unsigned char)instances[ii].txState.buffer[23]);
#elif CHANNEL_BLOCKLENGTH==16
	CHECK_EQUAL(0xB6,(unsigned char)instances[ii].txState.buffer[21]);
	CHECK_EQUAL(0xBD,(unsigned char)instances[ii].txState.buffer[22]);
#endif
	mock().checkExpectations();

}
#endif

#if 1 && DISABLE_TESTS
TEST(channel_codec, channel_push_to_RPC_find_preamble)
{
	const channel_codec_instance_index_t ii = channel_codec_instance_uart;
	channel_push_byte_to_RPC(ii,0x81);
	channel_push_byte_to_RPC(ii,0x81);
	CHECK_EQUAL(0,instances[ii].rxState.writePointer);
	channel_push_byte_to_RPC(ii,0xFF);
	channel_push_byte_to_RPC(ii,0xFF);
	channel_push_byte_to_RPC(ii,0xFF);
	CHECK_EQUAL(0,instances[ii].rxState.writePointer);
	channel_push_byte_to_RPC(ii,0x03);
	channel_push_byte_to_RPC(ii,0x80);
	channel_push_byte_to_RPC(ii,0x82);
	channel_push_byte_to_RPC(ii,0x82);

	CHECK_EQUAL(0x81,(unsigned char)instances[ii].rxState.buffer[0]);
#if CHANNEL_BLOCKLENGTH == 8
	CHECK_EQUAL(0x83,(unsigned char)instances[ii].rxState.buffer[1]);
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].rxState.buffer[2]);
#elif CHANNEL_BLOCKLENGTH == 16
	CHECK_EQUAL(0x82,(unsigned char)instances[ii].rxState.buffer[1]);
	CHECK_EQUAL(0x83,(unsigned char)instances[ii].rxState.buffer[2]);
#endif
	CHECK_EQUAL(3,instances[ii].rxState.writePointer);
	mock().checkExpectations();

}
#endif

#if 1 && DISABLE_TESTS
TEST(channel_codec, channel_push_to_RPC_many_bytes_withour_crashing)
{
	const channel_codec_instance_index_t ii = channel_codec_instance_uart;
	for(int i=0;i<256;i++) {
		channel_push_byte_to_RPC(ii,0x81);
	}
	CHECK_EQUAL(0,(unsigned char)instances[ii].rxState.writePointer);

}
#endif

#if 1 && DISABLE_TESTS
TEST(channel_codec, channel_push_to_RPC_bufsize)
{
	const channel_codec_instance_index_t ii = channel_codec_instance_uart;
	mock().expectOneCall("RPC_parse_request").withParameter("firstByte", 10).withParameter("length", 10);
	channel_push_byte_to_RPC(ii,0xFF);
	channel_push_byte_to_RPC(ii,0xFF);
	channel_push_byte_to_RPC(ii,0xFF);

	channel_push_byte_to_RPC(ii,0x0E);
	channel_push_byte_to_RPC(ii,0x0A);
	//printf("%d %d\n",rxState.messageResult.result, rxState.messageResult.size);
	CHECK_EQUAL(RPC_TRANSMISSION_COMMAND_INCOMPLETE,instances[ii].rxState.messageResult.result);
	channel_push_byte_to_RPC(ii,0x00);
	channel_push_byte_to_RPC(ii,0x00);
	channel_push_byte_to_RPC(ii,0x00);
	channel_push_byte_to_RPC(ii,0x00);
	channel_push_byte_to_RPC(ii,0x00);
	channel_push_byte_to_RPC(ii,0x00);
	CHECK_EQUAL(RPC_TRANSMISSION_COMMAND_INCOMPLETE,instances[ii].rxState.messageResult.result);
	CHECK_EQUAL(10,instances[ii].rxState.messageResult.size);
#if CHANNEL_BLOCKLENGTH == 8
	channel_push_byte_to_RPC(0x0E);
#endif
	channel_push_byte_to_RPC(ii,0x00);
	channel_push_byte_to_RPC(ii,0x00);
	channel_push_byte_to_RPC(ii,0x00);

#if CHANNEL_BLOCKLENGTH == 8
	channel_push_byte_to_RPC(0x79);
	channel_push_byte_to_RPC(0x42);
#elif CHANNEL_BLOCKLENGTH==16
	//16ByteBlock:
	channel_push_byte_to_RPC(ii,0x3f);
	channel_push_byte_to_RPC(ii,0x01);
#endif
	CHECK_EQUAL(RPC_TRANSMISSION_COMMAND_UNKNOWN,instances[ii].rxState.messageResult.result);
	CHECK_EQUAL(10,instances[ii].rxState.messageResult.size);
	mock().checkExpectations();
}
#endif

#if 1 && DISABLE_TESTS
TEST(channel_codec, channel_push_to_RPC_PingPong_10)
{
	#define RPC_IS_ANSWER 1
	#define RPC_IS_REQUEST 0
	const channel_codec_instance_index_t ii = channel_codec_instance_uart;
	const uint8_t length = 10  | RPC_IS_REQUEST;
	uint8_t buf_loc[length+20];
	uint8_t msgLength = getMsgLength(length,CHANNEL_BLOCKLENGTH);
#if 0
	printf("msgLength = %d\n",msgLength);
#endif
	mock().expectOneCall("eunistonePushDataBuffer").withParameter("length", msgLength);
	mock().expectOneCall("RPC_parse_request").withParameter("firstByte", length).withParameter("length", length);
	channel_start_message_from_RPC(ii,length);
	for (uint8_t i=0;i<length;i++){
		uint8_t byte;
		byte = getByteSequence(0,i);
		if (i==0){
			byte = length;
		}
		channel_push_byte_from_RPC(ii,byte);
	}
	channel_commit_from_RPC(ii);
#if 0
	printf("TXDataLength %d\n",TXDataLength);
#endif
	memcpy(buf_loc,instances[ii].txState.buffer,TXDataLength);
	for (uint8_t i=0;i<TXDataLength;i++){
		channel_push_byte_to_RPC(ii,buf_loc[i]);
	}
	for(int i = 0; i<length;i++){
		uint8_t byte;
		byte = getByteSequence(0,i);
		if (i==0){
			byte = length;
		}
		CHECK_EQUAL(byte,(unsigned char)instances[ii].rxState.buffer[i]);
	}
	mock().checkExpectations();
}
#endif


#if 1 && DISABLE_TESTS
TEST(channel_codec, channel_push_to_RPC_PingPong_50)
{
	#define RPC_IS_ANSWER 1
	#define RPC_IS_REQUEST 0
	const uint8_t length = 50 | RPC_IS_ANSWER;
	const channel_codec_instance_index_t ii = channel_codec_instance_uart;
	uint8_t buf_loc[length+30];
	uint8_t msgLength = getMsgLength(length,CHANNEL_BLOCKLENGTH);
#if 0
	printf("msgLength = %d\n",msgLength);
#endif
	mock().expectOneCall("eunistonePushDataBuffer").withParameter("length", msgLength);
	mock().expectOneCall("RPC_parse_answer").withParameter("firstByte", 0xFF).withParameter("length", length);
	channel_start_message_from_RPC(ii,length);
	for (uint8_t i=0;i<length;i++){
		uint8_t byte;
		byte = getByteSequence(1,i);
		if (i<20){ //tests if a payload consisting of only ones triggers a preamble
			byte = 0xFF;
		}
		channel_push_byte_from_RPC(ii,byte);
	}
	channel_commit_from_RPC(ii);
#if 0
	printf("TXDataLength %d\n",TXDataLength);
#endif
	memcpy(buf_loc,instances[ii].txState.buffer,TXDataLength);
	for (uint8_t i=0;i<TXDataLength;i++){
		channel_push_byte_to_RPC(ii,buf_loc[i]);
	}
	for(int i = 0; i<length;i++){
		uint8_t byte;
		byte = getByteSequence(1,i);
		if (i<20){
			byte = 0xFF;
		}
#if 0
		printf("%d\n",i);
#endif
		CHECK_EQUAL(byte,(unsigned char)instances[ii].rxState.buffer[i]);
	}
	mock().checkExpectations();
}
#endif
