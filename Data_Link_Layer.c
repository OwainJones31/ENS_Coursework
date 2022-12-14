#include "Data_Link_Layer.h"

void LLC_Net_Interface(uint8_t Dest_MAC, uint8_t Packet_Data[], uint8_t Net_Leng){  
   Frame_Struct * Frames;   
   uint8_t Packet_Num, Bytes_Left, L1;
   uint8_t * Store;

   uint8_t MAC, Packet_Len;

   Frames = (Frame_Struct*)calloc(3,sizeof(Frame_Struct));
   Store = (uint8_t*)calloc(30,sizeof(uint8_t));

   Bytes_Left = Net_Leng % 8;
   if(Bytes_Left == 0){
      Packet_Num = Net_Leng/8;
   }else{
      Packet_Num = Net_Leng/8 + 1;
   }
   for(L1=0; L1<Packet_Num; L1++){
      Frames[L1].Control[0] = Packet_Num;
      Frames[L1].Control[1] = L1;
      Frames[L1].Dest_MAC = Dest_MAC;
      Frames[L1].Source_MAC = Local_MAC;
      Frames[L1].Length = 8;
      Frames[L1].checksum[1] = 0;
      Frames[L1].checksum[2] = 0;
   }
   Packet_Breakdown(Frames, Packet_Data, Packet_Num, Bytes_Left); /*Breaks the packet into frames*/
   for(L1=0; L1<Packet_Num; L1++){
      Find_Check(Frames, Store, L1, 1); 
      Frames[L1].checksum[0] = Get_Ints(Store,1);
      Frames[L1].checksum[1] = Get_Ints(Store,9);
      Print_Frame(Frames, L1);
      printf("\n");
      Add_Flags(Frames, Store, L1, Flag_Byte, Esc_Byte);
      for(Bytes_Left=0; Bytes_Left<22; Bytes_Left++){
         printf("%d ", Store[Bytes_Left]);
         /*Store[Bytes_Left] = 0;*/
      }
      printf("\n");

      Receiving(&MAC, Store, &Packet_Len); 
   }
   free(Frames);
   free(Store);
}

void Print_Frame(Frame_Struct * Frames, uint8_t Frame_Num){
   uint8_t L1;
   printf("\n%d ",Frames[Frame_Num].Header);
   printf("%d ",Frames[Frame_Num].Control[0]);
   printf("%d ",Frames[Frame_Num].Control[1]);
   printf("%d ",Frames[Frame_Num].Source_MAC);
   printf("%d ",Frames[Frame_Num].Dest_MAC);
   printf("%d ",Frames[Frame_Num].Length);
   for(L1=0; L1<Frames[Frame_Num].Length; L1++){
      printf("%d ",Frames[Frame_Num].NET_Packet[L1]);
   }
   printf("%d ",Frames[Frame_Num].checksum[0]);
   printf("%d ",Frames[Frame_Num].checksum[1]);
   printf("%d ",Frames[Frame_Num].Footer);
}

void Packet_Breakdown(Frame_Struct * Frames, uint8_t * Packet_Data, uint8_t Packet_Num, uint8_t Bytes_Left){
   uint8_t L1, L2, Curr_Byte;
   for(L1=0; L1<Packet_Num; L1++){
      if(L1 == Packet_Num-1 && Bytes_Left != 0){
         Curr_Byte = L1*8;
         for(L2=0; L2<=Bytes_Left; L2++){
            Frames[L1].NET_Packet[L2] = Packet_Data[Curr_Byte + L2];
         }
         Frames[L1].Length = Bytes_Left;
      }else{
         Curr_Byte = L1*8;
         for(L2=0; L2<=7; L2++){
            Frames[L1].NET_Packet[L2] = Packet_Data[Curr_Byte + L2];
         }
      }

   }
}

void Find_Check(Frame_Struct * Frames, uint8_t * CRC_Sum, uint8_t Frame_Num, uint8_t Add_To_Self){
   uint8_t L1, L2, Count = 0;
   uint8_t * Frame_Bit_Val;

   Frame_Bit_Val = (uint8_t*)calloc(121,sizeof(uint8_t));

   Get_Bits(Frames[Frame_Num].Control[0], Frame_Bit_Val, Count);
   Count += 8;
   Get_Bits(Frames[Frame_Num].Control[1], Frame_Bit_Val, Count);
   Count += 8;
   Get_Bits(Frames[Frame_Num].Source_MAC, Frame_Bit_Val, Count);
   Count += 8;
   Get_Bits(Frames[Frame_Num].Dest_MAC, Frame_Bit_Val, Count);
   Count += 8;
   Get_Bits(Frames[Frame_Num].Length, Frame_Bit_Val, Count);
   for(L1 = 0; L1 < Frames[Frame_Num].Length; L1++){
      Count += 8;
      Get_Bits(Frames[Frame_Num].NET_Packet[L1], Frame_Bit_Val, Count);
   }
   Count += 8;
   Get_Bits(Frames[Frame_Num].checksum[0], Frame_Bit_Val, Count);
   Count += 8;
   Get_Bits(Frames[Frame_Num].checksum[1], Frame_Bit_Val, Count);
   Count += 8;

   Calc_CRC(Frame_Bit_Val, CRC_Sum, Count);/*Calculates the CRC*/

   L2 = 1;
   if(Add_To_Self == 1){
      for(L1 = Count-16; L1 <= Count; L1++){
         CRC_Sum[L2] = Frame_Bit_Val[L1] ^ CRC_Sum[L2];
         L2 += 1;
      }
   }
   free(Frame_Bit_Val);
}

