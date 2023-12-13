// file fft.h
#ifndef EXAMPLE_FFT

#define ARRAY_SIZE        64      //MUST BE power of 2 (e.g., 2, 4, 8, 16, 32,64,...)!!!! for fft
#define MS_BETWEEN_MEASUREMENTS 50  //here for testing will put it in .h later

void rearrange(float data_re[], float data_im[], unsigned int N);

void compute(float data_re[], float data_im[], unsigned int N);

float fft(const float data_re[], float freq_bins[], int N);

float findResonantFrequency(const float fftResult[], int arraySize, float sampleRate);


#endif