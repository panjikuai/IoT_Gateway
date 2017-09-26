#include "math.h"  
#include "fft.h"  
//精度0.0001弧度  

 
  
#define PI 3.1415926535897932384626433832795028841971  
 
void conjugate_complex(int n,complex in[],complex out[]);  
void c_plus(complex a,complex b,complex *c);//复数加  
void c_mul(complex a,complex b,complex *c) ;//复数乘  
void c_sub(complex a,complex b,complex *c); //复数减法  
void c_div(complex a,complex b,complex *c); //复数除法  
void fft(int N,complex f[]);//傅立叶变换 输出也存在数组f中
void ifft(int N,complex f[]); // 傅里叶逆变换  
void c_abs(complex f[],float out[],int n);//复数数组取模  
  
void conjugate_complex(int n,complex in[],complex out[])  
{  
  int i = 0;  
  for(i=0;i<n;i++){  
    out[i].imag = -in[i].imag;  
    out[i].real = in[i].real;  
  }   
}  
  
void c_abs(complex f[],float out[],int n)  
{  
  int i = 0;  
  float t;  
  for(i=0;i<n;i++){  
    t = f[i].real * f[i].real + f[i].imag * f[i].imag;  
    out[i] = sqrt(t);  
  }   
}  
  
  
void c_plus(complex a,complex b,complex *c)  
{  
  c->real = a.real + b.real;  
  c->imag = a.imag + b.imag;  
}  
  
void c_sub(complex a,complex b,complex *c)  
{  
  c->real = a.real - b.real;  
  c->imag = a.imag - b.imag;   
}  
  
void c_mul(complex a,complex b,complex *c)  
{  
  c->real = a.real * b.real - a.imag * b.imag;  
  c->imag = a.real * b.imag + a.imag * b.real;     
}  
  
void c_div(complex a,complex b,complex *c)  
{  
  c->real = (a.real * b.real + a.imag * b.imag)/(b.real * b.real +b.imag * b.imag);  
  c->imag = (a.imag * b.real - a.real * b.imag)/(b.real * b.real +b.imag * b.imag);  
}  
  
#define SWAP(a,b)  tempr=(a);(a)=(b);(b)=tempr  
  
void Wn_i(int n,int i,complex *Wn,char flag)  
{  
  Wn->real = cos(2*PI*i/n);  
  if(flag == 1){
    Wn->imag = -sin(2*PI*i/n);  
  }else if(flag == 0){
    Wn->imag = -sin(2*PI*i/n);  
  }
}  
  
//傅里叶变化  
void fft(int N,complex f[])  
{  
  complex t,wn;//中间变量  
  int i,j,k,m,n,l,r,M;  
  int la,lb,lc;  
  /*----计算分解的级数M=log2(N)----*/  
  for(i=N,M=1;(i=i/2)!=1;M++);   
  /*----按照倒位序重新排列原信号----*/  
  for(i=1,j=N/2;i<=N-2;i++){  
    if(i<j){  
      t=f[j];  
      f[j]=f[i];  
      f[i]=t;  
    }  
    k=N/2;  
    while(k<=j){  
      j=j-k;  
      k=k/2;  
    }  
    j=j+k;  
  }  
  
  /*----FFT算法----*/  
  for(m=1;m<=M;m++){  
    la=pow(2,m); //la=2^m代表第m级每个分组所含节点数       
    lb=la/2;    //lb代表第m级每个分组所含碟形单元数  
                 //同时它也表示每个碟形单元上下节点之间的距离  
    /*----碟形运算----*/  
    for(l=1;l<=lb;l++){  
      r=(l-1)*pow(2,M-m);     
      for(n=l-1;n<N-1;n=n+la){ //遍历每个分组，分组总数为N/la   
        lc=n+lb;  //n,lc分别代表一个碟形单元的上、下节点编号       
        Wn_i(N,r,&wn,1);//wn=Wnr  
        c_mul(f[lc],wn,&t);//t = f[lc] * wn复数运算  
        c_sub(f[n],t,&(f[lc]));//f[lc] = f[n] - f[lc] * Wnr  
        c_plus(f[n],t,&(f[n]));//f[n] = f[n] + f[lc] * Wnr  
      }  
    }  
  }  
}  
  
//傅里叶逆变换  
void ifft(int N,complex f[])  
{  
  int i=0;  
  conjugate_complex(N,f,f);  
  fft(N,f);  
  conjugate_complex(N,f,f);  
  for(i=0;i<N;i++){  
    f[i].imag = (f[i].imag)/N;  
    f[i].real = (f[i].real)/N;
  }  
}  

#define FFT_N  64

void FFT_DataInput(const uint8_t *data, uint32_t len)
{
    complex fftDataBuff[FFT_N];
    // fft data is uint16_t array
    if (len < FFT_N*2){
      return;
    }
    uint16_t *buff =  (uint16_t *)data;
    uint16_t step = len/FFT_N;
    step = step - (step%2);
    // select 64 data from the input
    for (uint16_t i = 0; i < FFT_N; i++){
        fftDataBuff[i].real = (float)buff[i*step];
        fftDataBuff[i].imag = 0;
    }

}

uint32_t FFT_Execute(int N,complex f[])
{
    fft(N,f);
    return 0;
}

// float level_low = 0;
// float level_medium = 0;
// float level_high = 0;
// for (uint16_t i = 0; i< (FFT_N/2);i++){
//     if (i < (FFT_N/6)){
//         level_low += sqrt( fft_result[i].real * fft_result[i].real+fft_result[i].imag * fft_result[i].imag);
//     }else if (i < (FFT_N/3)){
//         level_medium += sqrt( fft_result[i].real * fft_result[i].real+fft_result[i].imag * fft_result[i].imag);
//     }else{
//         level_high += sqrt( fft_result[i].real * fft_result[i].real+fft_result[i].imag * fft_result[i].imag);
//     }
// }

// level_low = level_low/(FFT_N/6);
// level_medium = level_medium/(FFT_N/6);
// level_high = level_high/((FFT_N/2)-FFT_N/3);