#include "dbg.h"
#include "hhstdio.h"

uint8_t
DBG__get_report(USB_DBGReport_Data_t *report_data)
{
    /// @todo Remove DBGReport altogether if no debug
#ifdef DEBUG_OUTPUT
    if (!stdout_is_empty()) {
        char ch;
        int index = 0;
        while(index < DBG_EPSIZE && (ch = stdout_popchar())) {
            report_data->data[index++] = ch;
        }
    }

    return sizeof(USB_DBGReport_Data_t);
#else
    return 0;
#endif
}
