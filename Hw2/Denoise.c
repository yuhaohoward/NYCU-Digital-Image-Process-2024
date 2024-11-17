#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

int i, j, k, l, m, n;

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
    // output.CR = (float)((-0.1687*R-0.3313*G+0.5000*B)) + 128;
    // output.CB = (float)((0.5000*R-0.4184*G-0.0816*B)) + 128;

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

    output.rgb_Red = (int)(R < 0 ? 0 : R > 255 ? 255
                                               : R);
    output.rgb_Green = (int)(G < 0 ? 0 : G > 255 ? 255
                                                 : G);
    output.rgb_Blue = (int)(B < 0 ? 0 : B > 255 ? 255
                                                : B);

    return output;
}

// Filter

float (*Filter_c1)[3]; // 宣告一個指向3x3陣列的指標
float Gaussian[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b); // 升序排列
}

int main()
{
    FILE *fp_in;
    FILE *fp_out;
    FILE *fp_out2;
    FILE *fp_Y_origin;
    FILE *fp_Y_new;
    FILE *fp_Y_new2;

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    char inputfile[20];
    char outputfile[20];
    char inputpath[30];
    char outputpath[30];
    char outputfile2[20];
    char outputpath2[30];

    int f1, f2, iter;
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

    printf("Please enter Filter1 number:");
    scanf("%d", &f1);

    printf("Please enter Filted times:");
    scanf("%d", &iter);
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

        // 第一張圖

        YCBCR ycbcr;
        RGB_32 RGB_origin;
        RGB_32 RGB_new;
        int(*Y_origin)
            [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體
        int(*CB_origin)
            [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體
        int(*CR_origin)
            [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體

        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out);

        fp_Y_origin = fopen("./Y_origin.txt", "w");
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
        // 列印
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                fprintf(fp_Y_origin, "%d    ", Y_origin[i][j]);
            }
            fprintf(fp_Y_origin, "\n");
        }

        fclose(fp_Y_origin);

        // Convolution
        fp_Y_new = fopen("./Y_new.txt", "w");

        int(*Y_new)
            [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體
        int(*CB_new)
            [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體
        int(*CR_new)
            [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體

        for (i = 0; i < Height; i++)
        {
            Y_new[i][0] = Y_origin[i][0];
            Y_new[i][Width - 1] = Y_origin[i][Width - 1];
            CB_new[i][0] = CB_origin[i][0];
            CB_new[i][Width - 1] = CB_origin[i][Width - 1];
            CR_new[i][0] = CR_origin[i][0];
            CR_new[i][Width - 1] = CR_origin[i][Width - 1];
        }
        for (j = 0; j < Width; j++)
        {
            Y_new[0][j] = Y_origin[0][j];
            Y_new[Height - 1][j] = Y_origin[Height - 1][j];
            CB_new[0][j] = CB_origin[0][j];
            CB_new[Height - 1][j] = CB_origin[Height - 1][j];
            CR_new[0][j] = CR_origin[0][j];
            CR_new[Height - 1][j] = CR_origin[Height - 1][j];
        }

        switch (f1)
        {
        case 1:
            Filter_c1 = Gaussian; //
            break;
            // case 2:
            //     Filter_c1 = Unsharp_Masking;
            //     break;
            // case 3:
            //     Filter_c1 = Gaussian;
            //     break;
            // case 4:
            //     Filter_c1 = Sobel_h;
            //     break;
            // case 5:
            //     Filter_c1 = Sobel_v;
            //     break;
        }

        for (k = 1; k <= iter; k++)
        {
            for (i = 1; i < Height - 1; i++)
            {
                for (j = 1; j < Width - 1; j++)
                {
                    Y_new[i][j] = (int)((Filter_c1[0][0] * Y_origin[i - 1][j - 1] + Filter_c1[0][1] * Y_origin[i - 1][j] + Filter_c1[0][2] * Y_origin[i - 1][j + 1] +
                                         Filter_c1[1][0] * Y_origin[i][j - 1] + Filter_c1[1][1] * Y_origin[i][j] + Filter_c1[1][2] * Y_origin[i][j + 1] +
                                         Filter_c1[2][0] * Y_origin[i + 1][j - 1] + Filter_c1[2][1] * Y_origin[i + 1][j] + Filter_c1[2][2] * Y_origin[i + 1][j + 1]) /
                                        16);
                    CB_new[i][j] = (int)((Filter_c1[0][0] * CB_origin[i - 1][j - 1] + Filter_c1[0][1] * CB_origin[i - 1][j] + Filter_c1[0][2] * CB_origin[i - 1][j + 1] +
                                          Filter_c1[1][0] * CB_origin[i][j - 1] + Filter_c1[1][1] * CB_origin[i][j] + Filter_c1[1][2] * CB_origin[i][j + 1] +
                                          Filter_c1[2][0] * CB_origin[i + 1][j - 1] + Filter_c1[2][1] * CB_origin[i + 1][j] + Filter_c1[2][2] * CB_origin[i + 1][j + 1]) /
                                         16);
                    CR_new[i][j] = (int)((Filter_c1[0][0] * CR_origin[i - 1][j - 1] + Filter_c1[0][1] * CR_origin[i - 1][j] + Filter_c1[0][2] * CR_origin[i - 1][j + 1] +
                                          Filter_c1[1][0] * CR_origin[i][j - 1] + Filter_c1[1][1] * CR_origin[i][j] + Filter_c1[1][2] * CR_origin[i][j + 1] +
                                          Filter_c1[2][0] * CR_origin[i + 1][j - 1] + Filter_c1[2][1] * CR_origin[i + 1][j] + Filter_c1[2][2] * CR_origin[i + 1][j + 1]) /
                                         16);

                    Y_new[i][j] = (Y_new[i][j] > 255) ? 255 : (Y_new[i][j] < 0 ? 0 : Y_new[i][j]);
                    CB_new[i][j] = (CB_new[i][j] > 255) ? 255 : (CB_new[i][j] < 0 ? 0 : CB_new[i][j]);
                    CR_new[i][j] = (CR_new[i][j] > 255) ? 255 : (CR_new[i][j] < 0 ? 0 : CR_new[i][j]);
                }
            }
            if (k > 1)
            {
                for (int i = 0; i < Height; i++)
                {
                    for (int j = 0; j < Width; j++)
                    {
                        Y_origin[i][j] = Y_new[i][j];
                        CB_origin[i][j] = CB_new[i][j];
                        CR_origin[i][j] = CR_new[i][j];
                    }
                }
            }
        }

        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                fprintf(fp_Y_new, "%d    ", Y_new[i][j]);
            }
            fprintf(fp_Y_new, "\n");
        }

        fclose(fp_Y_new);

        // 輸出
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                ycbcr.Y = Y_new[i][j];
                ycbcr.CB = CB_new[i][j];
                ycbcr.CR = CR_new[i][j];
                RGB_new = YCbCrtoRGB(ycbcr);
                rgb.rgbtRed = RGB_new.rgb_Red;
                rgb.rgbtGreen = RGB_new.rgb_Green;
                rgb.rgbtBlue = RGB_new.rgb_Blue;
                fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out); // 將 color matrix 寫入輸出圖檔
            }
        }
        fclose(fp_out);

        // 第二張圖

        // 要先寫Header不然圖片會開不起來
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_out2);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_out2);

        int(*Y_new2)
            [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體
        int(*CB_new2)
            [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體
        int(*CR_new2)
            [Width] = calloc(Height, (Width) * sizeof(int)); // 創造一個放原始像素的記憶體

        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                Y_new2[i][j] = Y_origin[i][j];
                CB_new2[i][j] = CB_origin[i][j];
                CR_new2[i][j] = CR_origin[i][j];
            }
        }

        fp_Y_new2 = fopen("./Y_new2.txt", "w");

        // Mask 3x3
        for (i = 0; i < Height - 2; i = i + 3)
        {
            for (j = 0; j < Width - 2; j = j + 3)
            {
                int Y_sort[] = {(int)(Y_new2[i][j]), (int)(Y_new2[i][j + 1]), (int)(Y_new2[i][j + 2]),
                                (int)(Y_new2[i + 1][j]), (int)(Y_new2[i + 1][j + 1]), (int)(Y_new2[i + 1][j + 2]),
                                (int)(Y_new2[i + 2][j]), (int)(Y_new2[i + 2][j + 1]), (int)(Y_new2[i + 2][j + 2])};

                int n1 = sizeof(Y_sort) / sizeof(Y_sort[0]);

                qsort(Y_sort, n1, sizeof(int), compare);

                for (k = 0; k <= 2; k++)
                {
                    for (l = 0; l <= 2; l++)
                    {
                        Y_new2[i + k][j + l] = Y_sort[4];
                    }
                }

                int CB_sort[] = {(int)(CB_new2[i][j]), (int)(CB_new2[i][j + 1]), (int)(CB_new2[i][j + 2]),
                                 (int)(CB_new2[i + 1][j]), (int)(CB_new2[i + 1][j + 1]), (int)(CB_new2[i + 1][j + 2]),
                                 (int)(CB_new2[i + 2][j]), (int)(CB_new2[i + 2][j + 1]), (int)(CB_new2[i + 2][j + 2])};
                int n2 = sizeof(CB_sort) / sizeof(CB_sort[0]);

                qsort(CB_sort, n2, sizeof(int), compare);

                for (k = 0; k <= 2; k++)
                {
                    for (l = 0; l <= 2; l++)
                    {
                        CB_new2[i + k][j + l] = CB_sort[4];
                    }
                }

                int CR_sort[] = {(int)(CR_new2[i][j]), (int)(CR_new2[i][j + 1]), (int)(CR_new2[i][j + 2]),
                                 (int)(CR_new2[i + 1][j]), (int)(CR_new2[i + 1][j + 1]), (int)(CR_new2[i + 1][j + 2]),
                                 (int)(CR_new2[i + 2][j]), (int)(CR_new2[i + 2][j + 1]), (int)(CR_new2[i + 2][j + 2])};
                int n3 = sizeof(CR_sort) / sizeof(CR_sort[0]);

                qsort(CR_sort, n3, sizeof(int), compare);

                for (k = 0; k <= 2; k++)
                {
                    for (l = 0; l <= 2; l++)
                    {
                        CR_new2[i + k][j + l] = CR_sort[4];
                    }
                }

                Y_new2[i][j] = (Y_new2[i][j] > 255) ? 255 : (Y_new2[i][j] < 0 ? 0 : Y_new2[i][j]);
                CB_new2[i][j] = (CB_new2[i][j] > 255) ? 255 : (CB_new2[i][j] < 0 ? 0 : CB_new2[i][j]);
                CR_new2[i][j] = (CR_new2[i][j] > 255) ? 255 : (CR_new2[i][j] < 0 ? 0 : CR_new2[i][j]);
            }
        }

        // //Mask 5x5

        // for (i = 0; i < Height-4; i=i+5)
        //         {
        //             for (j = 0; j < Width-4; j=j+5)
        //             {
        //                 int Y_sort[]={(int)(Y_new2[i][j]),(int)(Y_new2[i][j+1]),(int)(Y_new2[i][j+2]),(int)(Y_new2[i][j+3]),(int)(Y_new2[i][j+4]),
        //                               (int)(Y_new2[i+1][j]),(int)(Y_new2[i+1][j+1]),(int)(Y_new2[i+1][j+2]),(int)(Y_new2[i+1][j+3]),(int)(Y_new2[i+1][j+4]),
        //                               (int)(Y_new2[i+2][j]),(int)(Y_new2[i+2][j+1]),(int)(Y_new2[i+2][j+2]),(int)(Y_new2[i+2][j+3]),(int)(Y_new2[i+2][j+4]),
        //                               (int)(Y_new2[i+3][j]),(int)(Y_new2[i+3][j+1]),(int)(Y_new2[i+3][j+2]),(int)(Y_new2[i+3][j+3]),(int)(Y_new2[i+3][j+4]),
        //                               (int)(Y_new2[i+4][j]),(int)(Y_new2[i+4][j+1]),(int)(Y_new2[i+4][j+2]),(int)(Y_new2[i+4][j+3]),(int)(Y_new2[i+4][j+4])};

        //                 int n1 = sizeof(Y_sort) / sizeof(Y_sort[0]);

        //                 qsort(Y_sort, n1, sizeof(int), compare);

        //                 for (k=0;k<=4;k++)
        //                 {
        //                     for (l=0;l<=4;l++)
        //                         {
        //                             Y_new2[i+k][j+l] =Y_sort[13];
        //                         }
        //                 }

        //                 int CB_sort[]={(int)(CB_new2[i][j]),(int)(CB_new2[i][j+1]),(int)(CB_new2[i][j+2]),(int)(CB_new2[i][j+3]),(int)(CB_new2[i][j+4]),
        //                               (int)(CB_new2[i+1][j]),(int)(CB_new2[i+1][j+1]),(int)(CB_new2[i+1][j+2]),(int)(CB_new2[i+1][j+3]),(int)(CB_new2[i+1][j+4]),
        //                               (int)(CB_new2[i+2][j]),(int)(CB_new2[i+2][j+1]),(int)(CB_new2[i+2][j+2]),(int)(CB_new2[i+2][j+3]),(int)(CB_new2[i+2][j+4]),
        //                               (int)(CB_new2[i+3][j]),(int)(CB_new2[i+3][j+1]),(int)(CB_new2[i+3][j+2]),(int)(CB_new2[i+3][j+3]),(int)(CB_new2[i+3][j+4]),
        //                               (int)(CB_new2[i+4][j]),(int)(CB_new2[i+4][j+1]),(int)(CB_new2[i+4][j+2]),(int)(CB_new2[i+4][j+3]),(int)(CB_new2[i+4][j+4])};
        //                 int n2 = sizeof(CB_sort) / sizeof(CB_sort[0]);

        //                 qsort(CB_sort, n2, sizeof(int), compare);

        //                 for (k=0;k<=4;k++)
        //                 {
        //                     for (l=0;l<=4;l++)
        //                         {
        //                             CB_new2[i+k][j+l] =CB_sort[13];
        //                         }
        //                 }

        //                 int CR_sort[]={(int)(CR_new2[i][j]),(int)(CR_new2[i][j+1]),(int)(CR_new2[i][j+2]),(int)(CR_new2[i][j+3]),(int)(CR_new2[i][j+4]),
        //                               (int)(CR_new2[i+1][j]),(int)(CR_new2[i+1][j+1]),(int)(CR_new2[i+1][j+2]),(int)(CR_new2[i+1][j+3]),(int)(CR_new2[i+1][j+4]),
        //                               (int)(CR_new2[i+2][j]),(int)(CR_new2[i+2][j+1]),(int)(CR_new2[i+2][j+2]),(int)(CR_new2[i+2][j+3]),(int)(CR_new2[i+2][j+4]),
        //                               (int)(CR_new2[i+3][j]),(int)(CR_new2[i+3][j+1]),(int)(CR_new2[i+3][j+2]),(int)(CR_new2[i+3][j+3]),(int)(CR_new2[i+3][j+4]),
        //                               (int)(CR_new2[i+4][j]),(int)(CR_new2[i+4][j+1]),(int)(CR_new2[i+4][j+2]),(int)(CR_new2[i+4][j+3]),(int)(CR_new2[i+4][j+4])};
        //                 int n3 = sizeof(CR_sort) / sizeof(CR_sort[0]);

        //                 qsort(CR_sort, n3, sizeof(int), compare);

        //                 for (k=0;k<=4;k++)
        //                 {
        //                     for (l=0;l<=4;l++)
        //                         {
        //                             CR_new2[i+k][j+l] =CR_sort[13];
        //                         }
        //                 }

        //                 Y_new2[i][j] = (Y_new2[i][j] > 255) ? 255 : (Y_new2[i][j] < 0 ? 0 : Y_new2[i][j]);
        //                 CB_new2[i][j] = (CB_new2[i][j] > 255) ? 255 : (CB_new2[i][j] < 0 ? 0 : CB_new2[i][j]);
        //                 CR_new2[i][j] = (CR_new2[i][j] > 255) ? 255 : (CR_new2[i][j] < 0 ? 0 : CR_new2[i][j]);
        //             }
        //         }

        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                fprintf(fp_Y_new2, "%d    ", Y_new2[i][j]);
            }
            fprintf(fp_Y_new2, "\n");
        }

        fclose(fp_Y_new2);

        // 輸出
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                ycbcr.Y = Y_new2[i][j];
                ycbcr.CB = CB_new2[i][j];
                ycbcr.CR = CR_new2[i][j];
                RGB_new = YCbCrtoRGB(ycbcr);
                rgb.rgbtRed = RGB_new.rgb_Red;
                rgb.rgbtGreen = RGB_new.rgb_Green;
                rgb.rgbtBlue = RGB_new.rgb_Blue;
                fwrite(&rgb, sizeof(RGBTRIPLE), 1, fp_out2); // 將 color matrix 寫入輸出圖檔
            }
        }
        fclose(fp_out2);
    }

    return 0;
}