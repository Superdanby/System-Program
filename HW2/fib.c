#include <stdio.h>
int main()
{
    long long fir=0, sec=1;
    for(int i=0; i<=90; i++)
    {
        printf("F%d: %lld\n", i, fir);
        long long sum=fir+sec;
        fir=sec;
        sec=sum;
    }
    return 0;
}
