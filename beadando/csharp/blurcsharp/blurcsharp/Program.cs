using System;
using System.IO;
using System.Threading;

public class Program
{
    private const int NUM_THREADS = 12;

    private struct ThreadArgs
    {
        public int start;
        public int end;
        public byte[] image;
        public int imageWidth;
        public int imageHeight;
    }

    private static readonly float[,] blurKernel = {
        {1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49},
        {1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49},
        {1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49},
        {1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49},
        {1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49},
        {1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49},
        {1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49, 1.0f / 49}
    };

    private static void ApplyBlur(byte[] image, int start, int end, int imageWidth, int imageHeight)
    {
        byte[] tempImage = new byte[end - start];
        for (int i = start; i < end; i += 3)
        {
            int pixelIndex = i / 3;
            int y = pixelIndex / imageWidth;
            int x = pixelIndex % imageWidth;

            float sumR = 0.0f, sumG = 0.0f, sumB = 0.0f;
            for (int dy = -3; dy <= 3; dy++)
            {
                for (int dx = -3; dx <= 3; dx++)
                {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < imageWidth && ny >= 0 && ny < imageHeight)
                    {
                        int kernelX = dx + 3;
                        int kernelY = dy + 3;
                        int neighborPixelIndex = ((ny * imageWidth + nx) * 3);
                        sumR += image[neighborPixelIndex] * blurKernel[kernelY, kernelX];
                        sumG += image[neighborPixelIndex + 1] * blurKernel[kernelY, kernelX];
                        sumB += image[neighborPixelIndex + 2] * blurKernel[kernelY, kernelX];
                    }
                }
            }
            tempImage[(i - start)] = (byte)(sumR + 0.5f);
            tempImage[(i - start) + 1] = (byte)(sumG + 0.5f);
            tempImage[(i - start) + 2] = (byte)(sumB + 0.5f);
        }

        for (int i = start; i < end; i++)
        {
            image[i] = tempImage[i - start];
        }
    }

    private static void BlurThread(object args)
    {
        ThreadArgs threadArgs = (ThreadArgs)args;
        ApplyBlur(threadArgs.image, threadArgs.start, threadArgs.end, threadArgs.imageWidth, threadArgs.imageHeight);
    }

    private static void SaveImage(string filename, byte[] image, int imageWidth, int imageHeight)
    {
        using (FileStream file = new FileStream(filename, FileMode.Create))
        {
            using (StreamWriter writer = new StreamWriter(file))
            {
                writer.WriteLine("P6");
                writer.WriteLine($"{imageWidth} {imageHeight}");
                writer.WriteLine("255");
                file.Write(image, 0, imageWidth * imageHeight * 3);
            }
        }
    }

    public static void Main()
    {
        FileStream file = new FileStream("highres.ppm", FileMode.Open);
        using (StreamReader reader = new StreamReader(file))
        {
            string format = reader.ReadLine();
            if (format != "P6")
            {
                Console.WriteLine("Invalid image format.");
                return;
            }

            string[] dimensions = reader.ReadLine().Split(' ');
            int imageWidth = int.Parse(dimensions[0]);
            int imageHeight = int.Parse(dimensions[1]);

            reader.ReadLine(); 

            byte[] image = new byte[imageWidth * imageHeight * 3];
            file.Read(image, 0, imageWidth * imageHeight * 3);

            int sectionSize = (imageWidth * imageHeight * 3) / NUM_THREADS;

            Thread[] threads = new Thread[NUM_THREADS];
            ThreadArgs[] threadArgs = new ThreadArgs[NUM_THREADS];

            DateTime start = DateTime.Now;

            for (int i = 0; i < NUM_THREADS; i++)
            {
                threadArgs[i].start = i * sectionSize;
                threadArgs[i].end = (i == NUM_THREADS - 1) ? (imageWidth * imageHeight * 3) : ((i + 1) * sectionSize);
                threadArgs[i].image = image;
                threadArgs[i].imageWidth = imageWidth;
                threadArgs[i].imageHeight = imageHeight;
                threads[i] = new Thread(BlurThread);
                threads[i].Start(threadArgs[i]);
            }

            for (int i = 0; i < NUM_THREADS; i++)
            {
                threads[i].Join();
            }

            DateTime end = DateTime.Now;
            TimeSpan elapsed = end - start;

            Console.WriteLine($"Elapsed time: {elapsed.TotalSeconds} seconds");

            SaveImage("highres_blur.ppm", image, imageWidth, imageHeight);
        }
    }
}
