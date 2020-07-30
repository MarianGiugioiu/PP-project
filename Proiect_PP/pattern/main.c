#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void grayscale_image(char* nume_fisier_sursa,char* nume_fisier_destinatie)
{
    FILE *fin, *fout;
    unsigned int dim_img, latime_img, inaltime_img;
    unsigned char pRGB[3], header[54], aux;

    printf("nume_fisier_sursa = %s \n",nume_fisier_sursa);

    fin = fopen(nume_fisier_sursa, "rb");
    if(fin == NULL)
    {
        printf("nu am gasit imaginea sursa din care citesc");
        return;
    }

    fseek(fin, 2, SEEK_SET);
    fread(&dim_img, sizeof(unsigned int), 1, fin);
    printf("Dimensiunea imaginii in octeti: %u\n", dim_img);

    fseek(fin, 18, SEEK_SET);
    fread(&latime_img, sizeof(unsigned int), 1, fin);
    fread(&inaltime_img, sizeof(unsigned int), 1, fin);
    printf("Dimensiunea imaginii in pixeli (latime x inaltime): %u x %u\n",latime_img, inaltime_img);

    ///calculam padding-ul pentru o linie
    int padding;
    if(latime_img % 4 != 0)
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

    printf("padding = %d \n",padding);

    fseek(fin,0,SEEK_SET);
    fread(header, 54, 1, fin);

    unsigned char * MMM;
    MMM = (unsigned char*) calloc(inaltime_img*latime_img*3,sizeof(unsigned char));
    int i,j;
    int lol=0;
    for(i = 0; i < inaltime_img; i++)
    {
        for(j = 0; j < latime_img; j++)
        {
            //citesc culorile pixelului
            fread(pRGB, 3, 1, fin);
            //fac conversia in pixel gri
            aux = 0.299*pRGB[2] + 0.587*pRGB[1] + 0.114*pRGB[0];
            pRGB[0] = pRGB[1] = pRGB[2] = aux;
            MMM[i*latime_img*3+j*3]   = pRGB[0];
            MMM[i*latime_img*3+j*3+1] = pRGB[1];
            MMM[i*latime_img*3+j*3+2] = pRGB[2];
        }
        fseek(fin,padding,SEEK_CUR);
    }
    fclose(fin);

    fout = fopen(nume_fisier_destinatie, "wb");
    fwrite(header, 54, 1, fout);
    fwrite(MMM, dim_img-54, 1, fout);
    fclose(fout);
}
void XORSHIFT32(int numbers,unsigned int RANDOM[],unsigned int r)
{
    int i;
    RANDOM[0] = r;
    for(i=1;i<numbers;++i)
    {
        r = r ^ (r << 13);
        r = r ^ (r >> 17);
        r = r ^ (r << 5);
        RANDOM[i] = r;
    }
}
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
void permut_pixeli_poza(unsigned char A[],unsigned char B[],unsigned int P[],unsigned int inaltime_img,unsigned int latime_img)
{
    int i,j;
    for(i = 0; i < inaltime_img; i++)
        for(j = 0; j < latime_img; j++)
        {
            /// pozitia in permutare pentru pixelul (i,j)
            int poz = i*latime_img+j;
            int ii = P[poz]/latime_img;
            int jj = P[poz]%latime_img;
            A[ii*latime_img*3+jj*3]   = B[i*latime_img*3+j*3];
            A[ii*latime_img*3+jj*3+1] = B[i*latime_img*3+j*3+1];
            A[ii*latime_img*3+jj*3+2] = B[i*latime_img*3+j*3+2];
        }
}
void read_secret_key(unsigned int *R0,unsigned int *SV)
{
    FILE *fon = fopen("secret_key.txt","r");
    fscanf(fon,"%u %u",&*R0,&*SV);
    fclose(fon);
}
unsigned int *fac_permutarea(unsigned int *SV,unsigned int inaltime_img,unsigned int latime_img,unsigned int Random[])
{
    /// citesc cheia secreta
    unsigned int R0;
    read_secret_key(&R0,&*SV);

    /// obtin numerele random
    XORSHIFT32(2*inaltime_img*latime_img,Random,R0);

    /// FAC PERMUTAREA
    unsigned int * P;
    P = (unsigned int*) calloc(inaltime_img*latime_img,sizeof(unsigned int));
    int i,j;
    for(i=0;i<inaltime_img*latime_img;++i) P[i]=i;

    int ind = 1;
    for(i=inaltime_img*latime_img-1;i>0;--i)
    {
        int p = Random[ind]%(i+1);
        int aux = P[p];
        P[p] = P[i];
        P[i] = aux;
        ind++;
    }
    return P;
}
void criptare_poza(unsigned char Poza[],unsigned int inaltime_img,unsigned int latime_img)
{
    unsigned int * Random;
    Random = (unsigned int*) calloc(2*inaltime_img*latime_img,sizeof(unsigned int));
    unsigned int SV;
    unsigned int *P = fac_permutarea(&SV,inaltime_img,latime_img,Random);

    /// permut pixelii
    unsigned char * PozaPermutata;
    PozaPermutata = (unsigned char*) calloc(inaltime_img*latime_img*3,sizeof(unsigned char));
    permut_pixeli_poza(PozaPermutata,Poza,P,inaltime_img,latime_img);

    /// codific poza
    unsigned int act = SV;
    int ind = inaltime_img*latime_img;
    int i,j;
    for(i = 0; i < inaltime_img; i++)
        for(j = 0; j < latime_img; j++)
        {
            act^=Random[ind];
            act^=PozaPermutata[i*latime_img*3+j*3];
            act^=((unsigned int)PozaPermutata[i*latime_img*3+j*3+1]<<8);
            act^=((unsigned int)PozaPermutata[i*latime_img*3+j*3+2]<<16);
            Poza[i*latime_img*3+j*3]   =  act     &((1<<8)-1);
            Poza[i*latime_img*3+j*3+1] = (act>>8) &((1<<8)-1);
            Poza[i*latime_img*3+j*3+2] = (act>>16)&((1<<8)-1);
            ind++;
        }
}
void decriptare_poza(unsigned char Poza[],unsigned int inaltime_img,unsigned int latime_img)
{
    unsigned int * Random;
    Random = (unsigned int*) calloc(2*inaltime_img*latime_img,sizeof(unsigned int));
    unsigned int SV;
    unsigned int *P = fac_permutarea(&SV,inaltime_img,latime_img,Random);

    /// fac inversa
    unsigned int * invP;
    invP = (unsigned int*) calloc(inaltime_img*latime_img,sizeof(unsigned int));
    int i,j;
    for(i = 0; i < inaltime_img*latime_img ; ++i)
        invP[P[i]]=i;
    /// aplic decodificarea
    unsigned char * PozaSemiDecriptata;
    PozaSemiDecriptata = (unsigned char*) calloc(inaltime_img*latime_img*3,sizeof(unsigned char));

    unsigned int act = SV;
    int ind = inaltime_img*latime_img;
    for(i = 0; i < inaltime_img; i++)
        for(j = 0; j < latime_img; j++)
        {
            act^=Random[ind];
            act^=Poza[i*latime_img*3+j*3];
            act^=((unsigned int)Poza[i*latime_img*3+j*3+1]<<8);
            act^=((unsigned int)Poza[i*latime_img*3+j*3+2]<<16);
            PozaSemiDecriptata[i*latime_img*3+j*3]   =  act     &((1<<8)-1);
            PozaSemiDecriptata[i*latime_img*3+j*3+1] = (act>>8) &((1<<8)-1);
            PozaSemiDecriptata[i*latime_img*3+j*3+2] = (act>>16)&((1<<8)-1);
            act = Poza[i*latime_img*3+j*3] + ((unsigned int)Poza[i*latime_img*3+j*3+1]<<8) +((unsigned int)Poza[i*latime_img*3+j*3+2]<<16);
            ind++;
        }

    /// permut invers pixelii
    permut_pixeli_poza(Poza,PozaSemiDecriptata,invP,inaltime_img,latime_img);
}
void chi_patrat(unsigned char Poza[],unsigned int inaltime_img,unsigned int latime_img)
{
    int * FR[3];
    int i,j;
    for(i = 0 ; i < 3 ; ++i)
        FR[i] = (int*) calloc(1<<8,sizeof(int));
    for(i = 0; i < inaltime_img; i++)
        for(j = 0; j < latime_img; j++)
        {
            FR[0][Poza[i*latime_img*3+j*3]]++;
            FR[1][Poza[i*latime_img*3+j*3+1]]++;
            FR[2][Poza[i*latime_img*3+j*3+2]]++;
        }
    double valoare_ideala = 1.0 * inaltime_img * latime_img / 256.0;
    double *RGB;
    RGB = (double*) calloc(3,sizeof(double));
    RGB[0]=RGB[1]=RGB[2]=0;
    for( i = 0 ; i < 256 ; ++i)
        for( j = 0 ; j < 3 ; ++j)
            RGB[j]+=1.0*(FR[j][i]-valoare_ideala)*(FR[j][i]-valoare_ideala)/valoare_ideala;
    printf("Frecvente R->G->B\n");
    for( i = 2 ; i >= 0 ; --i)
        printf("%.2f\n",RGB[i]);
}
void make_image_grayscale(unsigned char Poza[],unsigned int inaltime_img,unsigned int latime_img)
{
    int i,j;
    for( i = 0 ; i < inaltime_img ; ++i)
        for( j = 0 ; j < latime_img ; ++j)
        {
            unsigned char aux = 0.299*Poza[i*latime_img*3+j*3+2] + 0.587*Poza[i*latime_img*3+j*3+1] + 0.114*Poza[i*latime_img*3+j*3];
            Poza[i*latime_img*3+j*3]   = aux;
            Poza[i*latime_img*3+j*3+1] = aux;
            Poza[i*latime_img*3+j*3+2] = aux;
        }
}
struct fereastra
{
    int x;
    int y;
    int xx;
    int yy;
    int cifra;
    double scor;
};
struct fereastra * template_matching(char * nume_imagine,char * nume_cifra,double prag,int *nr_ferestre)
{
    unsigned int dim_img,latime_img,inaltime_img;
    int padding_img;
    unsigned char header_img[54];
    unsigned char * Poza = liniarizare_poza(nume_imagine,header_img,&dim_img,&inaltime_img,&latime_img,&padding_img);

