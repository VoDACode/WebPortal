const express = require('express');
const app = express();
const port = 3300;

app.use(express.static('../src'));

app.listen(port, '10.0.1.20', () => console.log(`App listening on port ${port}!`));