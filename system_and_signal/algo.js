//something not particular for signal, temporarily put here
function forAll(func,arr){
    var ret=[];
    for(let i=0;i<arr.length;++i){
        ret[i]=func(arr[i]);
    }
    return ret;
}
//return digital signal sampled from func with sampling rate=fs in the interval [start,end],and the first sampled point is at start.
function sample(func,fs,start,end){
    var ret=[],t;
    for(let i=0;start+i/fs<=end;++i){
        t=start+i/fs;
        ret.push(func(t));
    }
    return ret;
}

//All the functions solving digital signals should output 1D Array.
function dft(samp){
    var st=new Date().getTime(),ed;
    var ret=[];
    var n=samp.length;
    var w=Complex.e(-360/n);
    for(let i=0;i<n;++i){//for every k of X(k)
        var Xk=new Complex(0);
        for(let j=0;j<n;++j){
            Xk=Xk.plus(w.exp(j*i).mul(samp[j]));
        }
        ret.push(Xk.amp);
    }
    ed=new Date().getTime();
    console.log("dft using time : "+(ed-st)+" ms");
    return ret;
}
////fft recursion implement 
//The signal of the representation is not modified to 1D, so don't use this.
//var ret=[],buf=[];
//function fft(samp){
//    var st=new Date().getTime(),ed;
//    for(let i=0;i<samp.length;++i)ret[i]=samp[i][1];//split the y value and assign it to ret;
//    while((ret.length&-ret.length!=ret.length)){
//        ret.push(0);
//    }
//    _fft(0,ret.length);
//    ed=new Date().getTime();
//    for(let i=0;i<ret.length;++i){
//        ret[i]=[i,ret[i].amp];
//    }
//    console.log("fft using time : "+(ed-st)+" ms");
//    return ret;
//}
//function _fft(s,len){
//    if(len==1)return;
//    //group
//    for(let i=0;i<len;++i){
//        if(i%2==0)buf[s+parseInt(i/2)]=ret[s+i];
//        else buf[s+parseInt(i/2)+len/2]=ret[s+i];
//    }
//    //copy
//    for(let i=0;i<len;++i){
//        ret[s+i]=buf[s+i];
//    }
//    //recursion
//    _fft(s,len/2),_fft(s+len/2,len/2);
//    var w=Complex.e(-360/len);
//    for(let i=0;i<len/2;++i){
//        buf[s+i]=w.exp(i).mul(ret[s+len/2+i]).plus(ret[s+i]);
//        buf[s+i+len/2]=w.exp(i).mul(ret[s+len/2+i]).mul(-1).plus(ret[s+i]);
//    }
//    for(let i=0;i<len;++i){
//        ret[s+i]=buf[s+i];
//    }
//}
//fft another implement
//this can be faster
function fft(samp){
    var st=new Date().getTime(),ed;
    var ret=[],t,l,r;
    for(let i=0;i<samp.length;++i)ret[i]=samp[i];
//    ret=[];
//    for(let i=0;i<1<<4;++i)ret[i]=i;
    while((ret.length&-ret.length)!=ret.length)ret[ret.length]=0;
    for(let i=1,j=ret.length/2;i<ret.length;++i){
        if(i<j)
            t=ret[i],ret[i],ret[i]=ret[j],ret[j]=t;
        b=ret.length/2;
        //++j in reverse way.
        while(j&b)j^=b,b>>=1;
        j^=b;
    }
//    for(let i=0;i<ret.length;++i)console.log(ret[i]);
    //merge
    for(let i=1;(1<<i)<=ret.length;++i){
        var glen=1<<i,gn=ret.length/glen,w=Complex.e(-360/glen);
        for(let j=0;j<gn;++j){
            var gs=glen*j;
            for(let k=0;k<glen/2;++k){
                l=w.exp(k).mul(ret[gs+k+glen/2]).plus(ret[gs+k]);
                r=w.exp(k).mul(ret[gs+k+glen/2]).mul(-1).plus(ret[gs+k]);
                ret[gs+k]=l;
                ret[gs+k+glen/2]=r;
            }
        }
    }
    for(let i=0;i<ret.length;++i){
        ret[i]=ret[i].amp;
    }
    ed=new Date().getTime();
    console.log("fft using time : "+(ed-st)+" ms");
    return ret;
}

//1D dct.
function dct(x){
    //this of function is window :<.
    //implement using matrix;
    var mat=dct.matrix(x.length),vec=Matrix.parseVec(x);
    return mat.cross(vec).mem;
//    var ret=[],N=x.length;
//    for(let k=0;k<N;++k){
//        var res=0;
//        for(let n=0;n<N;++n){
//            res+=x[n]*Math.cos((2*n+1)*k*Math.PI/(2*N));
//        }
//        ret.push(res*Math.sqrt(2/N)*(k==0?1/Math.sqrt(2):1));
//    }
//    return ret;
}

//1D idct.
function idct(x){
    return dct.matrix(x.length).transpose().cross(Matrix.parseVec(x)).mem;
}

//return the matrix for 1D dct.
dct.matrix=function(r,c=r){
    var ret=new Matrix(r,c);
    var N=r;
    for(let k=0;k<r;++k){
        for(let n=0;n<c;++n){
            ret.set(k,n,(k==0?1/Math.sqrt(2):1)*Math.sqrt(2/N)*Math.cos((2*n+1)*k*Math.PI/(2*N)));
        }
    }
    return ret;
}

//2D dct.
function dct2(sig){
    console.assert(sig.constructor==Matrix);
    return dct.matrix(sig.nrow)
    .cross(sig)
    .cross(dct.matrix(sig.ncol).transpose());
}

//2D idct.
function idct2(sig){
    console.assert(sig.constructor==Matrix);
    return dct.matrix(sig.nrow).transpose()
    .cross(sig)
    .cross(dct.matrix(sig.ncol));
}

