var panelObj=document.getElementById("panel"),backObj=document.getElementById("background"),indObj=document.getElementById("ind"),redrawTimer=null,REGION_WIDTH=128,REGION_HEIGHT=36;function _calcSteps(t,e){var n,i=e-t,a=Math.pow(10,Math.floor(Math.log10(i))),r={};return n=i<=2*a?a/2:i<=5*a?a:2*a,r.high=Math.ceil(e/n)*n,r.low=Math.floor(t/n)*n,r.step=n,r}function _calcDateSteps(t,e){const n=86400,i=60*new Date(0).getTimezoneOffset();var a,r={},s=e-t;if(s>43200&&s<n)a=21600;else if(s>n&&s<2*n)a=43200;else if(s>2*n&&s<6*n)a=n;else{if(!(s>6*n&&s<14*n))return _calcSteps(t,e);a=2*n}return r.high=Math.ceil(e/a)*a-i,r.low=Math.floor(t/a)*a+i,r.step=a,r}var graphs=[],minBox={left:1/0,right:-1/0,minY:0,maxY:1},displayBox=minBox;function fmtData(t,e){var n=e,i=t.split(":"),a=Number(e);return"num"==i[0]?n=String(a.toFixed(i[1]||0)):"date"==t?n=new Date(1e3*a).toLocaleDateString("de"):"time"==t?n=new Date(1e3*a).toLocaleTimeString("de"):"datetime"==t&&(n=new Date(1e3*a).toLocaleString("de")),n}function outerBox(t,e){var n=t;return n?e&&(n={left:Math.min(t.left,e.left),right:Math.max(t.right,e.right),minY:Math.min(t.minY,e.minY),maxY:Math.max(t.maxY,e.maxY)}):n=e,n}function boxEqual(t,e){return t.minY===e.minY&&t.maxY===e.maxY&&t.left===e.left&&t.right===e.right}var ChartLineClass=function(t){return{type:"line",redraw:!1,data:null,options:Object.assign({},{linetype:"line",color:"black"},t),svgObj:null,_calcBox:function(t){var e=null;if(t){var n=t.map((function(t){return t.x})),i=t.map((function(t){return t.y}));e={left:Math.min(...n),right:Math.max(...n),minY:Math.min(...i),maxY:Math.max(...i)}}return e},draw:function(t){this.data=t,this.box=this._calcBox(t),this.redraw=!0,_startRedraw()},clear:function(){this.svgObj&&this.svgObj.remove()},fBox:function(t){return outerBox(t,this.box)},fDraw:function(e){this.svgObj&&this.svgObj.remove();var n=this.data;if(n){var i=128/(e.right-e.left),a=REGION_HEIGHT/(e.maxY-e.minY),r=[];if("steps"==this.options.linetype)r=n.map((function(t,n){return"H"+(t.x-e.left)*i+" V"+(t.y-e.minY)*a})),r[0]="M"+(n[0].x-e.left)*i+","+(n[0].y-e.minY)*a;else if("line"==this.options.linetype)r=n.map((function(t,n){return(n>0?"L":"M")+(t.x-e.left)*i+","+(t.y-e.minY)*a}));else if("bezier"==this.options.linetype){var s=n.map((function(t){return{x:(t.x-e.left)*i,y:(t.y-e.minY)*a}})),o=(e.right-e.left)/n.length/2*i,l=[];l.push({dx:0,dy:0});for(let t=1;t<n.length-1;t++){var c=s[t+1].x-s[t-10].x,h=s[t+1].y-s[t-1].y,u=Math.sqrt(c*c+h*h);l.push({dx:o*c/u,dy:o*h/u})}l.push({dx:0,dy:0}),r.push("M"+s[0].x+","+s[0].y);for(let t=0;t<n.length-1;t++)r.push("C"+(s[t].x+l[t].dx)+","+(s[t].y+l[t].dy)+" "+(s[t+1].x-l[t+1].dx)+","+(s[t+1].y-l[t+1].dy)+" "+s[t+1].x+","+s[t+1].y)}var f={class:"chartline",style:"stroke:"+this.options.color,d:r.join("")};t.marker&&(f.style+=";marker:url(#circle)"),this.svgObj=createSVGNode(panelObj,"path",f)}}}};function redraw(){var t=graphs.reduce((function(t,e){return e.fBox(t)}),null),e=!boxEqual(t,displayBox);displayBox=t,graphs.forEach((function(t){(e||t.redraw)&&(t.fDraw(displayBox),t.redraw=!1)})),redrawTimer=null}function _startRedraw(){redrawTimer||(redrawTimer=window.setTimeout(redraw,20))}HLineClass=function(t){return{type:"hLine",data:(t=Object.assign({data:0,color:"black"},t)).data,color:t.color,redraw:!0,svgObj:null,fBox:function(t){return t&&(t.minY=Math.min(this.data,t.minY),t.maxY=Math.max(this.data,t.maxY)),t},fDraw:function(t){this.clear();var e=REGION_HEIGHT/(t.maxY-t.minY),n=(this.data-t.minY)*e;this.svgObj=createSVGNode(panelObj,"line",{class:"hline",style:"stroke:"+this.color,x1:0,y1:n,x2:128,y2:n}),this.redraw=!1},clear:function(){this.svgObj&&this.svgObj.remove(),this.redraw=!0}}},VAxisClass=function(){return{type:"vAxis",redraw:!0,svgObj:document.querySelector("#v-labels"),fBox:function(t){if(t){var e=_calcSteps(t.minY,t.maxY);t.minY=e.low,t.maxY=e.high,this.step=e.step}return t},fDraw:function(t){this.clear();for(var e=t.maxY,n=t.minY,i=this.step,a=i<1?String(i).length-2:0,r=REGION_HEIGHT/(e-n),s=n;s<=e;s+=i)createSVGNode(this.svgObj,"text",{x:11,y:-1*(s-n)*r},String(Number(s).toFixed(a)));this.redraw=!1},clear:function(){_removeChilds(this.svgObj),this.redraw=!0}}},HAxisClass=function(t){return{type:"hAxis",redraw:!0,options:Object.assign({format:"num",color:"black"},t),svgObj:document.querySelector("#h-labels"),fBox:function(t){if(t){var e=_calcDateSteps(t.left,t.right);t.left=e.low,t.right=e.high,this.step=e.step}return t},fDraw:function(t){this.clear();var e=t.right,n=t.left;if(isFinite(n)&&isFinite(e)){var i=this.step,a=i<1?String(i).length-2:0,r=REGION_WIDTH/(e-n),s=this.options.format;"num"===s&&(s="num:"+a);for(var o=n;o<=e;o+=i){var l=fmtData(this.options.format,o).split(",");createSVGNode(this.svgObj,"text",{x:(o-n)*r,y:0,style:"fill:"+this.options.color},l[0]),l[1]&&createSVGNode(this.svgObj,"text",{x:(o-n)*r,y:3,style:"fill:"+this.options.color},l[1])}}this.redraw=!1},clear:function(){_removeChilds(this.svgObj),this.redraw=!0}}},IndicatorClass=function(t){return{type:"indicator",redraw:!1,svgObj:document.querySelector("#ind"),options:Object.assign({xFormat:"num",yFormat:"num"},t),fBox:function(t){return t},fDraw:function(t){},clear:function(){},_setIndicator:function(t,e){indObj.style.display="";var n=indObj.querySelector("line"),i=indObj.querySelector("circle"),a=indObj.querySelector(".info"),r=(e.x-t.left)*REGION_WIDTH/(t.right-t.left),s=(e.y-t.minY)*(REGION_HEIGHT/(t.maxY-t.minY));n.x1.baseVal.value=n.x2.baseVal.value=r,i.cx.baseVal.value=r,i.cy.baseVal.value=s,r+=r<REGION_WIDTH/2?2:-20,s+=s<REGION_HEIGHT/2?1:-11,a.setAttribute("transform","translate("+r+","+s+")");var o=a.querySelectorAll("text");o[0].textContent=fmtData(this.options.yFormat,e.y);var l=fmtData(this.options.xFormat,e.x).split(",");o[1].textContent=l[0],o[2].textContent=l[1]},display:function(t){var e=eventPoint(t),n=graphs[0];if(n){var i=n.data;if(i&&i.length){var a=n.box,r=a.left+(e.x-12)*(a.right-a.left)/REGION_WIDTH,s=i.findIndex((function(t){return t.x>r}));s<0?s=i.length-1:s>0&&i[s].x-r>r-i[s-1].x&&(s-=1),this._setIndicator(displayBox,i[s])}}}}};var indicator=null;panelObj.addEventListener("mouseout",(function(){indObj.style.display="none"})),panelObj.addEventListener("mousemove",(function(t){indicator&&indicator.display(t)})),document.api={options:{},setOptions:function(t){},add:function(t,e){var n,i=-1,a=!1;return Array.isArray(t)?t.forEach(this.add.bind(this)):"object"==typeof t?this.add(t.type,t.options):(t=t.toLowerCase(),e=Object.assign({},e),"line"===t?(n=new ChartLineClass(e),a=!0):"hline"==t?n=new HLineClass(e):"vaxis"==t?n=new VAxisClass:"haxis"==t?n=new HAxisClass(e):"indicator"==t&&(n=new IndicatorClass(e),indicator=n),n&&(a?(i=Math.max(0,graphs.findIndex((function(t){return"line"==t.type}))),graphs.splice(i,0,n)):i=graphs.push(n)-1)),i},draw:function(t,e){var n=graphs[t]||graphs[0];n&&"line"===n.type&&n.draw(e)},clear:function(){graphs.forEach((function(t){t.clear()})),graphs=[],indicator=null},_defaultOptions:{}};