void Add_Space(uint8_t Count){
   uint8_t L1 = 0;
   for(L1 = 0; L1 <Count; L1++){
      printf("| ");
   }
}
void Equal(uint8_t Count){
   uint8_t L1 = 0;
   for(L1 = 0; L1 <=Count; L1++){
      printf("--");
   }
   printf("\n");
}

void Calc_CRC(uint8_t * Bit_Value, uint8_t * CRC_Sum, uint8_t Length){
   uint8_t Genarator[17] = {1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 0,0,0,1};
   uint8_t L1, L2, CRC_Loop = 1, Count, New_Count;

   Count = Find_Count(Bit_Value, 0);
   
   for(L1=0; L1<sizeof(Genarator);  L1++){
      CRC_Sum[L1] = Bit_Value[L1+Count] ^ Genarator[L1];
   }

   while(CRC_Loop == 1){
      New_Count = Find_Count(CRC_Sum, 0);
      L2 = Count+New_Count+17;
      if(Count+New_Count+17>=Length){
         L2 -= Length;
         New_Count -= L2;
      }
      if(New_Count == 0){
         New_Count = 1;
      }
      for(L1=New_Count; L1<sizeof(Genarator);  L1++){
         CRC_Sum[L1-New_Count] = CRC_Sum[L1];
      }


      L1 = 0;
      for(L2 = New_Count; L2>0;  L2--){
         CRC_Sum[17-L2] = Bit_Value[Count+17+L1];
         L1 += 1;
      } 


      for(L1=0; L1<=sizeof(Genarator)-1;  L1++){
         CRC_Sum[L1] = CRC_Sum[L1] ^ Genarator[L1];
      }

      Count += New_Count;
      if(Count+17 >= Length){
         CRC_Loop = 0;
      }
   }
}

uint8_t Find_Count(uint8_t * Bit_Value, uint8_t Count){
   uint8_t L1 = 1;
   while(L1 == 1){
      if(Bit_Value[Count] == 0){
         Count += 1;
      }else{
         L1 = 0;
      }
   }
   return Count;
}

void Get_Bits(uint8_t Value, uint8_t * Bit_Value, uint8_t Count){
   int L1, Result = 0;
   int L2 = 0;
   for(L1=7; L1>=0; L1--){
      Result = Value - pow(2,L1);
      if(Result>=0){
         Bit_Value[L2 + Count] = 1;
         Value = Value - pow(2,L1);
      }
      L2 +=1;
   }
}

uint8_t Get_Ints(uint8_t * Bit_Value, uint8_t Count){
   uint8_t Result = 0;
   int L1, L2 = 7;
   for(L1=0; L1<=7; L1++){
      if(Bit_Value[L1+Count] ==1){
         Result += pow(2,L2);
      }
      L2 -=1;
   }
   return Result;
}

void Add_Flags(Frame_Struct * Frames, uint8_t * Finalized_Frame, uint8_t Frame_Num, uint8_t Flag, uint8_t Esc){
   uint8_t L1 = 0, L2 = 0, L3;
   Finalized_Frame[0] = Flag;
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].Control[0];
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].Control[1];
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].Source_MAC;
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].Dest_MAC;
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].Length;
   L1 += 1;
   for(L2=0;L2<Frames[Frame_Num].Length;L2++){
      Finalized_Frame[L1] = Frames[Frame_Num].NET_Packet[L2];
      L1 += 1;
   }

   Finalized_Frame[L1] = Frames[Frame_Num].checksum[0];
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].checksum[1];
   L1 += 1;
   Finalized_Frame[L1] = Flag;
   L3 = L1;
   for(L2=1;L2<L3-1;L2++){
      if(Finalized_Frame[L2] == Flag || Finalized_Frame[L2] == Esc){
         for(L1=L3+1; L1>L2;L1--){
            Finalized_Frame[L1] = Finalized_Frame[L1-1];
         }
         Finalized_Frame[L2] = Esc;
         L2+=1;
         L3+=1;
      }
   }
}

