// lineChart.js
/// <reference path="microsvg.js" />

var panelObj = document.getElementById('panel');
var backObj = document.getElementById('background');
var indObj = document.getElementById('ind');

var redrawTimer = null;

var REGION_WIDTH = 128;
var REGION_HEIGHT = 36;

function maxReducer(acc, val) {
  return Math.max(acc, val);
}

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
  box: Box;
  redraw: boolean; // needs a redraw
  svgObj: SVGElement;
  fDraw: function(g){}
}
 */

// list of the graphical elements in the chart.
// real data based charts must come before hLines and axis.
var graphs = [];
var graph_cnt = 0;

var minBox = { minX: Infinity, maxX: -Infinity, minY: 0, maxY: 1 };
var dataBox = minBox;
var displayBox = minBox;

// Date and Time formatting
function fmtDate(t) {
  var d = new Date(t * 1000);
  return d.toLocaleDateString();
} // fmtDate

// Date and Time formatting
function fmtTime(t) {
  var d = new Date(t * 1000);
  return d.toLocaleTimeString();
} // fmtTime


// look in all graphs.data for maximum values

/** combine to boxes to a new box covering both. */
function outerBox(box1, box2) {
  if (!box2) {
    return box1;
  }
  return {
    minX: Math.min(box1.minX, box2.minX),
    maxX: Math.max(box1.maxX, box2.maxX),
    minY: Math.min(box1.minY, box2.minY),
    maxY: Math.max(box1.maxY, box2.maxY)
  };
} // outerBox()

/** combine to boxes to a new box covering both. */
function calcOuterBox(box, graph) {
  return outerBox(box, graph.box);
} // outerBox()

/** calculate the box of a graph  */
function calcGraphBox(graph) {
  if (!graph.data) {
    graph.box = Object.assign({}, minBox);
  } else {
    var xValues = graph.data.map(function(p) {
      return p.x;
    });
    var yValues = graph.data.map(function(p) {
      return p.y;
    });

    graph.box = {
      minX: Math.min.apply(null, xValues),
      maxX: Math.max.apply(null, xValues),
      minY: Math.min(0, Math.min.apply(null, yValues)),
      maxY: Math.max(1, Math.max.apply(null, yValues))
    };
  }
} // calcGraphBox

// Line Charts

function addLineChart(values) {
  var gID = ++graph_cnt;
  var g = { id: gID, data: values, box: null, redraw: true, fDraw: drawLineChart };
  calcGraphBox(g);
  graphs.unshift(g); // put data driven graphs in the fist place
  setRedraw();
  return gID;
} // addLine()

function drawLineChart(g) {
  if (g.svgObj) {
    g.svgObj.remove();
  }

  var values = g.data;
  if (values) {
    var scaleX = 128 / (displayBox.maxX - displayBox.minX);
    var scaleY = REGION_HEIGHT / (displayBox.maxY - displayBox.minY);

    var points = values.map(function(p) {
      return [(p.x - displayBox.minX) * scaleX, (p.y - displayBox.minY) * scaleY].join(',');
    });

    g.svgObj = createSVGNode(panelObj, 'polyline', {
      // id: 'line-' + lineID,
      class: 'linechart',
      points: points.join(' ')
    });
  }
}

