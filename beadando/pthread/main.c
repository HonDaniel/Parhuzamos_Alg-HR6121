#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 6

typedef struct {
    int start;
    int end;
    unsigned char* image;
    int imageWidth;
    int imageHeight;
} ThreadArgs;

const float blurKernel[7][7] = {
    {1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49},
    {1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49},
    {1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49},
    {1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49},
    {1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49},
    {1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49},
    {1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49, 1.0 / 49}
};

void applyBlur(unsigned char* image, int start, int end, int imageWidth, int imageHeight) {
    unsigned char* tempImage = (unsigned char*)malloc((end - start) * sizeof(unsigned char));
    for (int i = start; i < end; i += 3) {
        int pixelIndex = i / 3;
        int y = pixelIndex / imageWidth;
        int x = pixelIndex % imageWidth;

        float sumR = 0.0, sumG = 0.0, sumB = 0.0;
        for (int dy = -3; dy <= 3; dy++) {
            for (int dx = -3; dx <= 3; dx++) {
                int nx = x + dx;
                int ny = y + dy;
                if (nx >= 0 && nx < imageWidth && ny >= 0 && ny < imageHeight) {
                    int kernelX = dx + 3;
                    int kernelY = dy + 3;
                    int neighborPixelIndex = ((ny * imageWidth + nx) * 3);
                    sumR += image[neighborPixelIndex] * blurKernel[kernelY][kernelX];
                    sumG += image[neighborPixelIndex + 1] * blurKernel[kernelY][kernelX];
                    sumB += image[neighborPixelIndex + 2] * blurKernel[kernelY][kernelX];
                }
            }
        }
        tempImage[(i - start)] = (unsigned char)(sumR + 0.5);
        tempImage[(i - start) + 1] = (unsigned char)(sumG + 0.5);
        tempImage[(i - start) + 2] = (unsigned char)(sumB + 0.5);
    }

    for (int i = start; i < end; i++) {
        image[i] = tempImage[i - start];
    }

    free(tempImage);
}

void* blurThread(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    applyBlur(threadArgs->image, threadArgs->start, threadArgs->end, threadArgs->imageWidth, threadArgs->imageHeight);
    pthread_exit(NULL);
}

void saveImage(const char* filename, unsigned char* image, int imageWidth, int imageHeight) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Failed to save image.\n");
        return;
    }

    fprintf(file, "P6\n%d %d\n255\n", imageWidth, imageHeight);

    fwrite(image, sizeof(unsigned char), imageWidth * imageHeight * 3, file);

    fclose(file);
}

int main() {
    FILE* file = fopen("highres.ppm", "rb");
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

    unsigned char* image = (unsigned char*)malloc(imageWidth * imageHeight * 3 * sizeof(unsigned char));
    fread(image, sizeof(unsigned char), imageWidth * imageHeight * 3, file);

    fclose(file);

    int sectionSize = (imageWidth * imageHeight * 3) / NUM_THREADS;

    pthread_t threads[NUM_THREADS];
    ThreadArgs threadArgs[NUM_THREADS];

    clock_t start = clock();

    for (int i = 0; i < NUM_THREADS; i++) {
        threadArgs[i].start = i * sectionSize;
        threadArgs[i].end = (i == NUM_THREADS - 1) ? (imageWidth * imageHeight * 3) : ((i + 1) * sectionSize);
        threadArgs[i].image = image;
        threadArgs[i].imageWidth = imageWidth;
        threadArgs[i].imageHeight = imageHeight;
        pthread_create(&threads[i], NULL, blurThread, (void*)&threadArgs[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end = clock();
    double elapsedSeconds = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Elapsed time: %f", elapsedSeconds);

    saveImage("highres_blur.ppm", image, imageWidth, imageHeight);

    free(image);

    return 0;
}
