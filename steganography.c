#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#pragma pack(1)

typedef uint8_t BYTE;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint16_t WORD;

typedef struct
{
    BYTE ftype1; // B for bmp file
    BYTE ftype2; // M for bmp file
    DWORD fsize;
    WORD garbo1;
    WORD garbo2;
    DWORD offset; // location of first pixel
} BMPheader;

typedef struct
{
    DWORD hsize;
    LONG w; // number of pixels in a row
    LONG h; // number of pixels in a column
    WORD Cplanes;
    WORD bitcount;
    DWORD comp; // compression (0 for bmp files)
    DWORD hsizeImage;
    LONG hdensity;
    LONG vdensity;
    DWORD clrused;
    DWORD clrimp;
} INFOheader;

typedef struct
{
    BYTE b; // blue component of pixel
    BYTE g; // green component of pixel
    BYTE r; // red component of pixel
} RGBarr;

int main(void)
{
    int opt;
    printf("Press 1 for encryption\nPress 2 for decryption\n");
    scanf("%d", &opt);

    if (opt == 1)
    {
        char infile[50] = "testin.bmp";   // input image
        char outfile[50] = "testout.bmp"; // output image

        printf("Enter Image to encrypt\n");
        scanf("%s", infile);

        printf("Enter output image name:\n");
        scanf("%s", outfile);

        FILE *inptr = fopen(infile, "rb");

        if (inptr == NULL)
        {
            printf("Could not open image.\n");
            return 2;
        }

        FILE *outptr = fopen(outfile, "wb");
        if (outptr == NULL)
        {
            fclose(inptr);
            fprintf(stderr, "Could not create %s.\n", outfile);
            return 3;
        }

        char msg[640];
        printf("enter word to encrypt:\n");
        fflush(stdin);
        scanf("%[^\n]s", msg);

        BMPheader bmp;
        fread(&bmp, sizeof(BMPheader), 1, inptr);

        INFOheader bmpinfo;
        fread(&bmpinfo, sizeof(INFOheader), 1, inptr);

        fwrite(&bmp, sizeof(BMPheader), 1, outptr);

        fwrite(&bmpinfo, sizeof(INFOheader), 1, outptr);

        unsigned int total_pxl = bmpinfo.w * bmpinfo.h;

        int gap = bmpinfo.w / strlen(msg);

        BYTE pxl[total_pxl][3];

        for (int i = 0; i < total_pxl; i++)
        {
            RGBarr rgb;
            fread(&rgb, sizeof(RGBarr), 1, inptr);
            pxl[i][0] = rgb.b;
            pxl[i][1] = rgb.g;
            pxl[i][2] = rgb.r;
        }

        printf("Do you want to convert to black and white? (y/n)");
        char bw;
        fflush(stdin);
        scanf("%c", &bw);
        if (bw == 'y')
        {
            for (int i = 0; i < total_pxl; i++)
            {
                int avg = (pxl[i][0] + pxl[i][1] + pxl[i][2]) / 3;
                pxl[i][0] = avg;
                pxl[i][1] = avg;
                pxl[i][2] = avg;
            }
        }

        pxl[0][0] = strlen(msg);

        for (int i = 1; i <= bmpinfo.w; i = i + gap)
        {
            pxl[i][0] = 255;
            pxl[i][1] = 255;
            pxl[i][2] = 255;
        }

        for (int i = 1, j = 1; i <= bmpinfo.w; i = i + gap, j++)
        {
            pxl[i][0] = msg[j - 1];
            pxl[i][1] = msg[j - 1];
            pxl[i][2] = msg[j - 1];
            // printf("%d %c\n",pxl[i][0],pxl[i][0]);
        }

        fwrite(&pxl, sizeof(pxl), 1, outptr);
        printf("FILE SUCCESSFULLY ENCRYPTED\n");

        fclose(inptr);
        fclose(outptr);
    }

    else
    {

        char infile[50] = "testout.bmp"; // input image

        printf("enter image containing secret msg to decrypt:\n");
        scanf("%s", infile);

        FILE *inptr = fopen(infile, "rb");

        if (inptr == NULL)
        {
            printf("Could not open image.\n");
            return 2;
        }

        printf("FILE SUCCESSFULLY DECRYPTED, THE MESSAGE IS:\n");

        BMPheader bmp;
        fread(&bmp, sizeof(BMPheader), 1, inptr);

        INFOheader bmpinfo;
        fread(&bmpinfo, sizeof(INFOheader), 1, inptr);

        unsigned int total_pxl = bmpinfo.w * bmpinfo.h;

        BYTE pxl[total_pxl][3];

        for (int i = 0; i < total_pxl; i++)
        {
            RGBarr rgb;
            fread(&rgb, sizeof(RGBarr), 1, inptr);
            pxl[i][0] = rgb.b;
            pxl[i][1] = rgb.g;
            pxl[i][2] = rgb.r;
        }

        int gap = bmpinfo.w / pxl[0][0];

        for (int i = 1; i <= bmpinfo.w; i = i + gap)
        {
            printf("%c", pxl[i][0]);
        }

        fclose(inptr);
    }

    return 0;
}