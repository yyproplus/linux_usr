
#if 1
#include <stdio.h>
#include <string.h>
void fun(char *s,int n,int *t){
    int k=0,i=0;
    s[n]='a';
    s[n+1]='\0';
    while(s[k]!='a')++k;
    if(k==n){
        *t=0;
    }
    else{
        for(i=k;i<n;i++){
            s[i]=s[i+1];
        }
        s[i]='\0';
    }
}
int main(){
    char s[20];
    int len,t;
    gets(s);
    len=strlen(s);
    fun(s,len,t);
    if(t==0){
        printf("no exist\n");
    }else printf("s=%s\n",s);
    return 0;
}
#endif
int V(unsigned int x){
    int c=0;
    while (x)
    {
        c++;
        x=x&(x-1);
    }
    return c;
}