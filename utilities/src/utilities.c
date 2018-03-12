#include"inc/utilities.h"

u32_t my_strlen(char *str)
{
  u16_t i;
  for(i=0;(str[i]);i++);
  return i;
}

void my_strcpy(char *dst, char *src)
{
  u16_t i;
  for(i=0;(dst[i]=src[i]);i++);
}

void my_strcat(char *src, char *dst)
{
  u16_t i,j;
  for(i=0;(src[i]);i++);
  for(j=0;(src[i]=dst[j]);i++,j++);
}

void my_strrev(char *str, int j)
{
	int i=0;
	j=j-1;
	char temp;
	while(i<j)
	{
		temp=str[j];
		str[j]=str[i];
		str[i]=temp;
		i++;
		j--;
	}
}

int my_atoi(char *str)
{
	int result=0;
	int sign=1;
	if(*str=='-')
	{	
		sign=-1;
		str++;
	}
	while(*str!='\0')
	{
		result=*str-'0'+10*result;
		str++;
	}
	return result*sign;
}

/*void my_itoa(int a,char *str)
{
	int i=0,status=0;
	if(a<0)
	{
		status=1;
		a=-a;
	}
		
	while(a!=0)
	{
		str[i++]=a % 10+0x30;
		a=a/10;
	}
	if(status)
		str[i++]='-';
	my_strrev(str,i);
	str[i]='\0';
}*/
void my_itoa(int n, char *a)
{
  int i;
  int j;
  char c1;
  int sign = 0;


  if(n < 0)
  {
    sign = 1;
    n = -n;
  }

  i = 0;
  do
  {
    a[i++] = (char)(n % 10 + '0');
    n = n / 10;
  }while(n > 0);

  if(sign == 1)
    a[i++] = '-';
  a[i] = '\0';

  /* Now reverse it*/
  for(i = 0, j = strlen(a) - 1; i < j; i++,j--)
  {
    c1 = a[i];
    a[i] = a[j];
    a[j] = c1;
  }
  return a;
}


/*void my_ftoa(double f, char *str, int a)
{
	int temp,i=0,mul=1;
	double frac;
	temp=(int)f;
	if(temp < 0)
		frac=(float)temp-f;
	else
		frac=f-(float)temp;
	my_itoa(temp,&str[i]);
	for(i=0;str[i];i++);
	
	if(a!=0)
	{
		str[i++]='.';
		while(a)
		{
			mul=10*mul;
			--a;
		}	
		frac=frac*mul;	
		temp=(int)frac;
		my_itoa(temp,&str[i]);
	}
}*/
 
void my_ftoa(float fvalue,char *buffer, int MAXno_of_fractional_digits)
{
  int decvalue;
  unsigned short int no_of_fractional_digits = 0;
  float fracvalue;
  int len;
  int sign = 1;;
  int i;

  // Get the integer portion of the float value and convert it to string
  if(fvalue < 0.0)
  {
    sign = -1;
    fvalue = -fvalue;
  }

  decvalue = (int) fvalue;
  fracvalue = fvalue - (float) decvalue;

  if(decvalue == 0)
  {
    if(sign == -1)
      strcpy(buffer,"-0");
    else
      strcpy(buffer,"0");
	}
  else
  {
    if(sign == -1)
      decvalue = -decvalue;
    my_itoa(decvalue,buffer);
   }
  strcat(buffer,".");


  if(MAXno_of_fractional_digits == 0)
  {
    strcat(buffer,"0");
	//vts_strcat(buffer,"\0");
  }
  else
  {
    i = strlen(buffer);
    while(no_of_fractional_digits < MAXno_of_fractional_digits)
    {
      fracvalue =(fracvalue * 10);
      no_of_fractional_digits++;
      buffer[i++] = (int)fracvalue + '0';
      fracvalue = fracvalue - (int) fracvalue;
    }
    buffer[i] = '\0';
  }
  //return;
} 

int my_atof(char *str,double *number)
{
  char ch;
  double num=0;
  int divide = 0;
  double divfact = 10;
  int sign = 1;

  ch = *str++;
  if(ch == '-')
    sign = -1;
  else if(ch == '+')
    sign =1;
  else
    str--;

  while((ch = *str++) != '\0')
  {
    if(isdigit((int)ch))
    {
      if(divide == 0)
      {
        num=num*10;
        num=num+(ch - '0');
      }
      else
      {
        num = num+((ch - '0')/divfact);
        divfact = divfact * 10;
      }
    }
    else if(ch == '.')
    {
      if(divide == 1)
		return 1;
      divide = 1;
      continue;
    }
    else
      return 1;
  }
  *number = sign * num;
  return 0;
}

u32_t atox( char *str )
{
    u32_t value = 0;

    for(;; ++str ) 
	switch( *str )
    {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            value = (value << 4) | (*str & 0xf);
            break;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            value = ((value << 4 )| 9 )+ (*str & 0xf);
            break;
        default:
            return value;
    }
}

char* my_xtoa(u32_t n, char *a)
{
  int i;
  int j;
  char c1;
  int sign = 0;
  i = 0;
  do
  {
    if ((n % 16)>9)
      a[i++] = (char)(n % 16 + '7');
    else
      a[i++] = (char)(n % 16 + '0');
    n = n / 16;
  }while(n > 0);

  if(sign == 1)
    a[i++] = '-';
  a[i] = '\0';

  /* Now reverse it*/
  for(i = 0, j = my_strlen(a) - 1; i < j; i++,j--)
  {
    c1 = a[i];
    a[i] = a[j];
    a[j] = c1;
  }
  return a;
}
