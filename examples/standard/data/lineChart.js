// Line-Chart micro charts implementation.
//
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD 3-Clause license.
//
// * 25.04.2020 improved vertical scaling 
// * 07.08.2020 show values optional. 

// SVG file must include microsvg.js
/// <reference path="microsvg.js" />

var panelObj = document.getElementById('panel');
var backObj = document.getElementById('background');
var indObj = document.getElementById('ind');

var redrawTimer = null;

// Region of data drawing
var REGION_WIDTH = 128;
var REGION_HEIGHT = 36;

/**
 * @typedef {Object} Point
 * @property {number} x - x pos of the point, may also be a date.valueOf
 * @property {number} y - y pos of the point
 */

/**
 * @typedef {Object} Box
 * @property {number} left
 * @property {number} right
 * @property {number} minY
 * @property {number} maxY
 */
/// left, top, right, bottom, x, y, width, height

/**
interface GraphsData {
  type: "line", "vAxis", "hLine"
  format: "..."
  data: Point[], number[], number;
  box: Box; // only if relevant data 
  redraw: boolean; // needs a redraw

  svgObj: SVGElement;
  fBox: function(box) { return(box) };
  fDraw: function(g){}
}
 */

// ===== Utilities

// calculate friendly steps and range
function _calcSteps(l, h) {
  var v = h - l;
  var range = Math.pow(10, Math.floor(Math.log10(v)));
  var ret = {};

  var step;
  if (v <= range * 2) {
    step = range / 2;
    // } else if (v <= range * 4) {
    //   step = range;
  } else if (v <= range * 5) {
    step = range;
  } else {
    step = 2 * range;
  }
  ret.high = Math.ceil(h / step) * step;
  ret.low = Math.floor(l / step) * step;
  ret.step = step;
  return (ret);
} // _calcSteps()


// list of the graphical elements in the chart.
// real data based charts must come before hLines and axis.
var graphs = [];
// var options = {};

var minBox = { left: Infinity, right: -Infinity, minY: 0, maxY: 1 };
var displayBox = minBox;

// Date formatting
function fmtDate(t) {
  var d = new Date(Number(t) * 1000);
  return d.toLocaleDateString();
} // fmtDate

// Time formatting
function fmtTime(t) {
  var d = new Date(Number(t) * 1000);
  return d.toLocaleTimeString();
} // fmtTime

/** combine to boxes to a new box covering both. */
function outerBox(box1, box2) {
  var b = box1;

  if (!b) {
    b = box2;
  } else if (box2) {
    b = {
      left: Math.min(box1.left, box2.left),
      right: Math.max(box1.right, box2.right),
      minY: Math.min(box1.minY, box2.minY),
      maxY: Math.max(box1.maxY, box2.maxY)
    }
  }
  return (b);
} // outerBox()


/**
 * see if box1 and box2 are equal.
 * @param {Box} box1 
 * @param {Box} box2 
 * @returns {boolean} true when both boxes are equal.
 */
function boxEqual(box1, box2) {
  var result =
    (box1.minY === box2.minY)
    && (box1.maxY === box2.maxY)
    && (box1.left === box2.left)
    && (box1.right === box2.right);
  return (result);
} // boxEqual()


/** reduce-function: combine to boxes to a new box covering both. */
function calcOuterBox(box, graph) {
  if (graph.fBox)
    box = graph.fBox(box, graph); // use graph function
  else
    box = outerBox(box, graph.box);
  return (box);
} // calcOuterBox()


// ====== Line Charts

