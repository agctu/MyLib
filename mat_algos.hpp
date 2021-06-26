#ifndef ALGOS_HPP 
#define ALGOS_HPP 
#include <cassert>
#include <iostream>
#include <cmath>
#include <list>
#include "matrix.hpp"
namespace nm{
    using std::cerr;
    using std::endl;
    using std::list;
    typedef double (*Func)(double);
    typedef double (*DFunc)(double,double);
    template<typename FUNC>
    double binary(FUNC f,double l,double r,double eps=1e-6){
        double mid;
        assert(f(r)*f(l)<=0);
        while(r-l>eps){
            mid=(l+r)/2;
            if(f(l)*f(mid)>0)l=mid;
            else r=mid;
        }
        return mid;
    }
    void solveUpper(Matrix& up){
        assert(up.ncol-up.nrow==1);
        int n=up.nrow;
        for(int i=0;i<n;++i)for(int j=0;j<i;++j)assert(up[i][j]==0);
        for(int i=n-1;i>=0;--i){
            up[i][n]/=up[i][i];
            up[i][i]=1;
            for(int j=i-1;j>=0;--j){
                up[j][n]-=up[i][n]*up[j][i];
                up[j][i]=0;
            }
        }
    }
    void solveLower(Matrix& low){
        assert(low.ncol-low.nrow==1);
        int n=low.nrow;
        for(int i=0;i<n;++i)for(int j=i+1;j<n;++j)assert(low[i][j]==0);
        for(int i=0;i<n;++i){
            low[i][n]/=low[i][i];
            for(int j=i+1;j<n;++j){
                low[j][n]-=low[i][n]*low[j][i];
                low[j][i]=0;
            }
        }
    }
    void gauss(Matrix& mat){
        assert(mat.ncol-mat.nrow==1);
        int n=mat.nrow,m=mat.ncol;
        for(int i=0;i<n;++i){//for every row
            assert(mat[i][i]!=0);
            for(int j=i+1;j<n;++j){//for every row below ith row
                double f=mat[j][i]/mat[i][i];
                mat[j][i]=0;
                for(int k=i+1;k<m;++k){
                    mat[j][k]-=mat[i][k]*f;
                }
                //cerr<<string(mat)<<endl;
            }
        }
    }
    void gaussCR(Matrix& mat){//with row changes
        assert(mat.ncol>=mat.nrow);
        int n=mat.nrow,m=mat.ncol,mx,mxid;
        for(int i=0;i<n;++i){//for every row
            mx=0;
            //find the max
            for(int j=i;j<n;++j){
                if(fabs(mat[j][i])>mx)mx=fabs(mat[j][i]),mxid=j;
            }
            mat.changeRow(mxid,i);
            //cerr<<"change "<<i<<' '<<mxid<<endl;
            //cerr<<string(mat)<<endl;
            assert(mat[i][i]!=0);
            for(int j=i+1;j<n;++j){//for every row below ith row
                double f=mat[j][i]/mat[i][i];
                mat[j][i]=0;
                for(int k=i+1;k<m;++k){
                    mat[j][k]-=mat[i][k]*f;
                }
                //cerr<<string(mat)<<endl;
            }
        }
    }
    //get inverse matrix
    Matrix inverse(const Matrix& m){
        assert(m.nrow==m.ncol);
        Matrix mat=concatR(m,Matrix::eye(3));
        gaussCR(mat);
        cerr<<string(mat)<<endl;
        for(int i=mat.nrow-1;i>=0;--i){
            for(int j=mat.ncol-1;j>=i;--j)mat[i][j]/=mat[i][i];
            for(int j=i-1;j>=0;--j)for(int k=mat.ncol-1;k>=i;--k){
                mat[j][k]-=mat[i][k]*mat[j][i];
            }
        }
        cerr<<string(mat)<<endl;
        return mat.subMat(0,m.nrow,m.ncol,2*m.ncol);
    }
    //cauculate the result of Doolittle LU, where L[i][i]==1 for i in 1..n
    void LU(Matrix& mat){
        int n=mat.nrow,r,c;
        double sum;
        for(int i=0;i<n;++i){
            for(r=i,c=i;c<n;++c){//caculate current row
                sum=0;
                for(int k=i-1;k>=0;--k)sum+=mat[k][c]*mat[r][k];
                mat[r][c]-=sum;
            }
            for(r=i+1,c=i;r<n;++r){//caculate current column
                sum=0;
                for(int k=i-1;k>=0;--k)sum+=mat[k][c]*mat[r][k];
                mat[r][c]-=sum,mat[r][c]/=mat[c][c];
            }
        }
    }
    //only L is caculate. use positive definite matrix as input.
    void Choleysky(Matrix& mat){
        int n=mat.nrow;
        double sum;
        cerr<<string(mat)<<endl;
        for(int c=0;c<n;++c){
            for(int r=0;r<n;++r){
                if(r<c){
                    mat[r][c]=0;
                    continue;
                }
                sum=0;
                for(int k=0;k<c;++k)sum+=mat[r][k]*mat[c][k];
                mat[r][c]-=sum;
                if(r==c)mat[r][c]=sqrt(mat[r][c]);
                else mat[r][c]/=mat[c][c];
                cerr<<string(mat)<<endl;
            }
        }
    }
    //simple iteration for linear equation system
    void simpleIterationLES(const Matrix& B,const Matrix& g,Matrix& x,double eps=1e-6){
        Matrix last;
        do{
            last=x;
            x=B*x+g;
        }while((x-last).norminf()>eps);
    }
    //get required matrix for Jacobi
    //mat is the argumented matrix,B_j and g receive the result
    void Jacobi(const Matrix& mat,Matrix& B_j,Matrix& g){
        assert(mat.ncol-mat.nrow==1);
        if(B_j.nrow!=mat.nrow||B_j.ncol!=mat.nrow)B_j=Matrix(mat.nrow,mat.nrow);
        if(g.nrow!=mat.nrow||g.ncol!=1)g=Matrix(mat.nrow,1);
        Matrix Dr(mat.nrow,mat.nrow),LU(mat.nrow,mat.nrow);
        //split mat in to LU,g,D
        for(int i=0;i<mat.nrow;++i)for(int j=0;j<mat.ncol;++j){
            if(j==mat.ncol-1)g[i][0]=mat[i][j];
            else if(i==j)Dr[i][j]=1/mat[i][j];
            else LU[i][j]=mat[i][j];
        }
        B_j=-Dr*LU,g=Dr*g;
    }
    //convert B and g in place
    void GaussSeidel(Matrix& B,Matrix& g){
        assert(B.nrow==B.ncol);
        assert(g.nrow==B.nrow&&g.ncol==1);
        int siz=B.nrow;
        Matrix B1(siz,siz),B2(siz,siz),t;
        for(int i=0;i<siz;++i)for(int j=0;j<siz;++j){
            if(i>j)B1[i][j]=B[i][j];
            else B2[i][j]=B[i][j];
        }
        //cerr<<string(B1)<<endl;
        //cerr<<string(B2)<<endl;
        t=inverse(Matrix::eye(siz)-B1);
        B=t*B2;
        g=t*g;
    }
    //short path for GaussSeidel used on Jacobi
    void JGS(const Matrix& mat,Matrix& B_j,Matrix& g){
        Jacobi(mat,B_j,g);
        GaussSeidel(B_j,g);
    }
    //successive over-relaxation
    void SOR(const Matrix& mat,double w,Matrix& Bw,Matrix& g){
        assert(mat.ncol-mat.nrow==1);
        int siz=mat.nrow;
        Matrix L(siz,siz),D(siz,siz),U(siz,siz),b(siz,1);
        for(int i=0;i<mat.nrow;++i)for(int j=0;j<mat.ncol;++j){
            if(j==mat.ncol-1)b[i][0]=mat[i][j];
            else if(i==j)D[i][j]=mat[i][j];
            else if(i>j)L[i][j]=mat[i][j];
            else U[i][j]=mat[i][j];
        }
        Bw=inverse(D+w*L)*((1-w)*D-w*U);
        g=w*inverse(D+w*L)*b;
    }
    //solve for the least square root
    void leastSquareRoot(const Matrix& mat,Matrix& res){
        //assert(mat.nrow+1>mat.ncol);//?
        Matrix Ab=mat,C=mat.subMat(0,mat.nrow,0,mat.ncol-1);
        Ab=C.transpose()*Ab;
        cerr<<string(Ab)<<endl;
        //FIXME should I use Jacobi to do this to get more precision?
        gaussCR(Ab);
        for(int i=Ab.nrow-1;i>=0;--i){
            for(int j=Ab.ncol-1;j>=i;--j)Ab[i][j]/=Ab[i][i];
            for(int j=i-1;j>=0;--j)for(int k=Ab.ncol-1;k>=i;--k){
                Ab[j][k]-=Ab[i][k]*Ab[j][i];
            }
        }
        res=Ab.subMat(0,Ab.nrow,Ab.nrow,Ab.ncol);
    }
    double newtonCotes0Integrate(Func f,double a,double b){//mid-point
        return (b-a)*f((a+b)/2);
    }
    double newtonCotes1Integrate(Func f,double a,double b){//trapezoid
        return (b-a)*(f(a)+f(b))/2;
    }
    double newtonCotes2Integrate(Func f,double a,double b){//Simpson
        return (b-a)*(f(a)+4*f((a+b)/2)+f(b))/6;
    }
    double newtonCotes4Integrate(Func f,double a,double b){//Cotes
        return (b-a)*(7*f(a)+32*f((3*a+b)/4)+12*f((a+b)/2)+32*f((a+3*b)/4)+7*f(b))/90;
    }

