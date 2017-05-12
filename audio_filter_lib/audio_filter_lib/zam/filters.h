//#include <inttypes.h>
#include <math.h>
#include "typedefine.h"

void init_highpass_filter_zam(FilterStateZam *hpf, float fc, float fs);
void init_lowpass_filter_zam(FilterStateZam *lpf, float fc, float fs);
int run_filter_zam(FilterStateZam* fil, float* data, int length);
int run_saturator_zam(float *data, int length);
double sanitize_denormal(double v);
