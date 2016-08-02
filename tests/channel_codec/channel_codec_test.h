/*
 * btchannelTest.h
 *
 *  Created on: 30.03.2015
 *      Author: ak
 */

#ifndef CHANNELCODEC_CHANNELCODEC_TEST_H_
#define CHANNELCODEC_CHANNELCODEC_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#define PREAMBLE_LENGTH 3
#define CRC_LENGTH 2
#include "channel_codec/channel_codec_config.h"

RPC_TRANSMISSION_RESULT eunistonePushDataBuffer(const char *buffer, size_t length);
typedef struct{
	char buffer[CHANNEL_CODEC_TX_BUFFER_SIZE];
	uint16_t indexInBlock;
	uint16_t bitMaskPositionInBuffer;
	uint16_t writePointer;
	uint16_t crc16;
}txState_t;

typedef struct{
	char buffer[CHANNEL_CODEC_RX_BUFFER_SIZE];
	uint8_t indexInBlock;
	uint8_t bitmask;
	uint16_t writePointer;
	uint8_t preambleBuffer[PREAMBLE_LENGTH];
	RPC_TRANSMISSION_SIZE_RESULT messageResult;
}rxState_t;

typedef enum{csNone,csFoundPreamble,csLoadingPayload,csPayloadComplete,csCRCAndPackageComplete} cc_channel_state_t;
//extern txState_t txState;
//extern rxState_t rxState;
typedef struct{
	txState_t txState;
	rxState_t rxState;
	cc_channel_state_t ccChannelState;
}instances_t;



extern instances_t instances[channel_codec_instance_COUNT];

#if 0
extern uint16_t channel_tx_bitMaskBitPointer;

extern uint16_t channel_tx_bitMaskPointer;
extern uint16_t channel_tx_write_pointer;
extern char channel_tx_buffer[];


extern uint16_t channel_rx_bitMaskBitPointer;
extern uint16_t channel_rx_bitMaskPointer;
extern uint16_t channel_rx_write_pointer;
extern char channel_rx_buffer[];
extern RPC_TRANSMISSION_SIZE_RESULT channel_rx_message_size;
#endif
#ifdef __cplusplus
}
#endif
#endif /* CHANNELCODEC_CHANNELCODEC_TEST_H_ */
