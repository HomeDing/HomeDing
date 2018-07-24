// micro.js
// Collection of functions to help managing complex JSON objects.
// Only works with JSON compatible objects using Arrays, Object, String, Number, Boolean., no functions !

// See also:
// http://goessner.net/articles/JsonPath/


function loadAsync(url, mime, loadCallback, errorCallback) {
  var objHTTP = new XMLHttpRequest(); // new ActiveXObject("MSXML2.XMLHTTP");
  objHTTP.open("GET", url, true);
  if (mime) objHTTP.overrideMimeType(mime);
  objHTTP.addEventListener("readystatechange", function(p) {
    if (objHTTP.readyState == 4) {
      if ((objHTTP.status >= 200) && (objHTTP.status < 300)) {
        loadCallback(objHTTP.responseText);
      } else {
        if (errorCallback) errorCallback(this);
      } // if
    } // if
  });
  objHTTP.send();
} // loadAsync()


function dispatch(id, prop, val) {
  // loadAsync(url, mime, loadCallback, errorCallback)
  alert(id + "?" + prop + "=" + val);
}


// Traverse / Scan a complex object and send all nodes with attributes to a receiver function. */
function jsonParse(obj, cbFunc) {
  /** internal function used in recursion */
  function _jsonParse(path, key, value, cbFunc) {
    var path2 = key ? path + '/' + key : path;
    path2 = path2.replace("/[", "[");

    if (Array.isArray(value)) {
      // traverse all entries in the array
      for (var n = 0; n < value.length; n++) {
        _jsonParse(path2, '[' + n + ']', value[n], cbFunc);
      } // for

    } else if (typeof(value) == "object") {
      // this is an attribute for the receiver function
      cbFunc(path2, null, null);

      // traverse all entries in the object
      for (n in value) {
        _jsonParse(path2, n, value[n], cbFunc);
      } // for

    } else {
      // this is an attribute for the receiver function
      cbFunc(path, key, String(value));
    } // if
  } // _jsonParse()

  // start with root and scan recursively.
  _jsonParse("", "", obj, cbFunc);
} // jsonParse()


var MicroHub = function() {
    this._registrations = {};
    this._registrationsId = 0;

    // subscriptions to changes in the Event Data  
    /**
     * @param {string} matchPath expression for the registration
     * @param {*} callback 
     */
    this.subscribe = function(matchPath, callback) {
      var h = this._registrationsId;

      // treating upper/lowercase equal is not clearly defined, but true with domain names.
      var rn = matchPath.toLocaleLowerCase();

      // build a regexp pattern that will match the event names
      rn = "^" + rn.replace(/(\[|\]|\/|\?)/g, "\\$1").replace(/\*\*/g, "\\S{0,}").replace(/\*/g, "[^\/\?]*") + "$";
      console.log(matchPath, rn);

      var newEntry = {
        id: h,
        match: rn,
        callback: callback
      };
      this._registrations[h] = newEntry;

      this._registrationsId++;
      return (h);
    }; // subscribe

    this.unsubscribe = function(h) {
      this._registrations[h] = null;
    }; // unsubscribe


    this.publishObj = function(obj) {
        var _this = this;
        jsonParse(obj, function(path, key, value) {
          _this.publishValue(path, key ? key.toLowerCase() : null, value);
        });
      } // publishObj()

    this.publishValue = function(path, key, value) {
      var fullPath = path + (key ? '?' + key : "");

      if (fullPath) {
        fullPath = fullPath.toLocaleLowerCase();

        for (var h in this._registrations) {
          var r = this._registrations[h];
          if (r && (fullPath.match(r.match))) {
            r.callback.call(r.scope, path, key, value);
          } // if
        } // for
      } // if
    }; // publish

    this.onunload = function(evt) {
      for (var n in this._registrations) {
        this._registrations[n].callback = null;
        this._registrations[n] = null;
      }
    }; // onunload

  } // MicroEvents class

