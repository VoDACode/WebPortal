String.prototype.toBytes = function (ignoreLength = false) {
    let bytes = [];
    if (!ignoreLength) {
        bytes.push(this.length & 0xff);
        bytes.push((this.length >> 8) & 0xff);
    }
    for (let i = 0; i < this.length; i++) {
        bytes.push(this.charCodeAt(i));
    }
    return bytes;
};

String.prototype.fromBytes = function (bytes, length = -1) {
    if (length < 0) {
        length = bytes[0] | (bytes[1] << 8);
    }
    let str = '';
    for (let i = 0; i < length; i++) {
        str += String.fromCharCode(bytes[i + 2]);
    }
    return str;
};

Number.prototype.toBytes = function () {
    let bytes = [];
    bytes.push(this >= 0 ? 0 : 1);
    bytes.push(this & 0xff);
    bytes.push((this >> 8) & 0xff);
    return bytes;
};

Number.prototype.fromBytes = function (bytes) {
    return bytes[0] === 0 ? bytes[1] | (bytes[2] << 8) : -(bytes[1] | (bytes[2] << 8));
};

    (() => {
        let ws = new WebSocket(`ws://${location.host}:80/event`);
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

        function onClose() {
            console.log('Disconnected');
            location.reload();
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
        }

        // This is template for some event on some element
        function addEventTemplate(appId, event) {
            let element = getNode(appId);
            element.addEventListener(event, (e) => {
                lastPing = Date.now();
                // ws.send(JSON.stringify({
                //     event: event,
                //     "app-id": appId,
                //     data: e.target.value,
                // }));
                let bytes = new Uint8Array(request(event, appId, e.target.value));
                console.log(bytes);
                ws.send(bytes);
            });
        }

        window.app = {
            addEvent: addEventTemplate,
        };


        function request(event, appId, data) {
            if(data === undefined) {
                data = '';
            } else if (typeof data !== 'string') {
                data = String(data);
            }
            let bytes = [];

            // create header
            bytes = bytes.concat(event.length.toBytes());
            bytes = bytes.concat(data.length.toBytes());

            bytes = bytes.concat(event.toBytes(true));
            bytes = bytes.concat(appId.toBytes(true));
            bytes = bytes.concat(data.toBytes(true));
            return bytes;
        }

        // function parseRequest(bytes) {
        //     let header = {
        //         eventLength: Number.prototype.fromBytes(bytes.slice(0, 2)),
        //         dataLength: Number.prototype.fromBytes(bytes.slice(3, 5)),
        //     };
        //     let content = {
        //         event: String.prototype.fromBytes(bytes.slice(4), header.eventLength),
        //         appId: Number.prototype.fromBytes(bytes.slice(6 + header.eventLength)),
        //         data: String.prototype.fromBytes(bytes.slice(5 + header.eventLength + 2), header.dataLength),
        //     };
        //     return content;
        // }

    })();

/* protocol structure binary data
    JSON FORMAT:
    REQUEST:
    {
        "event": "event name",
        "app-id": "id of element",
        "data": "data of event",
    }
    RESPONSE:
    {
        "status": "status code",
        "data": {
            "update": [
                {
                    "app-id": "id of element",
                    "tag": "tag of element",
                    "attributes": {
                        "attribute": "value",
                    },
                    "innerText": "text",
                },
            ],
            "remove": [
                "id of element",
            ],
        },
    }

    BINARY FORMAT:
    REQUEST:
    ---Header---
    [0, 1] - event length
    [2, 3] - app-id length
    [4, 5] - data length
    ---Content---
    [6, 6 + event length] - event name
    [6 + event length, 6 + event length + app-id length] - app-id
    [6 + event length + app-id length, 6 + event length + app-id length + data length] - data
    RESPONSE:
    ---Header---
    [0, 1] - status code
    [2, 3] - update length
    [4, 5] - remove length
    ---Content---
    [6, 6 + update length] - update
    ---Update content format---
    [n, n + 1] - app-id length
    [n + 2, n + 3] - tag length
    [n + 4, n + 5] - attributes count
    [n + 6, n + 7] - innerText length
    [n + 8, n + 8 + app-id length] - app-id
    [n + 8 + app-id length, n + 8 + app-id length + tag length] - tag
    [n + 8 + app-id length + tag length, n + 8 + app-id length + tag length + attributes length] - attributes
    [n + 8 + app-id length + tag length + attributes length, n + 8 + app-id length + tag length + attributes length + innerText length] - innerText
    ---Attributes format---
    [a, a + 1] - attribute name length
    [a + 2, a + 3] - attribute value length
    [a + 4, a + 4 + attribute name length] - attribute name
    [a + 4 + attribute name length, a + 4 + attribute name length + attribute value length] - attribute value

    [6 + update length, 6 + update length + remove length] - remove
    ---Remove content format---
    [r, r + 1] - app-id length
    [r + 2, r + 2 + app-id length] - app-id
*/

