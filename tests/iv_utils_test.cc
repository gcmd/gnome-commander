#include <glib.h>
#include <stdio.h>
#include <intviewer/gvtypes.h>
#include <intviewer/viewer-utils.h>


void test_text2hex()
{
    g_return_if_fail (buf!=NULL);

    const gchar *text = "00 0102 03 04 05 06 AA BB CC FE";
    guint len;

    guint8 *buf = text2hex (text,len);

    for (int i=0; i<len; ++i)
        printf("%02x ", buf[i]);
    printf("\n");
}


int main()
{
    test_text2hex();
    return 0;
}
