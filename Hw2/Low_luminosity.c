#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

int i, j, k;

typedef struct
{
    unsigned char rgb_Blue;
    unsigned char rgb_Green;
    unsigned char rgb_Red;
    unsigned char rgb_Alpha;
} RGB_32; // 多了Alpha透明度,所以變成4Bytes

typedef struct
{
    float Y;
    float CB;
    float CR;
} YCBCR;

// 把RGB轉成YCbCr的函數

YCBCR RGBtoYCbCr(RGB_32 input)
{
    float R = input.rgb_Red;
    float G = input.rgb_Green;
    float B = input.rgb_Blue;

    YCBCR output;
    output.Y = (float)(0.299 * R + 0.587 * G + 0.114 * B);
    output.CR = (R - output.Y) * 0.713 + 128;
    output.CB = (B - output.Y) * 0.564 + 128;
    return output;
}

// 把YCbCr轉成RGB的函數

RGB_32 YCbCrtoRGB(YCBCR input)
{

    RGB_32 output;
    // output.rgb_Red = input.Y + (int)(1.402 * (input.CR - 128));
    // output.rgb_Green = input.Y - (int)(0.714136 * (input.CR - 128) - 0.344136 * (input.CB - 128));
    // output.rgb_Blue = input.Y + (int)(1.772 * (input.CB - 128));
        
        
    int R = input.Y + (int)(1.402 * (input.CR - 128));
    int G = input.Y - (int)(0.714136 * (input.CR - 128) - 0.344136 * (input.CB - 128));
    int B = input.Y + (int)(1.772 * (input.CB - 128));


    output.rgb_Red= (int)(R < 0 ? 0 : R > 255 ? 255 : R);
    output.rgb_Green = (int)(G < 0 ? 0 : G > 255 ? 255 : G);
    output.rgb_Blue = (int)(B < 0 ? 0 : B > 255 ? 255 : B);

    return output;
}

