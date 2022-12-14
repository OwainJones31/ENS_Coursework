#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#ifndef DATA_LINK_LAYER_H
#define DATA_LINK_LAYER_H
#define Local_MAC 1
#define Flag_Byte 126
#define Esc_Byte 125


typedef struct Frame_Struct{
	uint8_t Header;
	uint8_t Control[2];
	uint8_t Source_MAC;
	uint8_t Dest_MAC;
	uint8_t Length;
	uint8_t NET_Packet[8];
	uint8_t checksum[2];
	uint8_t Footer;
	int Escaped[15];
} Frame_Struct;

void LLC_Net_Interface(uint8_t Dest_MAC, uint8_t Net_Packet[], uint8_t Net_Leng);
void Packet_Breakdown(Frame_Struct * Frames, uint8_t * Packet_Data, uint8_t Packet_Num, uint8_t Bytes_Left);
void Add_Flags(Frame_Struct * Frames, uint8_t * Finalized_Frame, uint8_t Frame_Num, uint8_t Flag, uint8_t Esc);

void Find_Check(Frame_Struct * Frames, uint8_t * CRC_Sum, uint8_t Frame_Num, uint8_t Add_To_Self);
void Calc_CRC(uint8_t * Bit_Value, uint8_t * CRC_Sum, uint8_t Length);
uint8_t Find_Count(uint8_t * Bit_Value, uint8_t Count);
void Get_Bits(uint8_t Value, uint8_t * Bit_Value, uint8_t Count);
uint8_t Get_Ints(uint8_t * Bit_Value, uint8_t Count);

void Receiving(uint8_t * MAC_Address, uint8_t * NET_Packet, uint8_t * Packet_Len);
void MAC_LLC_Interface(Frame_Struct * Frames, uint8_t * Rec_Data, uint8_t Frame_Num, uint8_t * Source);
void Remove_Flags(uint8_t * In_Frame, uint8_t Length, uint8_t Flag, uint8_t Esc);

void Print_Frame(Frame_Struct * Frames, uint8_t Frame_Num);

#endif