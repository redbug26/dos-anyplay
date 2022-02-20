/**************************************************************************/
/* ANYPLAY: Version 0.4a                                                  */
/* Player de sons sur Speaker                                             */
/**************************************************************************/
/* Am‚lioration … faire                                                   */
/* - Gerer tout les PC                                                    */
/* - Utiliser la XMS                                                      */
/* - Supporter des cartes sonores                                         */
/* - Compter le nbr de clock dans la fonction playsound pour les <> PCs   */
/**************************************************************************/
/* Bugs:                                                                  */
/* -----                                                                  */
/* - ProblŠme avec la pile (solution actuelle : modification de _stklen ?)*/
/* - Playeur de sound en 1 bit ( decompression par bit ne marche pas ? )  */
/* - Plante quelque fois aprŠs avoir jouer un morceau .pq ? :(            */
/**************************************************************************/
/* History:                                                               */
/* --------                                                               */
/* v0.1:  010195 Plusieurs versions non dat‚es. Ne marche que sur PC 12Mhz*/
/* v0.2:  030795 Charge des plus gros blocs que 64Ko                      */
/*               Menu utilisateur (nom et nbr de blocs)                   */
/*               Procedure PLAYSOUND maintenant en assembleur             */
/*               Permet l'adaptation de vitesses suivant le PC            */
/*               Reconnait les .AU et .WAV (en partie seulement)          */
/*               Reconnait les fr‚quences                                 */
/* v0.3:  050795 Reconnait les .SON (format (C) RedBugs) entiŠrement      */
/*               Description du format des fichiers RedBugs ;)            */
/* v0.3a: 070795 Permet le chgt de bit ( 8 ou 1 ) dans le menu            */
/* v0.3b: 010895 AprŠs trois semaines de vacances, me revoil…. Fichu prog.*/
/*               Aide et affichage plus complets                          */
/* v0.5:  ?????? Bcp d'am‚lioration en vues                               */
/**************************************************************************/


/**************************************************************************
<H1>The Multimedia RedBugs File Format</H1>
<H2>The RedB File Format</H2>
The Redbugs file use this structure:<BR>
<PRE>
Offset          Name         Length (in byte)        Description
 00h            ID           4h                Contains the characters "RedB"
 04h            Rb_Data      Rb_Len            The data
 04h+Rb_Len     Rb_Data (2)  Rb_Len(2)         Another data
 ...            ...          ...               ...
</PRE>
<H2>The Rb_Data Format</H2>
<PRE>
Offset          Name         Length (in byte)        Description
 00h            Rb_ID        4h                Contains the type of this data
 04h            Rb_Len       4h                Length of this data
 08h            Ddata        Rb_Len-08h        Data of this data :=)
 </PRE>
<H2>The Ddata format</H2>
<H3>The .SON form definition</H3>
<PRE>
Offset          Name         Length (in byte)        Description
 00h            Freq         2                       Frequency of the sample
 02h            Cps          2                       Clock per second
 03h            Bit          1                       # of bit of the sample
</PRE>
<HR>
****************************************************************************/
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <alloc.h>
#include <stdio.h>
#include <string.h>

#include "c:\langages\src\lib.h"

#define attend {int loop; for (loop=0;loop<14;loop++); }

#define out count[*(buf+i)]

/* void playsound(unsigned char *buf,unsigned int longueur); */

extern void playsound(unsigned char *buf,unsigned int longueur,int vit1,int vit2,unsigned char *count);
extern void playsound1(unsigned char *buf,unsigned int longueur,int vit1,int vit2);
extern void convert(unsigned char *buf,unsigned char *dest,unsigned int longueur);
extern int SpeedPC(void);


