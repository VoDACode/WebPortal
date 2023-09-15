// (() => {
//     let esFunctions = {
//         eventSource: null,
//         connect: () => {
//             eventSource = new EventSource(`http://${location.host}:80/stream`);
//             eventSource.onerror = esFunctions.error;
//             eventSource.onopen = esFunctions.open;
//             eventSource.onclose = esFunctions.close;
//             eventSource.addEventListener('message', esFunctions.message);
//         },
//         message: (e) => {
//             onMessage(e.data);
//         },
//         error: (e) => {
//             console.log(e);
//             if (e.readyState == EventSource.CLOSED) {
//                 // try to reconnect
//                 setTimeout(() => {
//                     esFunctions.connect();
//                 }, 1000);
//             }
//         },
//         open: (e) => {
//             console.log(e);
//         },
//         close: (e) => {
//             // try to reconnect
//             setTimeout(() => {
//                 esFunctions.connect();
//             }, 1000);
//         }
//     };

//     esFunctions.connect();


//     function onMessage(msg) {
//         let response = JSON.parse(msg);
//         console.log(response);
//         let data = response.data;
//         if (response.status !== 200) {
//             return;
//         }
//         for (let item of data.update) {
//             updateNode(item);
//         }
//         for (let item of data.remove) {
//             removeNode(item);
//         }
//     }

//     function getNode(id) {
//         return document.querySelector(`[app-id="${id}"]`);
//     }

//     function removeNode(id) {
//         let node = getNode(id);
//         if (node) {
//             node.remove();
//         }
//     }

//     function updateNode(data) {
//         let node = getNode(data['app-id']);
//         if (!node) {
//             return;
//         }
//         // remove old attributes
//         while (node.attributes.length > 0) {
//             node.removeAttribute(node.attributes[0].name);
//         }
//         for (let key in data.attributes) {
//             node.setAttribute(key, data.attributes[key]);
//         }
//         if (data.innerText) {
//             node.innerText = data.innerText;
//         }
//         // set tag
//         if (data.tag && data.tag !== node.tagName) {
//             let newNode = document.createElement(data.tag);
//             for (let key in node.attributes) {
//                 newNode.setAttribute(key, node.attributes[key]);
//             }
//             newNode.innerText = node.innerText;
//             while (node.firstChild) {
//                 newNode.appendChild(node.firstChild);
//             }
//             node.replaceWith(newNode);
//         }
//     }

//     // This is template for some event on some element
//     function addEventTemplate(appId, event) {
//         let element = getNode(appId);
//         element.addEventListener(event, async (e) => {
//             if (event == 'input') {
//                 if (!element.inputStream) {

//                     element.inputStream = new WebSocket(`ws://${location.host}:80/event`);

//                     element.inputStream.onopen = () => {
//                         console.log('WebSocket opened');
//                         element.inputStreamInitialized = true;
//                         element.inputStream.send(JSON.stringify({
//                             event: event,
//                             'app-id': appId,
//                             data: e.target.value
//                         }));
//                     };
                    
//                     element.inputStream.onclose = () => {
//                         console.log('WebSocket closed');
//                         element.inputStreamInitialized = false;
//                         element.inputStream = null;
//                     };
//                 } else {
//                     while (!element.inputStreamInitialized) {
//                         await new Promise((resolve) => setTimeout(resolve, 100));
//                     }
//                     element.inputStream.send(JSON.stringify({
//                         event: event,
//                         'app-id': appId,
//                         data: e.target.value
//                     }));
//                 }
//             } else {
//                 let response = await fetch('/event', {
//                     method: 'PUT',
//                     body: JSON.stringify({
//                         event: event,
//                         'app-id': appId,
//                         data: e.target.value
//                     }),
//                 });
//             }
//         });
//     }

//     window.app = {
//         addEvent: addEventTemplate,
//     };
// })();