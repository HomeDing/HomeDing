// ding-ide javascript implementation

var filesObj = document.getElementById("files");
var contentObj = document.getElementById("content");
var progressPanelObj = document.getElementById("progressPanel");
var progressBarObj = document.getElementById("progressBar");
var progressTextObj = document.getElementById("progressText");
var activeFileObj = document.getElementById("activeFile");
var checkerObj = document.getElementById("checker");

var activeFile;
var activeFileName;

var icons = {
  "json": "json",
  "css": "css",
  "dir": "dir",
  "file": "file",
  "htm": "htm",
  "html": "htm",
  "ico": "ico",
  "js": "js",
  "svg": "svg",
  "txt": "txt"
};

// load file into editor from the server
function handleLoadFile(e) {
  var s = e.target.innerText;
  activeFileName = s.split(' ')[0];
  activeFileObj.innerText = activeFileName;
  loadAsync(activeFileName, null, function (txt) {
    contentObj.innerText = txt
  });
} // handleLoadFile()


// delete file on the server
function handleDeleteFile(e) {
  var xhr = new XMLHttpRequest();
  e.stopPropagation();
  e.preventDefault();
  var s = e.target.parentElement.firstElementChild.nextElementSibling.innerText;
  s = s.split(' ')[0];
  var yn = window.confirm("Delete " + s + " ?");
  if (yn) {
    // sync call
    xhr.open("DELETE", s, true);
    xhr.send();
    window.setTimeout(handleReloadFS, 100);
  } // if
} // handleDeleteFile()


// add one file to the directory listing.
function addFileEntry(container, f) {
  var row = document.createElement("div");
  row.className = "row nogutter";
  container.appendChild(row);
  row.addEventListener("click", handleLoadFile);

  // add icon based on file-type
  var fType = f.name.split('.').reverse()[0];
  var entry = document.createElement("div");
  entry.className = "col file-icon";
  var icon = document.createElement("img");
  icon.title = fType;
  if (icons[fType] !== undefined) {
    icon.src = "/ft/" + icons[fType] + ".svg";
  } else {
    icon.src = "/ft/file.svg";
  }

  entry.appendChild(icon);
  row.appendChild(entry);

  // add file name and size
  var entry = document.createElement("div");
  entry.className = "col stretch file-entry";
  entry.innerText = entry.title = f.name + " (" + f.size + ")";
  row.appendChild(entry);

  var delx = document.createElement("div");
  delx.className = "col file-delete";
  delx.innerText = "[X]";
  delx.addEventListener("click", handleDeleteFile);
  row.appendChild(delx);
} // addFileEntry()

// create the directory listing from the fileList dataset.
function listFiles(fileList) {
  // remove all exiting entries
  filesObj.innerHTML = "";

  fileList.forEach(function (f) {
    addFileEntry(filesObj, f);
  })
} // listFiles()


function handleReloadFS() {
  loadAsync('/$list', "application/json", function (result) {
    var x = JSON.parse(result);
    x.sort(function (a, b) {
      var an = a.name.toLowerCase();
      var bn = b.name.toLowerCase();
      if (an < bn) {
        return -1;
      }
      if (an > bn) {
        return 1;
      }
      return 0;
    });
    listFiles(x);
  });
} // handleReloadFS()

var uploadfilelist = []; // Ein Array, das alle hochzuladenden Files enthält
var totalSize = 0; // Enthält die Gesamtgröße aller hochzuladenden Dateien
var totalProgress = 0; // Enthält den aktuellen Gesamtfortschritt
var currentUpload = null; // Enthält die Datei, die aktuell hochgeladen wird

// some drag events need to call stopPropagation and preventDefault to allow custom interactions
function dragHelper(e) {
  e.stopPropagation();
  e.preventDefault();
} // dragHelper()

