/**
        c语言读取位图信息
**/


#include <QtCore/QCoreApplication>
#include<stdio.h>
#include<malloc.h>
#include <tchar.h>
#include <stdint.h>
#include <vector>
#include <windows.h>
#include <algorithm>
#include <memory>
#include <direct.h>

#define BM 19778 // 位图的标志

#define TEST_FILE_NAME "test1080p_1.bmp"  //打开的文件路径

#define OUTPUT_FILE_R "output_r14g14b14_buffer0_1.hex"
#define OUTPUT_FILE_G "output_r14g14b14_buffer1_1.hex"
#define OUTPUT_FILE_B "output_r14g14b14_buffer2_1.hex"

#define NEW_FILE_NAME "new_pic.bmp"

long width = 0;
long heigth = 0;
unsigned short depth = 0;

//判断是否是位图,在0-1字节
int IsBitMap(FILE *fp)
{
    unsigned short s;
    fread(&s,1,2,fp);
    if(s==BM)
        return 1;
    else
        return 0;
}

//获得图片的宽度,在18-21字节
long getWidth(FILE *fp)
{
    long width;
    fseek(fp,18,SEEK_SET);
    fread(&width,1,4,fp);
    return width;
}

//获得图片的高度 ，在22-25字节
long getHeigth(FILE *fp)
{
    long height;
    fseek(fp,22,SEEK_SET);
    fread(&height,1,4,fp);
    return height;
}

//获得每个像素的位数,在28-29字节
unsigned short getBit(FILE *fp)
{
    unsigned short bit;
    fseek(fp,28,SEEK_SET);
    fread(&bit,1,2,fp);
    return bit;
}

//获得数据的起始位置
unsigned int getOffSet(FILE *fp)
{
        unsigned int OffSet;
        fseek(fp,10L,SEEK_SET);
        fread(&OffSet,1,4,fp);
        return OffSet;
}

//获得数据的起始位置
u_int64 getFielSize(FILE *fp)
{
        u_int64 fSize = 0;
        fseek(fp,0L,/*SEEK_END*/2); /* 定位到文件末尾 */
        fSize=ftell(fp);
        fseek(fp,0L, 0);
        return fSize;
}

