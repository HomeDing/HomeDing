// panel.js: panel javascript functions

// calc the current event/mouse/touch position
function getSVGPos(panel, evt) {
  var newScale = panel.currentScale;
  var translation = panel.currentTranslate;
  var panelPos = panel.getBoundingClientRect();
  var ret = {
    x: Math.round((evt.clientX - translation.x) / newScale - panelPos.left),
    y: Math.round((evt.clientY - translation.y) / newScale - panelPos.top)
  };
  return (ret);
} // getSVGPos()

// mode a svg group to a specific position using transform:translate
function moveSVGGroup(g, x, y) {
  g.setAttribute('transform', 'translate(' + x + ',' + y + ')');
} // moveSVGGroup


// find the position of the pin of a node.
function _pinPos(node, pin) {
  var panelPos = panelObj.getBoundingClientRect();

  var obj = node.obj.querySelector('[pin="' + pin + '"]');
  if (!obj)
    obj = node.obj;

  var b2 = obj.getBoundingClientRect();

  var pos = {
    x: Math.round(b2.left + b2.width / 2 - panelPos.left),
    y: Math.round(b2.top + b2.height / 2 - panelPos.top)
  };
  return (pos);
} // _pinPos()

