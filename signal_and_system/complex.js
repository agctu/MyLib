function radian(d){
    return d*Math.PI/180;
}
function degree(r){
    return r*180/Math.PI;
}
class Complex{
    //n is in degrees.
    static e(n){
        n=radian(n);
        return new Complex(Math.cos(n),Math.sin(n));
    };
    //pha is in degrees.
    static amppha(amp,pha){
        pha=radian(pha);
        return new Complex(amp*Math.cos(pha),amp*Math.sin(pha));
    }
    static getReal(c){
        if(c.constructor==Complex)return c.real;
        else return c;
    }
    static getImagine(c){
        if(c.constructor==Complex)return c.virt;
        else return 0;
    }
    static getAmplitude(c){
        if(c.constructor==Complex)return c.amp;
        else return Math.abs(c);
    }
    static getPhase(c){
        if(c.constructor==Complex)return c.pha;
        else return 0;
    }
    constructor (a,b=0){
    this.real=a;
    this.virt=b;
    }
    mul(c){
        if(c.constructor==Number) return new Complex(this.real*c,this.virt*c);
        else if(c.constructor==Complex) return new Complex(this.real*c.real-this.virt*c.virt,this.real*c.virt+this.virt*c.real);
    }
    divide(c){
        if(c.constructor==Number) return new Complex(this.real/c,this.virt/c);
        else if(c.constructor==Complex)return Complex.amppha(this.amp/c.amp,this.pha-c.pha);
    }
    plus(c){
        if(c.constructor==Number) return new Complex(this.real+c,this.virt);
        else if(c.constructor==Complex) return new Complex(this.real+c.real,this.virt+c.virt);
    }
    minus(c){
        if(c.constructor==Number) return new Complex(this.real-c,this.virt);
        else if(c.constructor==Complex) return new Complex(this.real-c.real,this.virt-c.virt);
    }
    //is equal useful here ?
    equals(c){
        if(c.constructor==Number) return Math.abs(c-this.real)<1e-6&&Math.abs(this.virt)<1e-6;
        else if(c.constructor==Complex) return Math.abs(c.real-this.real)<1e-6&&Math.abs(c.virt-this.virt)<1e-6;
    }
    exp(n){
        return Complex.amppha(this.amp,this.pha*n);
    }
    get amp(){
        return Math.sqrt(this.real*this.real+this.virt*this.virt);
    }
    //return phase degrees -180-180.
    get pha(){
        //atan return [-\frac{\pi}{2},\frac{\pi}{2}], so that's not complete.
        //1/0==Infinity 1/-0==-Infinity, but atan(Infinity)-atan(Infinity)=180\degree.
        if(this.real==0)this.real=0;//Don't ask me why! 1/0==Infinity 1/-0==-Infinity.
        var ret= degree(Math.atan(this.virt/this.real))+(this.real<0?180:0);
        return (ret>180?ret-360:ret);
    }
};
//FIXME this should use radian as the unit of n.