void main(int argv,char **argc)
{
char type[9];
char bit=8;		/* Attend */
unsigned char *bufvrai;


FILE *fic;
unsigned char *buf[50];
unsigned int sizebuf[50];
long int taille,pos,pos2;
unsigned int combien;
int n,nbr=0,nbr2;

int cps=54;		/* clock per second */
int pcv=100;	/* vitesse du PC, 100 ‚tant la valeur pour un AT 12MHZ */

int vit1,vit2;
int oldvit1=0,oldvit2=0;

char oldkey=0,vrelat;

char Filefrom[20];

struct traitement tract[20];

unsigned char count[256];
unsigned int i;

long pour_vitesse;

char buff[200];

int freq;			/* frequence du sample */
char stereo;		/* Si stereo, alors =1 */

char ad;			/* adaptation suivant le fichier ? */

strcpy(Filefrom,"**");

clrscr();
textmode(C80);

Intro("ANYPLAY v0.4a (c) 1995 by Vanhove Miguel.");
PrintAt(1,1,"Player de sons sur Speaker");
Ligne(1,2,78,2);


pour_vitesse=0;

for (n=0;n<50;n++)	pour_vitesse+=(long)SpeedPC();

pour_vitesse=pour_vitesse/50;
pcv=(int)(3774300./pour_vitesse);



if (argv!=1)
	{
	int n;
	for (n=1;n<=argv-1;n++)
		{
/*		printf("\nPremiere option: (%s), premier caractere: (%c)\n",argc[n],(argc[n])[1]); */
		switch((argc[n])[0])
			{
			case '/': break;
			default:	strcpy(Filefrom,argc[n]);	break;
			}
		} /* Fin du FOR */
	} /* FIN DU ARGV */

vit1=6;
vit2=14;

InitPercent();

PrintAt(2,17,"File to convert    : %s ",Filefrom);

PrintAt(2,4,"%6d blocs de 32000 octets allou‚s",nbr);

PrintAt(2,10,"Clock par sec:  %6d",cps);

PrintAt(2,14,"Vitesse du PC:  %6d",pcv);

PrintAt(2,8,"Nombre de bits:  %6d",bit);

tract[1].type=3;
tract[1].pointeur=&nbr;
tract[1].length=6;
tract[1].posx=2;
tract[1].posy=4;
tract[1].from=&(tract[6]);
tract[1].next=&(tract[2]);

tract[2].type=3;
tract[2].pointeur=&vit1;
tract[2].length=6;
tract[2].posx=14;
tract[2].posy=6;
tract[2].from=&(tract[1]);
tract[2].next=&(tract[3]);


tract[3].type=3;
tract[3].pointeur=&vit2;
tract[3].length=6;
tract[3].posx=14;
tract[3].posy=7;
tract[3].from=&(tract[2]);
tract[3].next=&(tract[7]);

tract[7].type=3;
tract[7].pointeur=&bit;
tract[7].posx=14;
tract[7].posy=8;
tract[7].from=&(tract[3]);
tract[7].next=&(tract[4]);

tract[4].type=3;
tract[4].pointeur=&cps;
tract[4].length=6;
tract[4].posx=18;
tract[4].posy=10;
tract[4].from=&(tract[7]);
tract[4].next=&(tract[0]);

tract[0].type=3;
tract[0].pointeur=&pcv;
tract[0].length=6;
tract[0].posx=18;
tract[0].posy=14;
tract[0].from=&(tract[4]);
tract[0].next=&(tract[5]);

tract[5].type=2;
tract[5].pointeur=(void*)Filefrom;
tract[5].length=12;
tract[5].posx=23;
tract[5].posy=17;
tract[5].from=&(tract[0]);
tract[5].next=&(tract[6]);

tract[6].type=0;
tract[6].pointeur=NULL;
tract[6].posx=1;
tract[6].posy=1;
tract[6].from=&(tract[5]);
tract[6].next=&(tract[1]);

if (Traite(&(tract[4]))) The_End(0,"Stop by user");

/***Adaptation Traitement*********************/



/*********************************************/

if (!strcmp(Filefrom,"**"))
	The_End(1,"Pas de fichier d'entr‚e");
	else
	{
	fic=fopen(Filefrom,"rb");
	if (fic==NULL)
		The_End(2,"Le fichier %s n'existe pas !!!",Filefrom);
	}


freq=0;

strcpy(type,"Unknow");

/* Test si c'est un .WAV */
fseek(fic,0,SEEK_SET);
fread(buff,1,4,fic);
if (!strnicmp(buff,"RIFF",4))
	{	/* Oui !        C'est un WAV. Il est content Miguel...:) */
/*	long int taille; */
	strcpy(type,".WAV");

	fread(buff,1,4,fic);
/*	taille=buff[3]*16777216L+buff[2]*65536L+buff[1]*256L+buff[0]; */
	fread(buff,1,4,fic);
	if (!strnicmp(buff,"WAVE",4))
		{	/* Youppee, en plus c'est un data audio, je fais de bond mtn */
		fread(buff,1,4,fic);
		if (!strnicmp(buff,"LIST",4))
			{
			long int longueur;
			fread(buff,1,4,fic);
			longueur=buff[3]*16777216L+buff[2]*65536L+buff[1]*256L+buff[0];

			fread(buff,1,(int)longueur,fic);
			fread(buff,1,4,fic);
			}
		if (!strnicmp(buff,"fmt ",4))
			{ /* Ici, on a vraiment du bol , olalala....... */
			long int t1;
			int t2;

			fread(buff,1,4,fic);
			t1=buff[3]*16777216L+buff[2]*65536L+buff[1]*256L+buff[0];
			if (t1!=16) printf("Erreur");

			fread(buff,1,4,fic);
			t2=buff[1]*256+buff[0];

			fread(buff,1,4,fic);
			t2=buff[1]*256+buff[0];
			switch (t2) {
				case 1: stereo=0; break;
				case 2: stereo=1; break;
				default : /* printf("(Stereo '%d')",t2); */ break;
				}

			fread(buff,1,4,fic);
			t2=buff[1]*256+buff[0];
			freq=t2;

			fread(buff,1,4,fic);
			t2=buff[1]*256+buff[0];

			fread(buff,1,4,fic);
			t2=buff[1]*256+buff[0];

			fread(buff,1,4,fic);
			t2=buff[1]*256+buff[0];
			}
		}
	}

/* Teste si c'est un .AU */
fseek(fic,0,SEEK_SET);
fread(buff,1,4,fic);
if (!strnicmp(buff,".snd",4))
	{
	strcpy(type,".AU");
	/* C'est un .au */
	freq=8000;
	}

/* Teste si c'est un .SON  Poin‡on... ah ah ah */
fseek(fic,0,SEEK_SET);
fread(buff,1,4,fic);
if (!strnicmp(buff,"RedB",4))
	{
	/* C'est un fichier … moi */
    fread(buff,1,4,fic);
	if (!strnicmp(buff,".son",4))
		{
		strcpy(type,".SON");
		fseek(fic,4,SEEK_CUR);
		fread(&freq,2,1,fic);
		fread(&cps,2,1,fic);
		fread(&bit,1,1,fic);
		} /* Et en plus c'est un fichier son */
	}

/* Fin des tests */
fseek(fic,0,SEEK_SET);


/******************************************************************/
/*  Adaptation suivant le fichier                                 */
/******************************************************************/
for (i=0;i<256;i++)
	count[i]=(i*cps)/255 ; /* /2 pour doubler le volume */

if (freq==0)
	{
	ad=1;
	freq=(int)(640000./(vit1*vit2));
	}
else
	{
	ad=0;
	vit1=(int)(8000/(freq/8));
	vit2=10;
	}

PrintAt(40,4,"Type de fichier: %s",type);
PrintAt(40,6,"Stereo              : %s",stereo ? "Oui" : "Non");
PrintAt(40,7,"Fr‚quence %s:%6d",ad ? "adapt‚e   " : "du fichier",freq);
PrintAt(2,10,"Clock par sec:  %6d",cps);

PrintAt(2,8,"Nombre de bits:  %6d",bit);

/* vit1=(int)((vit1*(double)pcv)/690.); */
vit2=(int)((vit2*(double)pcv)/690.);

oldvit1=vit1;
oldvit2=vit2;
vrelat=0;

/**********************************************************************/
/* Fin des adaptations                                                */
/**********************************************************************/

fseek(fic,0,SEEK_END);
taille=ftell(fic);
fseek(fic,0,SEEK_SET);

pos=0;
pos2=0;

bufvrai=malloc(32000);
if (bufvrai==NULL) The_End(0,"Plus de m‚moire");

if (nbr==0) nbr=1000;
nbr2=nbr;
nbr=0;
while (((buf[nbr]=malloc(32000))!=NULL) & (nbr<nbr2)) nbr++;

PrintAt(2,4,"%6d blocs de 32000 octets allou‚s",nbr);

DrPercent(0,0,taille);

do
 {
 for (n=0;n<nbr;n++)
	{
	combien=32000;
	if (pos+combien>taille) combien=(int)(taille-pos);
	pos+=combien;
	sizebuf[n]=combien;
	fread(buf[n],combien,1,fic);
	DrPercent(pos,pos2,taille);
	}
 for (n=0;(n<nbr) & (inportb(0x60)!=1);n++)
	{
	register int Posit,Hoeveel;


	if (bit==1)
		{
		Posit=0;
		do
		  {
		  Hoeveel=200;
		  if (Posit+Hoeveel>sizebuf[n])
			Hoeveel=sizebuf[n]-Posit;
		  pos2+=Hoeveel;
		  poke(0x40,0x1A,peek(0x40,0x1C));  /* clearbuffer */
		  convert((buf[n])+Posit,bufvrai,Hoeveel);
		  playsound(bufvrai,Hoeveel*8,vit1,vit2,count);

		  DrPercent(pos,pos2,taille);
		  Posit+=Hoeveel;

		  poke(0x40,0x1A,peek(0x40,0x1C));  /* clearbuffer */

		  if (inportb(0x60)!=oldkey)
			{
			oldkey=inportb(0x60);
			switch(oldkey) {
				case 77:
					vit1=(vit1/2)+1;
					vit2=(vit2/2)+1;
					vrelat++;
					PrintAt(vrelat+5,18,"*");
					break;
				case 80:
					vit1=oldvit1;
					vit2=oldvit2;
					for(n=0;n<=vrelat;n++) PrintAt(n+5,18," ");
					vrelat=0;
					break;
				}
			}
		  }
		while ( (Posit!=sizebuf[n])  & (inportb(0x60)!=1) );
		}
		else
		{
		pos2+=sizebuf[n];
		poke(0x40,0x1A,peek(0x40,0x1C));  /* clearbuffer */
		playsound(buf[n],sizebuf[n],vit1,vit2,count);
		DrPercent(pos,pos2,taille);
		}
	}


 }
while ( (pos!=taille) & (inportb(0x60)!=1) );


for (n=0;n<nbr;n++)	free(buf[n]);
free(bufvrai);

The_End(0,"That's All");
}

