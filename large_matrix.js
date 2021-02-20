function next_permutation(a){//return the how many time swap is execute.
    if(a.constructor!=Array)throw new TypeError("Array is needed");
    var tr=a.length-1,t;
    //find the first position a[pos-1]<a[pos]
    while(tr-1>=0&&a[tr-1]>a[tr])--tr;
    if(tr==0)return false;
    //find the first element from the end that is larger that a[pos-1]
    for(let i=a.length-1;i>=tr;--i)if(a[i]>a[tr-1]){
       t=a[tr-1],a[tr-1]=a[i],a[i]=t;
       break;
    }
    //reverse the suffix
    for(let i=0;tr+i<a.length-1-i;++i){
        t=a[tr+i],a[tr+i]=a[a.length-1-i],a[a.length-1-i]=t;
    }
    //return the number of swaps, for every swap the pairity of the reverse number change once.
    return Number.parseInt((a.length-tr)/2)+1;
}
class Matrix{//using js array as mem, not Uint8Array, I am not familiar with it.
    //static method
    static e(s){
        var data=[];
        for(let i=0;i<s;++i){
            for(let j=0;j<s;++j){
                data[i*s+j]=(i==j?1:0);
            }
        }

        return new Matrix(s,s,data);
    }
    static z(r,c){
        return new Matrix(r,c);
    }
    static all(r,c,v){
        var data=[];
        for(let i=0;i<r;++i)for(let j=0;j<c;++j)data[i*c+j]=v;
        return new Matrix(r,c,data);
    }
    static parseVec(v){
        if(v.constructor!=Array)throw new TypeError("v should be Array");
        var ret=new Matrix(v.length,1,v.slice(0,v.length));
        return ret;
    }
    constructor(r,c,d){
        this.nrow=r;
        this.ncol=c;
        if(d==undefined){
            d=[];
        }
        d=d.slice(0,r*c);//new array data.
        if(d.length!=r*c){
            for(let i=d.length;i<r*c;++i)d[i]=0;//add 0 to the end of the r
        }
        this.mem=d;
    }
    at(i,j){
        if(i<0||i>=this.nrow||j<0||j>=this.ncol)throw new RangeError();
        return this.mem[i*this.ncol+j];
    }
    set(i,j,v){
        if(i<0||i>=this.nrow||j<0||j>=this.ncol)throw new RangeError();
        this.mem[i*this.ncol+j]=v;
    }
    transpose(){
        var ret=new Matrix(this.ncol,this.nrow);
        for(let i=0;i<this.nrow;++i){
            for(let j=0;j<this.ncol;++j){
                ret.set(j,i,this.at(i,j));
            }
        }
        return ret;
    }
    cross(b){
        if(b.constructor!=this.constructor)throw new TypeError("b should be a Matrix");
        if(this.ncol!=b.nrow)throw new TypeError("this.ncol should be equal to b.nrow");
        var ret=new Matrix(this.nrow,b.ncol);
        for(let i=0;i<this.nrow;++i){
            for(let j=0;j<b.ncol;++j){
                var t=0;
                for(let k=0;k<this.ncol;++k){
                    t+=this.at(i,k)*b.at(k,j);
                }
                ret.set(i,j,t);
            }
        }
        return ret;
    }
    dot(b){
        if(b.constructor!=this.constructor)throw new TypeError("b should be a Matrix.");
        if(b.nrow!=this.nrow||b.ncol!=this.ncol)throw new TypeError("this.nrow should be equal to b.nrow and this.ncol should be equal to b.ncol.")
        var ret=new Matrix(this.nrow,this.ncol);
        for(let i=0;i<this.nrow;++i){
            for(let j=0;j<this.ncol;++j){
                ret.set(i,j,this.at(i,j)*b.at(i,j));
            }
        }
        return ret;
    }
    mul(x){
        if(x.constructor!=Number)throw new TypeError("Number is required");
        var ret=new Matrix(this.nrow,this.ncol);
        for(let i=0;i<this.nrow;++i)for(let j=0;j<this.ncol;++j){
            ret.set(i,j,this.at(i,j)*x);
        }
        return ret;
    }
    plus(b){
        if(b.constructor!=this.constructor)throw new TypeError("b should be a Matrix.");
        if(b.nrow!=this.nrow||b.ncol!=this.ncol)throw new TypeError("this.nrow should be equal to b.nrow and this.ncol should be equal to b.ncol.")
        var ret=new Matrix(this.nrow,this.ncol);
        for(let i=0;i<this.nrow;++i){
            for(let j=0;j<this.ncol;++j){
                ret.set(i,j,this.at(i,j)*b.at(i,j));
            }
        }
        return ret;
    }
    reshape(row,col){
        if(!row||!col
            ||!Number.isInteger(row)||!Number.isInteger(col)
            ||!row*col!=this.nrow*this.ncol){
            console.error("Wrong input value:" +row+","+col);
            return;
        }
        return new Matrix(row,col,this.mem);
    }
    kron(x){
        var ret=new Matrix(this.nrow*x.nrow,this.ncol*x.ncol);
        for(let i=0;i<this.nrow;++i)for(let j=0;j<this.ncol;++j){
            let km=this.at(i,j);
            for(let k=0;k<x.nrow;++k)for(let l=0;l<x.ncol;++l){
                ret.set(i*x.nrow+k,j*x.ncol+l,x.at(k,l)*km);
            }
        }
        return ret;
    }
    inverse(){//no optimization, low precision algorithm
        if(this.nrow!=this.ncol){
            console.error("Not square : "+this.toString());
            return;
        }
        if(this.det==0){
            console.error("Singular Matrix!");
            return;
        }
    }
    get det(){
        if(this.nrow!=this.ncol){
            console.error("Not square : "+this.toString());
            return;
        }
        var ret=0,permu=[];
        for(let i=0;i<this.nrow;++i)permu[i]=i;
        for(let tp=0,ns=1;ns;tp+=ns){
            let item=(tp&1)?-1:1;
            for(let i=0;i<permu.length;++i){
                item*=this.at(i,permu[i]);
            }
            ret+=item;
            ns=next_permutation(permu);
        }
        return ret;
    }
    equal(x){
        if(x.constructor!=Matrix)throw new TypeError("x should have type of Matrix.");
        if(this.nrow!=x.nrow||this.ncol!=x.ncol)return false;
        for(let i=0;i<this.nrow;++i)for(let j=0;j<this.ncol;++j){
            if(this.at(i,j)!=x.at(i,j))return false;
        }
        return true;
    }
    normalized(){
        if(this.ncol!=1)throw new TypeError("only applied to vec");
        var t=0;
        for(let i=0;i<this.nrow;++i){
            t+=this.at(i,0)*this.at(i,0);
        }
        t=Math.sqrt(t);
        return this.mul(1/t);
    }
    toString(){
        var ret="";
        ret+=this.nrow+" rows, "+this.ncol+" columns.\n"
        for(let i=0;i<this.nrow;++i){
            for(let j=0;j<this.ncol;++j){
                ret+=this.at(i,j)+(j==this.ncol-1?'\n':' ');
            }
        }
        return ret;
    }
    get max(){
        var ret=-1e18;
        for(let i=0;i<this.mem.length;++i){
            ret=Math.max(ret,this.mem[i]);
        }
        return ret;
    }
    get min(){
        var ret=1e18;
        for(let i=0;i<this.mem.length;++i){
            ret=Math.min(ret,this.mem[i]);
        }

        return ret;
    }

}
