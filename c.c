#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct ArrHeader {
    uint16_t    capacity;
    uint16_t    size;
} arrh_t;

float* FillArr();

int main() {
    float *arr = FillArr();


    return 0;
}

float* FillArr() {
    float *arr = malloc(15 * sizeof(float));



    return arr;
}