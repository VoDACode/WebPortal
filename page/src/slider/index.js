// switch
(() => {
    window.lastDownTarget = null;
    document.addEventListener('mousedown', (e) => {
        window.lastDownTarget = e.target;
    }, false);
    const colors = {
        none: 'transparent',
        background: "#ccc",
        slider: "#2ecc71"
    };
    const margin = 1;
    const changeEvent = 'change';
    let canvases = document.querySelectorAll('.slider');
    canvases.forEach((canvas) => {
        if (canvas.getContext === undefined) {
            console.error('This object does not canvas');
            console.error(canvas);
            return;
        }
        let ctx = canvas.getContext('2d');
        let width = canvas.width;
        let height = canvas.height;

        let canMove = false;

        canvas.max = Number(canvas.attributes.max?.value || 100);
        canvas.min = Number(canvas.attributes.min?.value || 0);
        canvas.step = Number(canvas.attributes.step?.value || 1);
        canvas.value = Number(canvas.attributes.value?.value || 0);

        canvas.setAttribute('max', canvas.max);
        canvas.setAttribute('min', canvas.min);
        canvas.setAttribute('step', canvas.step);
        canvas.setAttribute('value', canvas.value);

        draw();

        function draw() {
            ctx.clearRect(0, 0, width, height);

            // Drawing border
            ctx.strokeStyle = colors.none;
            ctx.fillStyle = colors.background;
            ctx.beginPath();
            ctx.roundRect(margin, margin, width - margin * 2, height - margin * 2, 10);
            ctx.stroke();
            ctx.fill();

            // Drawing slider
            ctx.fillStyle = colors.slider;
            ctx.beginPath();

            // map max min to 0..1
            let fillLevel = (canvas.value - canvas.min) / (canvas.max - canvas.min);

            ctx.roundRect(margin, margin, width * fillLevel - margin * 2, height - margin * 2, 10);
            ctx.stroke();
            ctx.fill();

            requestAnimationFrame(draw);
        }

        canvas.addEventListener('mousemove', (e) => {
            if (canMove == false)
                return;
            changeFillLevelEvent(e);
        });
        canvas.addEventListener('touchmove', (e) => {
            if (canMove == false)
                return;
            changeFillLevelEvent(e);
        });
        canvas.addEventListener('click', changeFillLevelEvent);

        function changeFillLevelEvent(e) {
            let rect = canvas.getBoundingClientRect();
            let x = (!e.touches ? e : e.touches[0]).clientX - rect.left;
            if (x < margin)
                x = margin;
            if (x > width - margin)
                x = width - margin;
            // map 0..1 to max min
            let tmpVal = (x - margin) / (width - margin * 2) * (canvas.max - canvas.min) + canvas.min;
            if (tmpVal < canvas.min)
                tmpVal = canvas.min;
            if (tmpVal > canvas.max)
                tmpVal = canvas.max;
            // use step
            canvas.value = Math.round(tmpVal / canvas.step) * canvas.step;
            canvas.setAttribute('value', canvas.value);

            //console.log(value);
        }

        canvas.addEventListener('mousedown', (e) => {
            canMove = true;
        });

        document.addEventListener('touchend', (e) => {
            if (canMove == true) {
                canMove = false;
                canvas.dispatchEvent(new Event(changeEvent));
            }
        });

        canvas.addEventListener('touchstart', (e) => {
            canMove = true;
        });

        document.addEventListener('mouseup', (e) => {
            if (canMove == true) {
                canMove = false;
                canvas.dispatchEvent(new Event(changeEvent));
            }
        });

        document.addEventListener('keydown', (e) => {
            if (window.lastDownTarget != canvas)
                return;
            let k = 1;
            if (e.shiftKey)
                k = 10;
            if (e.keyCode == 37) {
                canvas.value -= canvas.step * k;
                if (canvas.value < canvas.min)
                    canvas.value = canvas.min;
                canvas.dispatchEvent(new Event(changeEvent));
            }
            if (e.keyCode == 39) {
                canvas.value += canvas.step * k;
                if (canvas.value > canvas.max)
                    canvas.value = canvas.max;
                canvas.dispatchEvent(new Event(changeEvent));
            }
        });

        // observer attribute change
        const observer = new MutationObserver((mutations) => {
            mutations.forEach((mutation) => {
                if (mutation.type === 'attributes') {
                    if (mutation.attributeName === 'value') {
                        canvas.value = Number(canvas.attributes.value?.value || 0);
                    }
                    if (mutation.attributeName === 'max') {
                        canvas.max = Number(canvas.attributes.max?.value || 100);
                    }
                    if (mutation.attributeName === 'min') {
                        canvas.min = Number(canvas.attributes.min?.value || 0);
                    }
                    if (mutation.attributeName === 'step') {
                        canvas.step = Number(canvas.attributes.step?.value || 1);
                    }
                }
            });
        });
        observer.observe(canvas, {
            attributes: true
        });
    });
})();