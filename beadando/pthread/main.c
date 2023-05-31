#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define NUM_THREADS 3

typedef struct {
    int start;
    int end;
    unsigned char* image;
} ThreadArgs;

const float blurKernel[3][3] = {
    {2.0 / 32, 4.0 / 32, 2.0 / 32},
    {4.0 / 32, 8.0 / 32, 4.0 / 32},
    {2.0 / 32, 4.0 / 32, 2.0 / 32}
};

void applyBlur(unsigned char* image, int start, int end) {
    unsigned char* tempImage = (unsigned char*)malloc((end - start) * 3 * sizeof(unsigned char));
    for (int i = start; i < end; i++) {

        int x = (i / 3) % IMAGE_WIDTH;
        int y = (i / 3) / IMAGE_WIDTH;

        float sumR = 0.0, sumG = 0.0, sumB = 0.0;
        int pixelIndex = i;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int nx = x + dx;
                int ny = y + dy;
                if (nx >= 0 && nx < IMAGE_WIDTH && ny >= 0 && ny < IMAGE_HEIGHT) {
                    int kernelX = dx + 1;
                    int kernelY = dy + 1;
                    int neighborPixelIndex = ((ny * IMAGE_WIDTH + nx) * 3) + pixelIndex % 3;
                    sumR += image[neighborPixelIndex] * blurKernel[kernelY][kernelX];
                    sumG += image[neighborPixelIndex + 1] * blurKernel[kernelY][kernelX];
                    sumB += image[neighborPixelIndex + 2] * blurKernel[kernelY][kernelX];
                }
            }
        }
        tempImage[i - start] = (unsigned char)(sumR + 0.5);
        tempImage[i - start + 1] = (unsigned char)(sumG + 0.5);
        tempImage[i - start + 2] = (unsigned char)(sumB + 0.5);
    }

    for (int i = start; i < end; i++) {
        image[i] = tempImage[i - start];
    }

    free(tempImage);
}

void* blurThread(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    applyBlur(threadArgs->image, threadArgs->start, threadArgs->end);
    pthread_exit(NULL);
}

void saveImage(const char* filename, unsigned char* image) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Failed to save image.\n");
        return;
    }

    fprintf(file, "P6\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);

    fwrite(image, sizeof(unsigned char), IMAGE_WIDTH * IMAGE_HEIGHT * 3, file);

    fclose(file);
}

int main() {
    FILE* file = fopen("art.ppm", "rb");
    if (file == NULL) {
        printf("Failed to load image.\n");
        return -1;
    }

    char format[3];
    fscanf(file, "%s\n", format);
    if (format[0] != 'P' || format[1] != '6') {
        printf("Invalid image format.\n");
        fclose(file);
        return -1;
    }

    int imageWidth, imageHeight, maxValue;
    fscanf(file, "%d %d\n%d\n", &imageWidth, &imageHeight, &maxValue);
    if (imageWidth != IMAGE_WIDTH || imageHeight != IMAGE_HEIGHT || maxValue != 255) {
        printf("Invalid image dimensions.\n");
        fclose(file);
        return -1;
    }

    unsigned char* image = (unsigned char*)malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 3 * sizeof(unsigned char));
    fread(image, sizeof(unsigned char), IMAGE_WIDTH * IMAGE_HEIGHT * 3, file);

    fclose(file);

    int sectionSize = (IMAGE_WIDTH * IMAGE_HEIGHT * 3) / NUM_THREADS;

    pthread_t threads[NUM_THREADS];
    ThreadArgs threadArgs[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        threadArgs[i].start = i * sectionSize;
        threadArgs[i].end = (i + 1) * sectionSize;
        threadArgs[i].image = image;
        pthread_create(&threads[i], NULL, blurThread, (void*)&threadArgs[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    saveImage("blurred_art.ppm", image);

    free(image);

    return 0;
}