var hub = new MicroHub();
window.addEventListener('unload', hub.onunload.bind(hub), false);

/*
 <div class="microRoot">
  <div microControl="">...</div>
</div> 
*/


var MicroJCL = function() {
    this._templates = {};
    this._behaviors = {};

    /// A list with all objects that are attached to any behaviour
    this.List = [];

    this.loadTemplates = function(rootObj, bRemove) {
        var all = rootObj.querySelectorAll("[microControl]");
        for (var n = 0; n < all.length; n++) {
          var t = all[n].getAttribute('microControl');
          this._templates[t] = all[n].outerHTML;
        } // for
        if (bRemove) {
          rootObj.parentNode.removeChild(rootObj);
        } // bRemove
      } // load()


    // attach all behaviours
    this.attachAll = function(root) {
        var elems = root.querySelectorAll("[microBehavior]");
        for (var n = 0; n < elems.length; n++) {
          var mb = elems[n].getAttribute('microBehavior');
          var bc = this._behaviors[mb];
          if (bc) this.LoadBehaviour(elems[n], bc);
        } // for
      } // attachAll()


    this.insertTemplate = function(root, controlName, props) {
        var e = null;
        if ((root) && (controlName)) {
          var te = this._templates[controlName];
          if (te) {
            for (var p in props) {
              te = te.replace(new RegExp('\\$\\{' + p + '\\}', 'g'), props[p]);
            }
            te = te.replace(/\$\{microcontrol\}/g, controlName);
            root.insertAdjacentHTML('beforeend', te);
            this.attachAll(root.lastElementChild);
          } // if
        } // if
        return (e);
      } // insertTemplate()


    // attach events, methods and default-values to a html object (using the english spelling)
    this.LoadBehaviour = function(obj, behaviour) {
      if (obj == null) {
        alert("LoadBehaviour: obj argument is missing.");
      } else if (behaviour == null) {
        alert("LoadBehaviour: behaviour argument is missing.");

      } else {
        if (obj.attributes) { // IE9 compatible
          // copy all new attributes to properties
          for (var n = 0; n < obj.attributes.length; n++)
            if (obj[obj.attributes[n].name] == null)
              obj[obj.attributes[n].name] = obj.attributes[n].value;
        } // if

        for (var p in behaviour) {
          if (p.substr(0, 2) == "on") {
            obj.addEventListener(p.substr(2), behaviour[p].bind(obj), false);

          } else if ((behaviour[p] == null) || (behaviour[p].constructor != Function)) {
            // set default-value
            if (obj[p] == null)
              obj[p] = behaviour[p];

          } else {
            // attach method
            obj[p] = behaviour[p];
          } // if
        } // for

        obj._attachedBehaviour = behaviour;
        obj.init();
        this.List.push(obj);
      }
    }; // LoadBehaviour

    /// Find the parent node of a given object that has any behavior attached.
    this.FindBehaviourElement = function(obj) {
      while ((obj) && (obj._attachedBehaviour == null))
        obj = obj.parentNode;
      return (obj);
    }; // FindBehaviourElement


    this.registerBehaviour = function(microType, microBehavior) {
      this._behaviors[microType] = microBehavior;
    };

    this.onunload = function(evt) {
      for (var n in this.List) {
        var obj = this.List[n];
        if ((obj) && (obj.term))
          obj.term();
        for (var a = 0; a < obj.attributes.length; a++)
          obj[obj.attributes[a].name] = null;
      } // for
      for (var n in this.List) {
        this.List[n] = null;
      }
    }; // onunload

  } // MicroDOMTemplate class

var jcl = new MicroJCL();
window.addEventListener('unload', jcl.onunload.bind(jcl), false);


function forAllNodeList(nodeList, callbackFn) {
  for (var n = 0; n < nodeList.length; n++) {
    callbackFn(nodeList[n]);
  }
} // forAllNodeList()

// End