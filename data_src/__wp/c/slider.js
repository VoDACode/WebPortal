(()=>{window.lastDownTarget=null,document.addEventListener('mousedown',e=>{window.lastDownTarget=e.target},!1);let e={none:'transparent',background:'#ccc',slider:'#2ecc71'},t='change';document.querySelectorAll('.slider').forEach(a=>{if(void 0===a.getContext){console.error('This object does not canvas'),console.error(a);return}let n=a.getContext('2d'),u=a.width,i=a.height,l=!1;function s(){n.clearRect(0,0,u,i),n.strokeStyle=e.none,n.fillStyle=e.background,n.beginPath(),n.roundRect(1,1,u-2,i-2,10),n.stroke(),n.fill(),n.fillStyle=e.slider,n.beginPath();let t=(a.value-a.min)/(a.max-a.min);n.roundRect(1,1,u*t-2,i-2,10),n.stroke(),n.fill(),requestAnimationFrame(s)}function r(e){let t=a.getBoundingClientRect(),n=(e.touches?e.touches[0]:e).clientX-t.left;n<1&&(n=1),n>u-1&&(n=u-1);let i=(n-1)/(u-2)*(a.max-a.min)+a.min;i<a.min&&(i=a.min),i>a.max&&(i=a.max),a.value=Math.round(i/a.step)*a.step,a.setAttribute('value',a.value),a.lastValue!=a.value&&(a.dispatchEvent(new Event('input')),a.lastValue=a.value)}a.max=Number(a.attributes.max?.value||100),a.min=Number(a.attributes.min?.value||0),a.step=Number(a.attributes.step?.value||1),a.value=Number(a.attributes.value?.value||0),a.lastValue=a.value,a.setAttribute('max',a.max),a.setAttribute('min',a.min),a.setAttribute('step',a.step),a.setAttribute('value',a.value),s(),a.addEventListener('mousemove',e=>{!1!=l&&r(e)}),a.addEventListener('touchmove',e=>{!1!=l&&r(e)}),a.addEventListener('click',r),a.addEventListener('mousedown',e=>{l=!0}),document.addEventListener('touchend',e=>{!0==l&&(l=!1,a.dispatchEvent(new Event(t)))}),a.addEventListener('touchstart',e=>{l=!0}),document.addEventListener('mouseup',e=>{!0==l&&(l=!1,a.dispatchEvent(new Event(t)))}),document.addEventListener('keydown',e=>{if(window.lastDownTarget!=a)return;let n=1;e.shiftKey&&(n=10),37==e.keyCode&&(a.value-=a.step*n,a.value<a.min&&(a.value=a.min),a.dispatchEvent(new Event(t))),39==e.keyCode&&(a.value+=a.step*n,a.value>a.max&&(a.value=a.max),a.dispatchEvent(new Event(t)))});let v=new MutationObserver(e=>{e.forEach(e=>{'attributes'===e.type&&('value'===e.attributeName&&(a.value=Number(a.attributes.value?.value||0)),'max'===e.attributeName&&(a.max=Number(a.attributes.max?.value||100)),'min'===e.attributeName&&(a.min=Number(a.attributes.min?.value||0)),'step'===e.attributeName&&(a.step=Number(a.attributes.step?.value||1)))})});v.observe(a,{attributes:!0})})})();