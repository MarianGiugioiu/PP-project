#include <stdio.h>
#include <stdlib.h>
#include<math.h>
int max(int a, int b)
{
    if(a>b)
        return a;
    return b;
}
int min(int a, int b)
{
    if(a<b)
        return a;
    return b;
}
struct fereastra{
    int x1,y1,x2,y2,cifra;
    double cor;
};
struct fereastra *templateMatching(char *numeImagine, char *numeSablon, double ps, int cif, int *m)
{
    unsigned int latime_i,inaltime_i,latime_s,inaltime_s,padding_s,padding_i;
    int n,i,j,k,l;
    double s1=0,s2=0,d1=0,d2=0,c=0;
    FILE *f=fopen(numeImagine,"rb");
    FILE *g=fopen(numeSablon,"rb");
    fseek(f, 18, SEEK_SET);
    fseek(g, 18, SEEK_SET);
    fread(&latime_i, sizeof(unsigned int), 1, f);
    fread(&inaltime_i, sizeof(unsigned int), 1, f);
    fread(&latime_s, sizeof(unsigned int), 1, g);
    fread(&inaltime_s, sizeof(unsigned int), 1, g);
    if(latime_i % 4 != 0)
        padding_i = 4 - (3 * latime_i) % 4;
    else
        padding_i = 0;
    if(latime_s % 4 != 0)
        padding_s = 4 - (3 * latime_s) % 4;
    else
        padding_s = 0;
    fseek(f, 54, SEEK_SET);
    fseek(g, 54, SEEK_SET);
    unsigned char mRGB[3],**a,**b,aux;
    a=(unsigned char**)malloc((inaltime_i+inaltime_s)*sizeof(unsigned char*));
    for(i=0;i<inaltime_i+inaltime_s;i++)
        a[i]=(unsigned char*)calloc((latime_i+latime_s),sizeof(unsigned char));
    b=(unsigned char**)malloc(inaltime_s*sizeof(unsigned char*));
    for(i=0;i<inaltime_s;i++)
        b[i]=(unsigned char*)calloc(latime_s,sizeof(unsigned char));
    n=inaltime_s*latime_s;
    for(i=(inaltime_i-1);i>=0;i--)
	{
		for(j = 0; j < latime_i; j++)
		{
			fread(mRGB, 3, 1, f);
			aux = 0.299*mRGB[2] + 0.587*mRGB[1] + 0.114*mRGB[0];
			a[i][j]=aux;
		}
		fseek(f,padding_i,SEEK_CUR);
	}
	fclose(f);
	for(i = (inaltime_s-1); i >= 0; i--)
	{
		for(j = 0; j < latime_s; j++)
		{
			fread(mRGB, 3, 1, g);
			aux = 0.299*mRGB[2] + 0.587*mRGB[1] + 0.114*mRGB[0];
			b[i][j]=aux;
			s2+=(double)aux;
		}
		fseek(g,padding_s,SEEK_CUR);
	}
	fclose(g);
	s2/=(double)n;
    for(i = 0; i < inaltime_s; i++)
		for(j = 0; j < latime_s; j++)
            d2+=((double)b[i][j]-s2)*((double)b[i][j]-s2);
    d2/=(n-1);
    d2=sqrt(d2);
    struct fereastra *v,*aux1;
    v=(struct fereastra*)calloc(1,sizeof(struct fereastra));
    int nr=0;
    for(i=0;i<inaltime_i;i++)
        for(j=0;j<latime_i;j++)
    {
        c=0;s1=0;d1=0;
        for(k = 0; k < inaltime_s; k++)
            for(l = 0; l < latime_s; l++)
            s1+=(double)a[i+k][j+l];
        s1/=(double)n;
        for(k = 0; k < inaltime_s; k++)
            for(l = 0; l < latime_s; l++)
            d1+=((double)a[i+k][j+l]-s1)*((double)a[i+k][j+l]-s1);
        d1/=(n-1);
        d1=sqrt(d1);
        for(k=0;k<inaltime_s;k++)
            for(l=0;l<latime_s;l++)
            c+=((double)a[i+k][j+l]-s1)*((double)b[k][l]-s2)/(d1*d2);
        c/=n;
        if(c>=0.5)
        {
            int e;
            if(nr>0)
            {
                aux1=(struct fereastra*)calloc((nr+1),sizeof(struct fereastra));
                for(e=0;e<nr;e++)
                {
                    aux1[e]=v[e];
                }
                free(v);
                v=aux1;
            }
            v[nr].x1=i;
            v[nr].x2=inaltime_s;
            if(i+k>=inaltime_i)
                v[nr].x2=inaltime_s-(i+k-inaltime_i);
            v[nr].y1=j;
            v[nr].y2=latime_s;
            if(j+l>=latime_i)
                v[nr].y2=latime_s-(j+l-latime_i);
            v[nr].cor=c;
            v[nr].cifra=cif;
            nr++;
        }
    }
    *m=nr;
    return v;
}
void contur(char *numeImagine, struct fereastra d, unsigned char RGB[])
{
    unsigned int latime_i,inaltime_i,i,j,k,l,n,padding_i;
    FILE *f=fopen(numeImagine,"rb+");
    fseek(f, 18, SEEK_SET);
    fread(&latime_i, sizeof(unsigned int), 1, f);
    fread(&inaltime_i, sizeof(unsigned int), 1, f);
    if(latime_i % 4 != 0)
        padding_i = 4 - (3 * latime_i) % 4;
    else
        padding_i = 0;
    fseek(f, 54, SEEK_SET);
    fseek(f, (latime_i*3+padding_i)*(inaltime_i-d.x1-1)+d.y1*3, SEEK_CUR);
    for(i=0;i<d.y2;i++)
    {
        fwrite(RGB, 3, 1, f);
        fflush(f);
    }
    for(i=1;i<(d.x2);i++)
    {
        fseek(f, 54, SEEK_SET);
        fseek(f, (latime_i*3+padding_i)*(inaltime_i-d.x1-i)+d.y1*3, SEEK_CUR);
        fwrite(RGB, 3, 1, f);
        fflush(f);
        fseek(f,(d.y2-2)*3,SEEK_CUR);
        fwrite(RGB, 3, 1, f);
        fflush(f);
    }
    fseek(f, 54, SEEK_SET);
    fseek(f, (latime_i*3+padding_i)*(inaltime_i-d.x1-d.x2)+d.y1*3, SEEK_CUR);

    for(i=0;i<d.y2;i++)
    {
        fwrite(RGB, 3, 1, f);
        fflush(f);
    }
    fclose(f);
}
int cmp(const void *a, const void *b)
{
    struct fereastra va=*(struct fereastra*)a;
    struct fereastra vb=*(struct fereastra*)b;
    if(va.cor==vb.cor)
        return 0;
    if(va.cor>vb.cor)
        return -1;
    return 1;
}
struct fereastra *eliminare(struct fereastra **w,int *m,int nr1)
{
    int n=0,i,j,k=0;
    struct fereastra *v,*u;
    for(i=0;i<nr1;i++)
        n+=m[i];
    v=(struct fereastra*)malloc(n*sizeof(struct fereastra));
    for(i=0;i<nr1;i++)
        for(j=0;j<m[i];j++)
        {
            v[k]=w[i][j];
            k++;
        }
    int nr=n,a1,a2;
    int a,b,e,d;
    double c;
    qsort(v,n,sizeof(struct fereastra),cmp);
    for(i=0;i<(k-1);i++)
        for(j=i+1;j<k;j++)
        if(v[i].x1>-1&&v[j].x1>-1)
        {
            a=max(v[i].x1,v[j].x1);
            b=max(v[i].y1,v[j].y1);
            d=min(v[i].x1+v[i].x2,v[j].x1+v[j].x2);
            e=min(v[i].y1+v[i].y2,v[j].y1+v[j].y2);
            a1=(d-a)*(e-b);
            a2=v[i].x2*v[i].y2+v[j].x2*v[j].y2-a1;
            c=(double)a1/a2;
            if(c>=0.2&&(d-a)>0&&(e-b)>0)
            {
                v[j].x1=-1;
                nr--;
            }
        }
    u=(struct fereastra*)malloc((nr+1)*sizeof(struct fereastra));
    u[0].x1=nr;
    j=0;
    for(i=0;i<k;i++)
        if(v[i].x1>=0)
            u[++j]=v[i];
    free(v);
    return u;
}
int main()
{
    char nume_img_sursa[20];
    char nume_img_sablon[20];
    int n,i,R,G,B,*m;
    unsigned char RGB[10][3] = { {  0,  0,255} , {  0,255,255} , {  0,255,  0} , {255,255,  0} , {255,  0,255} , {255,  0,  0} , {192,192,192} , {  0,140,255} , {128,  0,128} , {  0,  0,128} };
	FILE *f=fopen("imagini.txt","r");
	fscanf(f,"%s",nume_img_sursa);
	fscanf(f,"%d",&n);
	struct fereastra **w=(struct fereastra**)malloc(n*sizeof(struct fereastra*));
	m=(int*)malloc(n*sizeof(int));
	for(i=0;i<n;i++)
    {
        struct fereastra *v;
        fscanf(f,"%s",nume_img_sablon);
        w[i]=templateMatching(nume_img_sursa,nume_img_sablon,0.5,i,&m[i]);
    }
    struct fereastra *v=eliminare(w,m,n);
    for(i=1;i<=v[0].x1;i++)
    {
        contur("test.bmp",v[i],RGB[v[i].cifra]);
    }
    free(v);
    for(i=0;i<n;i++)
        free(w[i]);
    free(w);
    return 0;
}
