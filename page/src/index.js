(() => {
    let ws = new WebSocket(`ws://${location.host}:81/ws`);
    let lastPing = 0;
    let pingInterval = 10000;
    let pingTimer = null;
    let reconnectInterval = null;
    ws.onopen = onOpen;
    ws.onmessage = onMessage;
    ws.onclose = onClose;
    ws.onerror = onError;

    function onMessage(msg) {
        let response = JSON.parse(msg.data);
        console.log(response);
        let data = response.data;
        if (response.status !== 200) {
            return;
        }
        for (let item of data.update) {
            updateNode(item);
        }
        for (let item of data.remove) {
            removeNode(item);
        }
    }

    function onClose(){
        console.log('Disconnected');
        clearInterval(pingTimer);
        // try to reconnect
        reconnectInterval = setInterval(() => {
            console.log('Reconnecting...');
            ws = new WebSocket(`ws://${location.host}:81/ws`);
            ws.onopen = onOpen;
            ws.onmessage = onMessage;
            ws.onclose = onClose;
            ws.onerror = onError;        
        }, 5000);
    }

    function onOpen() {
        clearInterval(reconnectInterval);
        console.log('Connected');
        pingTimer = setInterval(ping, pingInterval);
    }

    function onError(err) {
        console.log(err);
    }

    function getNode(id) {
        return document.querySelector(`[app-id="${id}"]`);
    }

    function removeNode(id) {
        let node = getNode(id);
        if (node) {
            node.remove();
        }
    }

    function updateNode(data) {
        let node = getNode(data['app-id']);
        if (!node) {
            return;
        }
        // remove old attributes
        while (node.attributes.length > 0) {
            node.removeAttribute(node.attributes[0].name);
        }
        for (let key in data.attributes) {
            node.setAttribute(key, data.attributes[key]);
        }
        if (data.innerText) {
            node.innerText = data.innerText;
        }
        // set tag
        if (data.tag && data.tag !== node.tagName) {
            let newNode = document.createElement(data.tag);
            for (let key in node.attributes) {
                newNode.setAttribute(key, node.attributes[key]);
            }
            newNode.innerText = node.innerText;
            while (node.firstChild) {
                newNode.appendChild(node.firstChild);
            }
            node.replaceWith(newNode);
        }
    }

    function ping() {
        if (Date.now() - lastPing < pingInterval) {
            return;
        }
        lastPing = Date.now();
        ws.send(JSON.stringify({
            event: 'ping',
        }));
    }

    // This is template for some event on some element
    function addEventTemplate(appId, event) {
        let element = getNode(appId);
        element.addEventListener(event, (e) => {
            lastPing = Date.now();
            ws.send(JSON.stringify({
                event: event,
                "app-id": appId,
                data: e.target.value,
            }));
        });
    }

    window.app = {
        addEvent: addEventTemplate,
    };
})();

//set theme
(() => {
    const themeSwitcher = document.getElementById('theme-switcher');
    (function () {
        let theme = localStorage.getItem('theme');
        console.log(theme);
        if (theme) {
            themeSwitcher.checked = theme == 'dark';
            document.body.classList.add(theme);
        } else {
            document.body.classList.add('light');
        }
    })();
    themeSwitcher.addEventListener('click', (e) => {
        if (e.target.checked) {
            document.body.classList.add('dark');
            document.body.classList.remove('light');
            localStorage.setItem('theme', 'dark');
        } else {
            document.body.classList.add('light');
            document.body.classList.remove('dark');
            localStorage.setItem('theme', 'light');
        }
    });
})();

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
        let ignoreEvent = false;

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
                        moving = false;
                        canvas.classList.remove(activeClass);
                        canvas.dispatchEvent(new Event('switchedOff'));
                        emitEvent();
                    }
                } else {
                    if (switchPos.x < width - margin - radius / 2 - moveStep - 1.5) {
                        switchPos.x += moveStep - 1.5;
                    } else {
                        status = true;
                        moving = false;
                        canvas.classList.add(activeClass);
                        if (!ignoreEvent) {
                            canvas.dispatchEvent(new Event('switchedOn'));
                            emitEvent();
                        }
                        ignoreEvent = false;
                    }
                }
            }

            if (init < 1)
                init++;

            requestAnimationFrame(draw);
        }

        function emitEvent() {
            canvas.dispatchEvent(new Event(switchedEvent));
            canvas.dispatchEvent(new Event(changeEvent));
        }

        canvas.addEventListener('click', (e) => {
            if (moving) return;
            moving = true;
        });

        canvas.addEventListener('touchstart', (e) => {
            if (moving) return;
            moving = true;
        });
        
        window.addEventListener('load', () => {
            if (canvas.classList.contains(activeClass)) {
                ignoreEvent = true;
                moving = true;
            }
        });
    });
})();

