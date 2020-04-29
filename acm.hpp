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
    inline int qpow(int a,int p,int mod)
    {
        long long ans=1,acc=a;
        while(p){
            if(p&1)ans=ans*acc%mod;
            p>>=1;acc=acc*acc%mod;
        }
        return ans;
    }
    template<class dt,class st=int>//dt has default constructor and has the method merge
    class SegTree{
        struct Node{
            st l,r,m,pl,pr;
            dt v;
            Node(st l,st r):l(l),r(r),v(),m((l+r)/2){}
        };
        vector<Node> dat;
    public:
        template<class iterable>
        SegTree(const iterable& src,st s,st e)//exclusive
        {
            build(&iterable[s],&iterable[e]);
        }
        dt sum(st s,st e)
        {

        }
        
    private:
        template<class iter>
        void build(iter l,iter r,st cur=0)
        {
            cur=dat.size();
            dat.emplace_back()
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