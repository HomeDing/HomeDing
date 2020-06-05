// lineChart.js
// * 25.04.2020 improved vertical scaling 
/// <reference path="microsvg.js" />

var panelObj = document.getElementById('panel');
var backObj = document.getElementById('background');
var indObj = document.getElementById('ind');

var redrawTimer = null;

var REGION_WIDTH = 128;
var REGION_HEIGHT = 36;

// local timezone offset for correct time conversion
var tzOffset = new Date().getTimezoneOffset() * 60;


// point = [x,y], x may be date as seconds
/* 
interface Point {
  x: integer;
  y: integer;
}

interface Box {
  minX: number, maxX: number,
  minY: number, maxY: number;
}

interface GraphsData {
  id: number,
  type: "line", "vAxis", "hLine"
  data: Point[];
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
var graph_cnt = 0;

var minBox = { minX: Infinity, maxX: -Infinity, minY: 0, maxY: 1 };
var displayBox = minBox;

// Date and Time formatting
function fmtDate(t) {
  var d = new Date((Number(t) + tzOffset) * 1000);
  return d.toLocaleDateString();
} // fmtDate

// Date and Time formatting
function fmtTime(t) {
  var d = new Date((Number(t) + tzOffset) * 1000);
  return d.toLocaleTimeString();
} // fmtTime

/** combine to boxes to a new box covering both. */
function outerBox(box1, box2) {
  var b = box1;

  if (!b) {
    b = box2;
  } else if (box2) {
    b = {
      minX: Math.min(box1.minX, box2.minX),
      maxX: Math.max(box1.maxX, box2.maxX),
      minY: Math.min(box1.minY, box2.minY),
      maxY: Math.max(box1.maxY, box2.maxY)
    }
  }
  return (b);
} // outerBox()



/** reduce-function: combine to boxes to a new box covering both. */
function calcOuterBox(box, graph) {
  if (graph.fBox)
    box = graph.fBox(box, graph); // use graph function
  else
    box = outerBox(box, graph.box);
  return (box);
} // calcOuterBox()



/** calculate the box of a graph from the data */
function calcGraphBox(graph) {
  if (!graph.data) {
    graph.box = null;

  } else {
    var xValues = graph.data.map(function (p) {
      return p.x;
    });
    var yValues = graph.data.map(function (p) {
      return p.y;
    });

    graph.box = {
      minX: Math.min.apply(null, xValues),
      maxX: Math.max.apply(null, xValues),
      minY: Math.min.apply(null, yValues),
      maxY: Math.max.apply(null, yValues)
    };
  }
} // calcGraphBox


// ====== Line Charts

LineChartClass = function (values) {
  return {
    id: ++graph_cnt,
    type: "lineChart",
    data: values,
    redraw: false,
    svgObj: null,
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
        var scaleX = 128 / (box.maxX - box.minX);
        var scaleY = REGION_HEIGHT / (box.maxY - box.minY);

        var points = values.map(function (p) {
          return [(p.x - box.minX) * scaleX, (p.y - box.minY) * scaleY].join(',');
        });

        this.svgObj = createSVGNode(panelObj, 'polyline', {
          // id: 'line-' + lineID,
          class: 'linechart',
          points: points.join(' ')
        });
      }
    } // fDraw()
  }
};


function addLineChart(values) {
  var obj = new LineChartClass(values);

  calcGraphBox(obj);
  graphs.unshift(obj); // put data driven graphs in the fist place
  if (values)
    setRedraw();
  return obj.id;
} // addLineChart()


// add a new data to the data of a line
function addLineChartData(gID, values) {
  var g = graphs.find(function (e) {
    return e.id === gID;
  });
  if (g) {
    g.data.push(values);
    calcGraphBox(g);
    g.redraw = true;
    setRedraw();
  }
} // addLineChartData()


// Update the values for a line and defer redrawing
function updateLineChartData(gID, values) {
  var g = graphs.find(function (e) {
    return e.id === gID;
  });
  if (g) {
    g.data = values;
    calcGraphBox(g);
    g.redraw = true;
    setRedraw();
  }
} // updateLineChartData


// ===== HLine horizontal lines =====

HLineClass = function (y) {
  return {
    id: ++graph_cnt,
    type: "hLine",
    data: y,
    redraw: false,
    svgObj: null,
    fBox: function (box, g) {
      if (box) {
        box.minY = Math.min(y, box.minY);
        box.maxY = Math.max(y, box.maxY);
      }
      return (box);
    },
    fDraw: function (box) {
      // clear existing line
      if (this.svgObj) this.svgObj.remove();

      var scaleY = REGION_HEIGHT / (box.maxY - box.minY);
      var y = (this.data - box.minY) * scaleY;

      this.svgObj = createSVGNode(panelObj, 'line', {
        x1: 0,
        y1: y,
        x2: 128,
        y2: y,
        class: 'hline'
      });
    } // fDraw()
  }
};