    /// transform poza in imagine grayscale
    make_image_grayscale(Poza,inaltime_img,latime_img);

    unsigned int dim_s,latime_s,inaltime_s;
    int padding_s;
    unsigned char header_s[54];
    unsigned char * Sablon = liniarizare_poza(nume_cifra,header_s,&dim_s,&inaltime_s,&latime_s,&padding_s);

    /// transform sablonul in imagine grayscale
    make_image_grayscale(Sablon,inaltime_s,latime_s);

    /// dimensiunea pixelilor sablonului
    int n = inaltime_s * latime_s;
    int i,j;

    /// calculez media frecventelor pixelilor sablonului
    double media_pixeli_sablon = 0;
    long long s = 0;
    for( i = 0 ; i < inaltime_s ; ++i )
        for( j = 0 ; j < latime_s ; ++j)
            media_pixeli_sablon += 1.0 * Sablon[3 * (i*latime_s + j)];
    media_pixeli_sablon /= n;

    /// calculez deviatia pentru sablon
    double deviatie_pixeli_sablon = 0;
    for( i = 0 ; i < inaltime_s ; ++i)
        for( j = 0 ; j < latime_s ; ++j)
            deviatie_pixeli_sablon += 1.0 * (1.0* Sablon[3 * (i*latime_s + j)] - media_pixeli_sablon) * ( 1.0 * Sablon[3 * (i*latime_s + j)] - media_pixeli_sablon);
    deviatie_pixeli_sablon = sqrt( 1.0/(n-1)*deviatie_pixeli_sablon);

