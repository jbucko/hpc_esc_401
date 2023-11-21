#include <stdio.h>
#include <stdlib.h>
int N = 20;

typedef struct complex_vector{
	int* real;
	int* img;
} complex_vector;

int linear_combination(complex_vector *xx, complex_vector *yy, int aa, complex_vector *zz){
	for (int i = 0;i < N;i++){
		zz->real[i] = xx->real[i] + aa*yy->real[i];
		zz->img[i] = xx->img[i] + aa*yy->img[i];
	}
	return 0;
}

int main(void){

	complex_vector x,y,z;
	x.real = new int[N*sizeof(int)];
	x.img = new int[N*sizeof(int)];
	y.real = new int[N*sizeof(int)];
	y.img = new int[N*sizeof(int)];
	z.real = new int[N*sizeof(int)];
	z.img = new int[N*sizeof(int)];

	int a = 10,i;
	for (i=0;i<N;i++){
		x.real[i] = i%10;
		x.img[i] = i/10;
		y.real[i] = i%10 + 1;
		y.img[i] = i/10 + 1;
	}
	linear_combination(&x,&y,a,&z);

	for (i = 0;i < N;i++){
		printf("real: x[i]=%d, y[i]=%d, a=%d, x[i]+ay[i]=%d \n",x.real[i],y.real[i],a,z.real[i]);
		printf("imaginary: x[i]=%d, y[i]=%d, a=%d, x[i]+ay[i]=%d \n",x.img[i],y.img[i],a,z.img[i]);
	}

	delete[] x.real;
	delete[] x.img;
	delete[] y.real;
	delete[] y.img;
	delete[] z.real;
	delete[] z.img;
	
	return 0;
}