function addHLine(y) {
  var obj = new HLineClass(y);
  graphs.push(obj);
  return obj.id;
}

// ===== Vertical Axis =====

VAxisClass = function () {
  return {
    id: ++graph_cnt,
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
      Array.from(YAxisGroup.childNodes).forEach(function (c) {
        c.remove();
      });

      var high = box.maxY;
      var low = box.minY;
      var step = this.step;
      var prec = (step < 1) ? String(step).length - 2 : 0;

      var scaleY = REGION_HEIGHT / (high - low);

      for (var n = low; n <= high; n += step) {
        var txtObj = createSVGNode(YAxisGroup, 'text', {
          x: 11,
          y: -1 * (n - low) * scaleY
        });
        txtObj.textContent = String(n.toFixed(prec));
      }
    } // fDraw()
  }
};


function addVAxis() {
  var g = new VAxisClass();
  graphs.push(g);
  return g.id;
} // addVAxis()


// ===== Horizontal Axis =====

HAxisClass = function () {
  return {
    id: ++graph_cnt,
    type: "hAxis",
    data: null,
    redraw: false, // needs a redraw
    svgObj: null,
    fBox: null,
    fDraw: function (box) {
      // clear existing x-Axis labels
      var XAxisGroup = document.getElementById('h-labels');
      Array.from(XAxisGroup.childNodes).forEach(function (c) {
        c.remove();
      });

      var high = box.maxX;
      var low = box.minX;

      if (isFinite(low) && isFinite(high)) {
        var scaleX = REGION_WIDTH / (high - low);

        var n = low + Math.floor((high - low) / 2);
        // for (n = low; n <= high; n += g.step) {
        var txtObj = createSVGNode(XAxisGroup, 'text', {
          x: REGION_WIDTH / 2,
          y: 0
        });
        txtObj.textContent = fmtTime(n);
      } // if
    } // fDraw()
  }
};


function addHAxis() {
  var g = new HAxisClass();
  graphs.push(g);
  return g.id;
} // addHAxis()


// ===== Redraw =====

/** redraw lines when required. */
function redraw() {
  // find out if dataBox has changed
  var bx = graphs.reduce(calcOuterBox, null);

  var doAll =
    bx.minY != displayBox.minY || bx.maxY != displayBox.maxY || bx.minX != displayBox.minX || bx.maxX != displayBox.maxX;
  displayBox = bx;

  if (doAll)
    graphs.forEach(function (g) { g.redraw = true; });

  graphs.forEach(function (g) {
    if (g.redraw) {
      g.fDraw(displayBox);
      g.redraw = false;
    } // if
  });

  redrawTimer = null;
} // redraw()

function setRedraw() {
  if (!redrawTimer) redrawTimer = window.setTimeout(redraw, 20);
}

// === to:microsvg.js

/// calculate a event position using the document units.
function eventPoint(evt) {
  var svg = document.documentElement;
  var pt = svg.createSVGPoint();
  pt.x = evt.clientX;
  pt.y = evt.clientY;
  return pt.matrixTransform(svg.getScreenCTM().inverse());
} // eventPoint

function setIndicator(box, data) {
  if (box && data) {
    indObj.style.display = '';

    var indLine = indObj.querySelector('line');
    var indcircle = indObj.querySelector('circle');

    var xPos = ((data.x - box.minX) * REGION_WIDTH) / (box.maxX - box.minX);
    var yPos = (data.y - box.minY) * (REGION_HEIGHT / (box.maxY - box.minY));

    indLine.x1.baseVal.value = indLine.x2.baseVal.value = xPos;
    indcircle.cx.baseVal.value = xPos;
    indcircle.cy.baseVal.value = yPos;

    var infoObj = indObj.querySelector('.info');
    // calc infobox position
    xPos += xPos < REGION_WIDTH / 2 ? 2 : -20;
    yPos += yPos < REGION_HEIGHT / 2 ? 2 : -12;
    infoObj.setAttribute('transform', 'translate(' + xPos + ',' + yPos + ')');
    var txtObjs = infoObj.querySelectorAll('text');
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
      var xData = box.minX + ((p.x - 12) * (box.maxX - box.minX)) / REGION_WIDTH;

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

document['api'] = {
  addLineChart: addLineChart,
  addLineChartData: addLineChartData,
  updateLineChartData: updateLineChartData,

  addHLine: addHLine,
  addVAxis: addVAxis,
  addHAxis: addHAxis
};

// End.
