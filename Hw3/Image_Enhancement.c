#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

typedef struct
{
    unsigned int rgb_Blue;
    unsigned int rgb_Green;
    unsigned int rgb_Red;
} RGB_32; // 多了Alpha透明度,所以變成4Bytes
// typedef struct
// {
//     int R_max;
//     int G_max;
//     int B_max;
// } RGB_max;
// typedef struct
// {
//     double R_avg;
//     double G_avg;
//     double B_avg;
// } RGB_avg;

typedef struct
{
    float Y;
    float CB;
    float CR;
} YCBCR;

typedef struct
{
    float H;
    float S;
    float V;
} HSV;

// 把RGB轉成YCbCr的函數

YCBCR RGBtoYCbCr(RGB_32 input)
{
    float R = input.rgb_Red;
    float G = input.rgb_Green;
    float B = input.rgb_Blue;

    YCBCR output;
    output.Y = 0.257 * R + 0.504 * G + 0.098 * B + 16;
    output.CB = -0.148 * R - 0.291 * G + 0.439 * B + 128;
    output.CR = 0.439 * R - 0.368 * G - 0.071 * B + 128;

    output.Y = (output.Y > 235) ? 235 : (output.Y < 16) ? 16
                                                        : output.Y;
    output.CB = (output.CB > 240) ? 240 : (output.CB < 16) ? 16
                                                           : output.CB;
    output.CR = (output.CR > 240) ? 240 : (output.CR < 16) ? 16
                                                           : output.CR;

    return output;
}

// 把YCbCr轉成RGB的函數

RGB_32 YCbCrtoRGB(YCBCR input)
{

    RGB_32 output;

    float R = 1.164 * (input.Y - 16) - 0.002 * (input.CB - 128) + 1.596 * (input.CR - 128);
    float G = 1.164 * (input.Y - 16) - 0.391 * (input.CB - 128) - 0.813 * (input.CR - 128);
    float B = 1.164 * (input.Y - 16) + 2.018 * (input.CB - 128) - 0.001 * (input.CR - 128);

    output.rgb_Red = (int)(R < 0 ? 0 : R > 255 ? 255
                                               : R);
    output.rgb_Green = (int)(G < 0 ? 0 : G > 255 ? 255
                                                 : G);
    output.rgb_Blue = (int)(B < 0 ? 0 : B > 255 ? 255
                                                : B);

    return output;
}

int i, j, k;

unsigned int Width = 0;
unsigned int Height = 0;
unsigned int BitDepth = 0;
unsigned int TotalPixel = 0;

int method;
float combine_factor;

// Filter;
int Sobel_h[3][3] = {{-1, 0, 1}, {-2, 2, 2}, {-1, 0, 1}};
int Sobel_v[3][3] = {{-1, -1, -1}, {0, 1, 0}, {1, 1, 1}};
int Laplacian[3][3] = {{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}};

