/**
 * Lightweight hover gravity for Graphviz SVG in Doxygen
 * - Targets graph SVGs (inheritance, collaboration, dir, class, namespace, concept-like pages)
 * - On node hover: gently pull nearby nodes, highlight edges
 * - Non-destructive: only client-side transforms, no layout recompute
 */
(function(){
  function ready(fn){ if(document.readyState!=='loading'){ fn(); } else { document.addEventListener('DOMContentLoaded', fn); } }

  function distance(a,b){ var dx=a.x-b.x, dy=a.y-b.y; return Math.sqrt(dx*dx+dy*dy); }
  function applyLens(svg){
    if(!svg) return;
    var nodes = Array.from(svg.querySelectorAll('g.node'));
    if(nodes.length===0) return;
    // Tunables (gentle, global lens)
    var SIGMA = 180;       // Gaussian radius
    var MAX_T = 5;         // max translation in px
    var MAX_S = 1.05;      // max scale
    var ALPHA = 0.15;      // easing toward target per frame

    // cache centers once per svg render
    var centers = new Map();
    nodes.forEach(function(n){
      try {
        var bb = n.getBBox();
        centers.set(n, { x: bb.x + bb.width/2, y: bb.y + bb.height/2 });
      } catch(e) {}
      n.style.transformOrigin = 'center';
      n.style.transform = 'translate(0px,0px) scale(1)';
    });

    // per-node animation state
    var state = new Map();
    nodes.forEach(function(n){ state.set(n, { tx:0, ty:0, s:1, txT:0, tyT:0, sT:1 }); });

    // pointer in svg coords
    function toSvgPoint(evt){
      var pt = svg.createSVGPoint(); pt.x = evt.clientX; pt.y = evt.clientY;
      try{
        var ctm = svg.getScreenCTM();
        if(ctm && typeof ctm.inverse==='function') return pt.matrixTransform(ctm.inverse());
      }catch(e){}
      return { x: evt.clientX, y: evt.clientY };
    }

    var cursor = null;
    svg.addEventListener('pointermove', function(evt){ cursor = toSvgPoint(evt); });
    svg.addEventListener('pointerleave', function(){ cursor = null; });

    function frame(){
      // set targets
      nodes.forEach(function(n){
        var st = state.get(n); var c = centers.get(n);
        if(!st || !c) return;
        if(cursor){
          var dx = cursor.x - c.x, dy = cursor.y - c.y; var d = Math.sqrt(dx*dx+dy*dy) || 1;
          var g = Math.exp(-(d*d)/(2*SIGMA*SIGMA));
          var tmag = MAX_T * g; var s = 1 + (MAX_S - 1) * g;
          st.txT = (dx / d) * tmag;
          st.tyT = (dy / d) * tmag;
          st.sT  = s;
        } else {
          st.txT = 0; st.tyT = 0; st.sT = 1;
        }
      });

      // ease
      nodes.forEach(function(n){
        var st = state.get(n); if(!st) return;
        st.tx += (st.txT - st.tx) * ALPHA;
        st.ty += (st.tyT - st.ty) * ALPHA;
        st.s  += (st.sT  - st.s ) * ALPHA;
        n.style.transform = 'translate('+st.tx.toFixed(2)+'px,'+st.ty.toFixed(2)+'px) scale('+st.s.toFixed(4)+')';
      });

      requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
  }

  function bindInlineSvgs(){
    var svgs = document.querySelectorAll('svg[id^="graph_"], svg[id^="inherit_graph_"], svg[id^="coll_graph_"], svg[id^="dir_"], svg[id^="class_"], svg[id^="namespace_"]');
    svgs.forEach(function(svg){ applyLens(svg); });
  }

  function bindEmbeddedSvgs(){
    // iframes (Doxygen 1.14 uses <iframe ... src="*_dep.svg"> for graphs)
    var iframes = Array.from(document.querySelectorAll('iframe[src$=".svg"]'));
    iframes.forEach(function(fr){
      function tryBind(){
        try{
          var d = fr.contentDocument || fr.contentWindow && fr.contentWindow.document;
          if(d){ var svg = d.querySelector('svg'); if(svg){ applyLens(svg); } }
        }catch(e){}
      }
      if(fr.complete) tryBind();
      fr.addEventListener('load', tryBind, { once:false });
    });

    // <object type="image/svg+xml" data="...">
    var objects = Array.from(document.querySelectorAll('object[type="image/svg+xml"]'));
    objects.forEach(function(obj){
      function tryBindObj(){
        try{ var d = obj.contentDocument; if(d){ var svg = d.querySelector('svg'); if(svg){ applyLens(svg); } } }catch(e){}
      }
      tryBindObj();
      obj.addEventListener('load', tryBindObj, { once:false });
    });
  }

  function observeLazyGraphs(){
    // graphs can be lazy-loaded (iframe loading="lazy") inside dynsections; observe for insertion/visibility
    var mo = new MutationObserver(function(){ bindEmbeddedSvgs(); });
    mo.observe(document.documentElement, { childList:true, subtree:true });
  }

  function init(){
    bindInlineSvgs();
    bindEmbeddedSvgs();
    observeLazyGraphs();
  }

  ready(init);
  document.addEventListener('readystatechange', function(){ if(document.readyState==='complete') init(); });
})();


