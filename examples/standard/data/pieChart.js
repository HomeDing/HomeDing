// Pie-Chart micro charts implementation.
//
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD 3-Clause license.
//
// * 07.08.2020 show values optional. 

// SVG file must include microsvg.js
/// <reference path="microsvg.js" />

var panelObj = document.getElementById("panel");
var valuesObj = document.getElementById("values");

/**
 * Radius of pie chart
 */
var RAD_OUT = 20;


/**
 * Calculate an outer point on the circle, usable for svg paths
 */
function _piePoint(alpha, r) {
  var p = _cPoint(alpha, r);
  return (p.x + "," + p.y);
} // _piePoint()


/**
 * Append another pie slice inside inside the panelObj.
 * @param start start of slice in percent
 * @param size size of slice in percent
 * @param color color of slice
 * @param value value of slice to be shown
 * @param title description of slice to be shown
 */
function _addPieSlice(start, size, color, value, title) {
  var alpha = 2 * Math.PI * start;
  var beta = 2 * Math.PI * (start + size);
  var options = document.api.options;

  // create pie slice path
  var p = "";
  p += "M" + _piePoint(alpha, RAD_OUT);
  p += "A" + RAD_OUT + "," + RAD_OUT;
  if (size < 0.5) {
    p += " 0 0 1 ";
  } else {
    p += " 0 1 1 ";
  }
  p += _piePoint(beta, RAD_OUT);
  p += "L0,0Z";
  var pNode = createSVGNode(panelObj, "path", {
    class: "segment",
    style: "fill:" + color,
    d: p
  });

  if ((options.showTitle) || (options.showValue) || (options.showPercentage)) {
    var tvals = [];

    if (options.showTitle) { tvals.push(String(title)); }
    if (options.showValue) { tvals.push(Number(value).toLocaleString()); }
    if (options.showPercentage) { tvals.push('(' + Math.round(size * 100) + '%)'); }

    // calc lightness of fill color
    var lum = window.getComputedStyle(pNode)
      .fill  // returns e.g. rgb(0, 0, 139)
      .match(/\d+/g)
      .reduce(function (s, e) { return (s + Number(e)) }, 0) / 3;


    // create text element on top of pie slice
    var tp = _cPoint((alpha + beta) / 2, RAD_OUT * 0.7);
    createSVGNode(valuesObj, "text", {
      class: "text",
      style: "fill:" + ((lum > 127) ? "black" : "white"),
      x: tp.x, y: tp.y
    }, tvals.join(' '));
  } // if
} // _addPieSlice()



/**
 * Create all slices from the given pie data.
 * @param data: Array of pie chart segment definitions. 
 */
function _draw(data) {
  _clear();
  var cl = document.api._colors;
  var cll = document.api._colors.length

  if (data) {
    // calculate sum of all parts:
    var sum = data.reduce(function (x, e) { return (x + e.value); }, 0);

    data.reduce(function (start, el, indx) {
      var p = el.value / sum;
      _addPieSlice(start, p, el.color || cl[indx % cll] || "gray", el.value, el.title);
      return (start + p);
    }, 0);
  } // if
} // _draw()


function _setColors(cols) {
  if (typeof cols == "string")
    document.api._colors = cols.split(',');
  else if (Array.isArray(cols))
    document.api._colors = [...cols];
}

/**
 * Clear the pie chart.
 * Remove all elements inside the panelObj.
 */
function _clear() {
  _removeChilds(panelObj);
  _removeChilds(valuesObj);
} // _clear()


// expose API functions.
document.api = {
  clear: _clear,
  setOptions: _setOptions,
  setColors: _setColors,
  draw: _draw,
  options: {
    showPercentage: false,
    showTitle: false,
    showValue: false
  },
  _colors: []
};

// End.