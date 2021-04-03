#ifndef _GB_CONFIG_H
 #define _GB_CONFIG_H
 
 //Video mode
 #define use_lib_400x300 

 //Logs
 #define use_lib_log_serial
 

 #define usb_lib_wait_emu 
 #define usb_lib_fast_GetCyclesForOpcode
 #define usb_lib_fast_MemoryReadByte
 #define usb_lib_fast_MemoryWriteByte
 
 //Leer teclado cada x milisegundos
 #define gb_ms_keyboard 10
 


 //Read keyborad poll x millis
 #define gb_ms_keyboard 20
 //Read sound poll x millis
 #define gb_ms_sound 1
 //Skip x frames
 #define gb_frame_crt_skip 0
 //Delay millis iteration emulate
 #define gb_delay_emulate_ms 0



 #ifdef define use_lib_200x150
  #define gb_add_offset_x 0
  #define gb_add_offset_y 0
 #else
  #define gb_add_offset_x 60 
  #define gb_add_offset_y 40
 #endif
#endif
