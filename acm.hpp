#ifndef ICPC_HPP
#define ICPC_HPP 
#include <cstdio>
#include <cmath>
namespace icpc{
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
	//use add mul to operator dt, ot is replaced by dt.
	//dt has default constructor and has the method merge
	//st is the size type int , long or long long.
	template<class dt,class st=int>
	class SegTree{
		struct Node{
			st l,r,m,lp,rp;
			dt v;
			dt lt;
			Node(st l,st r):lp(-1),rp(-1),l(l),r(r),v(),lt(),m((l+r)/2){}
			Node(){}
		};
		vector<Node> dat;
	public:
		SegTree(){}
		template<class iterable>
		SegTree(const iterable& src,st s,st e)
		{
			build(src,s,e);
		}
		template<class iterable>
		void build(const iterable& src,st s,st e)//exclusive
		{
			dat.resize((e-s)*2-1);//avoid the address of data in vector changes which will cause the loss of the reference
			st i=st();
			build(s,e,src,i);
		}
		void update(st l,st r,dt v,st cur=0)
		{
			Node& cs=dat[cur];
			if(l<=cs.l&&cs.r<=r){
				cs.v=cs.v+mul(v,(cs.r-cs.l));
				cs.lt=add(cs.lt,v);
			}
			else{
				pushdown(cur);
				if(l<cs.m) update(l,r,v,cs.lp);
				if(r>cs.m) update(l,r,v,cs.rp);
				cs.v=add(dat[cs.lp].v,dat[cs.rp].v);
			}
		}
		dt query(st l,st r,st cur=0)//exclusive for r
		{
			Node& cs=dat[cur];
			if(l<=cs.l&&cs.r<=r){
				return cs.v;
			}
			else{
				pushdown(cur);
				return add((l<cs.m?query(l,r,cs.lp):dt()),(r>cs.m?query(l,r,cs.rp):dt()));
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
				cs.v=add(dat[cs.lp].v,dat[cs.rp].v);
			}
			else{
				cs.v=src[l];
			}
		}
		void pushdown(st cur)
		{
			Node& cs=dat[cur];
			if(cs.lt==dt()||cs.r-cs.l<=1)return;
			else{
				Node& ls=dat[cs.lp],& rs=dat[cs.rp];
				ls.v=add(ls.v,mul(cs.lt,(ls.r-ls.l)));
				ls.lt=add(ls.lt,cs.lt);
				rs.v=add(rs.v,mul(cs.lt,(rs.r-rs.l)));
				rs.lt=add(rs.lt,cs.lt);
				cs.lt=dt();
			}
		}
	};
	//binary-heap
	template<class T,int sz,class CMP>
	struct Heap{
		T mem[sz+1];
		const CMP _cmp=CMP();
		int siz=0;
		inline void clear() { siz=0; }
		void push(T x){
			mem[++siz]=x;
			adjustUp(siz);
		}
		inline bool empty() { return siz==0; }
		inline T top(){ return mem[1]; }
		void pop(){
			mem[1]=mem[siz--];
			adjustDown(1);
		}
	private:
		void adjustUp(int p){
			while(!cmp(p/2,p))swap(mem[p],mem[p/2]),p/=2;
		}
		void adjustDown(int p){
			int np;
			while(!cmp(p,np=cmp(p*2,p*2+1)?p*2:p*2+1))swap(mem[p],mem[np]),p=np;
		}
		bool cmp(int l,int r){
			if(l<1||r>siz)return true;
			if(l>siz||r<1)return false;
			return _cmp(mem[l],mem[r]);
		}
	};
			
