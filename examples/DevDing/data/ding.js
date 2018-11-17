// ding.js: Behaviors for Elements

// === Generic Behavior ===

var GenericElementBehavior = {
  microID: "",

  init: function() {
    hub.subscribe(this.microid + "?*", this.newData.bind(this));
    this.newData(this.microid, "id", this.microid);
  }, // init

  newData: function(path, key, value) {
    forAllNodeList(this.querySelectorAll("img[property='" + key + "']"), function(e) {
      if (value)
        e.classList.add('active');
      else
        e.classList.remove('active')
        // e.textContent = value;
      e.title = value;
    });

    forAllNodeList(this.querySelectorAll("span[property='" + key + "']"), function(e) {
      e.textContent = value;
      e.title = value;
    });

    forAllNodeList(this.querySelectorAll("input[property='" + key + "']"), function(e) {
      e.value = value;
    });
  }, // newData()

  onchange: function(e) {
    var src = e.srcElement;
    dispatch(this.microid, src.getAttribute('property'), e.srcElement.value);
    // debugger;
  },

  onclick: function(e) {
    var src = e.srcElement;
    dispatch(this.microid, src.getAttribute('property'), e.srcElement.value);
  }

}; // GenericElementBehavior

jcl.registerBehaviour("generic", GenericElementBehavior);

// === Timer Behavior ===

var TimerElementBehavior = {
  inheritFrom: GenericElementBehavior,
  microID: "",
  wt: 0,
  pt: 0,
  ct: 0,
  time: 0,

  _timeToSec: function(v) {
    v = v.toLowerCase();
    if (v.endsWith('h')) {
      v = parseInt(v, 10) * 60 * 60;
    } else if (v.endsWith('m')) {
      v = parseInt(v, 10) * 60;
    } else if (v.endsWith('s')) {
      v = parseInt(v, 10);
    } // if
    return (v);
  },

  init: function() {
    hub.subscribe(this.microid + "?*", this.newData.bind(this));
  }, // init

  newData: function(path, key, value) {
      // alert(key);
      if (key == "waittime") {
        this.wt = this._timeToSec(value);
      } else if (key == "pulsetime") {
        this.pt = this._timeToSec(value);
      } else if (key == "cycletime") {
        this.ct = this._timeToSec(value);
      } else if (key == "time") {
        this.time = this._timeToSec(value);
      }

      if (this.ct < 0 + this.wt + this.pt)
        this.ct = 0 + this.wt + this.pt

      // update bars
      if (this.ct > 0) {
        var el = this.querySelectorAll(".u-bar")[0];
        var f = el.clientWidth / this.ct;
        var pto = el.querySelectorAll(".pulse")[0];
        pto.style.left = Math.floor(this.wt * f) + "px";
        pto.style.width = Math.floor(this.pt * f) + "px";
        var cto = el.querySelectorAll(".current")[0];
        cto.style.width = Math.floor(this.time * f) + "px";
      }

      forAllNodeList(this.querySelectorAll("span[property='" + key + "']"), function(e) {
        e.textContent = value;
        e.title = value;
      });
    } // newData()
}; // TimerElementBehavior

jcl.registerBehaviour("timer", TimerElementBehavior);