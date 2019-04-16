// file: upload.h
// contains the minimal html for uploading files

const char *uploadContent=
"<!DOCTYPE html><html lang='en'><head><title>Upload</title></head><body><div id='zone' style='width:9em;height:6em;padding:1em;background-color:#ddd'>Drop here</div><a href='#ft'>FT-Upload</a> - <a href='#i'>I-Upload</a>"
"<script>function dragHelper(e){e.stopPropagation(),e.preventDefault()}function dropped(e){dragHelper(e);for(var n=e.dataTransfer.files,t=new FormData,a='/'+(location.hash?location.hash.substr(1)+'/':''),r=0;r<n.length;r++)t.append('file',n[r],a+n[r].name);var d=new XMLHttpRequest;d.addEventListener('readystatechange',function(e){4==d.readyState&&200<=d.status&&d.status<300&&window.alert('done.')}),d.open('POST','/'),d.send(t)}var zoneObj=document.getElementById('zone');zoneObj.addEventListener('dragenter',dragHelper,!1),zoneObj.addEventListener('dragover',dragHelper,!1),zoneObj.addEventListener('drop',dropped,!1)</script>"
"</body></html>";
