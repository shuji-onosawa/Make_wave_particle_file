/*  Exos-D LEP data dump tool (for science data base)
  	          ver 1.0  '2000/2/19  Yasumasa Kasaba                */
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>

struct TIMEDATA
{
  char stime[12];
  char comment[15649];
};

typedef struct TIMEDATA timedata;

struct CDATA
{
  unsigned char ele;
  unsigned char ion;
};

struct LEPDATA
{
  unsigned char blocknumber;
  struct CDATA	countdata[7830];  // 18 pitch angle *29 energy step *15 recode=7830
};

typedef struct LEPDATA lepdata;

FILE *fpi; // 入力ファイル(*.lep)のポインタ
FILE *fpo; // 出力ファイルのポインタ

int	PXL, year, mon, day, stime, stime1, etime, angle, spicies;

char	buf[100];
int     dat[2000];

struct {
  int	ele[18][29];
  int	ion[18][29];
} particle[450];

float	tml[3000];

void parse(int argc, char *argv[]);
void mkinfilename(char *fnme1, char *filnme);
int infileopen(char *fnme);
int headerread();
int dataread();
void fileclose();

/**************************************************************/
/**************************  MAIN  ****************************/
/**************************************************************/
int main(int argc,char **argv)
{
  int	i, n, x, y;
  static char infnme[100]="";  // 入力ファイル名

  // 出力ファイルを開く
  fpo = fopen("output.txt", "w");
  if (fpo == NULL) {
      fprintf(stderr, "Cannot open output file.\n");
      exit(1);
  }

  parse(argc,argv);

  mkinfilename(argv[1],infnme);
  infileopen(infnme);

  headerread();

  PXL=dataread();
  if(PXL==0) {
      fprintf(stderr," 0-length input file "); exit(1);
  }

  fileclose();

  fclose(fpo);
}


/************************************************************/
/**************************  SUB  ***************************/
/************************************************************/
void mkinfilename(char *fnme1,char *filnme)
{
  strcat(filnme,fnme1);
}


int infileopen(char *fnme)
{
  if(NULL == (fpi=fopen(fnme,"rb"))) {
    fprintf(stderr,"Can not open input file %s \n",fnme);
    exit(1);
  } else {
    return(0);
  }
}


int headerread()
{
  timedata *buf;
  int	h,m,s;
  static char sttime[20]="",stdate[20]="";

  if(NULL == (buf=(timedata *)malloc(16000))) {
    fprintf(stderr,"Can not allocate buffer memory !! \n");
    return(1);
  }

  fread(buf,sizeof(timedata),1,fpi);  // *.lepファイルのヘッダーを読み込む ヘッダーはyymmddhhmmssの12バイト
  strncpy(stdate, buf->stime, 12);
  stdate[12]='\0';  // stdateにはyymmddhhmmssが入る
  strncpy(sttime,&stdate[6],6);
  sttime[6]='\0';  // sttimeにはhhmmssが入る

  s=atol(sttime);
  h = s/10000; m = (s%10000)/100; s = s%100;
  fprintf(stderr, "\thead time : %02d:%02d:%02d\n",h,m,s);
  stime = s + 60*(m+60*h);

  free(buf);
  return(0);
}


int dataread()
{
  int i,j,k,s,n,readnum=0;
  lepdata *buf;
  unsigned char cr, cr1, cr2 ;

  if(NULL == (buf=(lepdata *)malloc(16000)))  {
    fprintf(stderr, "Can not allocate buffer memory !! \n");
    return(1);
  }

  while(stime+120 < stime1) { // stime1: 開始時刻
    if(fread(buf,sizeof(lepdata),1,fpi) != 1) return(1);
    stime += 120;
  }

  s = stime; stime %= 86400L;

  // ヘッダーを出力
  fprintf(fpo, "time[UT]  ");
  for (i=0; i<29; i++) fprintf(fpo, "C%02d ",i+1); fprintf(fpo, "\n") ;

  // データを出力
  for(n=0;n<450/15;n++) {/* max 30 blocs ie. 30*120 sec =1 hour */
    if(s > etime) break;
    if(fread(buf,sizeof(lepdata),1,fpi) != 1) {
	    free(buf);
	    return(readnum);
    }
    for(k=0;k<15;++k)	{
	    if(s+8 > stime1) {
	      fprintf(fpo, "%02d:%02d:%02d  ",
		                 (s-(s%3600))/3600,  // 時間
		                 ((s%3600)-(s%60))/60,  // 分
		                 s%60);  // 秒
	      for(i=0;i<18;i++) {
	        for(j=0;j<29;j++) {
            particle[readnum].ele[i][j]=buf->countdata[18*29*k+29*i+j].ele;
            particle[readnum].ion[i][j]=buf->countdata[18*29*k+29*i+j].ion;
          }
	      }
	      for(j=0;j<29;j++) {
	        cr1 = 0 ; cr2=0 ;
	        if (angle > 0) { // angleが0より大きい場合、指定した角度のデータを取得
	          i = angle;
	          if (spicies == 0) cr = particle[readnum].ele[i][j] ;
	          else              cr = particle[readnum].ion[i][j] ;
            fprintf(fpo, "%3d ",cr) ;
	        }
	        else { // angleが0の場合、最大のデータを取得
	          for(i=0;i<18;i++) {
              // speciesが0なら電子、1ならイオン
              if (spicies == 0) cr = particle[readnum].ele[i][j] ;
              else              cr = particle[readnum].ion[i][j] ;
              //
              cr2 = (cr > cr1) ? cr : cr1;
              cr1 = cr2 ;
            }
	          fprintf(fpo, "%3d ",cr2) ;
          }
	      }
	      fprintf(fpo, "\n") ;
	      ++readnum;
	      s+=8;
	      if(s > etime) break;
	    }
	    else {
	      s+=8;
	      if(s > etime) break;
	    }
    }
  }

  free(buf);
  return(readnum);
}