// start uploading file content
function startUpload(filename, contentType, content) {
  progressPanelObj.classList.remove("fadeout");
  progressTextObj.innerText = "saving...";

  var objHTTP = new XMLHttpRequest(); // new ActiveXObject("MSXML2.XMLHTTP");
  objHTTP.open("PUT", filename, true);
  objHTTP.setRequestHeader("Content-Type", contentType + "; charset=utf-8");

  if (objHTTP.upload) {
    objHTTP.upload.addEventListener('progress', function (e) {
      progressBarObj.style.width = Math.round(100 * e.loaded / e.total) + "%";
    });
  } // if 

  objHTTP.addEventListener("readystatechange", function (p) {
    if ((objHTTP.readyState == 4) && (objHTTP.status >= 200) && (objHTTP.status < 300)) {
      window.setTimeout(handleReloadFS, 100);
      progressTextObj.innerText = "done.";
      progressPanelObj.classList.add("fadeout");
      // fade progress
    } // if
  });
  objHTTP.send(content);
}

function startPutFile(file, contentType) {
  var filename = file.name;
  var reader = new FileReader();

  // show progress

  reader.onload = function (evt) {
    startUpload("/" + file.name, contentType, evt.target.result);
  }; // onload

  reader.readAsText(file); // ,"UTF-8"
} // startPutFile()

// save file from editor back to server.
function handleSave() {
  progressBarObj.style.width = "0";
  activeFileName = window.prompt("Upload File:", activeFileName);
  if (activeFileName !== undefined)
    startUpload(activeFileName, "text/html", contentObj.innerText);
} // handleSave()

// http://html5doctor.com/drag-and-drop-to-server/

// files was dropped on dropzone
function drop(e) {
  progressPanelObj.classList.remove("fadeout");
  progressTextObj.innerText = "saving...";
  progressBarObj.style.width = "0";
  e.stopPropagation();
  e.preventDefault();

  var dtFiles = e.dataTransfer.files;

  var formData = new FormData();
  var root = '/' + (location.hash ? location.hash.substr(1) + '/' : '')
  for (var i = 0; i < dtFiles.length; i++) {
    formData.append('file', dtFiles[i], root + dtFiles[i].name);
  }

  var xmlHttp = new XMLHttpRequest();

  xmlHttp.upload.addEventListener('progress', function (e) {
    progressBarObj.style.width = Math.round(100 * e.loaded / e.total) + "%";
  });

  xmlHttp.addEventListener("readystatechange", function (p) {
    if ((xmlHttp.readyState == 4) && (xmlHttp.status >= 200) && (xmlHttp.status < 300)) {
      window.setTimeout(handleReloadFS, 100);
      progressTextObj.innerText = "done.";
      progressPanelObj.classList.add("fadeout");
      // fade progress
    } // if
  });
  xmlHttp.open("POST", "/");
  xmlHttp.send(formData);
}

var box = document.getElementById("dropzone");
box.addEventListener("dragenter", dragHelper, false);
box.addEventListener("dragover", dragHelper, false);
box.addEventListener("drop", drop, false);

window.addEventListener("load", function () {
  window.setTimeout(handleReloadFS, 400)
});

function jsonCheck() {
  var fName = activeFileObj.innerText;
  if (!fName.toLowerCase().endsWith(".json")) {
    checkerObj.innerText = "--";
    checkerObj.className = "";

  } else {
    var t = contentObj.innerText;
    var o = null;

    try {
      o = JSON.parse(t);
      if (o && typeof o === "object") {
        // ok
      } else {
        o = null;
      } // if
    } catch (e) {
      checkerObj.innerText = e.message;
      checkerObj.className = "invalid";
    }

    if (o !== null) {
      checkerObj.innerText = "valid";
      checkerObj.className = "valid";
    } else {
      // checkerObj.innerText = "invalid";
      checkerObj.className = "invalid";
    }
    window.status = t.length + "--" + (o !== null);
  } // if

} // jsonCheck()

window.setInterval(jsonCheck, 500);