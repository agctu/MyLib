class System{
    //represent continuous-time system with rational frequency response.
    //zeros is Number or Complex
    constructor(zeros,poles,gain){
        this.zeros=zeros;
        this.poles=poles;
        this.gain=gain;
    }
    response(w){
        var ret=new Complex(this.gain);
        //multiply with zeros.
        for(let i=0;i<this.zeros.length;++i){
            if(new Complex(0).equals(this.zeros[i]))ret=ret.mul(new Complex(0,w));
            else ret=ret.mul(new Complex(0,w).divide(new Complex(-1).mul(this.zeros[i])).plus(1));
        }
        for(let i=0;i<this.poles.length;++i){
            if(new Complex(0).equals(this.poles[i]))ret=ret.divide(new Complex(0,w));
            else ret=ret.divide(new Complex(0,w).divide(new Complex(-1).mul(this.poles[i])).plus(1));
        }
        return ret;
    }
    //if use the phase of this.response as the phase ,then the phase may have a difference of 360\degree, so just use this.
    phase(w){
        var ret=0;
        for(let i=0;i<this.zeros.length;++i){
            if(new Complex(0).equals(this.zeros[i]))
                ret+=new Complex(0,w).pha;
            else ret+=new Complex(0,w).divide(new Complex(-1).mul(this.zeros[i])).plus(1).pha;
        }
        for(let i=0;i<this.poles.length;++i){
            if(new Complex(0).equals(this.poles[i]))
                ret-=new Complex(0,w).pha;
            else ret-=new Complex(0,w).divide(new Complex(-1).mul(this.poles[i])).plus(1).pha;
        }
        return ret;

    }
    //plot provide the environment of FuncPlot for drawing.
    draw(plot){}
    drawNyquist(plot){
        plot.pushLineColor("blue");
        var last;
        for(let i=-200;i<200;++i){
            var ths=this.response(Math.pow(1.02,i));
            /*plot.pushLineColor("purple");
            plot.drawLine(0,0,ths.real,ths.virt);
            plot.popLineColor();*/
            if(last){
                if(last.minus(ths).amp<this.gain*10)
                    plot.drawLine(last.real,last.virt,ths.real,ths.virt);
            }
            last=ths;
        }
        plot.popLineColor();
    }
}