void fileclose()
{
  fclose(fpi);
}


/************************************************************/
/**************************  SUB-2  ***************************/
/************************************************************/
void parse(int argc,char **argv)
{
  int	h,m,s;

  if(argc<6) {
    fprintf(stderr,"[Usage] lep <file> <stime> <etime> <angle> <spicies>\n");
    fprintf(stderr,"             file: file name\n");
    fprintf(stderr,"            stime: start time (hhmmss  0:all  +?:from next day)\n");
    fprintf(stderr,"            etime: end   time (hhmmss  0:all)\n");
    fprintf(stderr,"            angle: pitch angle  (0:peak, 1-18:deg)\n");
    fprintf(stderr,"          spicies: 0:e 1:i\n");
    fprintf(stderr,"[Data] hh:mm:ss energy1 energy2 ..... energy 29\n");
    fprintf(stderr,"           energy: 1-29  10.2*exp(0.25466*i) eV for e\n");
    fprintf(stderr,"                         12.5*exp(0.25466*i) eV for i\n");
    fprintf(stderr,"          number flux (1/cm^2 s sr eV): (count rate)/(G*E)\n");
    fprintf(stderr,"                                   G:5.8E-5 for e, 2.3E-4 for i\n");
    fprintf(stderr,"         phase space density [s^3/m^6]: 1.616E-19 * (J/E) for e\n");
    fprintf(stderr,"                                        5.449E-13 * (J/E) for i\n");
    exit(0);
  }

  /* date */
  year = atol(argv[1])/100; stime1 = 0; etime = 2*86400L;
  day  = year%100; mon = (year/100)%100; year = year/10000;
  fprintf(stderr,"\t   <file> : %s (%04d%02d%02d)\n", argv[1], year, mon, day);

  /* stime */
  s = atol(argv[2]);
  if (s == 0) {
      s = (atol(argv[1]) % 100) * 10000;
  }
  h = s / 10000;
  m = (s % 10000) / 100;
  s %= 100;
  fprintf(stderr, "\t  <stime> : %02d:%02d:%02d\n", h, m, s);
  stime1 = s + 60 * (m + 60 * h);

  if('+'==argv[2][0]){
    stime1 += 86400L; day++; if(day<29) return;
    switch(mon) {
    case  1:
    case  3:
    case  5:
    case  7:
    case  8:
    case 10:
    case 12: if(day<=31) break;
      mon++; day = 1; break;
    case  4:
    case  6:
    case  9:
    case 11: if(day<=30) break;
      mon++; day = 1; break;
    case  2: if(day<=28) break;
      if((0==(year%4))&&(day=29)) break;
      mon++; day = 1;
    }
    etime = stime1 + 86400L;
  }

  /* etime */
  s = atol(argv[3]);
  if (s!=0) {
    h = s/10000; m = (s%10000)/100; s %= 100;
    etime = s+60*(m+60*h);
    fprintf(stderr,"\t  <etime> : %02d:%02d:%02d\n", h,m,s);
  }

  /* pitch angle & spicies */
  angle = atol(argv[4]);    if (angle < 0   || angle > 18 ) angle   = 0;
  spicies = atol(argv[5]);  if (spicies < 0 || spicies > 1) spicies = 0;
  fprintf(stderr,"\t  <angle> : ");
  if (angle>0)   fprintf(stderr,"%d (%d-%d deg)\n", angle, (angle-1)*10, angle*10);
  else           fprintf(stderr,"max\n");
  fprintf(stderr,"\t<spicies> : ");
  if (spicies==0) fprintf(stderr,"electron\n");
  else            fprintf(stderr,"ion\n");

}



