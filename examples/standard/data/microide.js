// microide javascript implementation

var filesObj = document.getElementById("files");
var contentObj = document.getElementById("content");
var progressObj = document.getElementById("progress");

var activeFileObj = document.getElementById("activeFile");
var checkerObj = document.getElementById("checker");

var activeFileName;

// load file into editor from the server
function handleLoadFile(e) {
  var s = e.target.innerText;
  activeFileName = s.split(' ')[0];
  activeFileObj.innerText = activeFileName;

  fetch(activeFileName)
    .then(function (result) {
      return (result.text());
    }).then(function (txt) {
      contentObj.innerText = txt;
    });
} // handleLoadFile()

// General Purpose Promise
function DelayPromise(ms) {
  return new Promise(function (resolve, reject) {
    setTimeout(resolve, ms);
  });
}

// delete file on the server
function handleDeleteFile(e) {
  e.stopPropagation();
  e.preventDefault();
  var s = e.target.parentElement.firstElementChild.innerText;
  s = s.split(' ')[0];
  var yn = window.confirm("Delete " + s + " ?");
  if (yn) {
    fetch(s, { method: 'DELETE' });
    window.setTimeout(handleReloadFS, 100);
  } // if
} // handleDeleteFile()


// add one file to the directory listing.
function addFileEntry(container, f) {
  var row = document.createElement("div");
  row.className = "row nogutter";
  container.appendChild(row);
  row.addEventListener("click", handleLoadFile);

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
  fetch('/$list')
    .then(function (result) {
      return (result.json());
    }).then(function (x) {
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


// some drag events need to call stopPropagation and preventDefault to allow custom interactions
function dragHelper(e) {
  e.stopPropagation();
  e.preventDefault();
} // dragHelper()

// only paste plain text at cursor position.
contentObj.addEventListener("paste", function (e) {
  var sel = window.getSelection();
  if (sel.rangeCount) {
    var txt = e.clipboardData.getData('text/plain');
    sel.deleteFromDocument();
    sel.getRangeAt(0).insertNode(document.createTextNode(txt));
    sel.collapseToEnd();
    e.preventDefault();
  }
})

// start uploading file content
function startUpload(filename, contentType, content) {
  progressObj.classList.remove("fadeout");

  var formData = new FormData();
  var blob = new Blob([content], {
    type: contentType
  });
  formData.append(filename, blob, filename);

  var objHTTP = new XMLHttpRequest();
  objHTTP.open('POST', '/');

  if (objHTTP.upload) {
    objHTTP.upload.addEventListener('progress', function (e) {
      progressObj.max = e.total;
      progressObj.value = e.loaded;
    });
  } // if 

  objHTTP.addEventListener("readystatechange", function (p) {
    if ((objHTTP.readyState == 4) && (objHTTP.status >= 200) && (objHTTP.status < 300)) {
      window.setTimeout(handleReloadFS, 200);
      progressObj.classList.add("fadeout");
    } // if
  });
  objHTTP.send(formData);
}


// save file from editor back to server.
function handleSave() {
  progressObj.value = 0;
  progressObj.max = 1;
  activeFileName = window.prompt("Upload File:", activeFileName);
  if (activeFileName !== undefined)
    startUpload(activeFileName, "text/html", contentObj.innerText);
} // handleSave()


// Format JSON file, remove ugly characters...
function handleFmt() {
  var fName = activeFileObj.innerText;
  if (fName.toLowerCase().endsWith(".json")) {
    var t = contentObj.innerText;
    var o = null;

    // missing comma in '}{'
    t = t.replace(/\}\s*\{/g, "},{");
    // comma before close brackets
    t = t.replace(/,\s*([\}\]])/g, "$1");

    try {
      o = JSON.parse(t);
      if (o && typeof o === "object")
        contentObj.innerText = JSON.stringify(o, null, 2);
    } finally { }// try
  }
} // handleSave()


// files was dropped on dropzone
function drop(e) {
  progressObj.classList.remove("fadeout");
  progressObj.value = 0;
  progressObj.max = 1;
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
    progressObj.max = e.total;
    progressObj.value = e.loaded;
  });

  xmlHttp.addEventListener("readystatechange", function (p) {
    if ((xmlHttp.readyState == 4) && (xmlHttp.status >= 200) && (xmlHttp.status < 300)) {
      window.setTimeout(handleReloadFS, 100);
      progressObj.classList.add("fadeout");
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
    if (checkerObj.textContent != "--") {
      checkerObj.textContent = "--";
      checkerObj.className = "";
    }

  } else {
    var o = null;
    var t = contentObj.innerText;
    if (t.indexOf('\xA0') >= 0)
      contentObj.innerText = t = t.replace(/\u00A0/g, ' ');

    try {
      o = JSON.parse(t);
      if (o && typeof o === "object") {
        // ok
      } else {
        o = null;
      } // if
    } catch (e) {
      checkerObj.title = e.message;
    }

    if (o !== null) {
      checkerObj.textContent = "ok";
      checkerObj.className = "valid";
      checkerObj.title = "";
    } else {
      checkerObj.textContent = "no";
      checkerObj.className = "invalid";
    }
    window.status = t.length + "--" + (o !== null);
  } // if

} // jsonCheck()

window.setInterval(jsonCheck, 500);