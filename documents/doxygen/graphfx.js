/**
 * Lightweight hover gravity for Graphviz SVG in Doxygen
 * - Targets graph SVGs (inheritance, collaboration, dir, class, namespace, concept-like pages)
 * - On node hover: gently pull nearby nodes, highlight edges
 * - Non-destructive: only client-side transforms, no layout recompute
 */
(function(){
  function ready(fn){ if(document.readyState!=='loading'){ fn(); } else { document.addEventListener('DOMContentLoaded', fn); } }

  function distance(a,b){ var dx=a.x-b.x, dy=a.y-b.y; return Math.sqrt(dx*dx+dy*dy); }
  function lerp(a,b,t){ return a+(b-a)*t; }

  function applyGravity(svg){
    if(!svg) return;
    var nodes = Array.from(svg.querySelectorAll('g.node'));
    if(nodes.length===0) return;
    // cache centers once per svg render
    var centers = new Map();
    nodes.forEach(function(n){
      try {
        var bb = n.getBBox();
        centers.set(n, { x: bb.x + bb.width/2, y: bb.y + bb.height/2 });
      } catch(e) {}
    });

    nodes.forEach(function(n){
      n.addEventListener('mouseenter', function(){
        var c0 = centers.get(n);
        nodes.forEach(function(m){
          if(m===n) return;
          var c1 = centers.get(m);
          if(!c0 || !c1) return;
          var d = distance(c0, c1);
          var influence = Math.max(0, 1 - (d/180)); // radius ~180px
          if(influence<=0) return;
          var tx = lerp(0, (c0.x - c1.x)*0.04, influence); // move slightly towards
          var ty = lerp(0, (c0.y - c1.y)*0.04, influence);
          m.style.transition = 'transform 180ms ease, filter 180ms ease, stroke 180ms ease';
          m.style.transform = 'translate('+ (tx) +'px,'+ (ty) +'px)';
        });
        // de-emphasize all edges, then highlight adjacent ones
        svg.querySelectorAll('g.edge').forEach(function(e){
          e.style.transition = 'opacity 150ms ease, stroke 150ms ease';
          e.style.opacity = '0.35';
        });
        var title = (n.querySelector('title')||{}).textContent||'';
        if(title){
          svg.querySelectorAll('g.edge').forEach(function(e){
            var et = (e.querySelector('title')||{}).textContent||'';
            if(et.indexOf(title)!==-1){ e.style.opacity = '1'; }
          });
        }
      });
      n.addEventListener('mouseleave', function(){
        nodes.forEach(function(m){ m.style.transform=''; m.style.filter=''; });
        svg.querySelectorAll('g.edge').forEach(function(e){ e.style.opacity=''; });
      });
    });
  }

  function bindInlineSvgs(){
    var svgs = document.querySelectorAll('svg[id^="graph_"], svg[id^="inherit_graph_"], svg[id^="coll_graph_"], svg[id^="dir_"], svg[id^="class_"], svg[id^="namespace_"]');
    svgs.forEach(function(svg){ applyGravity(svg); });
  }

  function bindEmbeddedSvgs(){
    // iframes (Doxygen 1.14 uses <iframe ... src="*_dep.svg"> for graphs)
    var iframes = Array.from(document.querySelectorAll('iframe[src$=".svg"]'));
    iframes.forEach(function(fr){
      function tryBind(){
        try{
          var d = fr.contentDocument || fr.contentWindow && fr.contentWindow.document;
          if(d){ var svg = d.querySelector('svg'); if(svg){ applyGravity(svg); } }
        }catch(e){}
      }
      if(fr.complete) tryBind();
      fr.addEventListener('load', tryBind, { once:false });
    });

    // <object type="image/svg+xml" data="...">
    var objects = Array.from(document.querySelectorAll('object[type="image/svg+xml"]'));
    objects.forEach(function(obj){
      function tryBindObj(){
        try{ var d = obj.contentDocument; if(d){ var svg = d.querySelector('svg'); if(svg){ applyGravity(svg); } } }catch(e){}
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


