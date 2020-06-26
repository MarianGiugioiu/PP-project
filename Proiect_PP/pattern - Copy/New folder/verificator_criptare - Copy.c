#include <stdio.h>
#include <string.h>
#include <stdlib.h>
unsigned char *liniarizare_poza(char* nume_fisier_sursa,unsigned char header[54],unsigned int *dim_img,unsigned int *inaltime_img,unsigned int *latime_img,int *padding)
{
    FILE *fin = fopen(nume_fisier_sursa, "rb");

    fseek(fin, 2, SEEK_SET);
    fread(&*dim_img, sizeof(unsigned int), 1, fin);
    ///printf("Dimensiunea imaginii in octeti: %u\n", dim_img);

    fseek(fin, 18, SEEK_SET);
    fread(&*latime_img, sizeof(unsigned int), 1, fin);
    fread(&*inaltime_img, sizeof(unsigned int), 1, fin);
    ///printf("Dimensiunea imaginii in pixeli (latime x inaltime): %u x %u\n",latime_img, inaltime_img);

    ///calculam padding-ul pentru o linie
    *padding=0;
    if(*latime_img&3)
        *padding=4-(3*(*latime_img))&3;

    fseek(fin,0,SEEK_SET);
    fread(header, 54, 1, fin);
    unsigned char * Q;
    Q = (unsigned char*) calloc(*inaltime_img*(*latime_img)*3,sizeof(unsigned char));
    unsigned int i,j;
    unsigned char pRGB[3];
    for(i = 0; i < *inaltime_img; i++)
    {
        for(j = 0; j < *latime_img; j++)
        {
            fread(pRGB, 3, 1, fin);
            Q[(*inaltime_img-i-1)*(*latime_img)*3+j*3]   = pRGB[0];
            Q[(*inaltime_img-i-1)*(*latime_img)*3+j*3+1] = pRGB[1];
            Q[(*inaltime_img-i-1)*(*latime_img)*3+j*3+2] = pRGB[2];
        }
        fseek(fin,*padding,SEEK_CUR);
    }
    fclose(fin);
    return Q;
}
void afisare(char *nume_fisier_iesire,unsigned char Poza[],unsigned char header[54],unsigned int dim_img,unsigned int inaltime_img,unsigned int latime_img,int padding)
{
    FILE *fout = fopen(nume_fisier_iesire, "wb");

    fwrite(header, 54, 1, fout);
    int i;
    unsigned char RGB[1];
    RGB[0] = 255;
    for(i = 0; i < inaltime_img; i++)
    {
        fwrite(Poza+(inaltime_img-i-1)*latime_img*3,latime_img*3, 1, fout);
        int j;
        for( j = 0 ; j < padding ; ++j)
            fwrite(RGB,1,1,fout);
    }
    fclose(fout);
}
int main()
{
    char nume_img_testata[] = "baga_jan.bmp";
    char nume_img_criptata_ok[] = "test.bmp";
    char nume_img_diferente[] = "imagine_diferente_pentru_poza_mea.bmp";
    char nume_img_diferentee[] = "imagine_diferente_pentru_poza_corecta.bmp";
    unsigned int dim_img,latime_img,inaltime_img;
    int padding_img;
    unsigned char header_img[54];
    unsigned char * Poza = liniarizare_poza(nume_img_testata,header_img,&dim_img,&inaltime_img,&latime_img,&padding_img);

    unsigned int dim_pimg,latime_pimg,inaltime_pimg;
    int padding_pimg;
    unsigned char header_pimg[54];
    unsigned char * Pola = liniarizare_poza(nume_img_criptata_ok,header_pimg,&dim_pimg,&inaltime_pimg,&latime_pimg,&padding_pimg);

    int i,j;
    int number_diff = 0 ;
    for(i=0;i<inaltime_img;++i)
        for(j=0;j<latime_img;++j)
            if(Poza[3*(i*latime_img+j)]==Pola[3*(i*latime_pimg+j)]&&
               Poza[3*(i*latime_img+j)+1]==Pola[3*(i*latime_pimg+j)+1]&&
               Poza[3*(i*latime_img+j)+2]==Pola[3*(i*latime_pimg+j)+2])
    {
        Poza[3*(i*latime_img+j)]=Pola[3*(i*latime_pimg+j)]=50;
        Poza[3*(i*latime_img+j)+1]=Pola[3*(i*latime_pimg+j)+1]=50;
        Poza[3*(i*latime_img+j)+2]=Pola[3*(i*latime_pimg+j)+2]=50;
    }
    else
    {
        printf("%d %d \n",i,j);
        number_diff++;
    }
    printf("numarul de diferente este : %d \n",number_diff);
    afisare(nume_img_diferente,Poza,header_img,dim_img,inaltime_img,latime_img,padding_img);
    afisare(nume_img_diferentee,Pola,header_pimg,dim_pimg,inaltime_pimg,latime_pimg,padding_pimg);
    return 0;
}
