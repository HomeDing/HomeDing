// Micro charts implementation utilities.
//
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD 3-Clause license.
//
// * 08.08.2020 rework. 

/**
 * Create a SVG element
 * @param {SVGAElement} parentNode container node for the new element
 * @param {string} tagName tagName of the new element
 * @param {Object | undefined} attr attributes of the new element passed as Object 
 * @param {string | undefined} txt inner text content.   
 */
function createSVGNode(parentNode, tagName, attr, txt) {
  var n = document.createElementNS("http://www.w3.org/2000/svg", tagName);
  if (attr) {
    Object.getOwnPropertyNames(attr).forEach(function(p) {
      n.setAttribute(p, attr[p]);
    });
  }
  if (txt) { n.textContent = txt; }
  parentNode.appendChild(n);
  return (n);
} // createSVGNode()


function _removeChilds(p) {
  Array.from(p.childNodes).forEach(function (c) {
    c.remove();
  });
} // _removeChilds()

/**
 * Calculate the event position using document units from mouse position.
 * @param {*} evt 
 */
function eventPoint(evt) {
  var svg = document.documentElement;
  var pt = svg.createSVGPoint();
  pt.x = evt.clientX;
  pt.y = evt.clientY;
  return pt.matrixTransform(svg.getScreenCTM().inverse());
} // eventPoint


/**
 * Calculate a point on the circle
 * @param {number} alpha degree of angle
 * @param {number} r radius of circlt
 * @returns object with x and y coordinates.
 */
function _cPoint(alpha, r) {
  return ({
    x: (Math.sin(alpha) * r),
    y: (-Math.cos(alpha) * r)
  });
} // _cPoint()


/**
 * Utility function to set options in the api by using an Object
 * @param {any} options Object with options as properties. 
 */
function _setOptions(options) {
  Object.assign(document.api.options, options);
} // _setOptions

