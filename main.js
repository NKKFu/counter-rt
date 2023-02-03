const http = require('http');
const WebSocket = require('ws');
const url = require('url');
const fs = require('fs');
const path = require('path');

const allHosts = {};
const httpServer = http.createServer((req, res) => {
    if (req.method === 'GET') {
        fs.appendFile('access.log', `[${req.method}] ${req.url} - ${new Date()}\n`, (err) => {
            if (err) throw err;
        });

        if (req.url.startsWith('/ping')) {
            const query = url.parse(req.url, true).query;
            const message = {
                id: query.id,
                qty: query.qty
            };

            fs.appendFile('hosts.log', `[${new Date()}] ${JSON.stringify(query)} \n`, (err) => {
                if (err) throw err;
            });
    
            allHosts[query.id] = {
                id: query.id,
                qty: query.qty,
                lastPing: Date.now()
            };

            wss.clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify([allHosts[query.id]]));
                }
            });
            res.end();
        } else if (req.url === '/') {
            fs.readFile('./index.html', 'utf8', (err, data) => {
                if (err) {
                    res.writeHead(404, { 'Content-Type': 'text/html' });
                    return res.end("404 Not Found");
                }
                res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
                data = data.toString();
                res.write(data.replace('<IP_HERE>', '192.168.125.53'));
                return res.end();
            });
        } else if (req.url.startsWith('/static/')) {
            // Serve static file
            const filePath = path.join(__dirname, req.url);
            const fileExt = path.extname(filePath);
            let contentType = 'text/html';

            switch (fileExt) {
                case '.js':
                    contentType = 'text/javascript';
                    break;
                case '.css':
                    contentType = 'text/css';
                    break;
                case '.json':
                    contentType = 'application/json';
                    break;
                case '.png':
                    contentType = 'image/png';
                    break;      
                case '.jpg':
                case '.jpeg':
                    contentType = 'image/jpg';
                    break;
            }
            fs.readFile(filePath, (err, data) => {
                if (err) {
                  res.writeHead(404, {'Content-Type': 'text/html'});
                  res.end("<h1>404 Not Found</h1>");
                } else {
                  res.writeHead(200, {'Content-Type': contentType });
                  res.end(data);
                }
            });
        }
    } else {
        res.writeHead(404, { 'Content-Type': 'text/html' });
        return res.end("404 Not Found");
    }
});

httpServer.listen(8000, '0.0.0.0', () => {
    console.log('HTTP server is running on port 8000');
});

const wss = new WebSocket.Server({ port: 8001, host: '0.0.0.0' });

wss.on('connection', (ws) => {
    // Send allhosts in the form of a array
    ws.send(JSON.stringify(Object.values(allHosts)));
    // Handle WebSocket connections
});
console.log('WebSockets server is running on port 8001');
