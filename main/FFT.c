//
// Created by bert on 29/11/23.
//

#include "FFT.h"

#include "kiss_fft130\kiss_fft.h" 
//#include <math.h>

void fft(float data_re[], const unsigned int N) {
    //kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, 0, 0);
    float dataIM[ARRAY_SIZE] = {0.0};
    rearrange(data_re, dataIM, N);
    compute(data_re, dataIM, N);
}

void rearrange(float data_re[], float data_im[], const unsigned int N) {
    unsigned int target = 0;
    for (unsigned int position = 0; position < N; position++) {
        if (target > position) {
            const float temp_re = data_re[target];
            const float temp_im = data_im[target];
            data_re[target] = data_re[position];
            data_im[target] = data_im[position];
            data_re[position] = temp_re;
            data_im[position] = temp_im;
        }
        unsigned int mask = N;
        while (target & (mask >>= 1))
            target &= ~mask;
        target |= mask;
    }
}

void compute(float data_re[], float data_im[], const unsigned int N) {
    const float pi = -3.14159265358979323846;

    for (unsigned int step = 1; step < N; step <<= 1) {
        const unsigned int jump = step << 1;
        const float step_d = (float) step;
        float twiddle_re = 1.0;
        float twiddle_im = 0.0;
        for (unsigned int group = 0; group < step; group++) {
            for (unsigned int pair = group; pair < N; pair += jump) {
                const unsigned int match = pair + step;
                const float product_re = twiddle_re * data_re[match] - twiddle_im * data_im[match];
                const float product_im = twiddle_im * data_re[match] + twiddle_re * data_im[match];
                data_re[match] = data_re[pair] - product_re;
                data_im[match] = data_im[pair] - product_im;
                data_re[pair] += product_re;
                data_im[pair] += product_im;
            }

            // we need the factors below for the next iteration
            // if we don't iterate then don't compute
            if (group + 1 == step) {
                continue;
            }

            float angle = pi * ((float) group + 1) / step_d;
            twiddle_re = cos(angle);
            twiddle_im = sin(angle);
        }
    }
}



float findResonantFrequency(const float *fftResult, const int arraySize, float sampleRate) {
    // Find the index with the maximum amplitude
    int maxIndex = 0;
    float maxAmplitude = 0.0;
    float lowerCutoff = 1;
    float upperCutoff = 3;


    for (int i = 0; i < arraySize; ++i) {
        float Frequency = i * sampleRate / arraySize;
        if (fftResult[i] > maxAmplitude && (Frequency > lowerCutoff || Frequency < upperCutoff)) {
            maxAmplitude = fftResult[i];
            maxIndex = i;
        }
    }

    // Calculate the corresponding frequency in Hz
    float resonantFrequency = (float) maxIndex * sampleRate / arraySize;

    return resonantFrequency;
}
