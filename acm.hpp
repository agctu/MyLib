#ifndef ACM_HPP
#define ACM_HPP
#include <cstdio>
namespace acm{
    //just return what the default value using the default constructor of the type T
    template<class T> class Nul{
        template<class r>
        inline T operator[](r){return T();}
    };
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
    inline int qpow(int a,int p,int mod)
    {
        long long ans=1,acc=a;
        while(p){
            if(p&1)ans=ans*acc%mod;
            p>>=1;acc=acc*acc%mod;
        }
        return ans;
    }
    //dt has default constructor and has the method merge
    //ot has default constructor and it can be add to it self as well as to dt
    //st is the size type int , long or long long.
    template<class dt,class ot,class st=int>
    class SegTree{
        struct Node{
            st l,r,m,lp,rp;
            dt v;
            ot lt;
            Node(st l,st r):lp(-1),rp(-1),l(l),r(r),v(),lt(),m((l+r)/2){}
            Node(){}
        };
        vector<Node> dat;
    public:
        template<class iterable>
        SegTree(const iterable& src,st s,st e)//exclusive
        {
            dat.resize((e-s)*2-1);//avoid the address of data in vector changes which will cause the loss of the reference
            st i=0;
            build(s,e,src,i);
        }
        void update(st l,st r,ot v,st cur=0)
        {
            Node& cs=dat[cur];
            if(l<=cs.l&&cs.r<=r){
                cs.v=cs.v+v*(cs.r-cs.l);
                cs.lt=cs.lt+v;
            }
            else{
                pushdown(cur);
                if(l<cs.m) update(l,r,v,cs.lp);
                if(r>cs.m) update(l,r,v,cs.rp);
                cs.v=dat[cs.lp].v+dat[cs.rp].v;
            }
        }
        dt sum(st l,st r,st cur=0)//exclusive for r
        {
            Node& cs=dat[cur];
            if(l<=cs.l&&cs.r<=r){
                return cs.v;
            }
            else{
                pushdown(cur);
                return (l<cs.m?sum(l,r,cs.lp):dt())+(r>cs.m?sum(l,r,cs.rp):dt());
            }
        }
        
    private:
        template<class iterable>
        void build(st l,st r,const iterable& src,st& cur)/* (st l,st r,const iterable& src,st& cur) */
        {
            Node& cs=dat[cur++];
            cs=Node(l,r);
            if(r-l>1){
                cs.lp=cur;
                build(cs.l,cs.m,src,cur);
                cs.rp=cur;
                build(cs.m,cs.r,src,cur);
                cs.v=dat[cs.lp].v+dat[cs.rp].v;
            }
            else{
                cs.v=src[l];
            }
        }
        void pushdown(st cur)
        {
            Node& cs=dat[cur];
            if(cs.lt==ot()||cs.r-cs.l<=1)return;
            else{
                Node& ls=dat[cs.lp],& rs=dat[cs.rp];
                ls.v=ls.v+cs.lt*(ls.r-ls.l);
                ls.lt=ls.lt+cs.lt;
                rs.v=rs.v+cs.lt*(rs.r-rs.l);
                rs.lt=rs.lt+cs.lt;
                cs.lt=ot();
            }
        }
    };
    //Tree Problem
    template<int N>
    struct LBase{
        long long base[N+1];
        LBase()
        {
            fill(base,base+N+1,0);
        }
        LBase& operator+=(long long x)
        {
            for(int i=N;i>=0;--i){
                if((x&(1ll<<i))==0)continue;
                if(base[i]==0){base[i]=x;break;}
                else{
                    x=x^base[i];
                }
            }
            return *this;
        }
        LBase& operator+=(const LBase& x)
        {
            for(int i=0;i<N+1;++i){
                this->operator+=(x.base[i]);
            }
            return *this;
        }
        long long max()
        {
            long long ret=0;
            for(int i=N;i>=0;--i){
                if(ret^base[i]>ret)ret=ret^base[i];
            }
            return ret;
        }
    };
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
    //N max index 
    template<int N,int MOD> class MulInv{
        int *inv;
        public:
        MulInv()
        {
            inv=new int[N+1];
            inv[1]=1;
            for(int i=2;i<=N;++i)inv[i]=(long long)(MOD-MOD/i)*inv[MOD%i]%MOD;
        }
        template<class ARRAY>
        MulInv(ARRAY& a)
        {
            inv=new int[N+1];
            int *s=new int[N+1],*sv=new int[N+1];
            s[0]=1;
            for(int i=1;i<=N;++i)s[i]=(long long)s[i-1]*a[i]%MOD;
            sv[N]=qpow(s[N],MOD-2,MOD);
            for(int i=N-1;i>=1;--i)sv[i]=(long long)sv[i+1]*a[i+1]%MOD;
            for(int i=1;i<=N;++i)inv[i]=(long long)sv[i]*s[i-1]%MOD;
            delete[] s;delete[] sv;
        }
        long long get(int n)const
        {
            return inv[n];
        }
        long long operator[](int n)const
        {
            return get(n);
        }
        ~MulInv()
        {
            delete[] inv;
        }
        private:
        MulInv(const MulInv&);
    };
}
#endif