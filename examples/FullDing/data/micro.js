"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __assign = (this && this.__assign) || function () {
    __assign = Object.assign || function(t) {
        for (var s, i = 1, n = arguments.length; i < n; i++) {
            s = arguments[i];
            for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                t[p] = s[p];
        }
        return t;
    };
    return __assign.apply(this, arguments);
};
var MicroState;
(function (MicroState) {
    MicroState[MicroState["PREP"] = 1] = "PREP";
    MicroState[MicroState["INIT"] = 2] = "INIT";
    MicroState[MicroState["LOADED"] = 3] = "LOADED";
})(MicroState || (MicroState = {}));
var MicroRegistry = (function () {
    function MicroRegistry() {
        this._tco = null;
        this._registry = {};
        this._state = MicroState.PREP;
        this._unloadedList = [];
        this.List = [];
        window.addEventListener('load', this.init.bind(this));
        window.addEventListener('unload', this.onunload.bind(this));
    }
    MicroRegistry.prototype.init = function () {
        this._state = MicroState.INIT;
        this._tco = document.getElementById('u-templates');
        if (!this._tco) {
            var t = document.createElement('div');
            t.id = 'u-templates';
            this._tco = document.body.appendChild(t);
        }
        if (document.readyState === 'complete') {
            this.init2();
        }
        else {
            document.addEventListener('readystatechange', this.init2);
        }
    };
    MicroRegistry.prototype.init2 = function () {
        var _this = this;
        if (document.readyState === 'complete') {
            this._state = MicroState.LOADED;
            this._unloadedList.forEach(function (el) {
                var cn = el.getAttribute('u-is');
                if (cn) {
                    var bc = _this._registry[cn];
                    if (bc) {
                        _this.loadBehavior(el, bc);
                    }
                    _this.List.push(el);
                }
            });
            this._unloadedList = [];
        }
    };
    MicroRegistry.prototype.loadFile = function (fName) {
        var scope = this;
        var ret = fetch(fName)
            .then(function (result) {
            return result.text();
        })
            .then(function (html) {
            var f = document.createRange().createContextualFragment(html);
            if (scope._tco) {
                scope._tco.appendChild(f);
            }
        });
        return ret;
    };
    MicroRegistry.prototype.attach = function (elem) {
        if (this._state === MicroState.LOADED) {
            var cn = elem.getAttribute('u-is');
            if (cn) {
                var bc = this._registry[cn];
                if (bc) {
                    this.loadBehavior(elem, bc);
                }
            }
        }
        else {
            this._unloadedList.push(elem);
        }
    };
    MicroRegistry.prototype._setPlaceholders = function (obj, props) {
        var _this = this;
        function fill(val, props) {
            for (var p in props)
                val = val.replace(new RegExp('\\$\\{' + p + '\\}', 'g'), props[p]);
            return val;
        }
        if (obj.nodeType === Node.TEXT_NODE) {
            if (obj.textContent) {
                obj.textContent = fill(obj.textContent, props);
            }
        }
        else if (obj.nodeType == Node.ELEMENT_NODE) {
            var attr = obj.attributes;
            for (var i = 0; i < attr.length; i++) {
                var v = attr[i].value;
                if (v.indexOf('${') >= 0) {
                    obj[attr[i].name] = attr[i].value = fill(v, props);
                }
            }
            obj.childNodes.forEach(function (c) {
                _this._setPlaceholders(c, props);
            });
        }
    };
    MicroRegistry.prototype.insertTemplate = function (root, controlName, props) {
        var e = null;
        if (root && controlName && this._tco) {
            var te = this._tco.querySelector('[u-control="' + controlName.toLowerCase() + '"]');
            if (te)
                e = te.cloneNode(true);
            if (e) {
                e.params = props;
                this._setPlaceholders(e, props);
                root.appendChild(e);
            }
        }
        return e;
    };
    MicroRegistry.prototype.loadBehavior = function (obj, behavior) {
        var b = behavior;
        var o = obj;
        if (!obj) {
            console.error('loadBehavior: obj argument is missing.');
        }
        else if (!behavior) {
            console.error('loadBehavior: behavior argument is missing.');
        }
        else if (o._attachedBehavior == behavior) {
        }
        else {
            if (obj.attributes) {
                for (var n = 0; n < obj.attributes.length; n++) {
                    var a = obj.attributes[n];
                    if (!o[a.name])
                        o[a.name] = a.value;
                }
            }
            for (var p in b) {
                if (p.substr(0, 2) == 'on') {
                    obj.addEventListener(p.substr(2), b[p].bind(obj), false);
                }
                else if (b[p] == null || b[p].constructor != Function) {
                    if (!o[p])
                        o[p] = b[p];
                }
                else {
                    o[p] = b[p];
                }
            }
            o._attachedBehavior = behavior;
            if (obj.parentElement !== this._tco) {
                o.connectedCallback();
                this.List.push(obj);
            }
        }
    };
    MicroRegistry.prototype.define = function (name, mixin) {
        this._registry[name] = mixin;
    };
    MicroRegistry.prototype.openModal = function (tmplName, data) {
        var modalObj = document.getElementById('modal');
        var containerObj = document.getElementById('modalContainer');
        if ((modalObj) && (containerObj)) {
            containerObj.innerHTML = '';
            micro.insertTemplate(containerObj, tmplName, data);
            modalObj.classList.remove('hidden');
        }
    };
    MicroRegistry.prototype.closeModal = function () {
        var modalObj = document.getElementById('modal');
        var containerObj = document.getElementById('modalContainer');
        if ((modalObj) && (containerObj)) {
            modalObj.classList.add('hidden');
            containerObj.innerHTML = '';
        }
    };
    MicroRegistry.prototype.onunload = function (_evt) {
        for (var n in this.List) {
            var obj = this.List[n];
            if (obj && obj.term)
                obj.term();
            for (var a = 0; a < obj.attributes.length; a++)
                obj[obj.attributes[a].name] = null;
        }
        for (var n in this.List) {
            delete this.List[n];
        }
        this.List = [];
    };
    return MicroRegistry;
}());
var micro = new MicroRegistry();
var obs = new MutationObserver(function (mutationsList, _observer) {
    for (var _i = 0, mutationsList_1 = mutationsList; _i < mutationsList_1.length; _i++) {
        var mutation = mutationsList_1[_i];
        mutation.addedNodes.forEach(function (n) {
            if (n.getAttribute && n.getAttribute('u-is'))
                micro.attach(n);
        });
    }
});
obs.observe(document, { childList: true, subtree: true });
function MicroControl(isSelector) {
    return function (target) {
        micro.define(isSelector, new target());
        return target;
    };
}
var MicroControlClass = (function () {
    function MicroControlClass() {
    }
    MicroControlClass.prototype.connectedCallback = function () {
        this.el = this;
    };
    return MicroControlClass;
}());
var GenericWidgetClass = (function (_super) {
    __extends(GenericWidgetClass, _super);
    function GenericWidgetClass() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.microid = '';
        _this.data = {};
        _this.subId = 0;
        return _this;
    }
    GenericWidgetClass.prototype.connectedCallback = function () {
        _super.prototype.connectedCallback.call(this);
        this.data = { id: this.microid };
        this.subId = hub.subscribe(this.microid + '?*', this.newData.bind(this));
        hub.replay(this.subId);
    };
    GenericWidgetClass.prototype.newData = function (_path, key, value) {
        if (this.el && key && value) {
            this.data[key] = value;
            var ic = this.el.querySelector('img');
            if (ic) {
                setAttr(ic, 'title', JSON.stringify(this.data, null, 1)
                    .replace('{\n', '')
                    .replace('\n}', ''));
            }
        }
        ['span', 'div'].forEach(function (elType) {
            if (this.el) {
                this.el.querySelectorAll(elType + ("[u-active='" + key + "']")).forEach(function (elem) {
                    var b = toBool(value);
                    setAttr(elem, 'value', b ? '1' : '0');
                    setAttr(elem, 'title', b ? 'active' : 'not active');
                    elem.classList.toggle('active', b);
                });
            }
        }, this);
        ['h2', 'h4', 'span'].forEach(function (elType) {
            if (this.el) {
                this.el.querySelectorAll(elType + "[u-text='" + key + "']").forEach(function (elem) {
                    if (elem.textContent != value)
                        elem.textContent = value;
                });
            }
        }, this);
        ['input', 'select'].forEach(function (elType) {
            if (this.el) {
                this.el.querySelectorAll(elType + "[u-value='" + key + "']").forEach(function (elem) {
                    if (elem.value != value)
                        elem.value = value ? value : "";
                });
            }
        }, this);
    };
    GenericWidgetClass.prototype.dispatchAction = function (prop, val) {
        if (prop !== null && val !== null) {
            fetch("/$board" + this.microid + "?" + prop + "=" + encodeURI(val)).then(function () {
                if (updateAsap)
                    updateAsap();
            });
        }
    };
    GenericWidgetClass.prototype.onchange = function (e) {
        var src = e.target;
        this.dispatchAction(src.getAttribute('u-value'), src.value);
    };
    GenericWidgetClass.prototype.onclick = function (e) {
        var src = e.target;
        var a = src.getAttribute('u-action');
        if (src && a)
            this.dispatchAction(a, src['value']);
        if (this.el && src.classList.contains('setconfig')) {
            micro.openModal('configelementdlg', this.data);
        }
    };
    GenericWidgetClass = __decorate([
        MicroControl('generic')
    ], GenericWidgetClass);
    return GenericWidgetClass;
}(MicroControlClass));
var ButtonWidgetClass = (function (_super) {
    __extends(ButtonWidgetClass, _super);
    function ButtonWidgetClass() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    ButtonWidgetClass.prototype.onpointerdown = function (e) {
        var src = e.target;
        if ((src) && (src.classList.contains("u-button"))) {
            src.classList.add("active");
            this.dispatchAction("value", '1');
        }
    };
    ButtonWidgetClass.prototype.onpointerup = function (e) {
        var src = e.target;
        if (src.classList.contains("u-button")) {
            src.classList.remove("active");
            this.dispatchAction("value", '0');
        }
    };
    ButtonWidgetClass = __decorate([
        MicroControl("button")
    ], ButtonWidgetClass);
    return ButtonWidgetClass;
}(GenericWidgetClass));
var DSTimeWidgetClass = (function (_super) {
    __extends(DSTimeWidgetClass, _super);
    function DSTimeWidgetClass() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    DSTimeWidgetClass.prototype.isoDate = function () {
        function pad02(num) {
            return (((num < 10) ? '0' : '') + num);
        }
        ;
        var d = new Date();
        var ds = d.getFullYear() + '-' + pad02(d.getMonth() + 1) + '-' + pad02(d.getDate()) +
            ' ' + pad02(d.getHours()) + ':' + pad02(d.getMinutes()) + ':' + pad02(d.getSeconds());
        return (ds);
    };
    DSTimeWidgetClass.prototype.connectedCallback = function () {
        _super.prototype.connectedCallback.call(this);
        if (this.el) {
            this._nowObj = this.el.querySelector(".setnow");
            window.setInterval(function () {
                if (this._nowObj) {
                    setTextContent(this._nowObj, this.isoDate());
                }
            }.bind(this), 200);
        }
    };
    DSTimeWidgetClass.prototype.onclick = function (e) {
        var src = e.target;
        if ((src) && (src.classList.contains("setnow"))) {
            this.dispatchAction("time", this.isoDate());
        }
        else {
            _super.prototype.onclick.call(this, e);
        }
    };
    DSTimeWidgetClass = __decorate([
        MicroControl("dstime")
    ], DSTimeWidgetClass);
    return DSTimeWidgetClass;
}(GenericWidgetClass));
var DisplayDotWidgetClass = (function (_super) {
    __extends(DisplayDotWidgetClass, _super);
    function DisplayDotWidgetClass() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.lastValue = null;
        _this._dispElem = null;
        _this._elem = null;
        _this._x = 0;
        _this._y = 0;
        _this._value = false;
        return _this;
    }
    DisplayDotWidgetClass.prototype.connectedCallback = function () {
        _super.prototype.connectedCallback.call(this);
        this._dispElem = document.querySelector("#panel .display");
        hub.subscribe(this.microid + "?*", this.newValue.bind(this), true);
    };
    DisplayDotWidgetClass.prototype.updateDisp = function (create) {
        if (this._dispElem) {
            if (create && !this._elem) {
                this._elem = document.createElement("span");
                this._dispElem.appendChild(this._elem);
            }
            if (this._elem) {
                this._elem.className = "dot";
                this._elem.style.top = this._y + "px";
                this._elem.style.left = this._x + "px";
                this._elem.classList.toggle("active", this._value);
            }
        }
    };
    DisplayDotWidgetClass.prototype.newValue = function (_path, key, value) {
        if (key && value) {
            if (key === "active" && !this._elem) {
                this.updateDisp(true);
            }
            else if (key === "value") {
                this._value = toBool(value);
                this.updateDisp(false);
            }
            else if (key === "x") {
                this._x = Number(value);
                this.updateDisp(false);
            }
            else if (key === "y") {
                this._y = Number(value);
                this.updateDisp(false);
            }
            else {
                console.log("key", key, value);
            }
        }
    };
    DisplayDotWidgetClass = __decorate([
        MicroControl("displaydot")
    ], DisplayDotWidgetClass);
    return DisplayDotWidgetClass;
}(GenericWidgetClass));
var DisplayTextWidgetClass = (function (_super) {
    __extends(DisplayTextWidgetClass, _super);
    function DisplayTextWidgetClass() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.lastValue = null;
        _this._dispElem = null;
        _this._dispGrid = 1;
        _this._elem = null;
        _this._prefix = '';
        _this._postfix = '';
        return _this;
    }
    DisplayTextWidgetClass.prototype.connectedCallback = function () {
        _super.prototype.connectedCallback.call(this);
        this._dispElem = document.querySelector("#panel .display");
        if (this._dispElem) {
            if (this._dispElem.getAttribute('grid'))
                this._dispGrid = Number(this._dispElem.getAttribute('grid'));
            var e = this._elem = document.createElement('span');
            e.className = 'text';
            e.style.top = '0px';
            e.style.left = '0px';
            this._dispElem.appendChild(e);
        }
        hub.subscribe(this.microid + "?*", this.newValue.bind(this), true);
    };
    DisplayTextWidgetClass.prototype.newValue = function (_path, key, value) {
        if (key && value && this._elem) {
            if (key === 'value') {
                var t = "" + this._prefix + value + this._postfix;
                this._elem.innerHTML = t.replace(/ /g, '&nbsp;');
            }
            else if (key === 'x') {
                this._elem.style.left = (Number(value) * this._dispGrid * 7 / 10) + 'px';
            }
            else if (key === 'y') {
                this._elem.style.top = (Number(value) * this._dispGrid) + 'px';
            }
            else if (key === 'fontsize') {
                this._elem.style.fontSize = value + 'px';
                this._elem.style.lineHeight = value + 'px';
                this._elem.style.height = value + 'px';
            }
            else if (key === 'prefix') {
                this._prefix = value;
            }
            else if (key === 'postfix') {
                this._postfix = value;
            }
        }
    };
    DisplayTextWidgetClass = __decorate([
        MicroControl("displaytext")
    ], DisplayTextWidgetClass);
    return DisplayTextWidgetClass;
}(GenericWidgetClass));
function jsonParse(obj, cbFunc) {
    function _jsonParse(path, key, value, cbFunc) {
        var path2 = key ? path + '/' + key : path;
        path2 = path2.replace('/[', '[');
        if (Array.isArray(value)) {
            for (var n = 0; n < value.length; n++) {
                _jsonParse(path2, '[' + n + ']', value[n], cbFunc);
            }
        }
        else if (typeof value == 'object') {
            cbFunc(path2, null, null);
            for (var k in value) {
                _jsonParse(path2, k, value[k], cbFunc);
            }
        }
        else {
            cbFunc(path, key, String(value));
        }
    }
    _jsonParse('', '', obj, cbFunc);
}
function jsonFind(obj, path) {
    if (path[0] === '/') {
        path = path.substr(1);
    }
    var steps = path.split('/');
    while (obj && steps.length > 0) {
        var p = steps[0];
        if (!obj[p]) {
            obj[p] = {};
        }
        obj = obj[p];
        steps.shift();
    }
    return obj;
}
var LogWidgetClass = (function (_super) {
    __extends(LogWidgetClass, _super);
    function LogWidgetClass() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.filename = null;
        _this.lineSVGObj = null;
        return _this;
    }
    LogWidgetClass.prototype.connectedCallback = function () {
        _super.prototype.connectedCallback.call(this);
        if (this.el) {
            this.lineSVGObj = this.el.querySelector('object');
            hub.subscribe(this.microid + '?*', this.newValue.bind(this), true);
        }
    };
    LogWidgetClass.prototype.loadData = function () {
        fetch(this.filename)
            .then(function (result) {
            return result.text();
        })
            .then(function (pmValues) {
            var re = /^\d{2,},\d+/;
            var pmArray = pmValues.split('\n').filter(function (e) {
                return e.match(re);
            });
            this.api.updateLineChartData(this.lChart, pmArray.map(function (v) {
                var p = v.split(',');
                return { x: p[0], y: p[1] };
            }));
        }.bind(this));
    };
    LogWidgetClass.prototype.loadSVG = function () {
        var done = false;
        if (this.lineSVGObj) {
            var svgObj = null;
            try {
                svgObj = (this.lineSVGObj.getSVGDocument());
            }
            catch (err) { }
            if ((svgObj) && (svgObj.api)) {
                this.api = this.lineSVGObj.getSVGDocument().api;
                this.lChart = this.api.addLineChart();
                this.api.addVAxis();
                this.api.addHAxis();
                this.loadData();
                done = true;
            }
        }
        if (!done) {
            window.setTimeout(this.loadSVG.bind(this), 1000);
        }
    };
    LogWidgetClass.prototype.newValue = function (_path, key, value) {
        if (key === 'filename') {
            this.filename = value;
            this.loadSVG();
        }
    };
    LogWidgetClass = __decorate([
        MicroControl('log')
    ], LogWidgetClass);
    return LogWidgetClass;
}(GenericWidgetClass));
var PWMOutWidgetClass = (function (_super) {
    __extends(PWMOutWidgetClass, _super);
    function PWMOutWidgetClass() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this._range = 255;
        _this.lastValue = null;
        return _this;
    }
    PWMOutWidgetClass.prototype.connectedCallback = function () {
        _super.prototype.connectedCallback.call(this);
        hub.subscribe(this.microid + "?*", this.newValue.bind(this));
    };
    PWMOutWidgetClass.prototype.newValue = function (_path, key, value) {
        if (key == "range") {
            this._range = Number(value);
        }
        else if (key == "value") {
            if (this.el && this.lastValue !== value) {
                var o = this.el.querySelector(".ux-levelbar");
                var h = o.offsetHeight;
                var bh = (h * Number(value)) / this._range;
                if (bh > h - 1)
                    bh = h - 1;
                if (bh < 1)
                    bh = 1;
                o.style.borderBottomWidth = bh + "px";
                this.lastValue = value;
            }
        }
    };
    PWMOutWidgetClass = __decorate([
        MicroControl("pwmout")
    ], PWMOutWidgetClass);
    return PWMOutWidgetClass;
}(GenericWidgetClass));
var SliderWidgetClass = (function (_super) {
    __extends(SliderWidgetClass, _super);
    function SliderWidgetClass() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this._handle = null;
        _this._lastValue = -1;
        _this._maxright = 100;
        _this._x = 0;
        _this._xOffset = 0;
        _this.unit = 1;
        _this.minvalue = 0;
        _this.maxvalue = 255;
        return _this;
    }
    SliderWidgetClass.prototype.connectedCallback = function () {
        _super.prototype.connectedCallback.call(this);
        if (this.el) {
            this._handle = this.el.querySelector(".handle");
            var p = this._handle.parentElement;
            var ps = getComputedStyle(p);
            this._maxright = p.clientWidth - this._handle.offsetWidth - parseFloat(ps.paddingLeft) - parseFloat(ps.paddingRight);
        }
    };
    SliderWidgetClass.prototype._adjustHandle = function (val) {
        if (this._handle) {
            var left = val - this.minvalue;
            left = Math.round(left * this._maxright / (this.maxvalue - this.minvalue));
            left = Math.min(this._maxright, Math.max(0, left));
            this._handle.style.left = left + "px";
        }
    };
    SliderWidgetClass.prototype.newData = function (path, key, value) {
        _super.prototype.newData.call(this, path, key, value);
        if (key == 'value') {
            var v = Number(value);
            if (v != this._lastValue) {
                this._adjustHandle(v);
                this._lastValue = v;
            }
        }
        else if (key == 'min') {
            this.minvalue = Number(value);
        }
        else if (key == 'max') {
            this.maxvalue = Number(value);
        }
        else if (key == 'step') {
            this.unit = Number(value);
        }
    };
    SliderWidgetClass.prototype.onclick = function (e) {
        if (this.el) {
            var src = e.srcElement;
            while (src != null && src.classList.length == 0)
                src = src.parentElement;
            if (src != null) {
                if (src.classList.contains('up')) {
                    this.dispatchAction('up', '1');
                }
                else if (src.classList.contains('down')) {
                    this.dispatchAction('down', '1');
                }
                else {
                    _super.prototype.onclick.call(this, e);
                }
            }
        }
    };
    SliderWidgetClass.prototype.onmousedown = function (evt) {
        if (evt.target == this._handle) {
            this.MoveStart(evt);
        }
    };
    SliderWidgetClass.prototype.MoveStart = function (evt) {
        this._xOffset = 0;
        var obj = this._handle.offsetParent;
        while (obj != null) {
            this._xOffset += obj.offsetLeft;
            obj = obj.offsetParent;
        }
        this._x = evt.clientX - (this._handle.offsetLeft + this._xOffset);
        this._moveFunc = this._onmousemove.bind(this);
        document.addEventListener('mousemove', this._moveFunc, false);
        this._upFunc = this._onmouseup.bind(this);
        document.addEventListener('mouseup', this._upFunc, false);
        evt.cancelBubble = true;
        evt.returnValue = false;
    };
    SliderWidgetClass.prototype._onmousemove = function (evt) {
        var left = evt.clientX - this._x - this._xOffset;
        left = Math.min(this._maxright, Math.max(0, left));
        var val = Math.round(left * (this.maxvalue - this.minvalue) / this._maxright + this.minvalue);
        val = Math.round(val / this.unit) * this.unit;
        this._adjustHandle(val);
        if (val != this._lastValue) {
            this._lastValue = val;
            this.dispatchAction('value', String(val));
        }
    };
    SliderWidgetClass.prototype._onmouseup = function (evt) {
        evt = evt || window.event;
        document.removeEventListener("mousemove", this._moveFunc);
        document.removeEventListener("mouseup", this._upFunc);
    };
    SliderWidgetClass.prototype.ontouchstart = function (evt) {
        var t = evt.targetTouches[0].target;
        if (t == this._handle) {
            console.log('TouchStart');
        }
    };
    SliderWidgetClass = __decorate([
        MicroControl("slider")
    ], SliderWidgetClass);
    return SliderWidgetClass;
}(GenericWidgetClass));
var SwitchWidgetClass = (function (_super) {
    __extends(SwitchWidgetClass, _super);
    function SwitchWidgetClass() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    SwitchWidgetClass.prototype.onclick = function (e) {
        if (this.el) {
            var o = this.el.querySelector(".u-switch");
            var src = e.srcElement;
            while (src != null && src != this.el && src != o)
                src = src.parentElement;
            if (src == o) {
                this.dispatchAction('toggle', '1');
            }
            else {
                _super.prototype.onclick.call(this, e);
            }
        }
    };
    SwitchWidgetClass = __decorate([
        MicroControl("switch")
    ], SwitchWidgetClass);
    return SwitchWidgetClass;
}(GenericWidgetClass));
function upload(filename, content) {
    var formData = new FormData();
    var blob = new Blob([content], {
        type: 'text/html'
    });
    formData.append(filename, blob, filename);
    var objHTTP = new XMLHttpRequest();
    objHTTP.open('POST', '/');
    objHTTP.addEventListener('readystatechange', function () {
        if (this.readyState == 4 && this.status >= 200 && this.status < 300) {
            alert('saved.');
        }
    });
    objHTTP.send(formData);
}
function changeConfig(id, newConfig) {
    var c = JSON.parse(hub.read('config'));
    var node = jsonFind(c, id);
    for (var n in newConfig) {
        if (newConfig[n]) {
            node[n] = newConfig[n];
        }
        else {
            delete node[n];
        }
    }
    upload('/config.json', JSON.stringify(c));
}
var TimerWidgetClass = (function (_super) {
    __extends(TimerWidgetClass, _super);
    function TimerWidgetClass() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.wt = 0;
        _this.pt = 0;
        _this.ct = 0;
        _this.time = 0;
        return _this;
    }
    TimerWidgetClass.prototype._timeToSec = function (v) {
        var ret = 0;
        v = v.toLowerCase();
        if (v.endsWith('h')) {
            ret = parseInt(v, 10) * 60 * 60;
        }
        else if (v.endsWith('m')) {
            ret = parseInt(v, 10) * 60;
        }
        else if (v.endsWith('s')) {
            ret = parseInt(v, 10);
        }
        else {
            ret = Number(v);
        }
        return ret;
    };
    TimerWidgetClass.prototype.newData = function (path, key, value) {
        _super.prototype.newData.call(this, path, key, value);
        if (key == 'waittime') {
            this.wt = this._timeToSec(value);
        }
        else if (key == 'pulsetime') {
            this.pt = this._timeToSec(value);
        }
        else if (key == 'cycletime') {
            this.ct = this._timeToSec(value);
        }
        else if (key == 'time') {
            this.time = this._timeToSec(value);
        }
        if (this.ct < this.wt + this.pt)
            this.ct = this.wt + this.pt;
        if (this.el && this.ct > 0) {
            var el = this.el.querySelector('.u-bar');
            var f = el.clientWidth / this.ct;
            var pto = el.querySelector('.pulse');
            pto.style.left = Math.floor(this.wt * f) + 'px';
            pto.style.width = Math.floor(this.pt * f) + 'px';
            var cto = el.querySelector('.current');
            cto.style.width = Math.floor(this.time * f) + 'px';
        }
    };
    TimerWidgetClass.prototype.onclick = function (evt) {
        var tar = evt.target;
        if (this.el && tar.classList.contains('save')) {
            var d_1 = {};
            this.el.querySelectorAll('[u-value]').forEach(function (elem) {
                var n = elem.getAttribute('u-value');
                if (n)
                    d_1[n] = elem.value;
            });
            changeConfig(this.microid, d_1);
        }
        _super.prototype.onclick.call(this, evt);
    };
    TimerWidgetClass = __decorate([
        MicroControl('timer')
    ], TimerWidgetClass);
    return TimerWidgetClass;
}(GenericWidgetClass));
var MicroHub = (function () {
    function MicroHub() {
        this._registrations = {};
        this._registrationsId = 0;
        this._store = {};
    }
    MicroHub.prototype._findStoreObject = function (path) {
        var p = this._store;
        if (path[0] === '/') {
            path = path.substr(1);
        }
        var steps = path.split('/');
        while (steps.length > 0 && p[steps[0]]) {
            p = p[steps[0]];
            steps.shift();
        }
        while (steps.length > 0 && steps[0]) {
            p = p[steps[0]] = {};
            steps.shift();
        }
        return p;
    };
    MicroHub.prototype.pPath = function (path) {
        if (path[0] === '/') {
            path = path.substr(1);
        }
        var steps = path.split('/');
        var res = steps.slice(0, steps.length - 1).join('/');
        return res;
    };
    MicroHub.prototype.pKey = function (path) {
        var steps = path.split('/');
        var res = steps[steps.length - 1];
        return res;
    };
    MicroHub.prototype.read = function (path) {
        var o = this._findStoreObject(this.pPath(path));
        return o[this.pKey(path)];
    };
    MicroHub.prototype.write = function (path, value) {
        var o = this._findStoreObject(this.pPath(path));
        o[this.pKey(path)] = value;
    };
    MicroHub.prototype.subscribe = function (matchPath, fCallback, replay) {
        if (replay === void 0) { replay = false; }
        var h = this._registrationsId;
        var rn = matchPath.toLocaleLowerCase();
        var re = '^' +
            rn
                .replace(/(\[|\]|\/|\?)/g, '\\$1')
                .replace(/\*\*/g, '\\S{0,}')
                .replace(/\*/g, '[^/?]*') +
            '$';
        var newEntry = {
            id: h,
            match: RegExp(re),
            callback: fCallback
        };
        this._registrations[h] = newEntry;
        this._registrationsId++;
        if (replay) {
            jsonParse(this._store, function (path, key, value) {
                var fullPath = path + (key ? '?' + key : '');
                if (fullPath) {
                    fullPath = fullPath.toLocaleLowerCase();
                    if (fullPath.match(newEntry.match))
                        newEntry.callback(path, key ? key.toLowerCase() : null, value);
                }
            }.bind(this));
        }
        return h;
    };
    MicroHub.prototype.unsubscribe = function (h) {
        delete this._registrations[h];
    };
    MicroHub.prototype.replay = function (h) {
        var e = this._registrations[h];
        if (e) {
            jsonParse(this._store, function (path, key, value) {
                var fullPath = path + (key ? '?' + key : '');
                if (fullPath) {
                    fullPath = fullPath.toLocaleLowerCase();
                    if (fullPath.match(e.match))
                        e.callback(path, key ? key.toLowerCase() : null, value);
                }
            }.bind(this));
        }
    };
    MicroHub.prototype.publishObj = function (obj) {
        jsonParse(obj, function (path, key, value) {
            this.publishValue(path, key ? key.toLowerCase() : '', value ? value : '');
        }.bind(this));
    };
    MicroHub.prototype.publishValue = function (path, key, value) {
        var fullPath = path + (key ? '?' + key : '');
        if (fullPath) {
            if (key) {
                var p = this._findStoreObject(path);
                p[key] = value;
            }
            fullPath = fullPath.toLocaleLowerCase();
            Object.values(this._registrations).forEach(function (r) {
                if (fullPath.match(r.match))
                    r.callback(path, key, value);
            });
        }
    };
    MicroHub.prototype.onunload = function (_evt) {
        for (var n in this._registrations) {
            delete this._registrations[n];
        }
    };
    return MicroHub;
}());
var hub = new MicroHub();
window.addEventListener('unload', hub.onunload.bind(hub), false);
function toBool(s) {
    if (!s)
        return false;
    switch (s.toLowerCase().trim()) {
        case 'true':
        case 'yes':
            return true;
        case 'false':
        case 'no':
        case '0':
        case null:
            return false;
        default:
            return Boolean(s);
    }
}
function setTextContent(el, txt) {
    if (el.textContent !== txt)
        el.textContent = txt;
}
function setAttr(el, name, value) {
    if (el.getAttribute(name) !== value)
        el.setAttribute(name, value);
}
function getHashParams(defaults) {
    var params = __assign({}, defaults);
    window.location.hash
        .substr(1)
        .split('&')
        .forEach(function (p) {
        var pa = p.split('=');
        params[pa[0]] = pa[1];
    });
    return params;
}
//# sourceMappingURL=micro.js.map