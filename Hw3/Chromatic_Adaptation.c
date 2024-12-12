#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

typedef struct
{
    int R_max;
    int G_max;
    int B_max;
} RGB_max;
typedef struct
{
    double R_avg;
    double G_avg;
    double B_avg;
} RGB_avg;

int i, j, k;

unsigned int Width = 0;
unsigned int Height = 0;
unsigned int BitDepth = 0;
unsigned int TotalPixel = 0;

int method;
float combine_factor;
int combine_Grey[3];
double RGB_avg_avg;

double findmax(double a, double b, double c)
{

    if (a > b && a > c)
    {
        return a;
    }
    else if (b > a && b > c)
    {
        return b;
    }
    else if (c > a && c > b)
    {
        return c;
    }
}
double findmin(double a, double b, double c)
{

    if (a < b && a < c)
    {
        return a;
    }
    else if (b < a && b < c)
    {
        return b;
    }
    else if (c < a && c < b)
    {
        return c;
    }
}

int main()
{

    FILE *fp_in1;
    FILE *fp_out1;
    FILE *fp_in2;
    FILE *fp_out2;
    FILE *fp_in3;
    FILE *fp_out3;
    FILE *fp_in4;
    FILE *fp_out4;

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    char input1file[20];
    char output1file[20];
    char input1path[30];
    char output1path[30];
    char input2file[20];
    char output2file[20];
    char input2path[30];
    char output2path[30];
    char input3file[20];
    char output3file[20];
    char input3path[30];
    char output3path[30];
    char input4file[20];
    char output4file[20];
    char input4path[30];
    char output4path[30];

    // 第一張圖

    // 輸入
    // Key輸入1的檔案名稱
    printf("Please enter Input1 file name:");
    scanf("%s", &input1file);
    sprintf(input1path, "./%s.bmp", input1file);
    fp_in1 = fopen(input1path, "rb");

    if (!fp_in1)
    {
        printf("Input bmp file is not open successfully.\n");
    }
    else
    {
        printf("Input is successfully!\n");
    }

    printf("MaxRGB(1) or GreyWorld(2) or Combination(3):");
    scanf("%d", &method);

    // 讀出寬度和高度以及位元深度 Input1:24bits Input2:32bits

    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_in1);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_in1);

    Width = infoHeader.biWidth;
    Height = infoHeader.biHeight;
    BitDepth = infoHeader.biBitCount;
    TotalPixel = Width * Height;

    printf("Width:%d\n", Width);
    printf("Height:%d\n", Height);
    printf("BitDepth:%d\n", BitDepth);
    printf("TotalPixel:%d\n", TotalPixel);

    RGBTRIPLE rgb; // 要放從fp_in讀出來的rgb
    RGB_max maximum;
    RGB_avg average;
    RGBTRIPLE(*rgb_Vector)
    [Width] = calloc(Height, Width * sizeof(RGBTRIPLE)); // 創造一個放原始像素的記憶體

    // max RGB
    if (method == 1)
    {
        maximum.R_max = 0;
        maximum.G_max = 0;
        maximum.B_max = 0;
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                // 把讀到的rgb放入記憶體
                fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in1);
                rgb_Vector[i][j].rgbtRed = rgb.rgbtRed;
                rgb_Vector[i][j].rgbtGreen = rgb.rgbtGreen;
                rgb_Vector[i][j].rgbtBlue = rgb.rgbtBlue;
                maximum.R_max = (rgb.rgbtRed > maximum.R_max) ? rgb.rgbtRed : maximum.R_max;
                maximum.G_max = (rgb.rgbtGreen > maximum.G_max) ? rgb.rgbtGreen : maximum.G_max;
                maximum.B_max = (rgb.rgbtBlue > maximum.B_max) ? rgb.rgbtBlue : maximum.B_max;
            }
        }
        printf("R_max:%d G_max:%d B_max:%d\n", maximum.R_max, maximum.G_max, maximum.B_max);
        fclose(fp_in1);
        // 處理過程
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                rgb_Vector[i][j].rgbtRed = (int)((float)rgb_Vector[i][j].rgbtRed / maximum.R_max * 255);
                rgb_Vector[i][j].rgbtGreen = (int)((float)rgb_Vector[i][j].rgbtGreen / maximum.G_max * 255);
                rgb_Vector[i][j].rgbtBlue = (int)((float)rgb_Vector[i][j].rgbtBlue / maximum.B_max * 255);
            }
        }
    }
    else if (method == 2)
    {
        average.R_avg = 0;
        average.G_avg = 0;
        average.B_avg = 0;
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                // 把讀到的rgb放入記憶體
                fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in1);
                rgb_Vector[i][j].rgbtRed = rgb.rgbtRed;
                rgb_Vector[i][j].rgbtGreen = rgb.rgbtGreen;
                rgb_Vector[i][j].rgbtBlue = rgb.rgbtBlue;
                average.R_avg = average.R_avg + rgb.rgbtRed;
                average.G_avg = average.G_avg + rgb.rgbtGreen;
                average.B_avg = average.B_avg + rgb.rgbtBlue;
            }
        }
        average.R_avg = average.R_avg / TotalPixel;
        average.G_avg = average.G_avg / TotalPixel;
        average.B_avg = average.B_avg / TotalPixel;
        // RGB_avg_avg=findmax(average.R_avg,average.G_avg,average.B_avg);
        RGB_avg_avg = (average.R_avg + average.G_avg + average.B_avg) / 3;
        // RGB_avg_avg=(findmax(average.R_avg,average.G_avg,average.B_avg)+findmin(average.R_avg,average.G_avg,average.B_avg))/2;

        printf("R_avg:%f G_avg:%f B_avg:%f\n", average.R_avg, average.G_avg, average.B_avg);
        printf("RGB_avg_avg:%f\n", RGB_avg_avg);

        fclose(fp_in1);
        // 處理過程
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                rgb_Vector[i][j].rgbtRed = (int)(rgb_Vector[i][j].rgbtRed * RGB_avg_avg / average.R_avg);
                rgb_Vector[i][j].rgbtGreen = (int)(rgb_Vector[i][j].rgbtGreen * RGB_avg_avg / average.G_avg);
                rgb_Vector[i][j].rgbtBlue = (int)(rgb_Vector[i][j].rgbtBlue * RGB_avg_avg / average.B_avg);
                rgb_Vector[i][j].rgbtRed = (rgb_Vector[i][j].rgbtRed > 255) ? 255 : rgb_Vector[i][j].rgbtRed;
                rgb_Vector[i][j].rgbtGreen = (rgb_Vector[i][j].rgbtGreen > 255) ? 255 : rgb_Vector[i][j].rgbtGreen;
                rgb_Vector[i][j].rgbtBlue = (rgb_Vector[i][j].rgbtBlue > 255) ? 255 : rgb_Vector[i][j].rgbtBlue;
            }
        }
    }
    else if (method == 3)
    {
        printf("Combine factor:");
        scanf("%f", &combine_factor);
        average.R_avg = 0;
        average.G_avg = 0;
        average.B_avg = 0;
        maximum.R_max = 0;
        maximum.G_max = 0;
        maximum.B_max = 0;
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                // 把讀到的rgb放入記憶體
                fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in1);
                rgb_Vector[i][j].rgbtRed = rgb.rgbtRed;
                rgb_Vector[i][j].rgbtGreen = rgb.rgbtGreen;
                rgb_Vector[i][j].rgbtBlue = rgb.rgbtBlue;
                maximum.R_max = (rgb.rgbtRed > maximum.R_max) ? rgb.rgbtRed : maximum.R_max;
                maximum.G_max = (rgb.rgbtGreen > maximum.G_max) ? rgb.rgbtGreen : maximum.G_max;
                maximum.B_max = (rgb.rgbtBlue > maximum.B_max) ? rgb.rgbtBlue : maximum.B_max;
                average.R_avg = average.R_avg + rgb.rgbtRed;
                average.G_avg = average.G_avg + rgb.rgbtGreen;
                average.B_avg = average.B_avg + rgb.rgbtBlue;
            }
        }

        average.R_avg = average.R_avg / TotalPixel;
        average.G_avg = average.G_avg / TotalPixel;
        average.B_avg = average.B_avg / TotalPixel;
        RGB_avg_avg = (average.R_avg + average.G_avg + average.B_avg) / 3;

        printf("R_max:%d G_max:%d B_max:%d\n", maximum.R_max, maximum.G_max, maximum.B_max);
        printf("RGB_avg_avg:%f\n", RGB_avg_avg);

        fclose(fp_in1);
        // 處理過程

        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {

                combine_Grey[0] = (int)(rgb_Vector[i][j].rgbtRed * RGB_avg_avg / average.R_avg);
                combine_Grey[1] = (int)(rgb_Vector[i][j].rgbtGreen * RGB_avg_avg / average.G_avg);
                combine_Grey[2] = (int)(rgb_Vector[i][j].rgbtBlue * RGB_avg_avg / average.B_avg);
                combine_Grey[0] = (combine_Grey[0] > 255) ? 255 : combine_Grey[0];
                combine_Grey[1] = (combine_Grey[1] > 255) ? 255 : combine_Grey[1];
                combine_Grey[2] = (combine_Grey[2] > 255) ? 255 : combine_Grey[2];

                rgb_Vector[i][j].rgbtRed = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtRed / maximum.R_max * 255) + combine_factor * combine_Grey[0];
                ;
                rgb_Vector[i][j].rgbtGreen = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtGreen / maximum.G_max * 255) + combine_factor * combine_Grey[1];
                rgb_Vector[i][j].rgbtBlue = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtBlue / maximum.B_max * 255) + combine_factor * combine_Grey[2];
            }
        }
    }

    // 輸出
    // Key輸出1的檔案名稱
    printf("Please enter Output1 file name:");
    scanf("%s", &output1file);
    sprintf(output1path, "./%s.bmp", output1file);
    fp_out1 = fopen(output1path, "wb");

    if (!fp_out1)
    {
        printf("Output bmp file is not open successfully.\n");
    }
    else
    {
        printf("Output is successfully!\n");
    }
    // 要先寫Header不然圖片會開不起來
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out1);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out1);
    for (i = 0; i < Height; i++)
    {
        for (j = 0; j < Width; j++)
        {
            rgb.rgbtRed = rgb_Vector[i][j].rgbtRed;
            rgb.rgbtGreen = rgb_Vector[i][j].rgbtGreen;
            rgb.rgbtBlue = rgb_Vector[i][j].rgbtBlue;

            fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out1); // 將 color matrix 寫入輸出圖檔
        }
    }
    fclose(fp_out1);
    // 第二張圖

    // 輸入
    // Key輸入2的檔案名稱
    printf("Please enter Input2 file name:");
    scanf("%s", &input2file);
    sprintf(input2path, "./%s.bmp", input2file);
    fp_in2 = fopen(input2path, "rb");

    if (!fp_in2)
    {
        printf("Input bmp file is not open successfully.\n");
    }
    else
    {
        printf("Input is successfully!\n");
    }
    // 讀出寬度和高度以及位元深度 Input1:24bits Input2:32bits
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_in2);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_in2);

    Width = infoHeader.biWidth;
    Height = infoHeader.biHeight;
    BitDepth = infoHeader.biBitCount;
    TotalPixel = Width * Height;

    printf("Width:%d\n", Width);
    printf("Height:%d\n", Height);
    printf("BitDepth:%d\n", BitDepth);
    printf("TotalPixel:%d\n", TotalPixel);

    if (method == 3)
    {
        printf("Combine factor:");
        scanf("%f", &combine_factor);
        average.R_avg = 0;
        average.G_avg = 0;
        average.B_avg = 0;
        maximum.R_max = 0;
        maximum.G_max = 0;
        maximum.B_max = 0;
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                // 把讀到的rgb放入記憶體
                fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in2);
                rgb_Vector[i][j].rgbtRed = rgb.rgbtRed;
                rgb_Vector[i][j].rgbtGreen = rgb.rgbtGreen;
                rgb_Vector[i][j].rgbtBlue = rgb.rgbtBlue;
                maximum.R_max = (rgb.rgbtRed > maximum.R_max) ? rgb.rgbtRed : maximum.R_max;
                maximum.G_max = (rgb.rgbtGreen > maximum.G_max) ? rgb.rgbtGreen : maximum.G_max;
                maximum.B_max = (rgb.rgbtBlue > maximum.B_max) ? rgb.rgbtBlue : maximum.B_max;
                average.R_avg = average.R_avg + rgb.rgbtRed;
                average.G_avg = average.G_avg + rgb.rgbtGreen;
                average.B_avg = average.B_avg + rgb.rgbtBlue;
            }
        }

        average.R_avg = average.R_avg / TotalPixel;
        average.G_avg = average.G_avg / TotalPixel;
        average.B_avg = average.B_avg / TotalPixel;
        RGB_avg_avg = (average.R_avg + average.G_avg + average.B_avg) / 3;

        printf("R_max:%d G_max:%d B_max:%d\n", maximum.R_max, maximum.G_max, maximum.B_max);
        printf("RGB_avg_avg:%f\n", RGB_avg_avg);

        fclose(fp_in2);
        // 處理過程

        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {

                combine_Grey[0] = (int)(rgb_Vector[i][j].rgbtRed * RGB_avg_avg / average.R_avg);
                combine_Grey[1] = (int)(rgb_Vector[i][j].rgbtGreen * RGB_avg_avg / average.G_avg);
                combine_Grey[2] = (int)(rgb_Vector[i][j].rgbtBlue * RGB_avg_avg / average.B_avg);
                combine_Grey[0] = (combine_Grey[0] > 255) ? 255 : combine_Grey[0];
                combine_Grey[1] = (combine_Grey[1] > 255) ? 255 : combine_Grey[1];
                combine_Grey[2] = (combine_Grey[2] > 255) ? 255 : combine_Grey[2];

                rgb_Vector[i][j].rgbtRed = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtRed / maximum.R_max * 255) + combine_factor * combine_Grey[0];
                ;
                rgb_Vector[i][j].rgbtGreen = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtGreen / maximum.G_max * 255) + combine_factor * combine_Grey[1];
                rgb_Vector[i][j].rgbtBlue = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtBlue / maximum.B_max * 255) + combine_factor * combine_Grey[2];
            }
        }
    }

    // 輸出
    // Key輸出1的檔案名稱
    printf("Please enter Output2 file name:");
    scanf("%s", &output2file);
    sprintf(output2path, "./%s.bmp", output2file);
    fp_out2 = fopen(output2path, "wb");

    if (!fp_out2)
    {
        printf("Output bmp file is not open successfully.\n");
    }
    else
    {
        printf("Output is successfully!\n");
    }
    // 要先寫Header不然圖片會開不起來
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out2);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out2);
    for (i = 0; i < Height; i++)
    {
        for (j = 0; j < Width; j++)
        {
            rgb.rgbtRed = rgb_Vector[i][j].rgbtRed;
            rgb.rgbtGreen = rgb_Vector[i][j].rgbtGreen;
            rgb.rgbtBlue = rgb_Vector[i][j].rgbtBlue;

            fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out2); // 將 color matrix 寫入輸出圖檔
        }
    }

    fclose(fp_out2);

    // 第三張圖

    // 輸入
    // Key輸入2的檔案名稱
    printf("Please enter Input3 file name:");
    scanf("%s", &input3file);
    sprintf(input3path, "./%s.bmp", input3file);
    fp_in3 = fopen(input3path, "rb");

    if (!fp_in3)
    {
        printf("Input bmp file is not open successfully.\n");
    }
    else
    {
        printf("Input is successfully!\n");
    }
    // 讀出寬度和高度以及位元深度 Input1:24bits Input2:32bits
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_in3);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_in3);

    Width = infoHeader.biWidth;
    Height = infoHeader.biHeight;
    BitDepth = infoHeader.biBitCount;
    TotalPixel = Width * Height;

    printf("Width:%d\n", Width);
    printf("Height:%d\n", Height);
    printf("BitDepth:%d\n", BitDepth);
    printf("TotalPixel:%d\n", TotalPixel);

    if (method == 3)
    {
        printf("Combine factor:");
        scanf("%f", &combine_factor);
        average.R_avg = 0;
        average.G_avg = 0;
        average.B_avg = 0;
        maximum.R_max = 0;
        maximum.G_max = 0;
        maximum.B_max = 0;
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                // 把讀到的rgb放入記憶體
                fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in3);
                rgb_Vector[i][j].rgbtRed = rgb.rgbtRed;
                rgb_Vector[i][j].rgbtGreen = rgb.rgbtGreen;
                rgb_Vector[i][j].rgbtBlue = rgb.rgbtBlue;
                maximum.R_max = (rgb.rgbtRed > maximum.R_max) ? rgb.rgbtRed : maximum.R_max;
                maximum.G_max = (rgb.rgbtGreen > maximum.G_max) ? rgb.rgbtGreen : maximum.G_max;
                maximum.B_max = (rgb.rgbtBlue > maximum.B_max) ? rgb.rgbtBlue : maximum.B_max;
                average.R_avg = average.R_avg + rgb.rgbtRed;
                average.G_avg = average.G_avg + rgb.rgbtGreen;
                average.B_avg = average.B_avg + rgb.rgbtBlue;
            }
        }

        average.R_avg = average.R_avg / TotalPixel;
        average.G_avg = average.G_avg / TotalPixel;
        average.B_avg = average.B_avg / TotalPixel;
        RGB_avg_avg = (average.R_avg + average.G_avg + average.B_avg) / 3;

        printf("R_max:%d G_max:%d B_max:%d\n", maximum.R_max, maximum.G_max, maximum.B_max);
        printf("RGB_avg_avg:%f\n", RGB_avg_avg);

        fclose(fp_in3);
        // 處理過程

        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {

                combine_Grey[0] = (int)(rgb_Vector[i][j].rgbtRed * RGB_avg_avg / average.R_avg);
                combine_Grey[1] = (int)(rgb_Vector[i][j].rgbtGreen * RGB_avg_avg / average.G_avg);
                combine_Grey[2] = (int)(rgb_Vector[i][j].rgbtBlue * RGB_avg_avg / average.B_avg);
                combine_Grey[0] = (combine_Grey[0] > 255) ? 255 : combine_Grey[0];
                combine_Grey[1] = (combine_Grey[1] > 255) ? 255 : combine_Grey[1];
                combine_Grey[2] = (combine_Grey[2] > 255) ? 255 : combine_Grey[2];

                rgb_Vector[i][j].rgbtRed = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtRed / maximum.R_max * 255) + combine_factor * combine_Grey[0];
                ;
                rgb_Vector[i][j].rgbtGreen = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtGreen / maximum.G_max * 255) + combine_factor * combine_Grey[1];
                rgb_Vector[i][j].rgbtBlue = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtBlue / maximum.B_max * 255) + combine_factor * combine_Grey[2];
            }
        }
    }

    // 輸出
    // Key輸出3的檔案名稱
    printf("Please enter Output3 file name:");
    scanf("%s", &output3file);
    sprintf(output3path, "./%s.bmp", output3file);
    fp_out3 = fopen(output3path, "wb");

    if (!fp_out3)
    {
        printf("Output bmp file is not open successfully.\n");
    }
    else
    {
        printf("Output is successfully!\n");
    }
    // 要先寫Header不然圖片會開不起來
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out3);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out3);
    for (i = 0; i < Height; i++)
    {
        for (j = 0; j < Width; j++)
        {
            rgb.rgbtRed = rgb_Vector[i][j].rgbtRed;
            rgb.rgbtGreen = rgb_Vector[i][j].rgbtGreen;
            rgb.rgbtBlue = rgb_Vector[i][j].rgbtBlue;

            fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out3); // 將 color matrix 寫入輸出圖檔
        }
    }
    fclose(fp_out3);

    // 第四張圖

    // 輸入
    // Key輸入4的檔案名稱
    printf("Please enter Input4 file name:");
    scanf("%s", &input4file);
    sprintf(input4path, "./%s.bmp", input4file);
    fp_in4 = fopen(input4path, "rb");

    if (!fp_in4)
    {
        printf("Input bmp file is not open successfully.\n");
    }
    else
    {
        printf("Input is successfully!\n");
    }
    // 讀出寬度和高度以及位元深度 Input1:24bits Input2:32bits
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_in4);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_in4);

    Width = infoHeader.biWidth;
    Height = infoHeader.biHeight;
    BitDepth = infoHeader.biBitCount;
    TotalPixel = Width * Height;

    printf("Width:%d\n", Width);
    printf("Height:%d\n", Height);
    printf("BitDepth:%d\n", BitDepth);
    printf("TotalPixel:%d\n", TotalPixel);

    if (method == 3)
    {
        printf("Combine factor:");
        scanf("%f", &combine_factor);
        average.R_avg = 0;
        average.G_avg = 0;
        average.B_avg = 0;
        maximum.R_max = 0;
        maximum.G_max = 0;
        maximum.B_max = 0;
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                // 把讀到的rgb放入記憶體
                fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in4);
                rgb_Vector[i][j].rgbtRed = rgb.rgbtRed;
                rgb_Vector[i][j].rgbtGreen = rgb.rgbtGreen;
                rgb_Vector[i][j].rgbtBlue = rgb.rgbtBlue;
                maximum.R_max = (rgb.rgbtRed > maximum.R_max) ? rgb.rgbtRed : maximum.R_max;
                maximum.G_max = (rgb.rgbtGreen > maximum.G_max) ? rgb.rgbtGreen : maximum.G_max;
                maximum.B_max = (rgb.rgbtBlue > maximum.B_max) ? rgb.rgbtBlue : maximum.B_max;
                average.R_avg = average.R_avg + rgb.rgbtRed;
                average.G_avg = average.G_avg + rgb.rgbtGreen;
                average.B_avg = average.B_avg + rgb.rgbtBlue;
            }
        }

        average.R_avg = average.R_avg / TotalPixel;
        average.G_avg = average.G_avg / TotalPixel;
        average.B_avg = average.B_avg / TotalPixel;
        RGB_avg_avg = (average.R_avg + average.G_avg + average.B_avg) / 3;

        printf("R_max:%d G_max:%d B_max:%d\n", maximum.R_max, maximum.G_max, maximum.B_max);
        printf("RGB_avg_avg:%f\n", RGB_avg_avg);

        fclose(fp_in4);
        // 處理過程

        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {

                combine_Grey[0] = (int)(rgb_Vector[i][j].rgbtRed * RGB_avg_avg / average.R_avg);
                combine_Grey[1] = (int)(rgb_Vector[i][j].rgbtGreen * RGB_avg_avg / average.G_avg);
                combine_Grey[2] = (int)(rgb_Vector[i][j].rgbtBlue * RGB_avg_avg / average.B_avg);
                combine_Grey[0] = (combine_Grey[0] > 255) ? 255 : combine_Grey[0];
                combine_Grey[1] = (combine_Grey[1] > 255) ? 255 : combine_Grey[1];
                combine_Grey[2] = (combine_Grey[2] > 255) ? 255 : combine_Grey[2];

                rgb_Vector[i][j].rgbtRed = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtRed / maximum.R_max * 255) + combine_factor * combine_Grey[0];
                rgb_Vector[i][j].rgbtGreen = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtGreen / maximum.G_max * 255) + combine_factor * combine_Grey[1];
                rgb_Vector[i][j].rgbtBlue = (1 - combine_factor) * (int)((float)rgb_Vector[i][j].rgbtBlue / maximum.B_max * 255) + combine_factor * combine_Grey[2];
            }
        }
    }

    // 輸出
    // Key輸出4的檔案名稱
    printf("Please enter Output4 file name:");
    scanf("%s", &output4file);
    sprintf(output4path, "./%s.bmp", output4file);
    fp_out4 = fopen(output4path, "wb");

    if (!fp_out4)
    {
        printf("Output bmp file is not open successfully.\n");
    }
    else
    {
        printf("Output is successfully!\n");
    }
    // 要先寫Header不然圖片會開不起來
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out4);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out4);
    for (i = 0; i < Height; i++)
    {
        for (j = 0; j < Width; j++)
        {
            rgb.rgbtRed = rgb_Vector[i][j].rgbtRed;
            rgb.rgbtGreen = rgb_Vector[i][j].rgbtGreen;
            rgb.rgbtBlue = rgb_Vector[i][j].rgbtBlue;

            fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out4); // 將 color matrix 寫入輸出圖檔
        }
    }
    fclose(fp_out4);

    return 0;
}