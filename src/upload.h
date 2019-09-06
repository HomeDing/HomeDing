// file: upload.h
// contains the minimal html for uploading files

// from setup1.min.js: configure the network
const char *setupContent=
"<!DOCTYPE html><html lang='en'><head><title>HomeDing Setup</title></head><body><h1>HomeDing Setup</h1><h2>Connect to network</h2><table><tr><td>Network:</td><td><select id='conNetworks'style='width:18em'></select></td></tr><tr><td>Passphrase:</td><td><input id='conPass'style='width:18em'></td></tr></table><button id='connectPass'>Connect</button>"
"<script>var scanDone=2,checkTimer=0,selObj=document.getElementById('conNetworks');function load(e,n){var t=new XMLHttpRequest;t.open('GET',e,!0),t.addEventListener('readystatechange',function(e){4==t.readyState&&200<=t.status&&t.status<300&&n(t.responseText)}),t.send()}"
"function startScan(){2==scanDone&&(selObj.innerHTML='<option selected disabled>scanning...</option>',scanDone=0,checkTimer||(checkTimer=window.setInterval(checkScan,200)))}"
"function checkScan(){load('/$scan',function(e){0<e.length&&(scanDone=2,scanResult(JSON.parse(e)))})}"
"function scanResult(e){window.clearInterval(checkTimer),checkTimer=0,selObj.innerHTML='';var n=document.createElement('option');n.value=0,n.text='select...',n.disabled=!0,selObj.options.add(n),e.forEach(function(e){var n=document.createElement('option');n.value=n.text=e.id,selObj.options.add(n)})}document.getElementById('connectPass').addEventListener('click',function(){load('/$connect?n='+selObj.value+'&p='+document.getElementById('conPass').value,function(){})}),startScan()</script>"
"</body></html>";

// from upload.min.js
const char *uploadContent=
"<!DOCTYPE html><html lang='en'><head><title>Upload</title></head><body><div id='zone' style='width:9em;height:6em;padding:1em;background-color:#ddd'>Drop here</div><a href='#ft'>FT-Upload</a> - <a href='#i'>I-Upload</a>"
"<script>function dragHelper(e){e.stopPropagation(),e.preventDefault()}"
"function dropped(e){dragHelper(e);for(var n=e.dataTransfer.files,t=new FormData,a='/'+(location.hash?location.hash.substr(1)+'/':''),r=0;r<n.length;r++)t.append('file',n[r],a+n[r].name);var d=new XMLHttpRequest;d.addEventListener('readystatechange',function(e){4==d.readyState&&200<=d.status&&d.status<300&&window.alert('done.')}),d.open('POST','/'),d.send(t)}var zoneObj=document.getElementById('zone');zoneObj.addEventListener('dragenter',dragHelper,!1),zoneObj.addEventListener('dragover',dragHelper,!1),zoneObj.addEventListener('drop',dropped,!1)</script>"
"</body></html>";

// from boot.min.js
const char *bootContent=
"<!DOCTYPE html><html lang='en'><head><title>Bootstrap</title></head><body><h1>Bootstrap</h1><table><tr><td rowspan='2'><button id='startIt'>start it..</button></td><td><progress id='bar'value='0'max='1'></progress></td></tr><tr><td id='info'>&nbsp;</td></tr></table>"
"<script>var theRoot='https://homeding.github.io/',bObj=document.querySelector('#startIt'),theBar=document.querySelector('#bar'),theInfo=document.querySelector('#info'),work={status:'start',list:null,files:0,done:0},stepTimer=0;"
"function logText(t){theInfo.innerText=t}"
"function doStart(){work.status='wait';var o=new XMLHttpRequest;o.open('GET',theRoot+'list.txt',!0),o.responseType='text',o.onload=function(t){var e=o.responseText.replace(/\r?\n/g,';');e=e.replace(/;$/,''),work.list=e.split(';'),work.files=work.list.length,work.status='run'},o.send(null)}"
"function doFile(){work.status='wait';var n=work.list.shift();work.done++,theBar.max=work.files,theBar.value=work.done;var s=new XMLHttpRequest;s.open('GET',theRoot+n,!0),s.responseType='arraybuffer',s.onerror=function(t){logText(n+': error: failed.'),work.status='run'},s.onload=function(t){if(200!=s.status)logText(n+': error: '+s.status);else{var e=s.response;if(e){var o=new FormData;o.append('file',new Blob([e]),'/'+n);var r=new XMLHttpRequest;r.addEventListener('readystatechange',function(t){4==r.readyState&&200<=r.status&&r.status<300&&logText(n+'('+e.byteLength+')')}),r.open('POST','/'),r.send(o)}}work.status=0==work.list.length?'end':'run'},s.send(null),0==work.list.length&&(work.status='end')}"
"function step(){'start'==work.status?doStart():'run'==work.status?doFile():'end'==work.status&&(stepTimer&&(window.clearInterval(stepTimer),logText('done'),window.location.href='/'),stepTimer=0)}location.hash?theRoot+=location.hash.substr(1)+'/':theRoot+='v01/',bObj.addEventListener('click',function(){stepTimer=window.setInterval(step,330)})</script>"
"</body></html>";
