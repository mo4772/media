#include <stdio.h>
#include "typedefine.h"
#include "filter_audio.h"

int main()
{
    Filter_Audio *p = NULL;
    p = new_filter_audio(8000);
}