//only 4x4 matrix is implemented here.
//
function Vector(arr){
    if(arguments.length==0){
        this.data=[]
    }
    else if(arr.constructor==Array){
        this.data=arr.slice(0);
    }
}
Vector.prototype={
    constructor : Vector,
    get norm(){
        var result=0;
        for(let i=0;i<this.length;++i){
            result+=this.data[i]*this.data[i];
        }
        return Math.sqrt(result);
    },
    get length(){
        return this.data.length;
    },
    get x(){
        return this.data[0];
    },
    get y(){
        return this.data[1];
    },
    get z(){
        return this.data[2];
    },
    get w(){
        return this.data[3];
    },
    get xy(){
        return new Vector([this.data[0],this.data[1]]);
    },
    get xz(){
        return new Vector([this.data[0],this.data[2]]);
    },
    get yz(){
        return new Vector([this.data[1],this.data[2]]);
    },
    copy : function(){
        return new Vector(this.data);
    },
    toString : function(){
        var s='';
        for(let i=0;i<this.length;++i){
            s+=this.data[i]+' ';
        }
        return s;
    },
    normalized : function(){
        var result = new Vector();
        var scale = this.norm;
        for(let i=0;i<this.length;++i){
            result.data[i]=this.data[i]/scale;
        }
        return result;
    },
    dot : function(val){
        if(val.constructor!=Vector){
            throw "An Vector is needed.";
        }
        else if(this.length!=val.length){
            throw "The Vector as operand should have the same length."
        }
        var result = 0;
        for(let i=0;i<val.length;++i){
            result += val.data[i]*this.data[i];
        }
        return result;
    },
    cross : function(val){
        if(val.constructor!=Vector){
            throw "An Vector is needed.";
        }
        else if(this.length<2||this.length>3){
            throw "The left operand has a wrong length.";
        }
        else if(val.length<2||val.length>3){
            throw "The right operand has a wrong length.";
        }
        var a=this.data.slice(0);
        var b=val.data.slice(0);
        if(a.length==2){
            a[2]=0;
        }
        if(b.length==0){
            b[2]=0;
        }
        var result = new Vector();
        result.data[0]=a[1]*b[2]-a[2]*b[1];
        result.data[1]=a[2]*b[0]-a[0]*b[2];
        result.data[2]=a[0]*b[1]-a[1]*b[0];
        return result;
    }
};
function Matrix(arr){ 
    if(arguments.length==0){
        this.data=[
            [1.0,0.0,0.0,0.0],
            [0.0,1.0,0.0,0.0],
            [0.0,0.0,1.0,0.0],
            [0.0,0.0,0.0,1.0]
        ]
    }
    else if(arr.constructor==Array){
        this.data=[[],[],[],[]];
        for(let i in arr){
            this.data[Math.floor(i/4)][i%4]=parseFloat(arr[i]);
        }  
    }
    else if(typeof arr=='number'){
        this.data=[];
        var part=[]
        for(let i=0;i<4;++i)
            part.push(arr);
        for(let i=0;i<4;++i)
            this.data.push(part);
    }
}
Matrix.prototype={
    constructor : Matrix,
    type  : "4x4",
    size  : 16,
    copy  : function(){
        var c=new Matrix();
        for(let i=0;i<16;++i){
            c.data[parseInt(i/4)][i%4]=this.at(i);
        }
        return c;
    },
    at : function(i,j){//return the element on the row i and colomn j.
        if(arguments.length==2){
            return this.data[i][j];
        }else if(arguments.length==1){
            return this.data[Math.floor(i/4)][i%4];
        }
    },
    mul : function(val){
        var result = this.copy();
        // ???Object.assign(result,this); with this I find the four number is multiplied in one operation.
        if(typeof val=='number'){
            for(let i=0;i<result.size;++i){
                result.data[Math.floor(i/4)][i%4]*=val;
            }
        }
        else if(val.constructor==Matrix){
            for(let i=0;i<4;++i){
                for(let j=0;j<4;++j){
                    result.data[i][j]=0;
                    for(let k=0;k<4;++k){
                        result.data[i][j]+=this.at(i,k)*val.at(k,j);
                    }
                }
            }
        }
        else if(val.constructor==Vector){
            //this if shoud be abondoned in the future.
            if(val.length!=4){
                throw "The length of the Vector should be 4.";
            }
            let resultv = new Vector([0,0,0,0]);
            for(let i=0;i<val.length;++i){
                for(let j=0;j<4;++j){
                    resultv.data[i]+=val.data[j]*this.at(i,j);
                }
            }
            return resultv;
        }
        return result;
    },
    transpose : function(){
        var result = new Matrix();
        for(let i=0;i<4;++i){
            for(let j=0;j<4;++j){
                result.data[i][j]=this.at(j,i);
                result.data[j][i]=this.at(i,j);
            }
        }
        return result;
    },
    toArray : function(){
        var arr = [];
        var t = this.transpose();
        for(let i=0;i<t.size;++i){
            arr.push(t.at(i));   
        }
        return new Float32Array(arr);
    },
    toString : function(){
        var r='';
        for(let i=0;i<4;++i){
            for(let j=0;j<4;++j){
                r+=this.at(i,j)+' ';
            }
            r+='\n';
        }
        return r;
    }
}
Matrix.translate = function(x,y,z=0.0){
    
    var result = new Matrix([
        1.0,0.0,0.0,x,
        0.0,1.0,0.0,y,
        0.0,0.0,1.0,z,
        0.0,0.0,0.0,1.0
    ]);
    return result;
}
Matrix.scale = function(x,y,z=0.0){
    
    var result = new Matrix([
          x,0.0,0.0,0.0,
        0.0,  y,0.0,0.0,
        0.0,0.0,  z,0.0,
        0.0,0.0,0.0,1.0
    ]);
    return result;
}
Matrix.rotate = function(rad,nx,ny,nz){
    // var normal=new Vector(nx,ny,nz);
    // var axis_z=new Vector(0,0,1);
    // var cosx=normal.yz.dot(axis_z.yz)/(normal.yz.norm);
    // var sinx=Math.sqrt(1-cosx*cosx);
    // if(normal.y<0)
    //     sinx=-sinx;
    // var cosy=normal.xz.dot(axis_z.xz)/(normal.xz.norm);
    // var siny=Math.sqrt(1-cosy*cosy);
    // if(normal.x>0)
    //     siny=-siny;
    // var cost=Math.cos(rad);
    // var sint=Math.sin(rad);
    // var result = new Matrix([

    // ]);
    // return result;
    //I don't want to continue, because I just find an answer on the web,so I decide to use the result directly,to make it easier and avoid to use the cosx,sinx,cosy,siny.
    var normal = new Vector([nx,ny,nz]).normalized();
    nx=normal.x,ny=normal.y,nz=normal.z;
    var result = new Matrix([
        Math.cos(rad)+(1-Math.cos(rad))*nx*nx,(1-Math.cos(rad))*nx*ny-Math.sin(rad)*nz,(1-Math.cos(rad))*nx*nz+Math.sin(rad)*ny,0,
        (1-Math.cos(rad))*ny*nx+Math.sin(rad)*nz,Math.cos(rad)+(1-Math.cos(rad))*ny*ny,(1-Math.cos(rad))*ny*nz-Math.sin(rad)*nx,0,
        (1-Math.cos(rad))*nz*nx-Math.sin(rad)*ny,(1-Math.cos(rad))*nz*ny+Math.sin(rad)*nx,Math.cos(rad)+(1-Math.cos(rad))*nz*nz,0,
        0,0,0,1
    ]);
    return result;
}
Matrix.ortho=function(minx,maxx,miny,maxy,minz,maxz){
    var result = new Matrix([
        2/(maxx-minx),0.0,0.0,-(2*minx/(maxx-minx)+1),
        0.0,2/(maxy-miny),0.0,-(2*miny/(maxy-miny)+1),
        0.0,0.0,2/(minz-maxz),-(2*maxz/(minz-maxz)+1),
        0.0,0.0,0.0,1.0
    ]);
    return result;
}