LineChartClass = function (options) {
  options = Object.assign({ linetype: "line", color: "black" }, options);
  return {
    type: 'line',
    data: null,
    linetype: options.linetype,
    color: options.color,
    redraw: false,
    svgObj: null,

    /**
     * Calculate outer box of a array of points.
     * @param {Point[]} points 
     */
    _calcBox: function (points) {
      /** @type Box */
      var box = null;

      if (points) {
        var xValues = points.map(function (p) { return p.x; });
        var yValues = points.map(function (p) { return p.y; });
        box = {
          left: Math.min(...xValues),
          right: Math.max(...xValues),
          minY: Math.min(...yValues),
          maxY: Math.max(...yValues)
        };
      }
      return (box);
    }, // _calcBox()

    /**
     * use the new values for drawing.
     * The real drawing is deferred and done in fDraw
     * @param {*} values values as an array of points.
     */
    draw: function (values) {
      this.data = values;
      this.box = this._calcBox(values);
      this.redraw = true;
      _startRedraw();
    }, // draw

    clear: function () {
      if (this.svgObj) this.svgObj.remove();
    },

    fBox: null,

    XfBox: function (box, g) {
      if (box) {
        box.minY = Math.min(y, box.minY);
        box.maxY = Math.max(y, box.maxY);
      }
      return (box);
    },
    fDraw: function (box) {
      // remove existing line
      if (this.svgObj) this.svgObj.remove();

      var values = this.data;
      if (values) {
        var scaleX = 128 / (box.right - box.left);
        var scaleY = REGION_HEIGHT / (box.maxY - box.minY);
        var points;

        if (this.linetype == 'steps') {
          points = values.map(function (p, n) {
            return 'H' + (p.x - box.left) * scaleX + ' V' + (p.y - box.minY) * scaleY;
          });
          // starting point
          points[0] = "M" + (values[0].x - box.left) * scaleX + ',' + (values[0].y - box.minY) * scaleY;

        } else if (this.linetype == 'line') {
          points = values.map(function (p, n) {
            return (n > 0 ? 'L' : 'M') + (p.x - box.left) * scaleX + ',' + (p.y - box.minY) * scaleY;
          });
        }

        this.svgObj = createSVGNode(panelObj, 'path', {
          class: 'linechart',
          style: 'stroke:' + this.color,
          d: points.join(' ')
        });
      }
    } // fDraw()
  }
};


// ===== HLine horizontal lines =====

HLineClass = function (options) {
  options = Object.assign({ data: 0, color: 'black' }, options);
  return {
    type: "hLine",
    data: options.data,
    color: options.color,
    redraw: false,
    svgObj: null,

    fBox: function (box, g) {
      if (box) {
        box.minY = Math.min(this.data, box.minY);
        box.maxY = Math.max(this.data, box.maxY);
      }
      return (box);
    },
    fDraw: function (box) {
      // clear existing line
      if (this.svgObj) this.svgObj.remove();

      var scaleY = REGION_HEIGHT / (box.maxY - box.minY);
      var y = (this.data - box.minY) * scaleY;

      this.svgObj = createSVGNode(panelObj, 'line', {
        class: 'hline',
        style: 'stroke:' + this.color,
        x1: 0, y1: y,
        x2: 128, y2: y
      });
    } // fDraw()
  }
};


// ===== Vertical Axis =====

VAxisClass = function () {
  return {
    type: "vAxis",
    data: null,
    redraw: false, // needs a redraw
    svgObj: null,
    fBox: function (box, g) {
      if (box) {
        var s = _calcSteps(box.minY, box.maxY);
        Object.assign(box, { minY: s.low, maxY: s.high });
        Object.assign(g, s); // + high, low, step
      }
      return (box);
    },
    fDraw: function (box) {
      // clear existing y-Axis
      var YAxisGroup = document.getElementById('v-labels');
      _removeChilds(YAxisGroup);

      var high = box.maxY;
      var low = box.minY;
      var step = this.step;
      var prec = (step < 1) ? String(step).length - 2 : 0;

      var scaleY = REGION_HEIGHT / (high - low);

      for (var n = low; n <= high; n += step) {
        createSVGNode(YAxisGroup, 'text', {
          x: 11, y: -1 * (n - low) * scaleY
        }, String(n.toFixed(prec)));
      }
    } // fDraw()
  }
};


function addVAxis() {
  var g = new VAxisClass();
  var n = graphs.push(g); // append at end
  return (n - 1);
} // addVAxis()


// ===== Horizontal Axis =====

HAxisClass = function () {
  return {
    type: "hAxis",
    data: null,
    redraw: false, // needs a redraw
    svgObj: null,
    fBox: null,
    fDraw: function (box) {
      // clear existing x-Axis labels
      var XAxisGroup = document.getElementById('h-labels');
      _removeChilds(XAxisGroup);

      var high = box.right;
      var low = box.left;

      if (isFinite(low) && isFinite(high)) {
        var scaleX = REGION_WIDTH / (high - low);

        var n = low + Math.floor((high - low) / 2);
        // for (n = low; n <= high; n += g.step) {
        createSVGNode(XAxisGroup, 'text', {
          x: REGION_WIDTH / 2,
          y: 0
        }, fmtTime(n));
      } // if
    } // fDraw()
  }
};


