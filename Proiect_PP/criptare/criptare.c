#include <stdio.h>
#include <stdlib.h>
void xorshift32(unsigned int nr_pixeli,unsigned int r[], unsigned int R)
{
    int i;
    r[0]=R;
    for(i=1;i<2*nr_pixeli;i++)
    {
        r[i]=r[i-1];
        r[i]=r[i]^r[i]<<13;
        r[i]=r[i]^r[i]>>17;
        r[i]=r[i]^r[i]<<5;
    }
}
void permutare(unsigned int nr_pixeli,unsigned  int p[], unsigned int r[])
{
    int i,z,aux,j=1;7
    for(i=0;i<nr_pixeli;i++)
        p[i]=i;

    for(i=nr_pixeli-1;i>=1;i--)
    {
        z=r[j]%(i+1);
        aux=p[i];
        p[i]=p[z];
        p[z]=aux;
        j++;
    }
}
unsigned char *imagine_liniarizare(char* nume_fisier_sursa,unsigned char header[54],unsigned int *dim_img,unsigned int *latime_img,unsigned int *inaltime_img,unsigned int *padding)
{
   FILE *fin;
   unsigned char pRGB[3];
   fin = fopen(nume_fisier_sursa, "rb");
   if(fin == NULL)
   {
   		printf("nu am gasit imaginea sursa din care citesc");
   		return;
   }
    int i,j;
   fseek(fin,0,SEEK_SET);
   fread(header,54,1,fin);
   fseek(fin, 2, SEEK_SET);
   fread(&*dim_img, sizeof(unsigned int), 1, fin);


   fseek(fin, 18, SEEK_SET);
   fread(&*latime_img, sizeof(unsigned int), 1, fin);
   fread(&*inaltime_img, sizeof(unsigned int), 1, fin);

	fseek(fin,0,SEEK_SET);

    if(*latime_img % 4 != 0)
        *padding = 4 - (3 * *latime_img) % 4;
    else
        *padding = 0;


	fseek(fin, 54, SEEK_SET);
	unsigned char *v;
	v=(unsigned char*)malloc(*latime_img*(*inaltime_img)*3*sizeof(unsigned char));
	int k=0;
	for(i = 0; i < *inaltime_img; i++)
	{
		for(j = 0; j < *latime_img; j++)
		{
			fread(pRGB, 3, 1, fin);
			v[(*inaltime_img-1-i)*(*latime_img)*3+j*3]=pRGB[0];
			v[(*inaltime_img-1-i)*(*latime_img)*3+j*3+1]=pRGB[1];
			v[(*inaltime_img-1-i)*(*latime_img)*3+j*3+2]=pRGB[2];
		}
		fseek(fin,*padding,SEEK_CUR);
	}
	fclose(fin);
	return v;
}
void liniarizare_imagine(char* nume_fisier_destinatie,unsigned int latime_img, unsigned int inaltime_img, unsigned int padding, unsigned char header[54], unsigned char c[])
{
    FILE *fout;
    fout = fopen(nume_fisier_destinatie, "wb");
    fwrite(header,54,1,fout);
    int i,j,k=0;
    for(i = 0; i < inaltime_img; i++)
    {
        fwrite(c+(inaltime_img-i-1)*latime_img*3,latime_img*3,1,fout);
		unsigned char d='0';
		for(j=1;j<=padding;j++)
            fwrite(&d,1,1,fout);
    }
	fclose(fout);
}
void permutare_pixeli(unsigned int nr_pixeli, unsigned int p[], unsigned char v[], unsigned char u[])
{
    int i;
    for(i=nr_pixeli-1;i>=0;i--)
    {
        u[p[i]*3]=v[i*3];
        u[p[i]*3+1]=v[i*3+1];
        u[p[i]*3+2]=v[i*3+2];
    }
}
void permutare_inversa_pixeli(unsigned int nr_pixeli, unsigned int p[], unsigned char v[], unsigned char u[])
{
    int i;
    for(i=nr_pixeli-1;i>=0;i--)
    {
        u[i*3]=v[p[i]*3];
        u[i*3+1]=v[p[i]*3+1];
        u[i*3+2]=v[p[i]*3+2];
    }
}
void criptare_liniarizare(unsigned int nr_pixeli, unsigned char c[],unsigned int SV, unsigned int r[], unsigned char u[])
{
    int i,n=nr_pixeli;
    unsigned char x,y;
    x = (r[n] >> (8*0)) & 0xff;
    y = (SV >> (8*0)) & 0xff;
    c[0]=y^u[0]^x;
    x = (r[n] >> (8*1)) & 0xff;
    y = (SV >> (8*1)) & 0xff;
    c[1]=y^u[1]^x;
    x = (r[n] >> (8*2)) & 0xff;
    y = (SV >> (8*2)) & 0xff;
    c[2]=y^u[2]^x;
    for(i=1;i<n;i++)
    {
        x = (r[n+i] >> (8*2)) & 0xff;
        c[i*3+2]=c[(i-1)*3+2]^u[i*3+2]^x;
        x = (r[n+i] >> (8*1)) & 0xff;
        c[i*3+1]=c[(i-1)*3+1]^u[i*3+1]^x;
        x = (r[n+i] >> (8*0)) & 0xff;
        c[i*3]=c[(i-1)*3]^u[i*3]^x;
    }
}
void decriptare_liniarizare(unsigned int nr_pixeli, unsigned char c[],unsigned int SV, unsigned int r[], unsigned char u[])
{
    int i,n=nr_pixeli;
    unsigned char x,y;
    x = (r[n] >> (8*0)) & 0xff;
    y = (SV >> (8*0)) & 0xff;
    c[0]=y^u[0]^x;
    x = (r[n] >> (8*1)) & 0xff;
    y = (SV >> (8*1)) & 0xff;
    c[1]=y^u[1]^x;
    x = (r[n] >> (8*2)) & 0xff;
    y = (SV >> (8*2)) & 0xff;
    c[2]=y^u[2]^x;
    for(i=1;i<n;i++)
    {
        x = (r[n+i] >> (8*2)) & 0xff;
        c[i*3+2]=u[(i-1)*3+2]^u[i*3+2]^x;
        x = (r[n+i] >> (8*1)) & 0xff;
        c[i*3+1]=u[(i-1)*3+1]^u[i*3+1]^x;
        x = (r[n+i] >> (8*0)) & 0xff;
        c[i*3]=u[(i-1)*3]^u[i*3]^x;
    }

}
void chi_patrat(unsigned char v[], unsigned int latime_img, unsigned int inaltime_img)
{
    int *f[3],i,j;
    for(i=0;i<3;i++)
        f[i]=(int*)calloc(256,sizeof(int));
    for(i=0;i<inaltime_img;i++)
        for(j=0;j<latime_img;j++)
    {
        f[0][v[i*latime_img*3+j*3]]++;
        f[1][v[i*latime_img*3+j*3+1]]++;
        f[2][v[i*latime_img*3+j*3+2]]++;
    }
    double m= 1.0*inaltime_img * latime_img / 256.0;
    double RGB[3];
    RGB[0]=RGB[1]=RGB[2]=0;
    for(i=0;i<256;i++)
        for(j=0;j<3;j++)
        RGB[j]+=1.0*(f[j][i]-m)*(f[j][i]-m)/m;
    for(i=2;i>=0;i--)
    {
        printf("%.2f\n",RGB[i]);
        free(f[i]);
    }
}
void criptare(char* nume_fisier_sursa, char* nume_fisier_destinatie, char* nume_fisier_cheie)
{
    unsigned char *v, header[54],*c, *u;
    unsigned int *p,*r,R,SV,dim_img, latime_img, inaltime_img, padding, nr_pixeli;
    v=imagine_liniarizare(nume_fisier_sursa,header,&dim_img,&latime_img,&inaltime_img,&padding);
    nr_pixeli=latime_img*inaltime_img;
    FILE *g=fopen(nume_fisier_cheie,"r");
    fscanf(g,"%u %u",&R,&SV);
    fclose(g);
    p=(unsigned int*)malloc(nr_pixeli*sizeof(unsigned int));
    r=(unsigned int*)malloc(2*nr_pixeli*sizeof(unsigned int));
    u=(unsigned char*)malloc(nr_pixeli*3*sizeof(unsigned char));
    c=(unsigned char*)malloc(nr_pixeli*3*sizeof(unsigned char));
    xorshift32(nr_pixeli,r,R);
	permutare(nr_pixeli,p,r);
	permutare_pixeli(nr_pixeli, p, v, u);
	criptare_liniarizare(nr_pixeli, c, SV, r, u);
	printf("\n");
	printf("valorile testului chi patrat pentru imaginea sursa:\n");
	chi_patrat(v, latime_img, inaltime_img);
	printf("\n");
	printf("valorile testului chi patrat pentru imaginea criptata:\n");
	chi_patrat(c, latime_img, inaltime_img);
	liniarizare_imagine(nume_fisier_destinatie,latime_img,inaltime_img,padding,header,c);
	free(v);
    free(p);
    free(c);
    free(u);
    free(r);
}
void decriptare(char* nume_fisier_sursa, char* nume_fisier_destinatie, char* nume_fisier_cheie)
{
    unsigned char *v, header[54],*c, *u;
    unsigned int *p,*r,R,SV,dim_img, latime_img, inaltime_img, padding, nr_pixeli;
    v=imagine_liniarizare(nume_fisier_sursa,header,&dim_img,&latime_img,&inaltime_img,&padding);
    nr_pixeli=latime_img*inaltime_img;
    FILE *g=fopen(nume_fisier_cheie,"r");
    fscanf(g,"%u %u",&R,&SV);
    fclose(g);
    p=(unsigned int*)malloc(nr_pixeli*sizeof(unsigned int));
    r=(unsigned int*)malloc(2*nr_pixeli*sizeof(unsigned int));
    u=(unsigned char*)malloc(nr_pixeli*3*sizeof(unsigned char));
    c=(unsigned char*)malloc(nr_pixeli*3*sizeof(unsigned char));
    xorshift32(nr_pixeli,r,R);
	permutare(nr_pixeli,p,r);
	decriptare_liniarizare(nr_pixeli, c, SV, r, v);
	permutare_inversa_pixeli(nr_pixeli, p, c, u);
	liniarizare_imagine(nume_fisier_destinatie,latime_img,inaltime_img,padding,header,u);
	printf("\n");
	free(v);
    free(p);
    free(c);
    free(u);
    free(r);
}
int main()
{
    char nume_img_sursa1[20];
    char nume_img_sursa2[20];
	char nume_img_criptata[20];
	char nume_img_decriptata[20];
	char nume_fisier_cheie[20];
	printf("Introduceti nume fisier cheie secreta: ");
	scanf("%s",nume_fisier_cheie);
	printf("\nIntroduceti nume imagine sursa pentru criptare: ");
	scanf("%s",nume_img_sursa1);
	printf("\nIntroduceti nume imagine criptata: ");
	scanf("%s",nume_img_criptata);
	printf("\nIntroduceti nume imagine sursa pentru decriptare: ");
	scanf("%s",nume_img_sursa2);
	printf("\nIntroduceti nume imagine decriptata: ");
	scanf("%s",nume_img_decriptata);
	criptare(nume_img_sursa1, nume_img_criptata, nume_fisier_cheie);
	decriptare(nume_img_sursa2, nume_img_decriptata, nume_fisier_cheie);
    printf("\n");
    return 0;
}
