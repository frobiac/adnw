#ifndef __DBG_H__
#define __DBG_H__

#include "Descriptors.h"
#include "hhstdio.h"

typedef struct
{
  char data[DBG_EPSIZE];

} USB_DBGReport_Data_t;

uint8_t DBG__get_report(USB_DBGReport_Data_t* report);

#endif // __DBG_H__