function _addHAxis() {
  var g = new HAxisClass();
  var n = graphs.push(g);
  return (n - 1);
} // _addHAxis()

// ===== Redraw =====

/** redraw graphs when required. */
function redraw() {
  // find out if dataBox has changed
  var bx = graphs.reduce(calcOuterBox, null);

  // has the displaybox changed because of new data ? 
  var doAll = (!boxEqual(bx, displayBox));
  displayBox = bx;

  graphs.forEach(function (g) {
    if ((doAll) || (g.redraw)) {
      g.fDraw(displayBox);
      g.redraw = false;
    } // if
  });

  redrawTimer = null;
} // redraw()

function _startRedraw() {
  if (!redrawTimer) redrawTimer = window.setTimeout(redraw, 20);
} // _startRedraw()


// remove all data and data bound graphs. 
function _clear() {
  graphs.forEach(function (g) {
    if (g.clear)
      g.clear();
  });
  graphs = [];
} // _clear()


function setIndicator(box, data) {
  if (box && data) {
    indObj.style.display = '';

    var oLine = indObj.querySelector('line');
    var oCircle = indObj.querySelector('circle');
    var oInfo = indObj.querySelector('.info');

    var xPos = ((data.x - box.left) * REGION_WIDTH) / (box.right - box.left);
    var yPos = (data.y - box.minY) * (REGION_HEIGHT / (box.maxY - box.minY));

    oLine.x1.baseVal.value = oLine.x2.baseVal.value = xPos;
    oCircle.cx.baseVal.value = xPos;
    oCircle.cy.baseVal.value = yPos;

    // calc infobox position
    xPos += xPos < REGION_WIDTH / 2 ? 2 : -20;
    yPos += yPos < REGION_HEIGHT / 2 ? 2 : -12;
    oInfo.setAttribute('transform', 'translate(' + xPos + ',' + yPos + ')');

    // add textual information
    var txtObjs = oInfo.querySelectorAll('text');
    txtObjs[0].textContent = data.y;
    txtObjs[1].textContent = fmtDate(data.x);
    txtObjs[2].textContent = fmtTime(data.x);
  } // if
} // setIndicator()

function clearIndicator() {
  indObj.style.display = 'none';
}

panelObj.addEventListener('mouseout', clearIndicator);

panelObj.addEventListener('mousemove', function (evt) {
  var p = eventPoint(evt);
  var g = graphs[0];
  if (g) {
    var data = g.data;
    if (data && data.length) {
      var box = g.box;
      var xData = box.left + ((p.x - 12) * (box.right - box.left)) / REGION_WIDTH;

      // find nearest data by x
      var n = data.findIndex(function (e) {
        return e.x > xData;
      });
      // check for the n-1 value maybe nearer
      if (n > 0 && data[n].x - xData > xData - data[n - 1].x) {
        n = n - 1;
      }
      setIndicator(displayBox, data[n]);
    } // if
  }
});

/**
 * 
 * @param {string} type 
 * @param {Object} options 
 */
function _addChart(type, options) {
  var newClass;
  var n = -1;
  var isData = false;

  type = type.toLowerCase();
  options = Object.assign({}, options);

  if (type === 'line') {
    newClass = new LineChartClass(options);
    isData = true;

  } else if (type == 'hline') {
    newClass = new HLineClass(options);

  } else {
    debugger; // not implemented  

  }

  if (newClass) {
    if (isData) {
      n = Math.max(0, graphs.findIndex(function (g) { return g.type == 'line' }));
      graphs.splice(n, 0, newClass);
    } else {
      n = graphs.push(newClass) - 1;
    }
  } // if
  return (n);
} // _addChart


function _add(config) {
}

document['api'] = {
  clear: _clear,
  add: _add,
  addChart: _addChart,
  draw: function (gID, values) {
    var g = graphs[gID] || graphs[0];
    if ((g) && (g.type === 'line')) {
      g.draw(values);
    }
  }, // draw()

  addVAxis: addVAxis,
  addHAxis: _addHAxis,
  options: {

  }
};

// End.
