function createSVGNode(t,e,n,o){var r=document.createElementNS("http://www.w3.org/2000/svg",e);return n&&Object.getOwnPropertyNames(n).forEach((function(t){r.setAttribute(t,n[t])})),o&&(r.textContent=o),t.appendChild(r),r}function _removeChilds(t){Array.from(t.childNodes).forEach((function(t){t.remove()}))}function eventPoint(t){var e=document.documentElement,n=e.createSVGPoint();return n.x=t.clientX,n.y=t.clientY,n.matrixTransform(e.getScreenCTM().inverse())}function _cPoint(t,e){return{x:Math.sin(t)*e,y:-Math.cos(t)*e}}function _setOptions(t){Object.assign(document.api.options,t)}