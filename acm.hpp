#ifndef ACM_HPP
#define ACM_HPP
namespace acm{
    template<class T>
    inline void swap(T& a,T& b)
    {
        T temp=a;a=b;b=temp;
    }
    int read()
    {
        int ans=0,sign=1;
        char t=getchar();
        while((t>'9'||t<'0')&&t!='+'&&t!='-')t=getchar();
        if(t=='-'||t=='+'){
            sign=t=='-'?-1:1;
            t=getchar();
        }
        while(t>='0'&&t<='9')ans*=10,ans+=t-'0',t=getchar();
        return ans*sign;
    }
    //Number Theory
    template<class T>
    T gcd(T a,T b)
    {
        if(b==0)return a;
        return gcd(b,a%b);
    }
    template<class T>
    T ex_gcd(T a,T b,int& x,int& y)
    {
        if(b==0){
            x=1,y=0;
            return a;
        }
        T ret=ex_gcd(b,a%b,y,x);
        y=y-a/b*x;
        return ret;
    }
}
#endif