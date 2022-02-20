/* Tenter de traduire les sons .. */
/* selon leurs vrais valeurs (wav, voc, au..) */

#include <dos.h>
#include <mem.h>
#include <stdlib.h>
#include <alloc.h>
#include <stdio.h>
#include <bios.h>



void main(int argc,char **argv)
{
FILE *fic,*outfic;
unsigned char *buf,*buf2;
long int taille,pos;
unsigned int combien;
int n;
unsigned char puiss2[8]={128,64,32,16,8,4,2,1};

int freq,cps;
char bit;

fic=fopen(*(argv+1),"rb");
outfic=fopen(*(argv+2),"wb");

if (fic==NULL)
	{
	printf("pas de fic %d",argc);
	exit(1);
	}

fseek(fic,0,SEEK_END);
taille=ftell(fic);
fseek(fic,0,SEEK_SET);
pos=0;

buf=malloc(32000);
if (buf==NULL)
	{
	printf("pas de buf");
	exit(1);
	}

buf2=malloc(32000);
if (buf2==NULL)
	{
	printf("pas de buf");
	exit(1);
	}

fprintf(outfic,"RedB");
fprintf(outfic,".son");
fwrite(&taille,4,1,outfic);
freq=8000;
fwrite(&freq,2,1,outfic);
cps=2;
fwrite(&cps,2,1,outfic);
bit=1;
fwrite(&bit,1,1,outfic);

do
 {
 combien=32000;
 if (pos+combien>taille) combien=(int)(taille-pos);
 pos+=combien;
 fread(buf,1,combien,fic);
 memset(buf2,0,combien/8);
 for (n=0;n<combien;n++)
	buf2[n/8]=buf2[n/8] | (buf[n]>=128 ? puiss2[n%8] : 0);
 fwrite(buf2,1,combien/8,outfic);
 }
while ( (pos!=taille) & (!bioskey(1)) );

fclose(fic);
fclose(outfic);

free(buf);
free(buf2);
}

