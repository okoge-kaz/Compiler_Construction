int printf ();

int x;
int y;
char c;
char d;

int main ()
{

  x = (c = 127);
  y = x+1;
  d = c+1;  
  printf ("x=%d,y=%d,c=%d,d=%d\n",x,y,c,d);  
}