    template<class Integ>
    double compositeIntegrate(Integ integ,Func f,double a,double b,int n){//split [a,b] to n sections with n+1 points
        double ret=0,h=(b-a)/n,ai,bi;
        for(int i=0;i<n;++i){
            ai=a+i*h,bi=ai+h;
            ret+=integ(f,ai,bi);
        }
        return ret;
    }

    template<class Integ>
    double binaryIntegrate(Integ integ,Func f,double a,double b,double eps){
        double ret=0,last=1e18;
        //the exit condition is the same for different integ, can't come up with a good solution
        for(int i=0,n=1;fabs(ret-last)>eps;++i,n*=2){
            last=ret;
            ret=compositeIntegrate(integ,f,a,b,n);
        }
        return ret;
    }

    //very similar to binaryIntegrate
    double rombergIntegrate(Func f,double a,double b,double eps){
        double ret=0,last=1e18;
        //the exit condition is the same for different integ, can't come up with a good solution
        for(int i=0,n=1;fabs(ret-last)>eps;++i,n*=2){
            last=ret;
            ret=compositeIntegrate(newtonCotes4Integrate,f,a,b,n);
        }
        return ret+(ret-last)/63;

    }
    //return the value of y_{h*n}
    double eulerExplicit(DFunc f,double x0,double y0,double h,int n){
        double y;
        for(int i=0;i<n;++i){
            y=y0+h*f(x0+i*h,y0);
            y0=y;
        }
        return y;
    }
    //return the value of y_{h*n}
    double eulerTrapezoidAdjustion(DFunc f,double x0,double y0,double h,int n){
        double y;
        for(int i=0;i<n;++i){
            y=y0+h*f(x0+i*h,y0);
            y=y0+h/2*(f(x0+i*h,y0)+f(x0+(i+1)*h,y));
            y0=y;
        }
        return y;
    }
    //return the value of y_{h*n}
    double heun2(DFunc f,double x0,double y0,double h,int n){
        double K1,K2,y,xn;
        for(int i=0;i<n;++i){
            xn=x0+i*h;
            K1=f(xn,y0);
            //2/3 == 0
            K2=f(xn+2./3.*h,y0+2./3.*h*K1);
            y=y0+h/4*(K1+3*K2);
            y0=y;
        }
        return y;
    }
    //return the value of y_{h*n}
    double rungeKutta4_4(DFunc f,double x0,double y0,double h,int n){
        double K1,K2,K3,K4,y,xn;
        for(int i=0;i<n;++i){
            xn=x0+i*h;
            K1=f(xn,y0);
            //1/2 ==0 && 1./2 == 0.5
            K2=f(xn+1./2*h,y0+h/2*K1);
            K3=f(xn+1./2*h,y0+h/2*K2);
            K4=f(xn+h,y0+h*K3);
            y=y0+h/6*(K1+2*K2+2*K3+K4);
            y0=y;
        }
        return y;
    }
    //return the value of y_{h*n}
    //y_{n+1}=y_n+\frac{h}{24}(55f_n-59f_{n-1}+37f_{n-2}-9f_{n-3})
    double adamsExplicit(DFunc f,double x3,double *y0_3,double h,int n){
        n-=3;
        assert(n>0);
        const double factors[]={55,-59,37,-9};
        double y,t;
        list<double>::iterator y_;
        list<double> ys;
        for(int i=0;i<4;++i)ys.push_front(y0_3[i]);
        for(int i=0;i<n;++i){
            y_=ys.begin();
            y=*y_;
            t=0;
            for(int j=0;j<4;++j){
                t+=factors[j]*f(x3+(i-j)*h,*y_);
                ++y_;
            }
            y+=t*h/24;
            ys.pop_back();
            ys.push_front(y);
        }
        return y;
    }
}
#endif