// add a new data to the data of a line
function addLineChartData(gID, values) {
  var g = graphs.find(function(e) {
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
  var g = graphs.find(function(e) {
    return e.id === gID;
  });
  if (g) {
    g.data = values;
    calcGraphBox(g);
    g.redraw = true;
    setRedraw();
  }
} // updateLineChartData

function setMinScale(bBox) {
  Object.assign(minBox, bBox);
  setRedraw();
}

// ===== HLine horizontal lines =====

function addHLine(y) {
  var gID = ++graph_cnt;
  var g = { id: gID, data: y, redraw: true, fDraw: drawHLine };
  g.box = Object.assign({}, minBox);
  graphs.push(g);
  setRedraw();
  return gID;
}

function drawHLine(g) {
  if (g.svgObj) {
    g.svgObj.remove();
  }

  var scaleY = REGION_HEIGHT / (displayBox.maxY - displayBox.minY);
  var y = (g.data - displayBox.minY) * scaleY;

  g.svgObj = createSVGNode(panelObj, 'line', {
    x1: 0,
    y1: y,
    x2: 128,
    y2: y,
    class: 'hline'
  });
}

// ===== Vertical Axis =====

function _calcVAxisBox(g) {
  var dataRange = dataBox.maxY - dataBox.minY;
  var dispRange = Math.pow(10, Math.floor(Math.log10(dataRange)));

  var step;
  if (dataRange <= dispRange * 2) {
    step = dispRange / 2;
  } else if (dataRange <= dispRange * 4) {
    step = dispRange;
  } else if (dataRange <= dispRange * 5) {
    step = dispRange;
  } else {
    step = 2 * dispRange;
  }
  g.box = displayBox;

  if (g.step !== step || dataBox.maxY > displayBox.maxY || dataBox.minY < displayBox.minY) {
    var high = Math.ceil(dataBox.maxY / step) * step;
    var low = Math.floor(dataBox.minY / step) * step;
    g.step = step;
    g.box = displayBox = Object.assign({}, dataBox, { minY: low, maxY: high });
  } // if
}

function addVAxis() {
  var gID = ++graph_cnt;
  var g = { data: [], redraw: true, fDraw: drawVAxis };
  graphs.push(g);
  _calcVAxisBox(g);
  setRedraw();
  return gID;
}

function drawVAxis(g) {
  // clear existing y-Axis
  var YAxisGroup = document.getElementById('v-labels');
  Array.from(YAxisGroup.childNodes).forEach(function(c) {
    c.remove();
  });

  _calcVAxisBox(g);
  var high = displayBox.maxY;
  var low = displayBox.minY;

  var scaleY = REGION_HEIGHT / (high - low);

  var n;
  for (n = low; n <= high; n += g.step) {
    var txtObj = createSVGNode(YAxisGroup, 'text', {
      x: 11,
      y: -1 * (n - low) * scaleY
    });
    txtObj.textContent = String(n);
  }
}

// ===== Horizontal Axis =====

function addHAxis() {
  var gID = ++graph_cnt;
  var g = { data: [], redraw: true, fDraw: drawHAxis };
  graphs.push(g);
  return gID;
}

function drawHAxis(g) {
  // clear existing x-Axis labels
  var XAxisGroup = document.getElementById('h-labels');
  Array.from(XAxisGroup.childNodes).forEach(function(c) {
    c.remove();
  });

  var high = displayBox.maxX;
  var low = displayBox.minX;
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
}

// ===== Redraw =====

/** redraw lines when required. */
function redraw() {
  // find out if dataBox has changed
  var newBox = graphs.reduce(calcOuterBox, minBox);
  var doAll =
    newBox.minY != dataBox.minY || newBox.maxY != dataBox.maxY || newBox.minX != dataBox.minX || newBox.maxX != dataBox.maxX;
  dataBox = newBox;
  displayBox = outerBox(displayBox, dataBox);

  if (doAll) {
    graphs.forEach(function(g) {
      g.fDraw(g);
      g.redraw = false;
    });
  } else {
    graphs.forEach(function(g) {
      if (g.redraw) {
        g.fDraw(g);
        g.redraw = false;
      }
    });
  } // if
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
    var yPos = ((data.y - box.minY) * REGION_HEIGHT) / (box.maxY - box.minY);

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

panelObj.addEventListener('mousemove', function(evt) {
  var p = eventPoint(evt);
  var g = graphs[0];
  if (g) {
    var data = g.data;
    if (data && data.length) {
      var box = g.box;
      var xData = box.minX + ((p.x - 12) * (box.maxX - box.minX)) / REGION_WIDTH;

      // find nearest data by x
      var n = data.findIndex(function(e) {
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
  addHAxis: addHAxis,
  setMinScale: setMinScale
};

// End.
