#include <bits/stdc++.h>


int main(){

    void * a[40];
    for (int i = 0; i < 40; i++){

        printf("lol\n");
        a[i] = malloc(100000000000);

    }

    std::chrono::milliseconds timespan(5000);
    std::this_thread::sleep_for(timespan);
    for(int i = 0; i < 40; i++){

        free(a[i]);
    }
    return 0;
}