float r;

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
    float G =  (float)input.rgb_Green;
    float B = ( float)input.rgb_Blue;

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
        if(G>=B){
            output.H = 60 * (G - B)/delta;
        }
        else {
            output.H = 60 * (G - B)/delta+360;
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

    float hi =floor(H/60);
    float f = H/60-hi;
    float p = V*(1-S);
    float q = V*(1-f*S);
    float t = V*(1-(1-f)*S);


    // printf("%f\n",C);
    // printf("%f\n",X);
    // printf("%f\n",m);

    float R;
    float G;
    float B;


    RGB_32 output;
    if (H < 60 && H >= 0)
    {
        R= V;
        G= t;
        B= p;
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
        B= q;
    }

    // printf("%f\n",R);
    // printf("%f\n",G);
    // printf("%f\n",B);
    // printf("%f %f %f\n",R,G,B);

    output.rgb_Red = (int)(R*255);
    output.rgb_Green = (int)(G*255);
    output.rgb_Blue = (int)(B*255);

    // printf("%d %d %d\n",output.rgb_Red,output.rgb_Green,output.rgb_Blue);

    return output;
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
    // 轉到YCbCr
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

    YCBCR ycbcr;
    RGB_32 RGB_origin;
    RGB_32 RGB_new;
    float(*Y_origin)
        [Width] = calloc(Height, (Width) * sizeof(float)); // 創造一個放原始像素的記憶體
    int(*CB_origin)
        [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體
    int(*CR_origin)
        [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體

    for (i = 0; i < Height; i++)
    {
        for (j = 0; j < Width; j++)
        {
            RGB_origin.rgb_Red = rgb_Vector[i][j].rgbtRed;
            RGB_origin.rgb_Green = rgb_Vector[i][j].rgbtGreen;
            RGB_origin.rgb_Blue = rgb_Vector[i][j].rgbtBlue;

            ycbcr = RGBtoYCbCr(RGB_origin);
            Y_origin[i][j] = (int)ycbcr.Y;
            CB_origin[i][j] = (int)ycbcr.CB;
            CR_origin[i][j] = (int)ycbcr.CR;
        }
    }

    int(*Y_new)
        [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體

    for (i = 0; i < Height; i++)
    {
        Y_new[i][0] = Y_origin[i][0];
        Y_new[i][Width - 1] = Y_origin[i][Width - 1];
    }
    for (j = 0; j < Width; j++)
    {
        Y_new[0][j] = Y_origin[0][j];
        Y_new[Height - 1][j] = Y_origin[Height - 1][j];
    }

    // 銳化
    // Sobel_v
    for (i = 1; i < Height - 1; i++)
    {
        for (j = 1; j < Width - 1; j++)
        {
            Y_new[i][j] = Sobel_v[0][0] * Y_origin[i - 1][j - 1] + Sobel_v[0][1] * Y_origin[i - 1][j] + Sobel_v[0][2] * Y_origin[i - 1][j + 1] +
                          Sobel_v[1][0] * Y_origin[i][j - 1] + Sobel_v[1][1] * Y_origin[i][j] + Sobel_v[1][2] * Y_origin[i][j + 1] +
                          Sobel_v[2][0] * Y_origin[i + 1][j - 1] + Sobel_v[2][1] * Y_origin[i + 1][j] + Sobel_v[2][2] * Y_origin[i + 1][j + 1];
            Y_new[i][j] = (Y_new[i][j] > 255) ? 255 : (Y_new[i][j] < 0 ? 0 : Y_new[i][j]);
        }
    }

    // 亮度
    printf("Please enter Gamma value:");
    scanf("%f", &r);
    for (i = 0; i < Height; i++)
    {
        for (j = 0; j < Width; j++)
        {
            Y_origin[i][j] = pow(Y_origin[i][j] / 255, (double)r) * 255;
        }
    }

    HSV HSV_new;
    RGB_32 RGB_final;

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

            ycbcr.Y = 0.7*Y_origin[i][j]+0.3*Y_new[i][j];//調整銳化比例
            ycbcr.CB = CB_origin[i][j];
            ycbcr.CR = CR_origin[i][j];

            RGB_new = YCbCrtoRGB(ycbcr);
            
            // 飽和度
            HSV_new = RGBtoHSV(RGB_new);
            HSV_new.S=HSV_new.S*1.5;//調整飽和度
            HSV_new.S=(HSV_new.S>1)?1:(HSV_new.S<0)?0:HSV_new.S;
            // printf("%f %f %f\n",HSV_new.H,HSV_new.S,HSV_new.V);
            RGB_final=HSVtoRGB(HSV_new);
            // printf("%d %d %d\n",RGB_final.rgb_Red,RGB_final.rgb_Green,RGB_final.rgb_Blue);

            rgb.rgbtRed = RGB_final.rgb_Red;
            rgb.rgbtGreen = RGB_final.rgb_Green;
            rgb.rgbtBlue = RGB_final.rgb_Blue;
            fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out1); // 將 color matrix 寫入輸出圖檔
        }
    }
    fclose(fp_out1);
}