/*                        RECEIVING                        */
void Receiving(uint8_t * MAC_Address, uint8_t * Full_Packet, uint8_t * Packet_Len){
   uint8_t L1 = 0, L2 = 255, Rec_Loop = 0;
   Frame_Struct * Frames;   

   Frames = (Frame_Struct*)calloc(3,sizeof(Frame_Struct));

   while(Rec_Loop == 0){

      if(Full_Packet[0]== 126){
         MAC_LLC_Interface(Frames, Full_Packet, L1, &L2);
         Print_Frame(Frames,L1);
         L1 += 1;
      }

      if(L2 != 255 && Frames[L1].Control[0] == L1){
         *MAC_Address = L2;
         for (L1 = 0; L1 < Frames[L1].Control[0]; L1++){
            for (L2= 0;  L2<Frames[L1].Length ; L2++){
               Full_Packet[(Frames[L1].Control[1]*8)+L2] = Frames[L1].NET_Packet[L2];
               *Packet_Len += 1;
            }
         }
         Rec_Loop = 1;
      }
      Rec_Loop = 1;
   }
   free(Frames);

}

void MAC_LLC_Interface(Frame_Struct * Frames, uint8_t * Rec_Data, uint8_t Frame_Num, uint8_t * Source){
   uint8_t L1 = 0, L2 = 0;
   uint8_t * CRC_Sum;

   CRC_Sum = (uint8_t*)calloc(17,sizeof(uint8_t));

   if(Rec_Data[0] == Flag_Byte){
      L2 += 1;
      while(L1 == 0){
         if(Rec_Data[L2] == Esc_Byte){
            L2 += 2;
         }else if(Rec_Data[L2] == Flag_Byte){
            L2 += 1;
            L1 = 1;
         }else{
            L2 += 1;
         }
      }
      if(*Source == 255)      {
         *Source = Rec_Data[3];
      }
      Remove_Flags(Rec_Data, L2, Flag_Byte, Esc_Byte);
      if(*Source == Rec_Data[3]){
         Frames[Frame_Num].Control[0] = Rec_Data[1];
         Frames[Frame_Num].Control[1] = Rec_Data[2];
         Frames[Frame_Num].Source_MAC = Rec_Data[3];
         Frames[Frame_Num].Dest_MAC   = Rec_Data[4];
         Frames[Frame_Num].Length     = Rec_Data[5];
         for(L1=0; L1<Rec_Data[5]; L1++){
            Frames[Rec_Data[2]].NET_Packet[L1] = Rec_Data[L1+6];
         }
         Frames[Frame_Num].checksum[0] = Rec_Data[Rec_Data[5]+6];
         Frames[Frame_Num].checksum[1] = Rec_Data[Rec_Data[5]+7];
         Print_Frame(Frames,Frame_Num);

         Find_Check(Frames, CRC_Sum, Frame_Num, 0);

         printf("\n");
         L2 = 0;
         for(L1 = 1; L1 <17; L1++){
            printf("%d ", CRC_Sum[L1]);
            if(CRC_Sum[L1] == 1){
               L2 = 1;
            }
         }
         free(CRC_Sum);
         if(L2 == 1){
            printf("BBB");
            Frames[Frame_Num].Control[0] = 0;
            Frames[Frame_Num].Control[1] = 0;
            Frames[Frame_Num].Source_MAC = 0;
            Frames[Frame_Num].Dest_MAC   = 0;
            Frames[Frame_Num].Length     = 0;
            for(L1=0; L1<Rec_Data[5]; L1++){
               Frames[Frame_Num].NET_Packet[L1] = 0;
            }
            Frames[Frame_Num].checksum[0] = 0;
            Frames[Frame_Num].checksum[1] = 0;
         }else{
            Frame_Num +=1;
         }
      }
   }
}

void Remove_Flags(uint8_t * In_Frame, uint8_t Length, uint8_t Flag, uint8_t Esc){
   uint8_t L1 = 0, L2 = 0;
   for (L1 = 0; L1 < Length; L1++){
      if(In_Frame[L1] == Flag_Byte){
         In_Frame[L1] = 0;
      }else if(In_Frame[L1] == Esc_Byte){
         for(L2=L1; L2 < Length; L2++){
            In_Frame[L2] = In_Frame[L2+1];
         }
         L2+=1;
         Length -= 1;
      }
   }
}

int main() {
   uint8_t MAC = 2, Len = 8;
   uint8_t Pac[18] = {1,2,3,4,5,125,7,8,9,10,11,12,13,14,15,16,17,18};

   /*uint8_t Rec_Data[30]= {126, 2, 0, 1, 2, 8, 1, 2, 3, 4, 5, 125, 125, 7, 8, 200, 233, 126};
   uint8_t  Packet_Len = 0, L1;*/

   LLC_Net_Interface(MAC, Pac, Len);

   /*Receiving(&MAC, Rec_Data, &Packet_Len);
   printf("\nAddress: %d, Length: %d Data: ", MAC, Packet_Len);
   for (L1= 0;  L1<Packet_Len ; L1++){
      printf("%d ", Rec_Data[L1]);
   }*/

   return 0;
}