//获得图像的数据
void getData(FILE* fp)
{
    FILE* fpr;
    FILE* fpg;
    FILE* fpb;

    int i =0;
    int j=0;
    uint32_t stride;
    uint32_t seek = 0;
    uint32_t br = 0;
    uint32_t bg = 0;
    uint32_t bb = 0;

    unsigned char* pix=NULL;
    uint32_t union_pixr = 0;
    uint32_t union_pixg = 0;
    uint32_t union_pixb = 0;
    uint32_t clo_bytes = 0;

    //写入文件
    fpr=fopen(OUTPUT_FILE_R, "wb+");
    fpg=fopen(OUTPUT_FILE_G, "wb+");
    fpb=fopen(OUTPUT_FILE_B, "wb+");

    fprintf(fpr,"%s\r","@230000");
    fprintf(fpg,"%s\r","@240000");
    fprintf(fpb,"%s\r","@250000");

    fseek(fpr, 0, 2);
    fseek(fpg, 0, 2);
    fseek(fpb, 0, 2);

    //fseek(fp, getOffSet(fp), SEEK_SET);	//找到位图的数据区

    stride=(24*width+31)/8;	//对齐,计算一个width有多少个8位
    stride=stride/4*4;		//取四的倍数 r,g,b,alph

    //fseek(fp, 0, 0);
    //printf("stride = %d\n", stride);

#if 1 // add creat file
    uint32_t headersSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    uint32_t pixelDataSize = heigth * ((width * (depth / 8)));
    printf("pixelDataSize = %d\n", pixelDataSize);

    BITMAPFILEHEADER bmpFileHeader = {};
    bmpFileHeader.bfType = 0x4D42;
    bmpFileHeader.bfOffBits = headersSize;
    bmpFileHeader.bfSize = headersSize + pixelDataSize;

    BITMAPINFOHEADER bmpInfoHeader = {};
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfoHeader.biBitCount = depth;
    bmpInfoHeader.biClrImportant = 0;
    bmpInfoHeader.biClrUsed = 0;
    bmpInfoHeader.biCompression = BI_RGB;
    bmpInfoHeader.biHeight = heigth;
    bmpInfoHeader.biWidth = width;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biSizeImage = pixelDataSize;

    FILE* hFile = fopen(NEW_FILE_NAME, "wb+");
    if (!hFile)
            return;

    fwrite(&bmpFileHeader, 1, sizeof(bmpFileHeader), hFile);
    fwrite(&bmpInfoHeader, 1, sizeof(bmpInfoHeader), hFile);

#endif

    //写入数组
    pix=(unsigned char *)malloc(stride*heigth);

    printf("stride %d\n", stride);

    seek = getOffSet(fp);
    fseek(fp, seek, 0);

    memset(pix, 0, stride*heigth);

    fread(pix, sizeof(unsigned char), stride*heigth, fp);
    //fseek(fp, seek+stride*heigth/3, 0);
    //fread(pix, sizeof(unsigned char), stride*heigth/3, fp);
    //fseek(fp, seek+ stride*heigth/3+ stride*heigth/3, 0);
    //fread(pix, sizeof(unsigned char), stride*heigth/3, fp);

    fwrite(pix, 1, stride*heigth, hFile);
    fclose(hFile);

#if 0
    printf("\n read 1 pix: ");
    for(j = 0; j < 330; j++)
    {
        if(j%3 == 0)
        {
            printf("\n");
        }
        printf("%02x ", pix[j]);
    }
#endif

#if 1
    for(i = 0; i < heigth; i++)
    {
        clo_bytes = (heigth - i -1)* stride;
        //clo_bytes = 1* stride;
#if 0
        fseek(fp, seek+clo_bytes, 0);
        memset(pix, 0, stride);
        fread(pix, 1, stride, fp);
        printf("\n read 2 pix: ");

        for(j = 0; j < 330; j++)
        {
            if(j%3 == 0)
            {
                printf("\n");
            }
            printf("%02x ", pix[j]);
        }

        break;
 #endif

        for(j = 0; j < width; j += 2)
        {
            br =  clo_bytes + j * 3 + 0;
            bg =  clo_bytes + j * 3 + 1;
            bb =  clo_bytes + j * 3 + 2;

            union_pixr = (pix[br]<<16) + pix[br+3];
            union_pixg = (pix[bg]<<16) + pix[bg+3];
            union_pixb = (pix[bb]<<16) + pix[bb+3];

            if(i == 0)
            {
                if(j%10 == 0)
                {
                    printf("\n");
                }

                printf("%02x ", pix[br]);
                printf("%02x ", pix[bg]);
                printf("%02x ", pix[bb]);
                printf("%02x ", pix[br+3]);
                printf("%02x ", pix[bg+3]);
                printf("%02x ", pix[bb+3]);

                //printf("union_pixr = 0x%08x\n", union_pixr);
                //printf("pix[bg] = 0x%02x\n", pix[bg]);
                //printf("pix[bg+3] = 0x%02x\n", pix[bg+3]);
                //printf("union_pixg = 0x%08x\n", union_pixg);
                //printf("pix[bb+3] = 0x%02x\n", pix[bb+3]);
                //printf("union_pixb = 0x%08x\n", union_pixb);
                if(j == (width-2))
                {
                    printf("\n");
                }
            }

            fprintf(fpr,"%08x\r",union_pixr);
            fprintf(fpg,"%08x\r",union_pixg);
            fprintf(fpb,"%08x\r",union_pixb);
        }
    }
#endif

      free(pix);

      fclose(fpr);
      fclose(fpg);
      fclose(fpb);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FILE *fp=fopen(TEST_FILE_NAME,"rb");

    if(IsBitMap(fp))
        printf("Is bmp file!\n");
    else
    {
        printf("This file is not bmp file!\n");
        fclose(fp);
        return 0;
    }

    width = getWidth(fp);
    heigth = getHeigth(fp);
    printf("width=%ld\nheight=%ld\n", width, heigth);

    depth = getBit(fp);

    printf("this file depth is %d\n", depth);

    printf("file size = %ld\n", getFielSize(fp));

    printf("OffSet=%d\n",getOffSet(fp));

    getData(fp);

    printf("run finish!\n");

    return a.exec();
}

