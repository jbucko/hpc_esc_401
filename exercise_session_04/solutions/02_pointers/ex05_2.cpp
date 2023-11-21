#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int print_array(int ** arr){
		cout<<"arr[0]:"<<arr[0][0]<<","<<arr[0][1]<<","<<arr[0][2]<<","<<arr[0][3]<<","<<arr[0][4]<<"\n";
		cout<<"arr[1]:"<<arr[1][0]<<","<<arr[1][1]<<","<<arr[1][2]<<","<<arr[1][3]<<","<<arr[1][4]<<"\n";
	return 0;
}

int main(void){

	int ** M = new int *[2];
	int ** M2 = new int *[2];
	M[0] = new int[5]{0,1,2,3,4};
	M[1] = new int[5]{5,6,7,8,9};
	M2[0] = new int[5];
	M2[1] = new int[5];
	
	cout<<"Input array:"<<"\n";
	print_array(M);
	for (int i=0;i<5;i++){
		*(*M2+i) = *(*(M+1)+4-i);
		*(*(M2+1)+i) = *(*M+4-i); 
	}
	cout<<"Output array:"<<"\n";
	print_array(M2);

	delete[] M[0];
	delete[] M[1];
	delete[] M;
	delete[] M2[0];
	delete[] M2[1];
	delete[] M2;

	return 0;
}
