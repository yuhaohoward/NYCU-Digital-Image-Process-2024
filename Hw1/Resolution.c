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

int main()
{

    FILE *fp_in;
    FILE *fp_out1;
    FILE *fp_out2;
    FILE *fp_out3;

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    char inputfile[20];
    char outputfile1[20];
    char outputfile2[20];
    char outputfile3[20];

    char inputpath[30];
    char outputpath1[30];
    char outputpath2[30];
    char outputpath3[30];

    // Key要改變解析度的bmp檔名

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

    printf("Please enter Output file name(6bits):");
    scanf("%s", &outputfile1);
    sprintf(outputpath1, "./%s.bmp", outputfile1);
    fp_out1 = fopen(outputpath1, "wb");

    if (!fp_out1)
    {
        printf("Output bmp file (6bits) is not open successfully.\n");
    }
    else
    {
        printf("Output (6bits)is successfully!\n");
    }

    printf("Please enter Output file name(4bits):");
    scanf("%s", &outputfile2);
    sprintf(outputpath2, "./%s.bmp", outputfile2);
    fp_out2 = fopen(outputpath2, "wb");

    if (!fp_out2)
    {
        printf("Output bmp file (4bits) is not open successfully.\n");
    }
    else
    {
        printf("Output (4bits)is successfully!\n");
    }

    printf("Please enter Output file name(2bits):");
    scanf("%s", &outputfile3);
    sprintf(outputpath3, "./%s.bmp", outputfile3);
    fp_out3 = fopen(outputpath3, "wb");

    if (!fp_out3)
    {
        printf("Output bmp file (2bits) is not open successfully.\n");
    }
    else
    {
        printf("Output (2bits)is successfully!\n");
    }

    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_in);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_in);

    unsigned int Width = infoHeader.biWidth;
    unsigned int Height = infoHeader.biHeight;
    unsigned int BitDepth = infoHeader.biBitCount;

    printf("%d\n", Width);
    printf("%d\n", Height);
    printf("%d\n", BitDepth);

    int INT_Blue, INT_Green, INT_Red;

    if (BitDepth != 32)
    {

        RGBTRIPLE rgb;
        RGBTRIPLE(*rgb_Vector)
        [Width] = calloc(Height, Width * sizeof(RGBTRIPLE));


        //把fp_in讀進來
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                fread(&rgb, sizeof(RGBTRIPLE), 1, fp_in);
                rgb_Vector[i][j].rgbtBlue =rgb.rgbtBlue;
                rgb_Vector[i][j].rgbtGreen =rgb.rgbtGreen;
                rgb_Vector[i][j].rgbtRed =rgb.rgbtRed;
            }
        }
        fclose(fp_in);

        //量化為6Bits
        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out1);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out1);
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                INT_Blue = rgb_Vector[i][j].rgbtBlue / 4;
                INT_Green = rgb_Vector[i][j].rgbtGreen / 4;
                INT_Red = rgb_Vector[i][j].rgbtRed / 4;
                // 把剛剛存的像素從記憶體拿出來,並寫入fp_out
                rgb.rgbtBlue =INT_Blue*4;
                rgb.rgbtGreen = INT_Green*4;
                rgb.rgbtRed = INT_Red*4;
                fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out1);
            }
        }

        fclose(fp_out1);


        //量化為4Bits
        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out2);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out2);
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                INT_Blue = rgb_Vector[i][j].rgbtBlue / 16;
                INT_Green = rgb_Vector[i][j].rgbtGreen / 16;
                INT_Red = rgb_Vector[i][j].rgbtRed / 16;
                // 把剛剛存的像素從記憶體拿出來,並寫入fp_out
                rgb.rgbtBlue =INT_Blue*16;
                rgb.rgbtGreen = INT_Green*16;
                rgb.rgbtRed = INT_Red*16;
                fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out2);
            }
        }

        fclose(fp_out2);

        //量化為2Bits
        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out3);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out3);
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                INT_Blue = rgb_Vector[i][j].rgbtBlue / 64;
                INT_Green = rgb_Vector[i][j].rgbtGreen / 64;
                INT_Red = rgb_Vector[i][j].rgbtRed / 64;
                // 把剛剛存的像素從記憶體拿出來,並寫入fp_out
                rgb.rgbtBlue =INT_Blue*64;
                rgb.rgbtGreen = INT_Green*64;
                rgb.rgbtRed = INT_Red*64;
                fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out3);
            }
        }

        fclose(fp_out3);


    }
    else {

        RGB_32 rgba;
        RGB_32(*rgba_Vector)
        [Width] = calloc(Height, Width * sizeof(RGB_32));


        //把fp_in讀進來
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                fread(&rgba, sizeof(RGB_32), 1, fp_in);
                rgba_Vector[i][j].rgb_Blue =rgba.rgb_Blue;
                rgba_Vector[i][j].rgb_Green =rgba.rgb_Green;
                rgba_Vector[i][j].rgb_Red =rgba.rgb_Red;
                rgba_Vector[i][j].rgb_Alpha =rgba.rgb_Alpha;
            }
        }
        fclose(fp_in);

        //量化為6Bits
        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out1);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out1);
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                INT_Blue = rgba_Vector[i][j].rgb_Blue / 4;
                INT_Green = rgba_Vector[i][j].rgb_Green / 4;
                INT_Red = rgba_Vector[i][j].rgb_Red / 4;
                // 把剛剛存的像素從記憶體拿出來,並寫入fp_out
                rgba.rgb_Blue =INT_Blue*4;
                rgba.rgb_Green = INT_Green*4;
                rgba.rgb_Red = INT_Red*4;
                fwrite(&rgba, sizeof(RGB_32), 1, fp_out1);
            }
        }

        fclose(fp_out1);


        //量化為4Bits
        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out2);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out2);
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                INT_Blue = rgba_Vector[i][j].rgb_Blue / 16;
                INT_Green = rgba_Vector[i][j].rgb_Green / 16;
                INT_Red = rgba_Vector[i][j].rgb_Red / 16;
                // 把剛剛存的像素從記憶體拿出來,並寫入fp_out
                rgba.rgb_Blue =INT_Blue*16;
                rgba.rgb_Green = INT_Green*16;
                rgba.rgb_Red = INT_Red*16;
                fwrite(&rgba, sizeof(RGB_32), 1, fp_out2);
            }
        }

        fclose(fp_out2);


        //量化為2Bits
        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out3);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out3);
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                INT_Blue = rgba_Vector[i][j].rgb_Blue / 64;
                INT_Green = rgba_Vector[i][j].rgb_Green / 64;
                INT_Red = rgba_Vector[i][j].rgb_Red / 64;
                // 把剛剛存的像素從記憶體拿出來,並寫入fp_out
                rgba.rgb_Blue =INT_Blue*64;
                rgba.rgb_Green = INT_Green*64;
                rgba.rgb_Red = INT_Red*64;
                fwrite(&rgba, sizeof(RGB_32), 1, fp_out3);
            }
        }

        fclose(fp_out3);
    }

    return 0;
}