#define _CRT_SECURE_NO_WARNINGS
#define PI 4.0*atan(1.0)

#include <fstream>
#include <complex>
#include <cstdlib>
#include <cmath>
#include <iostream>

using namespace std;

void LPF(const char* argv[]);

int main(int argc, const char* argv[]) {
    LPF(argv);

    return 0;
}

void LPF(const char* argv[])
{
    float mCoef = atof(argv[1]);
    int mCount = atoi(argv[2]);
    fstream in(argv[3], ios_base::binary | ios_base::in);
    if (!in)
        return;

    char header[44];
    in.read(header, 44);
    unsigned size = *reinterpret_cast<unsigned*>(header + 40),
        rate = *reinterpret_cast<unsigned*>(header + 24);
    short channels = *reinterpret_cast<unsigned*>(header + 22),
          bytesPerSample = *reinterpret_cast<unsigned*>(header + 32);
    char* data = new char[size];
    in.read(data, size);
    unsigned mNumOfSamples = size / (channels * bytesPerSample);
    short* samples = reinterpret_cast<short*>(data);
    
    float CutOffFrequency = rate/2.f;

    float wCut = 2 * PI * CutOffFrequency;
    float wPeriod = 1 / wCut;
    float mPrevLow = 0;
    double mSampleTime = 1.f / rate;
    //LPF
    for (int i = 0; i < mCount; i++) {

        for (unsigned j = 0; j < mNumOfSamples; j++) {
            short temp = (wPeriod * mPrevLow + mSampleTime * samples[j]) / (wPeriod + mSampleTime);
            mPrevLow = temp*mCoef;
            samples[j] = mPrevLow;
        }
        
    }
    float max = pow(2,15);
    short peak = 0;
    for (unsigned i = 0; i < mNumOfSamples; i++){
            if (abs(samples[i]) > abs(peak)){
                peak =samples[i];
            }
    }

    short norm = max / peak;

    for (unsigned i = 0; i < mNumOfSamples; i++) {
        samples[i] *= norm;
    }

    fstream out("output.wav", ios_base::binary | ios_base::out);
    out.write(header, 44);
    out.write(data, size);

    delete[] data;
}