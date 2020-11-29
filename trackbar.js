/**
*for comfortable use of input[type=range].
*associate input[type=range] with some text to render the value.
*/
class Trackbar{
    constructor(divId,name,width,min,max,initialValue=0){
        this.name=name;
        this.width=width;
        this.min=min;
        this.max=max;
        this.onchanges=[];
		//create element and configure them.
		var elem;
        this.div=document.getElementById(divId);
		elem=document.createElement("span");
		elem.style.display="inline-block";
		//Maybe this can be calculated dynamically.
		elem.style.width="70px";
		this.div.append(elem);
		this.text=elem;
		
        elem=document.createElement("input");
		elem.type="range";
		elem.min=this.min; elem.max=this.max;
		elem.style.width=width+"px";
		this.div.append(elem);
		elem.onmousedown=elem.ontouchstart=function(){
			this.clicked=true;
		}
		elem.onmouseup=elem.onmouseout=elem.ontouchcancel=elem.ontouchend=function(){
			this.clicked=false;
		}
		elem.onmousemove=elem.ontouchmove=function(){
			if(this.clicked){
				this.tcb.update();
			}
		}
		elem.onchange=function(){ this.tcb.update(); }
		elem.tcb=this;
		this.inputRange=elem;
		this.value=initialValue;
    }
    get value(){
        return Number.parseInt(this.inputRange.value);
    }
    set value(v){
        this.inputRange.value=v;
        this.setText(v);
    }
    addOnchange(och){
        this.onchanges.push(och);
    }
    setText(v){
        this.text.innerText=this.name+":"+v;
    }
	update(){
		var v=Number.parseInt(this.inputRange.value);
		this.setText(v);
		for(let i=0;i<this.onchanges.length;++i){
			this.onchanges[i](v);
		}
	}


}