(() => {
    // control logic for the page
    (() => {
        let numberInputs = document.querySelectorAll('input[type="num"]');
        const selects = document.querySelectorAll('select');
        const modelsOverlays = document.querySelectorAll('.model-overlay');
        const titleText = document.getElementById('title-text');

        numberInputs.forEach((input) => {
            input.addEventListener('input', (e) => {
                let value = e.target.value;

                let newValue = value.replace(/[^0-9.-]/g, '');

                console.log(e.target.getAttribute('negative'));
                if (e.target.getAttribute('negative') == 'false') {
                    newValue = newValue.replace(/[^0-9.]/g, '');
                }

                if (e.target.getAttribute('float') == 'false') {
                    newValue = newValue.replace(/[^0-9-]/g, '');
                }

                newValue = newValue.replace(/(\..*)\./g, '$1');

                if (newValue !== value) {
                    e.target.value = newValue;
                }
            });
        });


        selects.forEach((select) => {
            select.addEventListener('change', (e) => {
                let value = e.target.value;
                let option = e.target.querySelector(`option[value="${value}"]`);
                if (option) {
                    option.setAttribute('selected', 'selected');
                }
            });

            let selectedOption = select.querySelector('option[selected]');
            if (selectedOption) {
                select.value = selectedOption.value;
            }

            let defaultOption = select.querySelector('option[default]');
            if (defaultOption) {
                select.value = defaultOption.value;
            }
        });

        modelsOverlays.forEach((overlay) => {
            overlay.addEventListener('click', (e) => {
                e.target.classList.remove('active');
            });
            overlay.querySelector('[close-model="true"]').addEventListener('click', (e) => {
                overlay.classList.remove('active');
                overlay.classList.remove('edit');
            });
        });

        document.getElementById('add-pin-open').addEventListener('click', () => {
            document.getElementById('add-pin-model').classList.remove('edit');
            changeModal();
            document.getElementById('add-pin-model').classList.add('active');
        });

        fetch('/api/title').then((res) => {
            if (res.ok) {
                res.text().then((text) => {
                    titleText.innerText = text;
                    document.title = `Controller '${text}' - Homium`;
                });
            }
        });
    })();

    // API authentication and host
    (() => {
        const hostNameInput = document.getElementById('host-name');
        const hostPortInput = document.getElementById('host-port');
        const apiKeyInput = document.getElementById('api-key');
        const saveButton = document.getElementById('save-host-settings');
        const cancelButton = document.getElementById('cancel-host-settings');

        let configData = {
            host: '',
            port: '',
            apiKey: '',
        };

        let newConfigData = {
            host: '',
            port: '',
            apiKey: '',
        };

        fetch('/api/hostConfig').then((res) => {
            if (res.ok) {
                res.json().then((data) => {
                    hostNameInput.value = data.host;
                    hostPortInput.value = data.port;
                    apiKeyInput.value = data.apiKey;
                    configData = data;
                    newConfigData = data;
                });
            }
        });

        hostNameInput.addEventListener('input', (e) => {
            newConfigData.host = e.target.value;
        });

        hostPortInput.addEventListener('input', (e) => {
            newConfigData.port = e.target.value;
        });

        apiKeyInput.addEventListener('input', (e) => {
            newConfigData.apiKey = e.target.value;
        });

        saveButton.addEventListener('click', () => {
            fetch('/api/hostConfig', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(newConfigData),
            }).then((res) => {
                if (res.ok) {
                    configData = newConfigData;
                }
            });
        });

        cancelButton.addEventListener('click', () => {
            hostNameInput.value = configData.host;
            hostPortInput.value = configData.port;
            apiKeyInput.value = configData.apiKey;
            newConfigData = configData;
        });
    })();

    // API wifi settings
    (() => {
        wifiConfig('wifi-connection-form', 'wifi-connection-save', 'wifi-connection-cancel', '/api/wifiConfig');
        wifiConfig('art-configure-form', 'art-configure-save', 'art-configure-cancel', '/api/apConfig');
        function wifiConfig(form, saveButton, cancelButton, url) {
            const wifiSettingsForm = document.getElementById(form);
            const wifiSettingsSaveButton = document.getElementById(saveButton);
            const wifiSettingsCancelButton = document.getElementById(cancelButton);

            let configData = {
                ssid: '',
                password: '',
            };

            let newConfigData = {
                ssid: '',
                password: '',
            };

            fetch(url).then((res) => {
                if (res.ok) {
                    res.json().then((data) => {
                        wifiSettingsForm.ssid.value = data.ssid;
                        wifiSettingsForm.password.value = data.password;
                        configData = data;
                        newConfigData = data;
                    });
                }
            });

            wifiSettingsForm.ssid.addEventListener('input', (e) => {
                newConfigData.ssid = e.target.value;
            });

            wifiSettingsForm.password.addEventListener('input', (e) => {
                newConfigData.password = e.target.value;
            });

            wifiSettingsSaveButton.addEventListener('click', () => {
                fetch(url, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify(newConfigData),
                }).then((res) => {
                    if (res.ok) {
                        configData = newConfigData;
                    }
                });
            });

            wifiSettingsCancelButton.addEventListener('click', () => {
                wifiSettingsForm.ssid.value = configData.ssid;
                wifiSettingsForm.password.value = configData.password;
                newConfigData = configData;
            });
        }
    })();

    // API pins
    (() => {
        const addPinModel = document.getElementById('add-pin-model');
        const addPinForm = document.getElementById('add-pin-form');
        const addPinSaveButton = document.getElementById('add-pin-save');
        const addPinCancelButton = document.getElementById('add-pin-cancel');
        const pinsList = document.getElementById('pins-list');
        addPinForm.hasClock.checked = false;
        addPinForm.inverted.checked = false;
        let configData = {
            pin: '',
            mode: '',
            object: '',
            property: '',
            clock: 0,
            hasClock: false,
            inverted: false,
        };

        fetch('/api/pins').then((res) => {
            if (res.ok) {
                res.json().then((data) => {
                    data.sort((a, b) => a.pinNumber - b.pinNumber);
                    data.forEach((pin) => {
                        addPinForm.pin.options.add(new Option(pin, pin));
                    });
                });
            }
        });

        fetch('/api/objects').then((res) => {
            if (res.ok) {
                res.json().then((data) => {
                    data.forEach((object) => {
                        addPinForm.object.options.add(new Option(object.name, object.id));
                    });
                });
            }
        });

        fetch('/api/pinsConfig').then((res) => {
            if (res.ok) {
                res.json().then((data) => {
                    data.sort((a, b) => a.pin - b.pin);
                    data.forEach((pin) => addPin(pin));
                    let deleteButtons = document.querySelectorAll('.delete-pin');
                    deleteButtons.forEach((button) => {
                        button.addEventListener('click', (e) => {
                            fetch('/api/pinsConfig?pin=' + e.target.getAttribute('pin'), { method: 'DELETE' }).then((res) => {
                                if (res.ok) {
                                    e.target.parentNode.parentNode.remove();
                                }
                            });
                        });
                    });
                });
            }
        });

        addPinForm.pin.addEventListener('change', (e) => {
            configData.pin = e.target.value;
            checkSatatus();
        });

        addPinForm.mode.addEventListener('change', (e) => {
            configData.mode = e.target.value;
            checkSatatus();
        });

        addPinForm.object.addEventListener('change', (e) => {
            if (configData.object != e.target.value) {
                configData.property = '';
                addPinForm.property.options.length = 1;
                fetch('/api/objects?obj=' + e.target.value).then((res) => {
                    if (res.ok) {
                        res.json().then((data) => {
                            let properties = Object.keys(data.properties);
                            properties.forEach((property) => {
                                addPinForm.property.options.add(new Option(property, property));
                            });
                        });
                    }
                });
                checkSatatus();
            }
            configData.object = e.target.value;
        });

        addPinForm.property.addEventListener('change', (e) => {
            configData.property = e.target.value;
            checkSatatus();
        });

        addPinForm.inverted.addEventListener('change', (e) => {
            configData.inverted = e.target.checked;
            checkSatatus();
        });

        addPinForm.hasClock.addEventListener('change', (e) => {
            document.getElementById('clock-box').style.display = e.target.checked ? 'flex' : 'none';
            configData.hasClock = e.target.checked;
            if (!e.target.checked) {
                configData.clock = 0;
            } else {
                configData.clock = addPinForm.clock.value ?? 0;
            }
            checkSatatus();
        });

        addPinForm.clock.addEventListener('change', (e) => {
            configData.clock = e.target.value;
            checkSatatus();
        });

        addPinSaveButton.addEventListener('click', () => {
            let options = {
                method: '',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(configData),
            }
            if (addPinModel.classList.contains('edit')) {
                options.method = 'PUT';
            } else {
                options.method = 'POST';
            }
            fetch('/api/pinsConfig', options).then((res) => {
                if (res.ok) {
                    location.reload();
                }
            });
        });
        addPinCancelButton.addEventListener('click', () => {
            addPinForm.pin.value = '';
            addPinForm.mode.value = '';
            addPinForm.object.value = '';
            addPinForm.property.value = '';
            addPinForm.inverted.checked = false;
            addPinForm.hasClock.checked = false;
            configData = {
                pin: '',
                mode: '',
                object: '',
                property: '',
                clock: 0,
                hasClock: false,
                inverted: false,
            };
        });

        function checkSatatus() {
            addPinSaveButton.disabled = configData.pin == '' ||
                configData.mode == '' ||
                configData.object == '' ||
                configData.property == '' ||
                (configData.hasClock && configData.clock <= 0);
        }

        function addPin(pin) {
            const pinRow = document.createElement('tr');
            pinRow.innerHTML = `<td>${pin.pin}</td><td>${pin.mode == 0 ? 'Input' : 'Output'}</td><td>${pin.objectName}</td><td>${pin.property}</td>`;
            pinRow.innerHTML += `<td><button class="button reject delete-pin" pin="${pin.pin}">Delete</button><button class="button accept edit-pin" id=pin_edit_${pin.pin}>Edit</button></td>`;
            pinsList.appendChild(pinRow);
            document.getElementById(`pin_edit_${pin.pin}`).addEventListener('click', async () => {
                addPinModel.classList.add('edit');
                addPinForm.pin.value = pin.pin;
                addPinForm.mode.value = pin.mode == 0 ? 'INPUT' : 'OUTPUT';
                addPinForm.object.value = pin.objectId;
                await loadPropFromObject(pin.objectId);
                addPinForm.property.value = pin.property;
                addPinForm.hasClock.checked = pin.clock > 0;
                addPinForm.clock.value = pin.clock;
                document.getElementById('clock-box').style.display = addPinForm.hasClock.checked ? 'flex' : 'none';
                addPinForm.inverted.checked = pin.inverted;
                configData = {
                    pin: pin.pin,
                    mode: addPinForm.mode.value,
                    object: pin.objectId,
                    property: pin.property,
                    clock: pin.clock,
                    hasClock: pin.clock > 0,
                    inverted: pin.inverted,
                };
                changeModal();
                addPinModel.classList.add('active');
                addPinSaveButton.disabled = false;
            });
        }

        async function loadPropFromObject(id) {
            addPinForm.property.options.length = 1;
            const object = await fetch(`/api/objects?obj=${id}`).then((res) => res.json());
            const properties = Object.keys(object.properties);
            properties.forEach((property) => {
                addPinForm.property.options.add(new Option(property, property));
            });
        }
    })();

    // API devices

    (() => {
        const deviceSettingsForm = document.getElementById('device-settings-form');
        const deviceSettingsSaveButton = document.getElementById('device-settings-save');
        const deviceSettingsCancelButton = document.getElementById('device-settings-cancel');

        let configData = {
            name: '',
            login: '',
            password: '',
        };

        fetch('/api/deviceConfig').then((res) => {
            if (res.ok) {
                res.json().then((data) => {
                    deviceSettingsForm.deviceName.value = data.name;
                    deviceSettingsForm.login.value = data.login;
                    deviceSettingsForm.password.value = data.password;
                    configData = data;
                });
            }
        });

        deviceSettingsForm.deviceName.addEventListener('input', (e) => {
            configData.name = e.target.value;
        });

        deviceSettingsForm.login.addEventListener('input', (e) => {
            configData.login = e.target.value;
        });

        deviceSettingsForm.password.addEventListener('input', (e) => {
            configData.password = e.target.value;
        });

        deviceSettingsSaveButton.addEventListener('click', () => {
            fetch('/api/deviceConfig', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(configData),
            }).then((res) => {
                if (res.ok) {
                    location.reload();
                }
            });
        });

        deviceSettingsCancelButton.addEventListener('click', () => {
            deviceSettingsForm.name.value = configData.name;
            deviceSettingsForm.login.value = configData.login;
            deviceSettingsForm.password.value = configData.password;
        });
    })();

})();
function changeModal() {
    let editMode = document.getElementById('add-pin-model').classList.contains('edit');
    document.getElementById('add-pin-model').querySelector('.title').innerText = editMode ? 'Edit Pin' : 'Add Pin';
    document.getElementById('add-pin-model').querySelector('.save').innerText = editMode ? 'Save' : 'Add';
}