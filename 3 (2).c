#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <linux/limits.h>
 
double Factorial(int n);
double Power(double number, int pow);
int ToInt(char symbol);
double FromStr(int j, char bufstr[50]);

//-----------------------------------programs-----------------------------------------//
 
int main(int argc, char *argv[])
{
   char const *progname;
   extern char *program_invocation_short_name;
   progname = program_invocation_short_name;

   if(argc != 3)
   {
      fprintf(stderr, "%s: Wrong number of arguments \n", progname);
      return 1;
   }

   int currentProcess=0 , numb;
   pid_t pid;
   double member, x;
   int N = atoi(argv[1]);
   int n = atoi(argv[2]);
   
   char path_res[PATH_MAX] = "./tmp/results.txt";
   char path_sum[PATH_MAX] = "./summ.txt";

   FILE *fSum = fopen(path_sum, "w+");
   if (fSum == NULL)
   {
      fprintf(stderr, "%s: open file error %s %s\n", progname, strerror(errno),path_sum);
      return -1;
   }

   FILE *fRes = fopen(path_res, "w+");
   if(fRes==NULL)
   {
      fprintf(stderr, "%s: open file error %s %s\n", progname, strerror(errno),path_res);
      return -1;
   }

   for (int i = 0;i<N; i++)
   {
      x = 2*M_PI*i/N;
      if((x>M_PI/2)&&(x<=M_PI))
         x = M_PI - x;
      if((x>M_PI)&&(x<=3*M_PI/2))
         x = (-1)*(x-M_PI);
      if((x>3*M_PI/2)&&(x<=2*M_PI))
         x = (-1)*(2*M_PI-x);

      for(int k=0; k<n; k++)
      {
         if(currentProcess >= n)
         {
            wait(NULL);
            currentProcess--;
         }

         pid = fork();
            
         if(pid<0)
            fprintf(stderr,"Неверное количество процессов.");
         if(pid == 0)
         {
            numb = 2*k+1;
            member=Power(x,numb)/Factorial(numb);
            if(k%2!=0)
               member = member*(-1);
            fRes = fopen(path_res, "a+");
            fprintf(fRes,"%d ",getpid());
            printf("%d ",getpid());
            fprintf(fRes,"%d ",i);
            printf("%d ",i);
            fprintf(fRes, "%.016f \n", member);
            printf("%.016f \n", member);             
            fclose(fRes);
            exit(0);
         }
         else 
            if(pid>0) 
               currentProcess++;
      }
   };

   int res;
   while((res = wait(NULL))>0)
      if(errno != ECHILD && res == -1)
      {
         fprintf(stderr,"%d : Error: %s! \n", getpid(),strerror(errno));
         exit(1);
      }

   int bufSum, j, k, l, currentValue;
   char str[50], bufstr[50], bufI[5];
   double sum;

   double *mas;
   mas = (double*)malloc(N*sizeof(double));

   fRes = fopen(path_res,"r");

   while(!feof(fRes))
   {       
      j = k = l = 0;
      if(fgets(str,50,fRes));

      do
      {  
         if (str[j]==' ')
            k++;
         j++;
      }while(k!=1);
      k=0;

      do
      {
         bufI[k] = str[j];    
         j++;
         k++;
      }while(str[j]!=' ');
      bufI[k]='\0';
      j++;
          
      do
      {
         bufstr[l] = str[j];    
            j++;
            l++;
      }while(str[j]!=' ');

      bufstr[l] = ' ';
      j=j+1;      
      
          
      currentValue=0;
      bufSum=0;
      k=k-1;
      for(int m=0;m<=k;m++)
      {
         bufSum = ToInt(bufI[m])*Power(10,k-m);
		
         currentValue += bufSum;
      }

      sum = FromStr(j-k-2,bufstr);
      mas[currentValue] += sum;
   }

   fSum = fopen(path_sum,"a+");

   for(j=0;j<N;j++)
   {
      //printf("y[%d] = %.06f\n",j,mas[j]);
      fprintf(fSum,"y[%d] = %.015f\n",j,mas[j]);
   }

   if ( fclose (fSum) == EOF)
   {
      fprintf(stderr, "%s: close file error %s %s \n", progname, strerror(errno),path_res );
      return -1;
   }

   return 1;
}

//-----------------------------------functions-----------------------------------------//

double Factorial(int n)
{
   double result=1;
   for(int i=2; i<=n; i++)
      result *=i;
   return result;
}
 
double Power(double number, int pow)
{
   double result = 1;
   for(int i=0; i<pow; i++)
      result *=number;
   return result;
}
 
int ToInt(char symbol)
{
   return symbol - '0';      
}
 
double FromStr(int j, char bufstr[50])
{
   int N = j, k=0, count=0;
   bool flag=true;
   double number=0,bufnumber=0;
   char *c;     
   j=j-2;
   while(k<N)
   {
      if(bufstr[k]=='-')
      {
         flag=false;
         j--;
      }
      else
         if(bufstr[k]=='.')
            count=0;
         else
         {
            bufnumber = 0;
            count++;
            bufnumber = Power(10,j);
            bufstr[k];
            bufnumber *= ToInt(bufstr[k]);
            j--;
            number += bufnumber;
         }
      k++;
   }
   if (flag==false)
      number *= (-1);
   number /= Power(10,count);
   return number;
}
