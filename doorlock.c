#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <asm/ioctls.h>
#include "unchar.h"

#define dip "/dev/dipsw"
#define dot "/dev/dot"
#define tact "/dev/tactsw"
#define dbg(x...) printf(x)
#define fnd "/dev/fnd"
#define clcd "/dev/clcd"
#define MAXCHR 32


int get_tact();

int get_tact_4(int matched);
int get_dipsw(int flag);
void set_dot_OX(int i);

void set_clcd(int na);

void set_clcd_Start();
int set_clcd_OX(int a, int b, int c, int d, int password);

void set_fnd(unsigned char a, unsigned char b, unsigned char c,unsigned char d, int tf);



//////////////////////////////////////////////////////////////tact

int get_tact()
{
	int tact_d; 
	int chr=0;

	if((tact_d=open(tact,O_RDWR))<0)
	{
		perror("open");
		exit(1);
	}
	while(1)
	{	
		usleep(8000);
		read(tact_d,&chr,sizeof(chr));

		if(chr){	
			dbg("input %d\n", chr);
			break;
		}
	}
	close(tact_d);
	return chr;
}



//////////////////////////////////////////////////////////////dip

int get_dipsw(int flag)
{
	unsigned char t;
	int dip_d;
	if((dip_d = open(dip, O_RDWR)) < 0)
	{
		perror("open dip\n");
		exit(0);
	}

	
	while(1)
	{	
		if(flag==0) set_clcd(2);
		flag = 1;
		read(dip_d,&t,sizeof(t));
		if(t) break;
		flag = 0;
	}

	close(dip_d);
	return t;

}


//////////////////////////////////////////////////////////////dot

void set_dot_OX(int i)
{	
	int dot_d;
	unsigned char c[2][8]={
		{0x3c, 0x42, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3c},	//O
		{0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81}	//X
	};
	

	if((dot_d=open(dot,O_RDWR))<0)
	{
		perror("open");
		exit(1);
	}

	if(i==1){
		write(dot_d,&c[0],sizeof(c[0]));
	}
	else{
		write(dot_d,&c[1],sizeof(c[1]));
	}
	sleep(1);
	close(dot_d);
}


//////////////////////////////////////////////////////////////clcd


static int lcdFd = (-1);


void set_clcd(int na)
{	

	int n;
	char buf[MAXCHR];
    	char *notice[3]={"Entering password...", "Changing password...", "LOCK MODE"};

	lcdFd=open(clcd, O_RDWR);
    	if(lcdFd<0)
    	{
        	exit(1);
    	}

	memset(buf, 0, sizeof(buf));
	n=strlen(notice[na]);
	if(n>MAXCHR)
		n=MAXCHR;
			memcpy(buf, notice[na], n);
	write(lcdFd, buf, MAXCHR);


}


void set_clcd_Start()
{
	int n;
	char buf[MAXCHR];
    	char *notice[2]={"Please Enter    Your Password", "BYE"};

	lcdFd=open(clcd, O_RDWR);
    	if(lcdFd<0)
    	{
        	exit(1);
    	}

	memset(buf, 0, sizeof(buf));
	n=strlen(notice[0]);
	if(n>MAXCHR)
		n=MAXCHR;
			memcpy(buf, notice[0], n);
	write(lcdFd, buf, MAXCHR);
	

}



int set_clcd_OX(int a, int b, int c, int d, int password)
{	
    	int n;
	int matched;
	char buf[MAXCHR];
	char match[MAXCHR], mismatch[MAXCHR];
	sprintf(match, "Match : %d%d%d%d    Open the door", a, b, c, d);
	sprintf(mismatch, "Mismatch : %d%d%d%d Please re-enter",a, b, c, d);

    	char *notice[2]={match, mismatch};

	lcdFd=open(clcd, O_RDWR);
    	if(lcdFd<0)
    	{
        	exit(1);
    	}

	memset(buf, 0, sizeof(buf));	
	if(password == a*1000+b*100+c*10+d){
		set_dot_OX(1);
		matched=1;
		n=strlen(notice[0]);
		if(n>MAXCHR)
			n=MAXCHR;
				memcpy(buf, notice[0], n);
	}
	else{
		set_dot_OX(0);
		matched=0;
		n=strlen(notice[1]);
		if(n>MAXCHR)
			n=MAXCHR;
				memcpy(buf, notice[1], n);
	}

	write(lcdFd, buf, MAXCHR);
	return matched;

}


//////////////////////////////////////////////////////////////fnd


void set_fnd(unsigned char a, unsigned char b, unsigned char c,unsigned char d, int tf)
{
	int fnd_d;	
	unsigned char fnd_data[4];


	if((fnd_d=open(fnd,O_RDWR))<0)
	{
		perror("open");
		exit(1);
	}

	fnd_data[0]=a;
	fnd_data[1]=b;
	fnd_data[2]=c;
	fnd_data[3]=d;	

	write(fnd_d, fnd_data, sizeof(fnd_data));
	sleep(1);
	if (tf){
		close(fnd_d);
	}
}

//////////////////////////////////////////////////////////////clcd

int main(){
	
	int password=0;
	int matched=0;
	int result, flag=1;
	unsigned char a;
	
	while(1){
		
		int a, b, c, d;
		int mode=0;

		get_dipsw(flag);
		set_clcd_Start(); 

		a= int2uc(get_tact());
		if(matched==1 && uc2int(a)==10 ) {
			mode=1; 
			set_clcd(1); 
			a= int2uc(get_tact());
			matched=1;
		}
		else {set_clcd(0);}

		set_fnd(a, 0xff, 0xff, 0xff, 0);

		b= int2uc(get_tact());
		set_fnd(a, b, 0xff, 0xff, 0);

		c= int2uc(get_tact());
		set_fnd(a, b, c, 0xff, 0);

		d= int2uc(get_tact());
		set_fnd(a, b, c, d, 1);

		switch(mode){
			case 0: matched = set_clcd_OX(uc2int(a), uc2int(b), uc2int(c), uc2int(d), password); sleep(1);  break;
			case 1: password=uc2int(a)*1000+uc2int(b)*100+uc2int(c)*10+uc2int(d); set_clcd_Start();  break;

		}

	}
	
	
  	return 0;
	
}






