"use strict";var MicroState,__extends=this&&this.__extends||function(){var n=function(t,e){return(n=Object.setPrototypeOf||{__proto__:[]}instanceof Array&&function(t,e){t.__proto__=e}||function(t,e){for(var i in e)Object.prototype.hasOwnProperty.call(e,i)&&(t[i]=e[i])})(t,e)};return function(t,e){function i(){this.constructor=t}n(t,e),t.prototype=null===e?Object.create(e):(i.prototype=e.prototype,new i)}}(),__decorate=this&&this.__decorate||function(t,e,i,n){var o,s=arguments.length,r=s<3?e:null===n?n=Object.getOwnPropertyDescriptor(e,i):n;if("object"==typeof Reflect&&"function"==typeof Reflect.decorate)r=Reflect.decorate(t,e,i,n);else for(var a=t.length-1;0<=a;a--)(o=t[a])&&(r=(s<3?o(r):3<s?o(e,i,r):o(e,i))||r);return 3<s&&r&&Object.defineProperty(e,i,r),r},__assign=this&&this.__assign||function(){return(__assign=Object.assign||function(t){for(var e,i=1,n=arguments.length;i<n;i++)for(var o in e=arguments[i])Object.prototype.hasOwnProperty.call(e,o)&&(t[o]=e[o]);return t}).apply(this,arguments)};!function(t){t[t.PREP=1]="PREP",t[t.INIT=2]="INIT",t[t.LOADED=3]="LOADED"}(MicroState=MicroState||{});var MicroRegistry=function(){function t(){this._tco=null,this._registry={},this._state=MicroState.PREP,this._unloadedList=[],this.List=[],window.addEventListener("load",this.init.bind(this)),window.addEventListener("unload",this.onunload.bind(this))}return t.prototype.loadFile=function(t){var e=this;return fetch(t).then(function(t){return t.text()}).then(function(t){t=document.createRange().createContextualFragment(t);e._tco&&e._tco.appendChild(t)})},t.prototype.attach=function(t){var e;this._state===MicroState.LOADED?!(e=t.getAttribute("u-is"))||(e=this._registry[e])&&this.loadBehavior(t,e):this._unloadedList.push(t)},t.prototype._setPlaceholders=function(t,i){var e=this;function n(e){return Object.getOwnPropertyNames(i).forEach(function(t){return e=e.replace(new RegExp("\\$\\{"+t+"\\}","g"),i[t])}),e}if(i)if(t.nodeType===Node.TEXT_NODE)t.textContent&&(t.textContent=n(t.textContent));else if(t.nodeType===Node.ELEMENT_NODE){var o=t.attributes;if("http://www.w3.org/2000/svg"===t.namespaceURI)for(var s=0;s<o.length;s++)0<=(r=o[s].value).indexOf("${")&&(t[o[s].name].baseVal=n(r));else for(var r,s=0;s<o.length;s++)0<=(r=o[s].value).indexOf("${")&&t.setAttribute(o[s].name,n(r));t.childNodes.forEach(function(t){e._setPlaceholders(t,i)})}},t.prototype.isVisible=function(t){var e=!1;return 0<t.offsetWidth&&0<t.offsetHeight&&(e=(t=t.getBoundingClientRect()).top<=window.innerHeight&&0<=t.bottom),e},t.prototype.loadDataImage=function(t){t.dataset.src&&this.isVisible(t)&&(t.src=t.dataset.src)},t.prototype.insertTemplate=function(t,e,i){var n=this,o=null;return t&&e&&this._tco&&((e=this._tco.querySelector('[u-control="'+e.toLowerCase()+'"]'))&&(o=e.cloneNode(!0)),o&&(o.params=i,this._setPlaceholders(o,i),t.appendChild(o),t.querySelectorAll("[u-is]").forEach(function(t){return micro.attach(t)}),this._setPlaceholders(o,i),t.querySelectorAll("[data-src]:not([src])").forEach(function(t){return n.loadDataImage(t)}))),o},t.prototype.loadBehavior=function(t,e){var i=e,n=t;if(t)if(e){if(n._attachedBehavior!==e){if(t.attributes)for(var o=0;o<t.attributes.length;o++){var s=t.attributes[o];t[s.name]||(t[s.name]=s.value)}for(var r in i)"on_touchstart"===r?t.addEventListener(r.substr(3),i[r].bind(t),{passive:!0}):"on_"===r.substr(0,3)?t.addEventListener(r.substr(3),i[r].bind(t),!1):"on"===r.substr(0,2)?t.addEventListener(r.substr(2),i[r].bind(t),!1):(null==i[r]||i[r].constructor!==Function)&&t[r]||(t[r]=i[r]);n._attachedBehavior=e,t.parentElement!==this._tco&&(n.connectedCallback(),this.List.push(t))}}else;else;},t.prototype.define=function(t,e){this._registry[t]=e},t.prototype.onunload=function(t){this.List.forEach(function(t){t&&t.term&&t.term();for(var e=0;e<t.attributes.length;e++)t[t.attributes[e].name]=null});for(var e=0;e<this.List.length;e++)delete this.List[e];this.List=[]},t.prototype.init=function(){this._state=MicroState.INIT,this._tco=document.getElementById("u-templates"),this._tco||(this._tco=createHTMLElement(document.body,"div",{id:"u-templates"})),"complete"===document.readyState?this.init2():document.addEventListener("readystatechange",this.init2)},t.prototype.init2=function(){var i=this;"complete"===document.readyState&&(this._state=MicroState.LOADED,this._unloadedList.forEach(function(t){var e=t.getAttribute("u-is");e&&((e=i._registry[e])&&i.loadBehavior(t,e),i.List.push(t))}),this._unloadedList=[])},t}(),micro=new MicroRegistry,obs=new MutationObserver(function(t,e){for(var i=0,n=t;i<n.length;i++)n[i].addedNodes.forEach(function(t){t.getAttribute&&t.getAttribute("u-is")&&micro.attach(t)})});obs.observe(document,{childList:!0,subtree:!0}),document.addEventListener("DOMContentLoaded",function(){function t(){document.querySelectorAll("[data-src]:not([src])").forEach(function(t){return micro.loadDataImage(t)})}window.addEventListener("scroll",t),window.setTimeout(t,40)});var MicroControlClass=function(){function t(){}return t.prototype.connectedCallback=function(){},t}();function MicroControl(e){return function(t){return micro.define(e,new t),t}}var GenericWidgetClass=function(e){function t(){var t=null!==e&&e.apply(this,arguments)||this;return t.microid="",t.data={},t.subId=0,t.actions=[],t}return __extends(t,e),t.prototype.connectedCallback=function(){e.prototype.connectedCallback.call(this),this.data={id:this.microid},this.subId=hub.subscribe(this.microid+"?*",this.newData.bind(this)),hub.replay(this.subId)},t.prototype.newData=function(t,e,i){var n;e&&i&&(this.data[e]=i,(n=this.querySelector("img,h3"))&&setAttr(n,"title",JSON.stringify(this.data,null,1).replace("{\n","").replace("\n}",""))),"active"===e&&this.classList.toggle("active",toBool(i)),["span","div"].forEach(function(t){this.querySelectorAll(t+"[u-active='"+e+"']").forEach(function(t){var e=toBool(i);setAttr(t,"value",e?"1":"0"),setAttr(t,"title",e?"active":"not active"),t.classList.toggle("active",e)})},this),["h2","h3","h4","span","button"].forEach(function(t){this.querySelectorAll(t+"[u-text='"+e+"']").forEach(function(t){t.textContent!==i&&(t.textContent=i)})},this),["input","select"].forEach(function(t){this.querySelectorAll(t+"[u-value='"+e+"']").forEach(function(t){"radio"===t.type?t.checked=t.value===i:t.value!==i&&(t.value=i||"")})},this),["button","label"].forEach(function(t){this.querySelectorAll(t+"[u-action='${"+e+"}']").forEach(function(t){setAttr(t,"u-action",i||"")})},this),this.querySelectorAll("span[u-color='"+e+"']").forEach(function(t){var e=(e=i?i.replace(/^x/,"#"):"#888").replace(/^#\S{2}(\S{6})$/,"#$1");t.style.backgroundColor=e})},t.prototype.dispatchNext=function(){var t,e=this;!this.actions||(t=this.actions.shift())&&(t=(t=t.split("="))[0]+"="+encodeURIComponent(t[1]),fetch(t).then(function(){0<e.actions.length?debounce(e.dispatchNext.bind(e))():updateAsap&&updateAsap()}))},t.prototype.dispatchAction=function(t,e){var i=this;null!==t&&null!==e&&(t.includes("/")?(t.replace("${v}",encodeURI(e)),t.split(",").forEach(function(t){return i.actions.push("/$board/"+t)})):this.actions.push("/$board"+this.microid+"?"+t+"="+encodeURI(e)),debounce(this.dispatchNext.bind(this))())},t.prototype.showSys=function(){return toBool(getHashParams({sys:!1}).sys)},t.prototype.on_change=function(t){t=t.target;this.dispatchAction(t.getAttribute("u-value"),t.value)},t.prototype.on_click=function(t){for(var t=t.target,e=t;e&&!e.getAttribute("u-action")&&e!==this;)e=e.parentElement;e&&this.dispatchAction(e.getAttribute("u-action"),e.getAttribute("value")||"1"),t.classList.contains("setconfig")?ModalDialogClass.open("configelementdlg",this.data):t.classList.contains("setactive")?this.dispatchAction(toBool(this.data.active)?"stop":"start","1"):"H3"===t.tagName&&ModalDialogClass.openFocus(this)},__decorate([MicroControl("generic")],t)}(MicroControlClass),BL0937WidgetClass=function(i){function t(){var t=null!==i&&i.apply(this,arguments)||this;return t.mode="",t}return __extends(t,i),t.prototype.connectedCallback=function(){i.prototype.connectedCallback.call(this),this.data={id:this.microid},this.subId=hub.subscribe(this.microid+"?mode",this.switchMode.bind(this)),hub.replay(this.subId)},t.prototype.setMode=function(t){var e;t&&t!==this.mode&&(e=void 0,null!=(e=this.querySelector('[u-text="'+this.mode+'"]'))&&e.parentElement&&(e.parentElement.style.display="none"),null!=(e=this.querySelector('span[u-text="'+t+'"]'))&&e.parentElement&&(e.parentElement.style.display=""),this.mode=t)},t.prototype.switchMode=function(t,e,i){this.setMode(i)},t.prototype.on_click=function(t){var e=t.target;"mode"===e.getAttribute("u-action")&&this.setMode(e.value),i.prototype.on_click.call(this,t)},__decorate([MicroControl("bl0937")],t)}(GenericWidgetClass),ButtonGroupWidgetClass=function(n){function t(){var t=null!==n&&n.apply(this,arguments)||this;return t._count=0,t._blockElem=null,t}return __extends(t,n),t.prototype.connectedCallback=function(){n.prototype.connectedCallback.call(this),this._blockElem=this.querySelector(".block")},t.prototype.newData=function(t,e,i){n.prototype.newData.call(this,t,e,i),e&&i&&"title"!==e&&(this._count%2==0&&(this._blockElem=createHTMLElement(this,"div",{class:"block"})),this._blockElem&&(createHTMLElement(this._blockElem,"button",{"u-action":i}).textContent=e),this._count++)},__decorate([MicroControl("buttongroup")],t)}(GenericWidgetClass),ButtonWidgetClass=function(n){function t(){var t=null!==n&&n.apply(this,arguments)||this;return t._onclick="",t._ondoubleclick="",t._onpress="",t._timer=0,t._start=0,t._duration=0,t}return __extends(t,n),t.prototype.newData=function(t,e,i){n.prototype.newData.call(this,t,e,i),"onclick"===e?this._onclick=i:"ondoubleclick"===e?this._ondoubleclick=i:"onpress"===e&&(this._onpress=i)},t.prototype.on_click=function(){var t;800<this._duration?this._onpress&&this.dispatchAction(this._onpress,"1"):((t=this)._timer&&window.clearTimeout(this._timer),this._timer=window.setTimeout(function(){t.dispatchAction(t._onclick,"1")},250))},t.prototype.on_dblclick=function(){this._timer&&window.clearTimeout(this._timer),this.dispatchAction(this._ondoubleclick,"1")},t.prototype.on_pointerdown=function(){this._start=(new Date).valueOf()},t.prototype.on_pointerup=function(){this._duration=(new Date).valueOf()-this._start},__decorate([MicroControl("button")],t)}(GenericWidgetClass),ColorWidgetClass=function(s){function t(){var t=null!==s&&s.apply(this,arguments)||this;return t.colObj=null,t.hueObj=null,t.lightObj=null,t.satObj=null,t.whiteObj=null,t._hvalue="00ff0000",t._value="00000000",t._hue=127,t._lightness=127,t._saturation=255,t._white=127,t._hasWhite=!1,t}return __extends(t,s),t.prototype.connectedCallback=function(){s.prototype.connectedCallback.call(this),this.colObj=this.querySelector(".color"),this.hueObj=this.querySelector(".hue"),this.satObj=this.querySelector(".band.saturation"),this.lightObj=this.querySelector(".band.lightness"),this.whiteObj=this.querySelector(".white")},t.prototype.updateBands=function(){},t.prototype.newData=function(t,e,i){var n,o;i&&("value"===e?(6===(i=i.replace("x","")).length?this._value="00"+i:8===i.length&&(this._value=i),n=this._value.substr(2),o=this.rgbToHsl(this.rgb(n)),this._hvalue=this.hslToRGB(o.h,100,50),this.hueObj&&(this.hueObj.value=String(o.h)),this.satObj&&(this.satObj.value=String(o.s),this.satObj.style.background="linear-gradient(to right, #808080 0%, #"+this._hvalue+" 100%)"),this.lightObj&&(this.lightObj.value=String(o.l),this.lightObj.style.background="linear-gradient(to right, #000 0%, #"+this._hvalue+" 50%, #fff 100%)"),this.whiteObj&&(this.whiteObj.value=String(parseInt(this._value.substr(0,2),16))),this.colObj&&(this.colObj.style.backgroundColor="#"+n)):"config"===e&&(this._hasWhite="WRGB"===i,this.whiteObj&&(this.whiteObj.style.display=this._hasWhite?"":"none"))),s.prototype.newData.call(this,t,e,i)},t.prototype.on_input=function(t){var e=t.target,t="";e===this.hueObj?(this._hue=parseInt(e.value,10),this._hvalue=this.hslToRGB(this._hue,100,50)):e===this.lightObj?(this._lightness=parseInt(e.value,10),this.dispatchAction("lightness",e.value)):e===this.satObj?(this._saturation=parseInt(e.value,10),this.dispatchAction("saturation",e.value)):e===this.whiteObj&&(this._white=parseInt(e.value,10)),t="x"+this.hslToRGB(this._hue,Math.round(100*this._saturation/255),Math.round(100*this._lightness/255)),this._hasWhite&&(t="x"+this.x16(this._white)+t.substr(1)),this.dispatchAction("value",t)},t.prototype.rgb=function(t){var e={r:0,g:0,b:0};return t&&6<=t.length&&(t=t.substr(t.length-6),e.r=parseInt(t.substr(0,2),16),e.g=parseInt(t.substr(2,2),16),e.b=parseInt(t.substr(4,2),16)),e},t.prototype.rgbToHsl=function(t){var e={h:0,s:0,l:0},i=Math.max(t.r,t.g,t.b),n=Math.min(t.r,t.g,t.b),o=i-n,s=i+n;return e.l=Math.round(s/2),0<o&&(n=0,e.s=Math.round(255*(127<e.l?o/(510-s):o/s)),i===t.r?n=(t.g-t.b)/o+(t.g<t.b?6:0):i===t.g?n=(t.b-t.r)/o+2:i===t.b&&(n=(t.r-t.g)/o+4),e.h=Math.round(60*n)%360),e},t.prototype.hslToRGB=function(t,e,i){var n=this.hueObj||this;n.style.backgroundColor="hsl("+t+", "+e+"%, "+i+"%)";n=getComputedStyle(n,null).backgroundColor,n=String(n).replace(/[^0-9,]/g,"").split(",");return this.x16(n[0])+this.x16(n[1])+this.x16(n[2])},t.prototype.x16=function(t){t=Number(t).toString(16);return 1===t.length&&(t="0"+t),t},__decorate([MicroControl("color")],t)}(GenericWidgetClass),DSTimeWidgetClass=function(i){function t(){return null!==i&&i.apply(this,arguments)||this}return __extends(t,i),t.prototype.isoDate=function(){function t(t){return(t<10?"0":"")+t}var e=new Date;return e.getFullYear()+"-"+t(e.getMonth()+1)+"-"+t(e.getDate())+" "+t(e.getHours())+":"+t(e.getMinutes())+":"+t(e.getSeconds())},t.prototype.connectedCallback=function(){i.prototype.connectedCallback.call(this),this._nowObj=this.querySelector(".setnow"),window.setInterval(function(){this._nowObj&&setTextContent(this._nowObj,this.isoDate())}.bind(this),200)},t.prototype.on_click=function(t){var e=t.target;e&&e.classList.contains("setnow")?this.dispatchAction("time",this.isoDate()):i.prototype.on_click.call(this,t)},__decorate([MicroControl("dstime")],t)}(GenericWidgetClass),DisplayDotWidgetClass=function(n){function t(){var t=null!==n&&n.apply(this,arguments)||this;return t.lastValue=null,t._dispElem=null,t._elem=null,t._x=0,t._y=0,t._value=!1,t}return __extends(t,n),t.prototype.connectedCallback=function(){n.prototype.connectedCallback.call(this),this._dispElem=document.querySelector("#panel .display"),this._dispElem&&(this._elem=createHTMLElement(this._dispElem,"span",{class:"dot"}),this.updateElem()),this.showSys()||(this.style.display="none")},t.prototype.newData=function(t,e,i){n.prototype.newData.call(this,t,e,i),e&&i&&this._elem&&("value"===e?this._value=toBool(i):"page"===e?this._elem.setAttribute("displayPage",i):"x"===e?this._x=Number(i):"y"===e&&(this._y=Number(i)),this.updateElem())},t.prototype.updateElem=function(){this._elem&&(this._elem.style.top=this._y+"px",this._elem.style.left=this._x+"px",this._elem.classList.toggle("active",this._value))},__decorate([MicroControl("displaydot")],t)}(GenericWidgetClass),DisplayLineWidgetClass=function(n){function t(){var t=null!==n&&n.apply(this,arguments)||this;return t._dispElem=null,t._elem=null,t._x0=0,t._x1=0,t._y0=0,t._y1=0,t}return __extends(t,n),t.prototype.connectedCallback=function(){n.prototype.connectedCallback.call(this),this._dispElem=document.querySelector("#panel .display"),this._dispElem&&(this._elem=createHTMLElement(this._dispElem,"span",{class:"line"}),this.updateElem()),this.showSys()||(this.style.display="none")},t.prototype.newData=function(t,e,i){n.prototype.newData.call(this,t,e,i),e&&i&&this._elem&&("page"===e?this._elem.setAttribute("displayPage",i):null!=this["_"+e]&&(this["_"+e]=i),this.updateElem())},t.prototype.updateElem=function(){this._elem&&(this._elem.style.top=this._y0+"px",this._elem.style.left=this._x0+"px",this._elem.style.width=this._x1-this._x0+"px",this._elem.style.height=this._y1-this._y0+"px")},__decorate([MicroControl("displayline")],t)}(GenericWidgetClass),DisplayTextWidgetClass=function(o){function t(){var t=null!==o&&o.apply(this,arguments)||this;return t.lastValue=null,t._dispElem=null,t._grid=1,t._elem=null,t._prefix="",t._postfix="",t}return __extends(t,o),t.prototype.connectedCallback=function(){o.prototype.connectedCallback.call(this),this._dispElem=document.querySelector("#panel .display"),this._dispElem&&(this._dispElem.getAttribute("grid")&&(this._grid=Number(this._dispElem.getAttribute("grid"))),this._elem=createHTMLElement(this._dispElem,"span",{class:"text",style:"top:0;left:0;display:none"})),this.showSys()||(this.style.display="none")},t.prototype.newData=function(t,e,i){var n;o.prototype.newData.call(this,t,e,i),e&&i&&this._elem&&("value"===e?(n=(""+this._prefix+i+this._postfix).replace(/ /g,"&nbsp;"),this._elem.innerHTML!==n&&(this._elem.innerHTML=n)):"page"===e?this._elem.setAttribute("displayPage",i):"x"===e?(n=Number(i)*this._grid,this._elem.style.left=(1<this._grid?7*n/10:n)+"px"):"y"===e?this._elem.style.top=Number(i)*this._grid+"px":"fontsize"===e?(this._elem.style.fontSize=i+"px",this._elem.style.lineHeight=i+"px",this._elem.style.height=i+"px"):"prefix"===e?this._prefix=i:"postfix"===e&&(this._postfix=i))},__decorate([MicroControl("displaytext")],t)}(GenericWidgetClass),DisplayWidgetClass=function(o){function t(){var t=null!==o&&o.apply(this,arguments)||this;return t.displayPage="",t._dialogElem=null,t}return __extends(t,o),t.prototype.connectedCallback=function(){o.prototype.connectedCallback.call(this),this._dialogElem=this.querySelector(".display")},t.prototype.newData=function(t,e,i){var n=this;o.prototype.newData.call(this,t,e,i),e&&i&&"page"===e&&i!==this.displayPage&&(this.displayPage=i,null!==(i=this._dialogElem)&&void 0!==i&&i.querySelectorAll(":scope > span").forEach(function(t){var e=t.getAttribute("displayPage")||"1";t.style.display=e===n.displayPage?"":"none"}))},__decorate([MicroControl("display")],t)}(GenericWidgetClass),IncludeWidgetClass=function(e){function t(){var t=null!==e&&e.apply(this,arguments)||this;return t.query=null,t}return __extends(t,e),t.prototype.connectedCallback=function(){this.query=this.getAttribute("ref");var t,e=document.querySelector("#u-templates "+this.query);e&&(t=e.cloneNode(!0),null!=(e=this.parentElement)&&e.replaceChild(t,this))},__decorate([MicroControl("include")],t)}(MicroControlClass);function jsonParse(t,r){!function e(t,i,n){var o=(o=i?t+"/"+i:t).replace("/[","[");if(Array.isArray(n))for(var s=0;s<n.length;s++)e(o,"["+s+"]",n[s]);else"object"==typeof n?(r(o,null,null),Object.getOwnPropertyNames(n).forEach(function(t){return e(o,t,n[t])})):r(t,i,String(n))}("","",t)}function jsonFind(t,e){"/"===e[0]&&(e=e.substr(1));for(var i=e.split("/");t&&0<i.length;){var n=i[0];t[n]||(t[n]={}),t=t[n],i.shift()}return t}var LogWidgetClass=function(n){function t(){var t=null!==n&&n.apply(this,arguments)||this;return t.filename=null,t.lineSVGObj=null,t.xFormat="datetime",t.yFormat="num",t}return __extends(t,n),t.prototype.connectedCallback=function(){n.prototype.connectedCallback.call(this),this.lineSVGObj=this.querySelector("object")},t.prototype.loadData=function(){var t=this.filename,i="",e=fetch(t,{cache:"no-store"}).then(function(t){return t.text()}).then(function(t){i=i+"\n"+t}),t=fetch(t.replace(".txt","_old.txt"),{cache:"no-store"}).then(function(t){return t.text()}).then(function(t){i=t+"\n"+i}).catch(function(){});Promise.allSettled([e,t]).then(function(){var e=/^\d{4,},\d+/,t=i.split("\n").filter(function(t){return t.match(e)});this.api.draw(this.lChart,t.map(function(t){t=t.split(",");return{x:t[0],y:t[1]}}))}.bind(this))},t.prototype.loadSVG=function(){var t=!1;if(this.lineSVGObj){var e=null;try{e=this.lineSVGObj.getSVGDocument()}catch(t){}e&&e.api&&(this.api=this.lineSVGObj.getSVGDocument().api,this.lChart=this.api.add("line",{linetype:"line"}),this.api.add(["VAxis",{type:"hAxis",options:{format:"datetime"}},{type:"indicator",options:{xFormat:this.xFormat,yFormat:this.yFormat}}]),this.loadData(),t=!0)}t||window.setTimeout(this.loadSVG.bind(this),1e3)},t.prototype.newData=function(t,e,i){n.prototype.newData.call(this,t,e,i),"filename"===e?(this.filename=i,this.loadSVG()):"xformat"===e?this.xFormat=i:"yformat"===e&&(this.yFormat=i)},__decorate([MicroControl("log")],t)}(GenericWidgetClass),ModalDialogClass=function(e){function t(){var t=e.call(this)||this;return t._isOpen=!1,t.params={},t}var i;return __extends(t,e),(i=t).open=function(t,e){micro.insertTemplate(document.body,"modal",e).open(t,e)},t.openFocus=function(t){micro.insertTemplate(document.body,"modal",{}).openFocus(t)},t.next=function(t,e){this._stack[this._stack.length-1].next(t,e)},t.save=function(t){var e=null===(e=this._stack[this._stack.length-2]._frameObj)||void 0===e?void 0:e.firstElementChild;e.save&&e.save(t)},t.close=function(){this._stack[this._stack.length-1].close()},t.prototype.connectedCallback=function(){e.prototype.connectedCallback.call(this),this._backObj=this.querySelector(".modalBack"),this._frameObj=this.querySelector(".modalFrame")},t.prototype.open=function(t,e){i._stack.push(this),this._backObj&&this._frameObj&&(null!=(e=null==(e=micro.insertTemplate(this._frameObj,t,e))?void 0:e.querySelector("input,button,select"))&&e.focus())},t.prototype.next=function(t,e){var i;this._backObj&&this._frameObj&&(null!==(i=this._frameObj.firstElementChild)&&void 0!==i&&i.remove(),null!=(e=null==(e=micro.insertTemplate(this._frameObj,t,e))?void 0:e.querySelector("input,button,select"))&&e.focus())},t.prototype.openFocus=function(t){var e,i;t&&t.parentElement&&this._frameObj&&(i=(this._focusObj=t).getBoundingClientRect(),this._placeObj=createHTMLElement(t.parentElement,"div",{style:"width:"+i.width+"px;height:"+i.height+"px",class:t.className},t),e=4,e=Math.min(4,(window.innerWidth-64)/i.width),e=Math.min(e,(window.innerHeight-64)/i.height),i=createHTMLElement(this._frameObj,"div",{style:"width:"+e*i.width+"px;height:"+e*i.height+"px"}).getBoundingClientRect(),t.classList.add("modal-object"),t.style.top=i.top+"px",t.style.left=i.left+"px",t.style.width=i.width+"px",t.style.height=i.height+"px")},t.prototype.on_click=function(t){"close"===t.target.getAttribute("u-action")&&this.close()},t.prototype.close=function(){var t;this._focusObj&&((t=this._focusObj).classList.remove("modal-object"),t.style.top="",t.style.left="",t.style.width="",t.style.height="",null!==(t=this._placeObj)&&void 0!==t&&t.remove()),i._stack.pop(),this.remove()},t._stack=[],i=__decorate([MicroControl("modal")],t)}(GenericWidgetClass),PWMOutWidgetClass=function(e){function t(){var t=null!==e&&e.apply(this,arguments)||this;return t._range=255,t.lastValue=null,t}return __extends(t,e),t.prototype.connectedCallback=function(){e.prototype.connectedCallback.call(this),hub.subscribe(this.microid+"?*",this.newValue.bind(this))},t.prototype.newValue=function(t,e,i){var n,o;"range"===e?this._range=Number(i):"value"===e&&this.lastValue!==i&&((o=(n=this.querySelector(".ux-levelbar")).offsetHeight)-1<(e=o*Number(i)/this._range)&&(e=o-1),e<1&&(e=1),n.style.borderBottomWidth=e+"px",this.lastValue=i)},__decorate([MicroControl("pwmout")],t)}(GenericWidgetClass),SliderWidgetClass=function(n){function t(){var t=null!==n&&n.apply(this,arguments)||this;return t._slider=null,t._handle=null,t._lastValue=-1,t._maxright=100,t._x=0,t._xOffset=0,t.unit=1,t._type="int",t.minvalue=0,t.maxvalue=255,t}return __extends(t,n),t.prototype.connectedCallback=function(){var t,e;this._slider=this.querySelector(".u-slider"),this._handle=this.querySelector(".handle"),n.prototype.connectedCallback.call(this),this._handle&&(t=this._handle.parentElement,e=getComputedStyle(t),this._maxright=t.clientWidth-this._handle.offsetWidth-parseFloat(e.paddingLeft)-parseFloat(e.paddingRight))},t.prototype._adjustHandle=function(t){this._handle&&(t=t-this.minvalue,t=Math.round(t*this._maxright/(this.maxvalue-this.minvalue)),t=Math.min(this._maxright,Math.max(0,t)),this._handle.style.left=t+"px")},t.prototype.newData=function(t,e,i){n.prototype.newData.call(this,t,e,i),"value"===e?(t=Number(i))!==this._lastValue&&(this._adjustHandle(t),this._lastValue=t):"min"===e?this.minvalue=Number(i):"max"===e?this.maxvalue=Number(i):"step"===e?this.unit=Number(i):"type"===e&&(this._type=i,this._slider&&"string"===i&&(this._slider.style.display="none"))},t.prototype.on_click=function(t){for(var e=t.target;e&&e!==this;){if("LABEL"===e.tagName&&e.classList.contains("up")){this.dispatchAction("up","1");break}if("LABEL"===e.tagName&&e.classList.contains("down")){this.dispatchAction("down","1");break}e=e.parentElement}n.prototype.on_click.call(this,t)},t.prototype.on_mousedown=function(t){t.target===this._handle&&this.MoveStart(t)},t.prototype.MoveStart=function(t){this._xOffset=0;for(var e=this._handle.offsetParent;null!=e;)this._xOffset+=e.offsetLeft,e=e.offsetParent;this._x=t.clientX-(this._handle.offsetLeft+this._xOffset),this._moveFunc=this._onmousemove.bind(this),document.addEventListener("mousemove",this._moveFunc,!1),this._upFunc=this._onmouseup.bind(this),document.addEventListener("mouseup",this._upFunc,!1),t.cancelBubble=!0,t.returnValue=!1},t.prototype._onmousemove=function(t){t=t.clientX-this._x-this._xOffset,t=Math.min(this._maxright,Math.max(0,t)),t=Math.round(t*(this.maxvalue-this.minvalue)/this._maxright+this.minvalue),t=Math.round(t/this.unit)*this.unit;this._adjustHandle(t),t!==this._lastValue&&(this._lastValue=t,this.dispatchAction("value",String(t)))},t.prototype._onmouseup=function(t){t=t||window.event,document.removeEventListener("mousemove",this._moveFunc),document.removeEventListener("mouseup",this._upFunc)},t.prototype.on_touchstart=function(t){t.targetTouches[0].target;this._handle},__decorate([MicroControl("slider")],t)}(GenericWidgetClass),SwitchWidgetClass=function(n){function t(){return null!==n&&n.apply(this,arguments)||this}return __extends(t,n),t.prototype.on_click=function(t){for(var e=this.querySelector(".u-switch"),i=t.srcElement;null!==i&&i!==this&&i!==e;)i=i.parentElement;i===e?this.dispatchAction("toggle","1"):n.prototype.on_click.call(this,t)},__decorate([MicroControl("switch")],t)}(GenericWidgetClass);function toBool(t){if(!t)return!1;switch(t.toLowerCase().trim()){case"true":case"yes":return!0;case"false":case"no":case"0":case null:return!1;default:return Boolean(t)}}function toSeconds(t){return(t=t.toLowerCase()).endsWith("h")?60*parseInt(t,10)*60:t.endsWith("m")?60*parseInt(t,10):t.endsWith("s")?parseInt(t,10):t.includes(":")?(Date.parse("1.1.1970 "+t)-Date.parse("1.1.1970"))/1e3:Number(t)}function setTextContent(t,e){t.textContent!==e&&(t.textContent=e)}function setAttr(t,e,i){t.getAttribute(e)!==i&&t.setAttribute(e,i)}function changeConfig(t,e){var i,n="/env.json",o=JSON.parse(hub.read("env")),s=jsonFind(o,t);for(i in 0===Object.keys(s).length&&(n="/config.json",s=jsonFind(o=JSON.parse(hub.read("config")),t)),e)e[i]?s[i]=e[i]:delete s[i];t=new FormData;t.append(n,new Blob([JSON.stringify(o)],{type:"text/html"}),n),fetch("/",{method:"POST",body:t}).then(function(){window.alert("saved.")})}function debounce(i,n){var o;return void 0===n&&(n=20),function(){var t=this,e=arguments;o&&clearTimeout(o),o=window.setTimeout(function(){o=0,i.apply(t,e)},n)}}function getHashParams(t){var e=__assign({},t);return window.location.hash.substr(1).split("&").forEach(function(t){t=t.split("=");e[t[0]]=t[1]}),e}function createHTMLElement(t,e,i,n){void 0===n&&(n=null);var o=document.createElement(e);if(i)for(var s in i)i.hasOwnProperty(s)&&o.setAttribute(s,i[s]);return n?t.insertBefore(o,n):t.appendChild(o),o}var TimerWidgetClass=function(n){function t(){var t=null!==n&&n.apply(this,arguments)||this;return t.wt=0,t.pt=0,t.ct=0,t.time=0,t}return __extends(t,n),t.prototype.newData=function(t,e,i){n.prototype.newData.call(this,t,e,i),"waittime"===e?this.wt=toSeconds(i):"pulsetime"===e?this.pt=toSeconds(i):"cycletime"===e?this.ct=toSeconds(i):"time"===e&&(this.time=toSeconds(i)),this.ct<this.wt+this.pt&&(this.ct=this.wt+this.pt),0<this.ct&&(e=(t=this.querySelector(".u-bar")).clientWidth/this.ct,(i=t.querySelector(".pulse")).style.left=Math.floor(this.wt*e)+"px",i.style.width=Math.floor(this.pt*e)+"px",t.querySelector(".current").style.width=Math.floor(this.time*e)+"px")},__decorate([MicroControl("timer")],t)}(GenericWidgetClass),MicroHub=function(){function t(){this._registrations={},this._registrationsId=0,this._store={}}return t.prototype.read=function(t){return this._findStoreObject(this.pPath(t))[this.pKey(t)]},t.prototype.write=function(t,e){this._findStoreObject(this.pPath(t))[this.pKey(t)]=e},t.prototype.subscribe=function(t,e,i){void 0===i&&(i=!1);var n=this._registrationsId,t="^"+t.toLocaleLowerCase().replace(/(\[|\]|\/|\?)/g,"\\$1").replace(/\*\*/g,"\\S{0,}").replace(/\*/g,"[^/?]*")+"$",o={id:n,match:RegExp(t),callback:e};return this._registrations[n]=o,this._registrationsId++,i&&jsonParse(this._store,function(t,e,i){var n=t+(e?"?"+e:"");n&&(n=n.toLocaleLowerCase()).match(o.match)&&o.callback(t,e?e.toLowerCase():null,i)}.bind(this)),n},t.prototype.unsubscribe=function(t){delete this._registrations[t]},t.prototype.replay=function(t){var o=this._registrations[t];o&&jsonParse(this._store,function(t,e,i){var n=t+(e?"?"+e:"");n&&(n=n.toLocaleLowerCase()).match(o.match)&&o.callback(t,e?e.toLowerCase():null,i)}.bind(this))},t.prototype.publishObj=function(t){jsonParse(t,function(t,e,i){this.publishValue(t,e?e.toLowerCase():"",i||"")}.bind(this))},t.prototype.publishValue=function(e,i,n){var o=e+(i?"?"+i:"");o&&(i&&(this._findStoreObject(e)[i]=n),o=o.toLocaleLowerCase(),Object.values(this._registrations).forEach(function(t){o.match(t.match)&&t.callback(e,i,n)}))},t.prototype.onunload=function(){var e=this;Object.getOwnPropertyNames(this._registrations).forEach(function(t){return delete e._registrations[t]})},t.prototype._findStoreObject=function(t){var e=this._store;"/"===t[0]&&(t=t.substr(1));for(var i=t.split("/");0<i.length&&e[i[0]];)e=e[i[0]],i.shift();for(;0<i.length&&i[0];)e=e[i[0]]={},i.shift();return e},t.prototype.pPath=function(t){"/"===t[0]&&(t=t.substr(1));t=t.split("/");return t.slice(0,t.length-1).join("/")},t.prototype.pKey=function(t){t=t.split("/");return t[t.length-1]},t}(),hub=new MicroHub;window.addEventListener("unload",hub.onunload.bind(hub),!1);