int main()
{
    FILE *fp_in;
    FILE *fp_out;
    FILE *fp_out2;
    FILE *fp_RGB;
    FILE *fp_YCBCR;
    FILE *fp_Ycdf;
    FILE *fp_Ynew;
    FILE *fp_RGBnew;
    FILE *fp_Gamma;
    FILE *fp_RGBnew2;

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    char inputfile[20];
    char outputfile[20];
    char inputpath[30];
    char outputpath[30];
    char outputfile2[20];
    char outputpath2[30];

    float r; 
    // Key要翻轉的bmp檔名

    printf("Please enter Input file name:");
    scanf("%s", &inputfile);
    sprintf(inputpath, "./%s.bmp", inputfile);
    fp_in = fopen(inputpath, "rb");

    if (!fp_in)
    {
        printf("Input bmp file is not open successfully.\n");
    }
    else
    {
        printf("Input is successfully!\n");
    }


    printf("Please enter Gamma value:");
    scanf("%f", &r);

    // Key輸出的bmp檔名

    printf("Please enter Output1 file name:");
    scanf("%s", &outputfile);
    sprintf(outputpath, "./%s.bmp", outputfile);
    fp_out = fopen(outputpath, "wb");

    if (!fp_out)
    {
        printf("Output bmp file is not open successfully.\n");
    }
    else
    {
        printf("Output is successfully!\n");
    }

    printf("Please enter Output2 file name:");
    scanf("%s", &outputfile2);
    sprintf(outputpath2, "./%s.bmp", outputfile2);
    fp_out2 = fopen(outputpath2, "wb");

    if (!fp_out2)
    {
        printf("Output bmp file is not open successfully.\n");
    }
    else
    {
        printf("Output is successfully!\n");
    }

    

    // 讀出寬度和高度以及位元深度 Input1:24bits Input2:32bits

    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_in);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_in);

    unsigned int Width = infoHeader.biWidth;
    unsigned int Height = infoHeader.biHeight;
    unsigned int BitDepth = infoHeader.biBitCount;
    unsigned int TotalPixel = Width * Height;

    printf("%d\n", Width);
    printf("%d\n", Height);
    printf("%d\n", BitDepth);
    printf("%d\n", TotalPixel);

    if (BitDepth != 32)
    {

        RGBTRIPLE rgb; // 要放從fp_in讀出來的rgb
        RGBTRIPLE(*rgb_Vector)
        [Width] = calloc(Height, Width * sizeof(RGBTRIPLE)); // 創造一個放原始像素的記憶體

        for (i = 0; i < Height; i++)
        {

            for (j = 0; j < Width; j++)
            {

                // 把讀到的rgb放入記憶體
                fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in);

                rgb_Vector[i][j].rgbtBlue = rgb.rgbtBlue;
                rgb_Vector[i][j].rgbtGreen = rgb.rgbtGreen;
                rgb_Vector[i][j].rgbtRed = rgb.rgbtRed;
            }
        }

        fclose(fp_in);

        fp_RGB = fopen("./input_RGB.txt", "w");
        fprintf(fp_RGB, "R   G   B   \n");
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                fprintf(fp_RGB, "%d   %d   %d   \n", rgb_Vector[i][j].rgbtRed, rgb_Vector[i][j].rgbtGreen, rgb_Vector[i][j].rgbtBlue);
            }
        }
        fclose(fp_RGB);

        fp_YCBCR = fopen("./output_YCBCR.txt", "w");
        fp_Ycdf = fopen("./output_Ycdf.txt", "w");
        RGB_32 input;
        YCBCR ycbcr;
        unsigned int Y_hist[256];
        for (i = 0; i < 256; i++)
        {
            Y_hist[i] = 0;
        }

        fprintf(fp_YCBCR, "Y   CB   CR   \n");
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                input.rgb_Red = rgb_Vector[i][j].rgbtRed;
                input.rgb_Green = rgb_Vector[i][j].rgbtGreen;
                input.rgb_Blue = rgb_Vector[i][j].rgbtBlue;
                ycbcr = RGBtoYCbCr(input);
                fprintf(fp_YCBCR, "%f   %f   %f   \n", ycbcr.Y, ycbcr.CB, ycbcr.CR);
                Y_hist[(int)ycbcr.Y] = Y_hist[(int)ycbcr.Y] + 1;
            }
        }
        fclose(fp_YCBCR);

        double Y_pmf[256];
        double Y_cdf[256];
        for (i = 0; i < 256; i++)
        {
            Y_pmf[i] = 0;
            Y_cdf[i] = 0;
        }
        for (i = 0; i < 256; i++)
        {
            Y_pmf[i] = (double)Y_hist[i] / TotalPixel;
        }

        fprintf(fp_Ycdf, "%f\n", Y_cdf[0]);
        for (i = 1; i < 256; i++)
        {
            Y_cdf[i] = Y_cdf[i - 1] + Y_pmf[i - 1];
            fprintf(fp_Ycdf, "%f\n", Y_cdf[i]);
        }
        fclose(fp_Ycdf);

        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out);
        fp_Ynew = fopen("./output_Ynew.txt", "w");
        fp_RGBnew = fopen("./output_RGBnew.txt", "w");
        fprintf(fp_RGBnew, "R   G   B   \n");
        double Y_new[Height][Width];

        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                input.rgb_Red = rgb_Vector[i][j].rgbtRed;
                input.rgb_Green = rgb_Vector[i][j].rgbtGreen;
                input.rgb_Blue = rgb_Vector[i][j].rgbtBlue;
                ycbcr = RGBtoYCbCr(input);
                Y_new[i][j] = Y_cdf[(int)ycbcr.Y] * 255;
                ycbcr.Y = Y_new[i][j];
                fprintf(fp_Ynew, "%f\n", Y_new[i][j]);

                RGB_32 RGB_new = YCbCrtoRGB(ycbcr);

                rgb.rgbtRed = RGB_new.rgb_Red;
                rgb.rgbtGreen = RGB_new.rgb_Green;
                rgb.rgbtBlue = RGB_new.rgb_Blue;

                fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out); // 將 color matrix 寫入輸出圖檔
                fprintf(fp_RGBnew, "%d   %d   %d   \n", rgb.rgbtRed, rgb.rgbtGreen, rgb.rgbtBlue);
            }
        }
        fclose(fp_Ynew);
        fclose(fp_RGBnew);
        fclose(fp_out);


        //Gamma correction

        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out2);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out2);
        fp_Gamma = fopen("./output_Gamma.txt", "w");
        fp_RGBnew2 = fopen("./output_RGBnew2.txt", "w");
            for (i = 0; i < Height; i++)
            {
                for (j = 0; j < Width; j++)
                {
                    input.rgb_Red = rgb_Vector[i][j].rgbtRed;
                    input.rgb_Green = rgb_Vector[i][j].rgbtGreen;
                    input.rgb_Blue = rgb_Vector[i][j].rgbtBlue;
                    ycbcr = RGBtoYCbCr(input);
                    Y_new[i][j]=pow(ycbcr.Y/255,(double)r)*255;
                    ycbcr.Y=Y_new[i][j];
                    fprintf(fp_Gamma, "%f\n", ycbcr.Y);

                    RGB_32 RGB_new2 = YCbCrtoRGB(ycbcr);

                    rgb.rgbtRed = RGB_new2.rgb_Red;
                    rgb.rgbtGreen = RGB_new2.rgb_Green;
                    rgb.rgbtBlue = RGB_new2.rgb_Blue;

                    fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out2); // 將 color matrix 寫入輸出圖檔
                    fprintf(fp_RGBnew2, "%d   %d   %d   \n", rgb.rgbtRed, rgb.rgbtGreen, rgb.rgbtBlue);
                }
            }
        fclose(fp_Gamma);
        fclose(fp_RGBnew2);
        fclose(fp_out2);
    }



    return 0;
}