    char *nume_cifre[10] = {"cifra0.bmp","cifra1.bmp","cifra2.bmp","cifra3.bmp","cifra4.bmp","cifra5.bmp","cifra6.bmp","cifra7.bmp","cifra8.bmp","cifra9.bmp"};
    int cf = 0;
    int kk;
    for( kk = 0 ; kk < 10 ; ++kk)
        if(nume_cifra == nume_cifre[kk])
            cf = kk;

    struct fereastra * ans = NULL;
    *nr_ferestre = 0;
    /// trec la gasit ferestre
    printf("%d \n",cf);
    for( i = 0 ; i < inaltime_img ; ++i)
        for( j = 0 ; j < latime_img ; ++j)
    {
        /// calculez media frecventelor pixelilor ferestrei
        double media_pixeli_fereastra = 0;
        int k,l;
        for( k = i ; k < i + inaltime_s ; ++k)
            for( l = j ; l < j + latime_s ; ++l)
                if(k < inaltime_img && l < latime_img)
                    media_pixeli_fereastra += 1.0 * Poza[3 * (k*latime_img + l)];
        media_pixeli_fereastra /= n;

        /// calculez deviatia pentru fereastra
        double deviatie_pixeli_fereastra = 0;
        for( k = i ; k < i + inaltime_s ; ++k)
            for( l = j ; l < j + latime_s ; ++l)
                if(k < inaltime_img && l < latime_img)
                    deviatie_pixeli_fereastra += 1.0 * ( 1.0 * Poza[3 * (k*latime_img + l)] - media_pixeli_fereastra) * ( 1.0 * Poza[3 * (k*latime_img + l)] - media_pixeli_fereastra);
                else deviatie_pixeli_fereastra += 1.0 * ( - media_pixeli_fereastra) * (  - media_pixeli_fereastra);
        deviatie_pixeli_fereastra = sqrt(1.0/(n-1)*deviatie_pixeli_fereastra);

        /// calculez corelatia
        double corelatie = 0;
        for( k = i ; k < i + inaltime_s ; ++k)
            for( l = j ; l < j + latime_s ; ++l)
                if(k < inaltime_img && l < latime_img)
                    corelatie += 1.0 * ( 1.0 * Poza[3 * (k*latime_img + l)] - media_pixeli_fereastra) * ( 1.0 * Sablon[3 * ((k-i)*latime_s + (l-j))] - media_pixeli_sablon) / deviatie_pixeli_fereastra / deviatie_pixeli_sablon ;
                else corelatie += 1.0 * ( - media_pixeli_fereastra) * ( 1.0 * Sablon[3 * ((k-i)*latime_s + (l-j))] - media_pixeli_sablon) / deviatie_pixeli_fereastra / deviatie_pixeli_sablon ;
        corelatie /= n;
        if(corelatie >= prag)
        {
            *nr_ferestre += 1 ;
            ans = realloc(ans , *nr_ferestre * sizeof(struct fereastra));
            ans[*nr_ferestre-1].x = i;
            ans[*nr_ferestre-1].y = j;
            ans[*nr_ferestre-1].xx = i + inaltime_s - 1;
            ans[*nr_ferestre-1].yy = j + latime_s - 1;
            ans[*nr_ferestre-1].scor = corelatie;
            ans[*nr_ferestre-1].cifra = cf;
        }
    }
    return ans;
};
int cmp(const void *a,const void *b)
{
    if((((struct fereastra*)a)->scor) < (((struct fereastra*)b)->scor)) return 1;
    if((((struct fereastra*)a)->scor) > (((struct fereastra*)b)->scor)) return -1;
    int A = (((struct fereastra*)a)->scor) * 1e9;
    int B = (((struct fereastra*)b)->scor) * 1e9;
    return B - A;
}
int main()
{
    char nume_img_sursa[] = "test.bmp";
    char nume_img_grayscale[] = "test_grayscale.bmp";
    grayscale_image(nume_img_sursa, nume_img_grayscale);
    unsigned int dim_img,latime_img,inaltime_img;
    int padding;
    unsigned char header[54];
    unsigned char * Poza = liniarizare_poza(nume_img_sursa,header,&dim_img,&inaltime_img,&latime_img,&padding);
    chi_patrat(Poza,inaltime_img,latime_img);
    criptare_poza(Poza,inaltime_img,latime_img);
    afisare("crypto.bmp",Poza,header,dim_img,inaltime_img,latime_img,padding);
    chi_patrat(Poza,inaltime_img,latime_img);
    decriptare_poza(Poza,inaltime_img,latime_img);
    afisare("decrypto.bmp",Poza,header,dim_img,inaltime_img,latime_img,padding);
    char *nume_cifra[10] = {"cifra0.bmp","cifra1.bmp","cifra2.bmp","cifra3.bmp","cifra4.bmp","cifra5.bmp","cifra6.bmp","cifra7.bmp","cifra8.bmp","cifra9.bmp"};
    double prag = 0.5;
    struct fereastra *Ferestre = NULL;
    int i;
    int nr_ferestre = 0;
    for( i = 0 ; i < 10 ; ++i)
    {
        int add_ferestre;
        struct fereastra *F = template_matching(nume_img_sursa,nume_cifra[i],prag,&add_ferestre);
        Ferestre = realloc(Ferestre , (add_ferestre+nr_ferestre)*sizeof(struct fereastra));
        int j;
        for( j = 0 ; j < add_ferestre ; ++j)
            Ferestre[nr_ferestre++] = F[j];
        free(F);
    }
    qsort(Ferestre,nr_ferestre,sizeof(struct fereastra),cmp);
    char *ok;
    ok = (char *) calloc(nr_ferestre,sizeof(char));
    for( i = 0 ; i < nr_ferestre ; ++i)
        if(ok[i]==0)
    {
        int j;
        int aria_i = (Ferestre[i].xx-Ferestre[i].x+1)*(Ferestre[i].yy-Ferestre[i].y+1);
        for( j = i+1; j < nr_ferestre ; ++j)
            if(ok[j] == 0)
        {
            int aria_intersectiei = 0;
            int x = Ferestre[i].x;
            if(x < Ferestre[j].x)
                x = Ferestre[j].x;
            int y = Ferestre[i].y;
            if(y < Ferestre[j].y)
                y = Ferestre[j].y;
            int xx = Ferestre[i].xx;
            if(xx > Ferestre[j].xx)
                xx = Ferestre[j].xx;
            int yy = Ferestre[i].yy;
            if(yy > Ferestre[j].yy)
                yy = Ferestre[j].yy;
            if(x<=xx&&y<=yy)
                aria_intersectiei = (xx-x+1)*(yy-y+1);
            int aria_j = (Ferestre[j].xx-Ferestre[j].x+1)*(Ferestre[j].yy-Ferestre[j].y+1);
            double suprapunere = (double)aria_intersectiei/(double)(aria_i+aria_j-aria_intersectiei);
            if(suprapunere >= 0.2) ok[j] = 1;
        }
    }
    unsigned char culori[10][3] = {
                                     {  0,  0,255} ,
                                     {  0,255,255} ,
                                     {  0,255,  0} ,
                                     {255,255,  0} ,
                                     {255,  0,255} ,
                                     {255,  0,  0} ,
                                     {192,192,192} ,
                                     {  0,140,255} ,
                                     {128,  0,128} ,
                                     {  0,  0,128}
                                  };
    int number = 0;
    for( i = 0 ; i < nr_ferestre ; ++i)
        if(ok[i] == 0)
    {
        number ++ ;
        int j;
        unsigned char a = culori[Ferestre[i].cifra][0];
        unsigned char b = culori[Ferestre[i].cifra][1];
        unsigned char c = culori[Ferestre[i].cifra][2];
        if(Ferestre[i].xx > inaltime_img - 1)
                Ferestre[i].xx = inaltime_img - 1;
        if(Ferestre[i].yy > latime_img - 1)
                Ferestre[i].yy = latime_img - 1;
        for(j=Ferestre[i].x;j<=Ferestre[i].xx;++j)
        {

            Poza[3*(j*latime_img+Ferestre[i].y)]    = a;
            Poza[3*(j*latime_img+Ferestre[i].y)+1]  = b;
            Poza[3*(j*latime_img+Ferestre[i].y)+2]  = c;
            Poza[3*(j*latime_img+Ferestre[i].yy)]   = a;
            Poza[3*(j*latime_img+Ferestre[i].yy)+1] = b;
            Poza[3*(j*latime_img+Ferestre[i].yy)+2] = c;
        }
        for(j=Ferestre[i].y;j<=Ferestre[i].yy;++j)
        {
            Poza[3*(Ferestre[i].x*latime_img+j)]    = a;
            Poza[3*(Ferestre[i].x*latime_img+j)+1]  = b;
            Poza[3*(Ferestre[i].x*latime_img+j)+2]  = c;
            Poza[3*(Ferestre[i].xx*latime_img+j)]   = a;
            Poza[3*(Ferestre[i].xx*latime_img+j)+1] = b;
            Poza[3*(Ferestre[i].xx*latime_img+j)+2] = c;
        }
    }
    printf("NR de ferestre gasite : %d \n Nr de ferestre afisate dupa eliminare : %d",nr_ferestre,number);
    afisare("baga_jan.bmp",Poza,header,dim_img,inaltime_img,latime_img,padding);
    free(Ferestre);
    free(Poza);
    return 0;
}
