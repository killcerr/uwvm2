/* Gource-style renderer for Doxygen graphs (canvas + glow + simple physics)
 * - Reads nodes/edges from an existing graph iframe/object SVG on the page
 * - Renders to a full-screen canvas overlay with force-directed layout
 * - Controls: mousewheel zoom, drag canvas, click a node to focus
 */
(function(){
  function $e(tag,attrs){ const el=document.createElement(tag); if(attrs) Object.assign(el,attrs); return el; }
  function now(){ return performance.now(); }
  function clamp(v,a,b){ return Math.max(a,Math.min(b,v)); }

  // UI bootstrap: inject toolbar button near graph areas
  function insertButtons(){
    // for each dyncontent that contains an iframe/object svg graph, add a button
    document.querySelectorAll('.dyncontent, .contents').forEach(function(container){
      const hasGraph = container.querySelector('iframe[src$=\.svg], object[type="image/svg+xml"]');
      if(!hasGraph) return;
      if(container.querySelector('.gourcefx-toolbar')) return;
      const bar = $e('div',{className:'gourcefx-toolbar'});
      const btn = $e('button',{className:'gourcefx-btn',innerText:'Interactive View'});
      btn.addEventListener('click', function(){ launchOverlay(container); });
      bar.appendChild(btn);
      container.prepend(bar);
    });
  }

  function readGraph(container, cb){
    // read from iframe/object -> parse <g class="node"> and <g class="edge">
    const iframe = container.querySelector('iframe[src$=\.svg]');
    const object = container.querySelector('object[type="image/svg+xml"]');
    function parseDoc(doc){
      const svg = doc && doc.querySelector('svg');
      if(!svg) return cb({nodes:[],edges:[]});
      const nodeEls = svg.querySelectorAll('g.node');
      const edgeEls = svg.querySelectorAll('g.edge');
      const nodes = [];
      nodeEls.forEach(function(n,i){
        const t = (n.querySelector('title')||{}).textContent||('n'+i);
        nodes.push({ id:i, title:t, x:Math.random()-0.5, y:Math.random()-0.5, vx:0, vy:0, fixed:false });
      });
      const edges = [];
      edgeEls.forEach(function(e){
        const tt = (e.querySelector('title')||{}).textContent||'';
        // best-effort: "a -> b" appears in title for graphviz
        const m = tt.split('->');
        if(m.length===2){
          const a = m[0].trim(), b = m[1].trim();
          const ai = nodes.findIndex(n=>a && n.title.indexOf(a)!==-1);
          const bi = nodes.findIndex(n=>b && n.title.indexOf(b)!==-1);
          if(ai>=0 && bi>=0) edges.push({ source:ai, target:bi });
        }
      });
      cb({nodes,edges});
    }
    if(iframe && (iframe.contentDocument||iframe.contentWindow)){ return parseDoc(iframe.contentDocument || iframe.contentWindow.document); }
    if(object && object.contentDocument){ return parseDoc(object.contentDocument); }
    cb({nodes:[],edges:[]});
  }

  function launchOverlay(container){
    readGraph(container, function(graph){
      const overlay = $e('div',{className:'gourcefx-overlay active'});
      const canvas = $e('canvas',{className:'gourcefx-canvas'});
      const close = $e('button',{className:'gourcefx-close',innerText:'Close (Esc)'});
      const hint = $e('div',{className:'gourcefx-hint',innerHTML:'Scroll: zoom · Drag: pan · Click: focus'});
      overlay.appendChild(canvas); overlay.appendChild(close); overlay.appendChild(hint);
      document.body.appendChild(overlay);

      const ctx = canvas.getContext('2d');
      let dpr = Math.min(2,window.devicePixelRatio||1);
      function resize(){ canvas.width=overlay.clientWidth*dpr; canvas.height=overlay.clientHeight*dpr; }
      resize();
      const onResize = ()=>{ dpr=Math.min(2,window.devicePixelRatio||1); resize(); };
      window.addEventListener('resize', onResize);

      // camera
      let cam = { x:0, y:0, z:1 };
      let dragging=false, lx=0, ly=0;
      overlay.addEventListener('wheel', (ev)=>{ ev.preventDefault(); cam.z = clamp(cam.z*Math.pow(1.001,-ev.deltaY), 0.25, 3.5); });
      overlay.addEventListener('mousedown', (ev)=>{ dragging=true; lx=ev.clientX; ly=ev.clientY; });
      window.addEventListener('mouseup', ()=>dragging=false);
      window.addEventListener('mousemove', (ev)=>{ if(!dragging) return; cam.x += (ev.clientX-lx)/(canvas.height*0.5)/cam.z; cam.y += (ev.clientY-ly)/(canvas.height*0.5)/cam.z; lx=ev.clientX; ly=ev.clientY; });
      close.addEventListener('click', destroy);
      window.addEventListener('keydown', (e)=>{ if(e.key==='Escape') destroy(); });

      // physics params
      const nodes = graph.nodes, edges = graph.edges;
      const N = nodes.length;
      const linkK = 0.1, linkLen = 0.18; // in world units
      const charge = 0.00035, damping = 0.9;

      // layout seed in circle
      for(let i=0;i<N;i++){ const a=2*Math.PI*i/N; nodes[i].x=Math.cos(a)*0.6; nodes[i].y=Math.sin(a)*0.6; }

      let focusId = -1;
      overlay.addEventListener('click', (ev)=>{
        const rect = canvas.getBoundingClientRect();
        const mx = ((ev.clientX-rect.left)/rect.width*2-1) * (canvas.width/Math.min(canvas.width,canvas.height));
        const my = ((ev.clientY-rect.top)/rect.height*2-1) * (canvas.height/Math.min(canvas.width,canvas.height));
        // hit test
        let minD=1e9, idx=-1; nodes.forEach((n,i)=>{ const x=(n.x+cam.x)*cam.z, y=(n.y+cam.y)*cam.z; const d=(x-mx)*(x-mx)+(y-my)*(y-my); if(d<minD){minD=d; idx=i;} });
        focusId = idx;
      });

      function step(dt){
        // repulsion
        for(let i=0;i<N;i++){
          for(let j=i+1;j<N;j++){
            const a=nodes[i], b=nodes[j];
            let dx=a.x-b.x, dy=a.y-b.y; let d2=dx*dx+dy*dy+1e-6; let f=charge/d2;
            dx*=f; dy*=f; a.vx+=dx; a.vy+=dy; b.vx-=dx; b.vy-=dy;
          }
        }
        // springs
        edges.forEach(e=>{
          const a=nodes[e.source], b=nodes[e.target];
          let dx=b.x-a.x, dy=b.y-a.y; const d=Math.sqrt(dx*dx+dy*dy)||1e-6; const k=linkK*(d-linkLen); dx/=d; dy/=d; a.vx+=dx*k; a.vy+=dy*k; b.vx-=dx*k; b.vy-=dy*k;
        });
        // focus gravity
        if(focusId>=0){ const f=nodes[focusId]; nodes.forEach((n,i)=>{ if(i===focusId) return; n.vx+=(f.x-n.x)*0.002; n.vy+=(f.y-n.y)*0.002; }); }
        // integrate
        nodes.forEach(n=>{ n.vx*=damping; n.vy*=damping; n.x+=n.vx*dt; n.y+=n.vy*dt; });
      }

      function draw(){
        const W=canvas.width, H=canvas.height; const S=Math.min(W,H)/2;
        ctx.setTransform(1,0,0,1,0,0); ctx.clearRect(0,0,W,H);
        // background glow
        const g=ctx.createRadialGradient(W*0.5,H*0.35,0,W*0.5,H*0.35,Math.max(W,H)*0.7);
        g.addColorStop(0,'#131418'); g.addColorStop(1,'#08090b'); ctx.fillStyle=g; ctx.fillRect(0,0,W,H);

        // edges
        ctx.save();
        ctx.translate(W/2,H/2); ctx.scale(S,S);
        ctx.lineWidth=1/ S; ctx.strokeStyle='rgba(255,180,80,.22)';
        edges.forEach(e=>{
          const a=nodes[e.source], b=nodes[e.target];
          ctx.beginPath(); ctx.moveTo((a.x+cam.x)*cam.z,(a.y+cam.y)*cam.z); ctx.lineTo((b.x+cam.x)*cam.z,(b.y+cam.y)*cam.z); ctx.stroke();
        });
        // nodes with glow
        nodes.forEach((n,i)=>{
          const x=(n.x+cam.x)*cam.z, y=(n.y+cam.y)*cam.z;
          const r= 0.012 * Math.max(0.7, Math.min(1.6, cam.z));
          // glow
          ctx.fillStyle='rgba(255,160,60,.05)'; ctx.beginPath(); ctx.arc(x,y,r*5,0,Math.PI*2); ctx.fill();
          // core
          const sel = (i===focusId);
          ctx.fillStyle = sel ? '#ffd87a' : '#e8e6e3';
          ctx.beginPath(); ctx.arc(x,y,r,0,Math.PI*2); ctx.fill();
        });
        ctx.restore();
      }

      let last=now(); let rafId=0; function loop(){ const t=now(); const dt=Math.min(0.05,(t-last)/1000); last=t; step(dt); draw(); rafId=requestAnimationFrame(loop); }
      loop();

      function destroy(){ cancelAnimationFrame(rafId); window.removeEventListener('resize', onResize); overlay.remove(); }
    });
  }

  function init(){ insertButtons(); }
  if(document.readyState!=='loading') init(); else document.addEventListener('DOMContentLoaded', init);
})();