function response(status, update, remove) {
    let bytes = [];
    // create header
    bytes = bytes.concat(status.toBytes());
    bytes = bytes.concat(update.length.toBytes());
    bytes = bytes.concat(remove.length.toBytes());

    for (let item of update) {
        bytes = bytes.concat(item.tag.length.toBytes());
        bytes = bytes.concat(item.innerText.length.toBytes());
        bytes = bytes.concat(Object.keys(item.attributes).length.toBytes());

        bytes = bytes.concat(item.appId.toBytes(true));
        bytes = bytes.concat(item.tag.toBytes(true));
        bytes = bytes.concat(item.innerText.toBytes(true));

        for (let key in item.attributes) {
            bytes = bytes.concat(key.length.toBytes());
            bytes = bytes.concat(String(item.attributes[key]).length.toBytes());
            bytes = bytes.concat(key.toBytes(true));
            bytes = bytes.concat(String(item.attributes[key]).toBytes(true));
        }
    }

    for (let item of remove) {
        bytes = bytes.concat(item.toBytes());
    }
    return bytes;
}

function parseResponse(bytes) {
    console.log(bytes);
    // Number - 3 bytes
    let header = {
        status: Number.prototype.fromBytes(bytes.slice(0, 3)),
        updateLength: Number.prototype.fromBytes(bytes.slice(3, 6)),
        removeLength: Number.prototype.fromBytes(bytes.slice(6, 9)),
    };
    let content = {
        update: [],
        remove: [],
        status: header.status,
    };
    let offset = 9;
    for (let i = 0; i < header.updateLength; i++) {
        let item = {
            header: {
                tagLength: Number.prototype.fromBytes(bytes.slice(offset, offset + 3)),
                innerTextLength: Number.prototype.fromBytes(bytes.slice(offset + 3, offset + 6)),
                attributesCount: Number.prototype.fromBytes(bytes.slice(offset + 6, offset + 9)),
            },
            content: {
                attributes: {},
                innerText: '',
                appId: 0,
                tag: '',
            },
        };
        offset += 9;
        item.content.appId = Number.prototype.fromBytes(bytes.slice(offset, offset + 3));
        offset += 2;
        item.content.tag = String.prototype.fromBytes(bytes.slice(offset - 1), item.header.tagLength);
        offset += item.header.tagLength;
        item.content.innerText = String.prototype.fromBytes(bytes.slice(offset - 1), item.header.innerTextLength);
        offset += item.header.innerTextLength + 1;

        for (let j = 0; j < item.header.attributesCount; j++) {
            let attribute = {
                header: {
                    nameLength: Number.prototype.fromBytes(bytes.slice(offset, offset + 3)),
                    valueLength: Number.prototype.fromBytes(bytes.slice(offset + 3, offset + 6)),
                },
                content: {
                    name: '',
                    value: '',
                },
            };
            offset += 5;
            attribute.content.name = String.prototype.fromBytes(bytes.slice(offset - 1), attribute.header.nameLength);
            offset += attribute.header.nameLength;
            attribute.content.value = String.prototype.fromBytes(bytes.slice(offset - 1), attribute.header.valueLength);
            offset += attribute.header.valueLength + 1;
            item.content.attributes[attribute.content.name] = attribute.content.value;
        }
        content.update.push(item.content);
    }

    for (let i = 0; i < header.removeLength; i++) {
        content.remove.push(Number.prototype.fromBytes(bytes.slice(offset, offset + 3)));
        offset += 3;
    }
    return content;
}
