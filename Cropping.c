#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

int i, j, k;
int x, y, w, h;

typedef struct
{
    unsigned char rgb_Blue;
    unsigned char rgb_Green;
    unsigned char rgb_Red;
    unsigned char rgb_Alpha;
} RGB_32; // 多了Alpha透明度,所以變成4Bytes

int main()
{
    FILE *fp_in;
    FILE *fp_out;

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    char inputfile[20];
    char outputfile[20];
    char inputpath[30];
    char outputpath[30];

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

    // Key輸出的bmp檔名

    printf("Please enter Output file name:");
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

    // 讀出寬度和高度以及位元深度 Input1:24bits Input2:32bits

    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_in);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_in);

    unsigned int Width = infoHeader.biWidth;
    unsigned int Height = infoHeader.biHeight;
    unsigned int BitDepth = infoHeader.biBitCount;

    printf("%d\n", Width);
    printf("%d\n", Height);
    printf("%d\n", BitDepth);

    // 輸入要裁切的範圍
    printf("x=");
    scanf("%d", &x);
    printf("y=");
    scanf("%d", &y);
    printf("w=");
    scanf("%d", &w);
    printf("h=");
    scanf("%d", &h);

    if (BitDepth != 32)
    {

        RGBTRIPLE rgb; // 要放從fp_in讀出來的rgb
        RGBTRIPLE(*rgb_Vector)
        [w] = calloc(h, w * sizeof(RGBTRIPLE)); // 創造一個放原始像素的記憶體

        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                // 把讀到的rgb放入記憶體
                fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in);
                if (i >= y && i < y + h && j >= x && j < x + w)
                {
                    rgb_Vector[i - y][j - x].rgbtBlue = rgb.rgbtBlue;
                    rgb_Vector[i - y][j - x].rgbtGreen = rgb.rgbtGreen;
                    rgb_Vector[i - y][j - x].rgbtRed = rgb.rgbtRed;
                }
            }
        }

        fclose(fp_in);

        infoHeader.biWidth = w;
        infoHeader.biHeight = h;

        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out);

        for (i = 0; i < h; i++)
        {

            for (j = 0; j < w; j++)
            {
                // 把剛剛存的像素從記憶體拿出來,並寫入fp_out
                rgb.rgbtBlue = rgb_Vector[i][j].rgbtBlue;
                rgb.rgbtGreen = rgb_Vector[i][j].rgbtGreen;
                rgb.rgbtRed = rgb_Vector[i][j].rgbtRed;
                fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out);
            }
        }

        fclose(fp_out);
    }
    else
    {
        RGB_32 rgba; // 要放從fp_in讀出來的rgb
        RGB_32(*rgba_Vector)
        [w] = calloc(h, w * sizeof(RGB_32)); // 創造一個放原始像素的記憶體

        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                // 把讀到的rgb放入記憶體
                fread(&rgba, sizeof(RGB_32), 1, fp_in);
                if (i >= y && i < y + h && j >= x && j < x + w)
                {
                    rgba_Vector[i - y][j - x].rgb_Blue = rgba.rgb_Blue;
                    rgba_Vector[i - y][j - x].rgb_Green = rgba.rgb_Green;
                    rgba_Vector[i - y][j - x].rgb_Red = rgba.rgb_Red;
                    rgba_Vector[i - y][j - x].rgb_Alpha = rgba.rgb_Alpha;
                }
            }
        }

        fclose(fp_in);

        infoHeader.biWidth = w;
        infoHeader.biHeight = h;

        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out);

        for (i = 0; i < h; i++)
        {

            for (j = 0; j < w; j++)
            {
                // 把剛剛存的像素從記憶體拿出來,並寫入fp_out
                rgba.rgb_Blue = rgba_Vector[i][j].rgb_Blue;
                rgba.rgb_Green = rgba_Vector[i][j].rgb_Green;
                rgba.rgb_Red = rgba_Vector[i][j].rgb_Red;
                fwrite(&rgba, sizeof(RGB_32), 1, fp_out);
            }
        }

        fclose(fp_out);
    }

    return 0;
}