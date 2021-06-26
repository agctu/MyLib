# ifndef MATRIX_HPP
# define MATRIX_HPP
#include <iostream>
#include <initializer_list>
#include <vector>
#include <cstdio>
#include <string>
#include <cassert>
#include <algorithm>
#include <random>
#include <ctime>
namespace nm{
	using std::vector;
	using std::initializer_list;
	using std::string;
	using std::swap;
	using std::mt19937;
    using std::max;
	//two dimension matrix
	template<typename T> struct _Matrix{
		struct CRow{
			int rid;
			const _Matrix& mt;
			CRow(const _Matrix& mt,int rid):rid(rid),mt(mt){}
			const T& operator[](int cid){
				int id=mt.rids[rid]*mt.ncol+mt.cids[cid];
				return mt.mem[id];
			}
		};
		struct Row{
			int rid;
			_Matrix& mt;
			Row(_Matrix& mt, int rid):rid(rid),mt(mt){}
			T& operator[](int cid){
				int id=mt.rids[rid]*mt.ncol+mt.cids[cid];
				return mt.mem[id];
			}
		};
		//struct Col{
			//int cid;
			//const _Matrix& mt;
			//Row(const _Matrix& mt,int cid):cids(cid),mt(mt){}
		//};
		static _Matrix random(int nrow,int ncol,bool alter=false);
		static _Matrix eye(int siz){
			_Matrix ret(siz,siz);
			for(int i=0;i<siz;++i) ret[i][i]=1;
			return ret;
		}
        static _Matrix fromBytes(const string& data){
            if(data.size()<sizeof(int)*2)throw "data.size should be at least sizeof(int)*2!";
            int nrow,ncol;
            const char *p=&data[0];
            nrow=*reinterpret_cast<const int*>(p);
            p+=sizeof(int);
            ncol=*reinterpret_cast<const int*>(p);
            p+=sizeof(int);
            if(data.size()!=sizeof(T)*nrow*ncol+sizeof(int)*2)throw "wrong data size!";
            //FIXME conciser method to do this?such as read data to a vector,then call one of the constructor
            _Matrix ret(nrow,ncol);
            for(int i=0;i<nrow;++i)for(int j=0;j<ncol;++j){
                ret[i][j]=*reinterpret_cast<const T*>(p);
                p+=sizeof(T);
            }
            return ret;
        }
		int nrow,ncol;
		vector<T>mem,cids,rids;
		_Matrix():nrow(0),ncol(0){
			init();
		}
		_Matrix(int nrow,int ncol):nrow(nrow),ncol(ncol){
			init();
		}
		_Matrix(int nrow,int ncol,initializer_list<T> data):nrow(nrow),ncol(ncol),mem(data.begin(),data.end()){
			init();
		}
		friend _Matrix operator*(const _Matrix& a, const _Matrix& b){
			assert(a.ncol==b.nrow);
			int len=a.ncol;
			_Matrix ret(a.nrow,b.ncol);
			for(int i=0;i<ret.nrow;++i){
				for(int j=0;j<ret.ncol;++j){
					ret[i][j]=0;
					for(int k=0;k<len;++k){
						ret[i][j]+=a[i][k]*b[k][j];
					}
				}
			}
			return ret;
		}
		friend _Matrix operator*(const _Matrix& a,T b){
			_Matrix ret=_Matrix(a.nrow,a.ncol);
			for(int i=0;i<ret.nrow;++i)for(int j=0;j<ret.ncol;++j){
				ret[i][j]=a[i][j]*b;
			}
			return ret;
		}
		friend _Matrix operator*(T a,const _Matrix& b){
			return b*a;
		}
		friend _Matrix operator/(const _Matrix& a,T b){
			return a*(1/b);
		}
		friend _Matrix operator/(T a,const _Matrix& b){
			return (1/a)*b;
		}
		friend _Matrix operator+(const _Matrix& a,const _Matrix& b){
			assert(a.ncol==b.ncol&&a.nrow==b.nrow);
			_Matrix ret(a.nrow,a.ncol);
			for(int i=0;i<ret.nrow;++i)for(int j=0;j<ret.ncol;++j){
				ret[i][j]=a[i][j]+b[i][j];
			}
			return ret;
		}
		friend _Matrix operator-(const _Matrix& a,const _Matrix& b){
			return a+(-b);
		}
		_Matrix operator-()const{
			_Matrix ret(nrow,ncol);
			for(int i=0;i<nrow;++i)for(int j=0;j<ncol;++j){
				ret[i][j]=-(*this)[i][j];
			}
			return ret;
		}
		friend _Matrix concatR(const _Matrix& a,const _Matrix& b){
			assert(a.nrow==b.nrow);
			int n=a.nrow,m=a.ncol+b.ncol;
			_Matrix ret(n,a.ncol+b.ncol);
			for(int i=0;i<n;++i)for(int j=0;j<m;++j)
			 	ret[i][j]=j<a.ncol?a[i][j]:b[i][j-a.ncol]; 
			return ret;
		}
        void tranverse();//some day,this should be implemented and some code should be reconstructed.
		_Matrix transpose(){
			_Matrix ret(ncol,nrow);
			for(int i=0;i<ret.nrow;++i)for(int j=0;j<ret.ncol;++j)
				ret[i][j]=(*this)[j][i];
			return ret;
		}
		void changeRow(int a,int b){
			swap(rids[a],rids[b]);
		}
		void changeCol(int a,int b){
			swap(cids[a],cids[b]);
		}
		_Matrix subMat(int sr,int er,int sc,int ec)const{
			assert(0<=sr&&sr<=nrow);
			assert(0<=er&&er<=nrow);
			assert(0<=sc&&sc<=ncol);
			assert(0<=ec&&ec<=ncol);
			assert(er>sr&&ec>sc);
			_Matrix ret(er-sr,ec-sc);
			for(int i=0;i<ret.nrow;++i)for(int j=0;j<ret.ncol;++j){
				ret[i][j]=(*this)[i+sr][j+sc];
			}
			return ret;
		}
		operator string(){
			string ret;
			char buf[1<<10];
			sprintf(buf,"nrow : %d, ncol : %d\n",nrow,ncol);
			ret+=buf;
			for(int i=0;i<nrow;++i){
				for(int j=0;j<ncol;++j){
					sprintf(buf,"%.4f ",(*this)[i][j]);
					ret+=buf;
				}
				ret+='\n';
			}
			return ret;
		}
        //if some members of T is pointer,then error occurs
        string toBytes(){
            string ret;
            ret.resize(sizeof(int)*2+sizeof(T)*nrow*ncol);
            //unsafe{
            char *p=&ret[0];
            *reinterpret_cast<int*>(p)=nrow;
            p+=sizeof(int);
            *reinterpret_cast<int*>(p)=ncol;
            p+=sizeof(int);
            for(int i=0;i<nrow;++i)for(int j=0;j<ncol;++j){
                *reinterpret_cast<T*>(p)=(*this)[i][j];
                p+=sizeof(T);
            }
            //}//unsafe
            return ret;
        }
		T det(){
			using std::cout;
			using std::endl;
			assert(nrow==ncol);
			vector<T>perm;
			int ns=0,t=0,itm,ret=0;
			for(int i=0;i<nrow;++i){
				perm.push_back(i);
			}
			do{
				itm=ns==0?1:-1;
				for(int i=0;i<ncol;++i){
					itm*=(*this)[i][perm[i]];
				}
				ret+=itm;
			}while(ns^=(t=next_permu(perm))%2,t);
			return ret;
		}
		T norm1(){//max column sum
			T ret=0,sum;
			for(int c=0;c<ncol;++c){
				sum=0;
				for(int r=0;r<nrow;++r) sum+=fabs((*this)[r][c]);
				ret+=max(ret,sum);
			}
			return ret;
		}
		T norm2(){//spectro norm not implemented

		}
		T norminf(){//max row sum
			T ret=0,sum;
			for(int r=0;r<nrow;++r){
				sum=0;
				for(int c=0;c<ncol;++c) sum+=fabs((*this)[r][c]);
				ret=max(ret,sum);
			}
			return ret;
		}

		T normF(){//square root
			int ret=0;
			for(int i=0;i<nrow;++i)for(int j=0;j<ncol;++j)ret+=(*this)[i][j];
			ret=sqrt(ret);
			return ret;
		}
		CRow operator[](int rid)const{
			return CRow(*this,rid);
		}
		Row operator[](int rid){
			return Row(*this,rid);
		}
		private:
			void init(){
				cids.resize(ncol),rids.resize(nrow),mem.resize(nrow*ncol);
				for(int i=0;i<nrow;++i)rids[i]=i;
				for(int i=0;i<ncol;++i)cids[i]=i;
			}
			int next_permu(vector<T>& p){
				int n=p.size(),tr=n-1;
				while(tr>0&&p[tr-1]>p[tr])--tr;
				if(tr==0)return 0;
				for(int i=n-1;i>=tr;--i)if(p[i]>p[tr-1]){
					swap(p[i],p[tr-1]);
					break;
				}
				for(int i=0;tr+i<n-1-i;++i){
					swap(p[tr+i],p[n-1-i]);
				}
				return (n-tr)/2+1;
			}
	};
	using Matrix=_Matrix<double>;
	template<> Matrix Matrix::random(int nrow,int ncol,bool alter){
		mt19937 rand(alter?time(0):0);
		Matrix ret(nrow,ncol);
		for(int i=0;i<nrow;++i)for(int j=0;j<ncol;++j)
			ret[i][j]=rand()/1e9;
		return ret;
	}
}//namespace nm

# endif
