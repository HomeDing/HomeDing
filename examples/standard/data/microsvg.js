/// svg implementation helpers

function createSVGNode(parentNode, tagName, properties) {
  var n = document.createElementNS("http://www.w3.org/2000/svg", tagName);
  for (var p in properties) {
    n.setAttribute(p, properties[p]);
  }
  parentNode.appendChild(n);
  return (n);
} // createSVGNode()


