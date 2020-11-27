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
        if(d.length!=r*c){
            d=d.slice(0,r*c);
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
