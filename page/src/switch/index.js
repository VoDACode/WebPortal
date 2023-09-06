// switch
(() => {
    const colors = {
        none: 'transparent',
        background: {
            active: '#2ecc71',
            inactive: '#ccc'
        },
        switch: '#FFFFFF'
    };
    const moveStep = 8;
    const radius = 140;
    const margin = 1;
    const activeClass = 'active';
    const switchedEvent = 'switched';
    const changeEvent = 'change';
    let canvases = document.querySelectorAll('.switch');
    canvases.forEach((canvas) => {
        if (canvas.getContext === undefined) {
            console.error('This object does not canvas');
            console.error(canvas);
            return;
        }
        let ctx = canvas.getContext('2d');
        let width = canvas.width;
        let height = canvas.height;

        let status = false;
        let moving = false;
        let init = 0;

        let processing = false;
        let ignoreObserver = false;

        // default it`s left side
        let switchPos = {
            x: margin + radius / 2 + 5,
            y: height / 2
        };

        draw();

        function draw() {
            if (moving == false && init > 1) {
                requestAnimationFrame(draw);
                return;
            }
            ctx.clearRect(0, 0, width, height);

            // Drawing border
            ctx.strokeStyle = colors.none;
            ctx.fillStyle = status ? colors.background.active : colors.background.inactive;
            ctx.beginPath();
            ctx.roundRect(margin, margin, width - margin * 2, height - margin * 2, radius);
            ctx.stroke();

            // Drawing sub border
            ctx.beginPath();
            ctx.roundRect(margin + 10, margin + 10, width - margin * 2 - 20, height - margin * 2 - 20, radius - 20);
            ctx.stroke();
            ctx.fill();

            ctx.fillStyle = colors.switch;
            // Drawing switch
            ctx.beginPath();
            ctx.arc(switchPos.x, switchPos.y, height - margin - 95, 0, 2 * Math.PI);
            ctx.stroke();
            ctx.fill();

            if (moving) {
                if (status) {
                    if (switchPos.x > margin + radius / 2 + moveStep) {
                        switchPos.x -= moveStep;
                    } else {
                        status = false;
                        if (ignoreObserver)
                            canvas.classList.remove(activeClass);
                        moving = false;
                        processing = false;
                    }
                } else {
                    if (switchPos.x < width - margin - radius / 2 - moveStep - 1.5) {
                        switchPos.x += moveStep - 1.5;
                    } else {
                        status = true;
                        if (ignoreObserver)
                            canvas.classList.add(activeClass);
                        moving = false;
                        processing = false;
                    }
                }
            }

            if (init < 1)
                init++;

            requestAnimationFrame(draw);
        }

        canvas.addEventListener('click', (e) => {
            if (processing)
                return;
            processing = true;
            canvas.dispatchEvent(new Event('switched' + (status ? 'On' : 'Off')));
            canvas.dispatchEvent(new Event(switchedEvent));
            canvas.dispatchEvent(new Event(changeEvent));
        });

        window.addEventListener('load', () => {
            if (canvas.classList.contains(activeClass)) {
                ignoreObserver = true;
                moving = true;
            }
        });

        // observer class change
        let observer = new MutationObserver((mutations) => {
            mutations.forEach((mutation) => {
                if (mutation.attributeName == 'class') {
                    if (ignoreObserver) {
                        ignoreObserver = false;
                        return;
                    }
                    moving = true;
                }
            });
        });
        observer.observe(canvas, {
            attributes: true,
            attributeFilter: ['class'],
            attributeOldValue: true
        });
    });
})();
