#include "dbg.h"
#include "hhstdio.h"

uint8_t
DBG__get_report(USB_DBGReport_Data_t *report_data)
{
    return 0;
/*
    if (!stdout_is_empty()) {
        char ch;
        int index = 0;
        while(index < DBG_EPSIZE && (ch = stdout_popchar())) {
            report_data->data[index++] = ch;
        }
    }

    return sizeof(USB_DBGReport_Data_t);
*/
}
