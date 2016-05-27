/* Automat komórkowy jednowymiarowy */
#include <stdio.h>
//#include <time.h>
#include <stdlib.h> /* dla funkcji exit */
#include <curses.h>
#include <locale.h>

/*#include "vm.h"
#include "load.h"
#include "errorcode.h"
#include "static.h" */

#include "aut_sample01.c"

void koniec(char *msg);

#define MAX_KOMORKA 500
int komorka0[MAX_KOMORKA]; /* pierwsza linijka */
int komorka1[MAX_KOMORKA];
int max_k=0;               /* maksymalna ilość komórek */

void pocz_k() /* initializujemy pierwszą linijkę */
{
int x;
  for(x=0;x<max_k;x++)
    {
    komorka0[x]=0;
    };
  komorka0[max_k / 2] =1;
}

int kolor_kx(int x1,int x2,int x3)   /* oblicz kolor x-tej komórki */
{
  if( x1==0 && x2==0 && x3==0 ) { return 0; }
  if( x1==0 && x2==0 && x3==1 ) { return 1; }
  if( x1==0 && x2==1 && x3==0 ) { return 0; }
  if( x1==0 && x2==1 && x3==1 ) { return 1; }
  if( x1==1 && x2==0 && x3==0 ) { return 1; }
  if( x1==1 && x2==0 && x3==1 ) { return 0; }
  if( x1==1 && x2==1 && x3==0 ) { return 1; }
  if( x1==1 && x2==1 && x3==1 ) { return 0; }
}

void dalej_k()         /* initializujemy pierwszą linijkę */
{
int x,a,c;

  for(x=0;x<max_k;x++) /* obliczamy każdą komórkę */
    {
    if(x-1 < 0)     {a=komorka0[max_k];} else {a=komorka0[x-1];}
    if(x+1 > max_k) {c=komorka0[0];} else {c=komorka0[x+1];}
    komorka1[x]=kolor_kx(a,komorka0[x],c);
    };

  for(x=0;x<max_k;x++) /* przenosimy wyniki do pierwszej linijki */
    {
    komorka0[x]=komorka1[x];
    };
}

int main()
{
int maxy,maxx;
int x,y;

  setlocale(LC_ALL, "pl_PL.utf8");
//  srand(time(NULL));

  initscr();
  curs_set(0);
  getmaxyx(stdscr,maxy,maxx); // maxy--; maxx--;

  if(!has_colors())       koniec("Terminal nie posiada kolorów\n");
  if(start_color() != OK) koniec("Nie umiem zainicjować kolorów\n");
  init_pair(1,COLOR_YELLOW,COLOR_YELLOW); /* kolor */
  init_pair(2,COLOR_BLACK,COLOR_BLACK);   /* puste */
  init_pair(3,COLOR_GREEN,COLOR_BLACK);   /* opis */
  attrset(COLOR_PAIR(3));
  mvprintw(y,x,"[%d,%d] Automat komórkowy ",maxy,maxx);

  max_k = maxx;
  pocz_k();
  for(y=1;y<maxy;y++)
    {
    for(x=0;x<maxx;x++)
      {
      if(komorka0[x] == 1)
        {
        attrset(COLOR_PAIR(1));
        mvprintw(y,x,"x");
        }
      else
        {
        attrset(COLOR_PAIR(2));
        mvprintw(y,x,"x");
        }
      }
    dalej_k();
    }

  refresh();
   getch();
  endwin();

return 0;
}

void koniec(char *msg)
{
  endwin();
  puts(msg);

exit(1);
}
