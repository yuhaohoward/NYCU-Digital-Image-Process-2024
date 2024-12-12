#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

typedef struct
{
    unsigned int rgb_Blue;
    unsigned int rgb_Green;
    unsigned int rgb_Red;
} RGB_32;

typedef struct
{
    float H;
    float S;
    float V;
} HSV;

// 把RGB轉成YCbCr的函數

int i, j, k;

unsigned int Width = 0;
unsigned int Height = 0;
unsigned int BitDepth = 0;
unsigned int TotalPixel = 0;

int method;
float combine_factor;

double findmax(double a, double b, double c)
{

    if (a >= b && a >= c)
    {
        return a;
    }
    else if (b >= a && b >= c)
    {
        return b;
    }
    else
    {
        return c;
    }
}
double findmin(double a, double b, double c)
{

    if (a <= b && a <= c)
    {
        return a;
    }
    else if (b <= a && b <= c)
    {
        return b;
    }
    else
    {
        return c;
    }
}

HSV RGBtoHSV(RGB_32 input)
{
    float R = (float)input.rgb_Red;
    float G = (float)input.rgb_Green;
    float B = (float)input.rgb_Blue;

    R = R / 255;
    G = G / 255;
    B = B / 255;

    float Cmax = findmax(R, G, B);
    float Cmin = findmin(R, G, B);
    float delta = Cmax - Cmin;

    // printf("%f ",R);
    // printf("%f ",G);
    // printf("%f ",B);
    // printf("%f ",Cmax);
    // printf("%f\n",Cmin);
    // printf("%f",delta);

    HSV output;

    // 算H
    if (delta == 0)
    {
        output.H = 0;
    }
    else if (Cmax == R)
    {
        if (G >= B)
        {
            output.H = 60 * (G - B) / delta;
        }
        else
        {
            output.H = 60 * (G - B) / delta + 360;
        }
    }
    else if (Cmax == G)
    {
        output.H = 60 * ((B - R) / delta + 2);
        // if (output.H < 0)
        // {
        //     output.H = output.H + 360;
        // }
    }
    else if (Cmax == B)
    {
        output.H = 60 * ((R - G) / delta + 4);
        // if (output.H < 0)
        // {
        //     output.H = output.H + 360;
        // }
    }
    // 算S
    if (Cmax == 0)
    {
        output.S = 0;
    }
    else
    {
        output.S = delta / Cmax;
    }
    // 算V
    output.V = Cmax;

    return output;
}

RGB_32 HSVtoRGB(HSV input)
{

    float H = input.H;
    float S = input.S;
    float V = input.V;

    float hi = floor(H / 60);
    float f = H / 60 - hi;
    float p = V * (1 - S);
    float q = V * (1 - f * S);
    float t = V * (1 - (1 - f) * S);

    // printf("%f\n",C);
    // printf("%f\n",X);
    // printf("%f\n",m);

    float R;
    float G;
    float B;

    RGB_32 output;
    if (H < 60 && H >= 0)
    {
        R = V;
        G = t;
        B = p;
    }
    else if (H < 120 && H >= 60)
    {
        R = q;
        G = V;
        B = p;
    }
    else if (H < 180 && H >= 120)
    {
        R = p;
        G = V;
        B = t;
    }
    else if (H < 240 && H >= 180)
    {
        R = p;
        G = q;
        B = V;
    }
    else if (H < 300 && H >= 240)
    {
        R = t;
        G = p;
        B = V;
    }
    else if (H < 360 && H >= 300)
    {
        R = V;
        G = p;
        B = q;
    }

    // printf("%f\n",R);
    // printf("%f\n",G);
    // printf("%f\n",B);
    // printf("%f %f %f\n",R,G,B);

    output.rgb_Red = (int)(R * 255);
    output.rgb_Green = (int)(G * 255);
    output.rgb_Blue = (int)(B * 255);

    // printf("%d %d %d\n",output.rgb_Red,output.rgb_Green,output.rgb_Blue);

    return output;
}

int warm, cool;

int main()
{

    FILE *fp_in1;
    FILE *fp_out1;
    FILE *fp_out2;

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    char input1file[20];
    char output1file[20];
    char output2file[20];
    char input1path[30];
    char output1path[30];
    char output2path[30];

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
    RGBTRIPLE(*rgb_Vector)
    [Width] = calloc(Height, Width * sizeof(RGBTRIPLE)); // 創造一個放原始像素的記憶體

    // 處理過程
    for (i = 0; i < Height; i++)
    {
        for (j = 0; j < Width; j++)
        {
            // 把讀到的rgb放入記憶體
            fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in1);
            rgb_Vector[i][j].rgbtRed = rgb.rgbtRed;
            rgb_Vector[i][j].rgbtGreen = rgb.rgbtGreen;
            rgb_Vector[i][j].rgbtBlue = rgb.rgbtBlue;
        }
    }
    fclose(fp_in1);

    RGB_32 RGB_origin;
    HSV HSV_new;
    RGB_32 RGB_final;

    // 輸出
    // Key輸出1的檔案名稱
    printf("Please enter Output1 file name:");
    scanf("%s", &output1file);
    sprintf(output1path, "./%s.bmp", output1file);
    fp_out1 = fopen(output1path, "wb");
    printf("Please enter Warm value:");
    scanf("%d", &warm);

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
            RGB_origin.rgb_Red = rgb_Vector[i][j].rgbtRed;
            RGB_origin.rgb_Green = rgb_Vector[i][j].rgbtGreen;
            RGB_origin.rgb_Blue = rgb_Vector[i][j].rgbtBlue;

            // // 飽和度
            // HSV_new = RGBtoHSV(RGB_origin);
            // HSV_new.H=warm;
            // HSV_new.S=1;
            // HSV_new.V=1;
            // RGB_final=HSVtoRGB(HSV_new);

            if (RGB_origin.rgb_Red + warm >= 255)
            {
                rgb.rgbtRed = 255;
            }
            else
            {
                rgb.rgbtRed = RGB_origin.rgb_Red + warm;
            }
            rgb.rgbtGreen = RGB_origin.rgb_Green;
            rgb.rgbtBlue = RGB_origin.rgb_Blue;
            fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out1); // 將 color matrix 寫入輸出圖檔
        }
    }
    fclose(fp_out1);

    // Key輸出2的檔案名稱
    printf("Please enter Output2 file name:");
    scanf("%s", &output2file);
    sprintf(output2path, "./%s.bmp", output2file);
    fp_out2 = fopen(output2path, "wb");
    printf("Please enter Cool value:");
    scanf("%d", &cool);

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
            RGB_origin.rgb_Red = rgb_Vector[i][j].rgbtRed;
            RGB_origin.rgb_Green = rgb_Vector[i][j].rgbtGreen;
            RGB_origin.rgb_Blue = rgb_Vector[i][j].rgbtBlue;

            // HSV_new = RGBtoHSV(RGB_origin);
            // HSV_new.H=cool;
            // HSV_new.S=1;
            // HSV_new.V=1;
            // RGB_final=HSVtoRGB(HSV_new);

            rgb.rgbtRed = RGB_origin.rgb_Red;
            rgb.rgbtGreen = RGB_origin.rgb_Green;
            if (RGB_origin.rgb_Blue + cool >= 255)
            {
                rgb.rgbtBlue = 255;
            }
            else
            {
                rgb.rgbtBlue = RGB_origin.rgb_Blue + cool;
            }
            fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out2); // 將 color matrix 寫入輸出圖檔
        }
    }
    fclose(fp_out2);
}