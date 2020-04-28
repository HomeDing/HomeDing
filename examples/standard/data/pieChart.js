var panelObj = document.getElementById("panel");
var RAD_OUT = 20;

/** create an outer point on the circle */
function piePoint(alpha, r) {
  var s = " " + (Math.sin(alpha) * r) + "," + (Math.cos(alpha) * -r);
  return (s);
}

function addPiePart(start, part, color) {
  var alpha = 2 * Math.PI * start;
  var beta = 2 * Math.PI * (start + part);

  // create gauge part path
  var p = "";
  p += "M" + piePoint(alpha, RAD_OUT);
  p += "A" + RAD_OUT + "," + RAD_OUT;
  if (part < 0.5) {
    p += " 0 0 1 ";
  } else {
    p += " 0 1 1 ";
  }
  p += piePoint(beta, RAD_OUT);
  p += "L" + " 0,0";
  p += "Z";
  createSVGNode(panelObj, "path", {
    class: "segment",
    style: "fill:" + color,
    d: p
  });

} // addPiePart()

/** create a set of ranges for the gauge panel in the background. The sum of all parts should add-up to 1.
 * @rangeList: List of gauge segments. 
 */
function setRange(rangeList) {
  if (rangeList) {
    // calculate sum:
    var sum = 0;
    for (var n = 0; n < rangeList.length; n++) {
      sum += rangeList[n].part;
    } // for

    var x = 0;
    for (var n = 0; n < rangeList.length; n++) {
      var p = rangeList[n].part / sum;
      addPiePart(x, p, rangeList[n].color);
      x += p;
    } // for
  } // if
} // setRange()


document.api = {
  setRange: setRange
};

// End.