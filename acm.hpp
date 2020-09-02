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
	template<class T,int sz>
	struct SplayTree{
		struct Node{
			int f,ch[2],cnt,size;
			T val;
			Node(){}
			Node(int f,T val,int cnt,int size):f(f),val(val),cnt(cnt),size(size)
			{
				ch[0]=ch[1]=0;
			}
		};
		int root=0,len=0;
		Node mem[sz];
		SplayTree()
		{
			mem[0]=Node(0,0,0,0);//avoid error ocurs when maintain a node which has a child 0.
		}
		void rotate(int rt)
		{
			Node& nd=mem[rt],& fa=mem[nd.f];
			int tp=r(rt),ftp=r(nd.f),frt=nd.f,srt=nd.ch[tp^1],grt=fa.f;
			fa.ch[tp]=srt,nd.ch[tp^1]=frt;
			nd.f=grt,fa.f=rt;
			mem[srt].f=frt;
			if(grt==0)root=rt;
			else mem[grt].ch[ftp]=rt;
			maintain(frt),maintain(rt);
		}
		void insert(T v)
		{
			int f=0,cur=root;
			while(cur!=0&&v!=mem[cur].val) ++mem[cur].size/*if not here,splay will do the job too ? */,f=cur,cur=mem[cur].ch[v>mem[cur].val];
			if(cur==0){
				Node& nd=mem[++len];
				cur=len;
				nd=Node(f,v,1,1);
				if(f==0) root=len;
				else mem[f].ch[v>mem[f].val]=len;
			}
			else{
				++mem[cur].size,++mem[cur].cnt;
			}
			splay(cur);
		}
		int rank(T v)
		{
			int ret=0,cur=root;
			while(cur){
				if(v>mem[cur].val) ret+=mem[mem[cur].ch[0]].size+mem[cur].cnt,cur=mem[cur].ch[1];
				else if(v<mem[cur].val)cur=mem[cur].ch[0];
				else{
					ret+=mem[mem[cur].ch[0]].size;
					splay(cur);
					break;
				}
			}
			if(mem[root].val!=v)cerr<<v<<" is not in the tree"<<endl;
			return ret+1;
		}
		T kth(int n)
		{
			int cur=root,nn=n;
			while(cur){
				if(n>mem[mem[cur].ch[0]].size+mem[cur].cnt){
					n-=mem[mem[cur].ch[0]].size+mem[cur].cnt;
					cur=mem[cur].ch[1];
				}
				else if(n<=mem[mem[cur].ch[0]].size){
					cur=mem[cur].ch[0];
				}
				else{
					splay(cur);
					return mem[cur].val;
				}
			}
			cerr<<"I think you call this with a wrong index: "<<nn<<endl;
			return T();
		}
		T pre(T v)
		{
			int cur=root,pr=0;
			while(cur){
				if(mem[cur].val>=v)cur=mem[cur].ch[0];
				else pr=cur,cur=mem[cur].ch[1];
			}
			if(pr==0){
				cerr<<"No previous for v="<<v<<endl;
				return T();
			}
			splay(pr);
			return mem[pr].val;
		}
		T next(T v)
		{
			int cur=root,pr=0;
			while(cur){
				if(mem[cur].val<=v)cur=mem[cur].ch[1];
				else pr=cur,cur=mem[cur].ch[0];
			}
			if(pr==0){
				cerr<<"No next for v="<<v<<endl;
				return T();
			}
			splay(pr);
			return mem[pr].val;
		}
		void erase(T v)
		{
			rank(v);
			Node& nd=mem[root];
			--nd.cnt,--nd.size;
			if(!nd.cnt){
				if(!nd.ch[0]&&!nd.ch[1]) root=0;
				else if(nd.ch[0]&&nd.ch[1]){
					root=nd.ch[0];
					mem[nd.ch[0]].f=0;
					kth(mem[nd.ch[0]].size);//or pre(nd.val) I think, No nd not in the left subtree now!
					mem[root].ch[1]=nd.ch[1];
					mem[nd.ch[1]].f=root;
					maintain(root);
				}
				else{
					root=nd.ch[0]?nd.ch[0]:nd.ch[1];
					mem[root].f=0;
				}
			}
		}
		bool check(int rt)
		{
			if(rt==0)
				return true;
			return check(mem[rt].ch[0])&&check(mem[rt].ch[1])&&mem[mem[rt].ch[0]].size+mem[mem[rt].ch[1]].size+mem[rt].cnt==mem[rt].size;
		}
		void print()
		{
			int siz,cur;
			queue<int> q;
			q.push(root);
			while(!q.empty()){
				siz=q.size();
				for(int i=0;i<siz;++i){
					Node& nd=mem[q.front()];
					q.pop();
					cerr<<'('<<nd.val<<','<<((&nd-mem))<<','<<nd.cnt<<','<<nd.size<<')'<<"	";
					if(nd.ch[0])q.push(nd.ch[0]);
					if(nd.ch[1])q.push(nd.ch[1]);
				}
				cerr<<endl;
			}
			cerr<<"------------------------------------------"<<endl;
		}
		void splay(int rt)
		{
			Node& nd=mem[rt];
			while(nd.f!=0){
				if(mem[nd.f].f!=0)rotate(r(rt)==r(nd.f)?nd.f:rt);
				rotate(rt);
			}
		}
		void maintain(int rt)//maintain the size field of mem[rt]
		{
			Node& nd=mem[rt];
			nd.size=mem[nd.ch[0]].size+mem[nd.ch[1]].size+nd.cnt;
		}
		//I don't want to use this anyway.Too dizzy. But now I think I'm wrong.
		inline bool r(int rt){ return mem[mem[rt].f].ch[1]==rt; }
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
		double operator+()
		{
			return sqrt(x*x+y*y);
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
	};
	
	double operator*(const Vec& a,const Vec& b)
	{
		return a.x*b.x+a.y*b.y;
	}
	Vec operator+(const Vec& a,const Vec& b)
	{
		Vec ret(a);
		ret+=b;
		return ret;
	}
	Vec operator-(const Vec& a,const Vec& b)
	{
		Vec ret(a);
		ret-=b;
		return ret;
	}
	
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
