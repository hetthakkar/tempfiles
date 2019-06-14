#include <bits/stdc++.h>


int main(){

    int n = 2000000;
    float *abc;

    abc = (float *)malloc(n * sizeof(float));

    int i;

    #pragma acc parallel loop private(i) copyout(abc[0:n])
    for(i = 0; i < n; i ++){

        abc[i] = i/1.0;

    }

    std::cout << abc[34] << "\n";
    return 0;
}