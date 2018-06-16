/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _COMM_H_
#define _COMM_H_


//--- Exported types ---//

//--- Exported constants ---//
typedef enum {
	resp_ok = 0,
	resp_not_own,
	resp_error

} resp_t;

//--- Exported macro ---//

//--- Exported functions ---//
void UpdateCommunications (void);
unsigned char SerialProcess (void);
unsigned char InterpretarMsg (void);


#endif
//--- End ---//
//--- END OF FILE ---//
