// file fft.h
#ifndef EXAMPLE_FFT

#define ARRAY_SIZE        64      //MUST BE power of 2 (e.g., 2, 4, 8, 16, 32,64,...)!!!! for fft

void rearrange(float data_re[], float data_im[], unsigned int N);

void compute(float data_re[], float data_im[], unsigned int N);

void fft(float data_re[], unsigned int N);

float findResonantFrequency(const float fftResult[], int arraySize, float sampleRate);


#endif