	//tree-like array or binary indexed tree
	template<class T,int sz>
	struct BinaryIndexedTree{
		T mem[sz+1]={T()};
		void build(T *s,int n)
		{
			--s;//s[0] is never accessed 
			for(int i=1;i<=n;++i){
				mem[i]+=s[i];
				int f=lowbit(i)+i;
				if(f<=sz)mem[f]+=mem[i];
			}
		}
		void update(T v,int n)
		{
			while(n<=sz){
				mem[n]+=v;
				n+=lowbit(n);
			}
		}
		T query(int n)
		{
			T ret=T();
			while(n){
				ret+=mem[n];
				n-=lowbit(n);
			}
			return ret;
		}
	private:
		inline long long lowbit(long long x){return x&(-x);}
	};
	template<class T,int sz>
	struct SparseTable{
		typedef T (*Add)(const T& a,const T& b);
		T mem[sz+1][20];//ceil(log(1e6))==20;
		int lg[sz+1];
		Add add;
		SparseTable(Add add):add(add){}
		SparseTable(T *dt,int len,Add add):add(add)
		{
			build(dt,len);
		}
		void build(T *dt,int len)
		{
			for(int i=0;i<len;++i){
				mem[i][0]=dt[i];
			}
			for(int i=1;i<19;++i){
				for(int j=0;j<len;++j){
					if(j+(1<<i-1)>=sz)break;
					mem[j][i]=add(mem[j][i-1],mem[j+(1<<i-1)][i-1]);
				}
			}
			getlg();
		}
		T get(int l,int r)//exclusive
		{
			int lb=lg[r-l];
			return add(mem[l][lb],mem[r-(1<<lb)][lb]);
		}
	private:
		void getlg()
		{
			lg[1]=0;
			for(int i=2;i<=sz;++i)lg[i]=lg[i/2]+1;
		}
	};
//Tree Problem
	//self implemented set
	template<class T,int MSZ>
	struct set{
		struct node{ int l=-1,r=-1,f=-1; T v=T(); };
		int  head,sz;
		node mem[MSZ];
		set():head(-1),sz(0){}
		inline void insert(T v){insert(v,head);}
		void insert(T v,int &rt,int f=-1)
		{
			if(rt==-1){
				rt=sz++; 
				mem[rt]=node();
				mem[rt].v=v;
				mem[rt].f=f;
			}
			else if(v<mem[rt].v){
				insert(v,mem[rt].l,rt);
			}
			else if(v==mem[rt].v){
				return;
			}
			else{
				insert(v,mem[rt].r,rt);
			}
		}
		inline int find(T v){return find(v,head);}
		int find(T v,int rt)
		{
			if(rt==-1)return -1;
			if(v==mem[rt].v){
				return rt;
			}
			if(v<mem[rt].v)return find(v,mem[rt].l);
			return find(v,mem[rt].r);
		}
		inline int  begin() { int t=head; while(~mem[t].l)t=mem[t].l; return t; }
		inline int end() { return -1; }
		int next(int pre)
		{
			int t;
			if(~mem[pre].r){
				t=mem[pre].r;
				while(~mem[t].l)t=mem[t].l;
				return t;
			}
			else{
				t=pre;
				while(~t&&mem[t].v<=mem[pre].v) t=mem[t].f;
				return t;
			}
		} 
		inline T operator[](int id) { return mem[id].v; }
		inline void clear() { sz=0; head=-1; }
	private:
		set(const set&){}
		
	};
	//Splay Tree self balanced tree
	// this version only inner operation, and use ch[0][0] as the root;
	// please definite this in the global scope
	// all arguments should be the inner addr of the node
	template<class T, int sz>
	struct SplayTree{
		T v[sz];
		int f[sz],ch[sz][2],cnt[sz],siz[sz],& root=ch[sp][0],len=2;
		bool rv[sz];
		const int sp=0,ninf=1,inf=2;
		SplayTree()
		{ 
			f[sp]=-1,f[ninf]=sp,f[inf]=ninf;
			ch[sp][0]=ninf,ch[sp][1]=0,ch[ninf][0]=0,ch[ninf][1]=inf,ch[inf][0]=ch[inf][1]=0;
			cnt[sp]=cnt[ninf]=cnt[inf]=0,siz[sp]=siz[ninf]=siz[inf]=0;
			//define you inf and ninf here for compare.
		}
		//if the interface insert or delete increase or decrease the cnt, REMEMBER to maintain
		int adjacent(int cur,int dire)//return the inner addr may return inf or ninf. dire -1 or 1
		{
			assert(dire==-1||dire==1);
			dire=max(dire,0);//conver to son type.
			assert(dire&&cur!=2||!dire&&cur!=1);
			splay(cur);
			cur=ch[cur][dire];
			while(ch[cur][dire^1])cur=ch[cur][dire^1];
			return cur;
		}
		int rank(int cur)//use inner addr
		{
			assert(siz[0]==0);
			splay(cur);
			return	siz[ch[cur][0]]+1;
		}
		void erase(int cur)//no release and clear code for the node here, only erase it from the tree.
		{
			int pr=adjacent(cur,-1),nx=adjacent(cur,1);
			splay(nx),splay(pr,nx);
			ch[pr][1]=0;
			maintain(pr),maintain(nx);
		}
		void insert(int cur,int nx=0)//no new node code here, only insert it.
		{
			int pr=adjacent(nx,-1);
			splay(nx),splay(pr,nx);
			ch[pr][1]=cur,f[cur]=pr;
			maintain(pr),maintain(nx);
		}
		int kth(int k)//return inner addr
		{
			assert(siz[0]==0);
			int cur=root;
			while(k<=siz[ch[cur][0]]&&cur!=ninf||k>siz[cur]-siz[ch[cur][1]]&&cur!=inf){
				if(k<=siz[ch[cur][0]])cur=ch[cur][0];
				else k-=siz[cur]-siz[ch[cur][1]],cur=ch[cur][1];
			}
			return cur;
		}
		void rotate(int cur)
		{
			assert(cur!=0&&f[cur]!=0);
			int fa=f[cur],gr=f[fa],so=ch[cur][r(cur)^1],rc=r(cur),rf=r(fa);
			ch[cur][rc^1]=fa,ch[fa][rc]=so,ch[gr][rf]=cur;
			f[cur]=gr,f[fa]=cur,f[so]=fa;//the f[0] may changed here but I think it doesn't matter.
			maintain(fa),maintain(cur);
		}
		void splay(int cur,int tg=0)//rotate the node cur to the children of tg(default root).
		{
			assert(cur>0&&cur!=tg);//we don't check if tg is one of the ancestors of cur.
			while(f[cur]!=tg){
				assert(cur>0&&f[cur]>0);
				if(f[f[cur]]!=tg)rotate(r(cur)==r(f[cur])?f[cur]:cur);
				rotate(cur);
			}
		}
		void maintain(int cur)
		{
			assert(cur!=0&&siz[0]==0);//the siz[0] should always be 0, or this make an error
			siz[cur]=cnt[cur]+siz[ch[cur][0]]+siz[ch[cur][1]];
		}
		bool r(int cur)
		{
			assert(cur!=0);//f[cur]==0 is permitted.
			return ch[f[cur]][1]==cur;
		}
	//	void pushdown(int cur)
	//	{
	//		if(!rv[cur])return;
	//		assert(cur!=sp&&cur!=ninf&&cur!=inf);
	//		rv[cur]=false,rv[ch[cur][0]]^=1,rv[ch[cur][1]]^=1;
	//		swap(ch[cur][0],ch[cur][1]);
	//	}
	//	void reverse(int l,int r)//to use this add pushdown in the function you need to use in the same file.
	//	{
	//		l=adjacent(l,-1),r=adjacent(r,1);
	//		splay(r),splay(l,r);
	//		rv[ch[l][1]]^=1;
	//	}
	};
	//Linear Base
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
				if((ret^base[i])>ret)ret=ret^base[i];
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
	//N : max index 
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
//computional geometry
	int sign(double x,double eps=1e-8)
	{
		if(fabs(x)<eps)return 0;
		if(x>0)return 1;
		return -1;
	}
	//Point subset
	struct Point{
		double x,y;
		Point():x(0.),y(0.){};
		Point(double x,double y):x(x),y(y){}
	};
	Point operator+(const Point& a,const Point& b)
	{
		return Point(a.x+b.x,a.y+b.y);
	}
	Point operator-(const Point& a,const Point& b)
	{
		return Point(a.x-b.x,a.y-b.y);
	}
	//two components vector
	struct Vec{
		double x,y;
		Vec():x(0.),y(0.){}
		Vec(double x,double y):x(x),y(y){}
		//get the length of the vector
		double len()
		{
			return sqrt(x*x+y*y);
		}		
		double len2()
		{
			return x*x+y*y;
		}
		Vec& operator+=(const Vec& a)
		{
			x+=a.x;
			y+=a.y;
			return *this;
		}
		Vec& operator-=(const Vec& a)
		{
			x-=a.x;
			y-=a.y;
			return *this;
		}
		friend double operator^(const Vec& a,const Vec& b)//x multiply multiply
		{
			return a.x*b.y-b.x*a.y;
		}
		friend double operator*(const Vec& a,const Vec& b)
		{
			return a.x*b.x+a.y*b.y;
		}
		friend Vec operator+(const Vec& a,const Vec& b)
		{
			Vec ret(a);
			ret+=b;
			return ret;
		}
		friend Vec operator-(const Vec& a,const Vec& b)
		{
			Vec ret(a);
			ret-=b;
			return ret;
		}
		double angle()const
		{
			return atan2(y,x);//(-pi,pi]
		}
		Vec e()
		{
			return Vec(x/len(),y/len());
		}
		double distance(const Vec& a,const Vec& b)
		{
			return (a-b).len();
		}
		void rotate(double agl)//(-pi,pi] Please use the value.
		{
			double x=this->x*cos(agl)-this->y*sin(agl),y=this->x*sin(agl)+this->y*cos(agl);
			this->x=x,this->y=y;
		}

	};
	struct Line{
		Vec dir;//direction
		Point pnt;//a point to specify the position.
		Line(const Vec& v,Point& p):dir(v),pnt(p){}
		int relation(Point p)//return -1 when the point is on the right of the line, return 1 when the point is on the left of the line, return 0 when the point is just on the line.
		{
			Vec t(p.x-pnt.x,p.y-pnt.y);
			return sign(dir*t);
		}
		double distance(const Point& p)
		{
			Vec t(p.x-pnt.x,p.y-pnt.y);
			return fabs(dir^t)/dir.len();//dir ^ t == dir.len() * t.len() * sin(dir,t)
		}
	};
	
	
	//n row m colomn matrix for matrix power series	
	template<class T>
	struct Mat{
		typedef vector<T>& Row;
		typedef const vector<T>& Row;;
		int nr,nc;
		vector<vector<T>>mem;
		long long mod;
		Mat(int nr,int nc,long long mod=1e9):nr(nr),nc(nc),mod(mod),mem(nr,Row(nc,T())){}
		inline CRow operator[](int x)const{ return mem[x]; }
		inline Row operator[](int x){ return mem[x]; }
		void print()
		{
			for(int i=0;i<nr;++i)
				for(int j=0;j<nc;++j)
					cout<<mem[i][j]<<(j==nc-1?'\n':' ');
		}
	};
	template<class T>
	Mat<T> operator*(const Mat<T>& a,const Mat<T>& b)
	{
		Mat<T> ret(a.nr,b.nc,a.mod);//or b.mod?
		for(int i=0;i<ret.nr;++i){
			for(int j=0;j<ret.nc;++j){
				ret[i][j]=0;
				for(int k=0;k<a.nc;++k){
					ret[i][j]=(ret[i][j]+a[i][k]*b[k][j]%ret.mod)%ret.mod;
				}
			}
		}
		return ret;
	}
}
#endif
