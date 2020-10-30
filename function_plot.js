//static functions
function getCanvasContext(id){
    return document.getElementById(id).getContext("2d");
}
function clamp(x,low,high){
    return Math.max(Math.min(x,high),low);
}
//draw line using the canvas coord.
function drawLine(canv,x1,y1,x2,y2){
    //FIXME
    canv.beginPath();
    canv.moveTo(x1,y1);
    canv.lineTo(x2,y2);
    canv.stroke();
}
function drawPoint(canv,x,y,size=1){
    canv.beginPath();
    canv.arc(x,y,size,0,2*Math.PI);
    canv.fill();
}
//constructor
function FuncPlot(plotId,width,height,precision,xscale=width/10,yscale=height/10,mx=width/2,my=height/2){
    this.graph=document.getElementById(plotId);
    this.graph.plot=this;
    this.graph.width=width;
    this.graph.height=height;
    this.graph.onmousewheel=function(e){
        e.preventDefault();
        var delta=e.wheelDelta,k;
        k=delta>0?1.2:1/1.2;
        this.plot.scaleX*=k;
        this.plot.scaleY*=k;
        if(this.in){
            this.plot.midX=(this.plot.midX-this.lastPoint[0])*k+this.lastPoint[0];
            this.plot.midY=(this.plot.midY-this.lastPoint[1])*k+this.lastPoint[1];
        }
        else{
            this.plot.midX=(this.plot.midX-this.width/2)*k+this.width/2;
            this.plot.midY=(this.plot.midY-this.height/2)*k+this.height/2;
        }
        this.plot.draw();
    };
    this.graph.onmousedown=function(e){
        this.down=true;
        this.lastPoint=[e.offsetX,e.offsetY];
        return true;
    };
    this.graph.onmouseup=function(e){
        this.down=false;
    };
    this.graph.onmouseenter=function(e){
        this.in=true;
    };
    this.graph.onmouseout=function(e){
        this.down=false;
        this.in=false;
        //clear the mouse axis.
        this.plot.draw();
        return true;
    };
    this.graph.onmousemove=function(e){
        x=e.offsetX,y=e.offsetY;
        if(this.down){
            this.plot.midX+=x-this.lastPoint[0];
            this.plot.midY+=y-this.lastPoint[1];
        }
        //console.log(this.lastPoint[0]+" "+y-this.lastPoint[1]);
        this.lastPoint=[x,y];
        this.plot.draw();
        return true;
    };
    this.canvas=this.graph.getContext("2d");
    this.width=width;
    this.height=height;
    this.precision=precision;
    this.midX=mx;
    this.midY=my;
    this.scaleX=xscale;
    this.scaleY=yscale;
    this.funcs=[];
    this.pointsets=[];
}
FuncPlot.prototype={
    constructor:FuncPlot,
    setBorder:function(l,r,d,u){
        this.scaleX=this.width/(r-l);
        this.midX=-(l+r)/2*this.scaleX+this.width/2;
        if(u==undefined||d==undefined){
            ;
        }
        else{
            this.scaleY=this.height/(u-d);
            this.midY=(u+d)/2*this.scaleY+this.height/2;
        }
        this.draw();
    },
    addFunc:function(func){
        this.funcs.push(func);
        this.draw();
    },
    addPoints:function(ps){
        this.pointsets.push(ps);
        this.draw();
    },
    setFunc:function(func){
        this.funcs[0]=func;
        this.draw();
    },
    clearFunc:function(){
        this.funcs.splice(0,this.funcs.length);
    },
    clearPointset:function(){
        this.pointsets.splice(0,this.pointsets.length);
    },
    draw:function(){
        this.clear();
        this.drawAxis();
        for(let i=0;i<this.funcs.length;++i){
            this.drawFunc(this.funcs[i]);
        }
        for(let i=0;i<this.pointsets.length;++i){
            this.drawPoints(this.pointsets[i]);
        }
        if(this.graph.in)this.drawMouse();
    },
    //draw function(analog signals)
    //too frequent to call this for mouse draw, I can't come up with a optimizing method right now.
    drawMouse:function(){
        //console.log("draw mouse");
        var x=this.graph.lastPoint[0],y=this.graph.lastPoint[1];
        //vertical
        drawLine(this.canvas,x,0,x,this.height);
        //horizontal
        drawLine(this.canvas,0,y,this.width,y);
        //draw mouse coordinate.
        this.canvas.fillText('('+((x-this.midX)/this.scaleX).toString().slice(0,4)+','+(-(y-this.midY)/this.scaleY).toString().slice(0,4)+')',x,y);
    },
    drawFunc:function(func){
        for(let i=0;i<this.precision;++i){
            //for every virtual point on the canvas, calculate the real points in the 2D plane.
            var x1=i*this.width/this.precision;
            var x2=(i+1)*this.width/this.precision;
            var y1=-func((x1-this.midX)/this.scaleX)*this.scaleY+this.midY;
            var y2=-func((x2-this.midX)/this.scaleX)*this.scaleY+this.midY;
            drawLine(this.canvas,x1,y1,x2,y2);
            //console.log(""+x2+" "+y2);
            //if((i+1)%parseInt(this.precision/10)==0)
            //    drawPoint(this.canvas,x1,this.height-y1,2);
        }
    },
    //draw digital signals
    drawPoints:function(ps){
        if(ps.length==0)return;
        var x,y;
        for(let i=0;i<ps.length;++i){
            if(ps[0].constructor==Array){
                x=ps[i][0]*this.scaleX+this.midX;
                y=-ps[i][1]*this.scaleY+this.midY;
            }
            else{
                x=i*this.scaleX+this.midX;
                y=-ps[i]*this.scaleY+this.midY;
            }
            drawPoint(this.canvas,x,y);
        }
    },
    clear:function(){
        this.canvas.clearRect(0,0,this.width,this.height);
    },
    drawGrid:function(){
        //draw the grid.
        ;
    },
    drawAxis:function(){
        this.canvas.font="#f00";
        //draw X axis and Y axis.
        //X axis
        var minX=1;
        if(this.width/this.scaleX/minX>20){
            for(let i=0;this.width/this.scaleX/minX>20;++i)minX*=(i%2?2:5);
        }
        else if(this.width/this.scaleX/minX<=4){
            for(let i=0;this.width/this.scaleX/minX<=4;++i)minX/=(i%2?5:2);
        }
        drawLine(this.canvas,0,this.midY,this.width,this.midY);
        for(let x=Math.ceil((0-this.midX)/this.scaleX/minX)*minX;x*this.scaleX+this.midX<this.width;x+=minX){//the x is the number x not in pixels
            drawLine(this.canvas,x*this.scaleX+this.midX,this.midY-3,x*this.scaleX+this.midX,this.midY+3);
            this.canvas.fillText(x.toString().slice(0,4),x*this.scaleX+this.midX,clamp(this.midY,0+10/*hard code here*/,this.height));
        }
        //Y axis
        var minY=1;
        if(this.height/this.scaleY/minY>20){
            for(let i=0;this.height/this.scaleY/minY>20;++i)minY*=(i%2?2:5);
        }
        else if(this.height/this.scaleY/minY<=4){
            for(let i=0;this.height/this.scaleY/minY<=4;++i)minY/=(i%2?5:2);
        }
        drawLine(this.canvas,this.midX,0,this.midX,this.height);
        for(let y=Math.ceil((0-this.midY)/this.scaleY/minY)*minY;y*this.scaleY+this.midY<this.height;y+=minY){//the x is the number x not in pixels
            drawLine(this.canvas,this.midX-3,y*this.scaleY+this.midY,this.midX+3,y*this.scaleY+this.midY);
            this.canvas.fillText(-y.toString().slice(0,5),clamp(this.midX,0,this.width-20/*hard code here*/),y*this.scaleY+this.midY);
        }

    }
}



