const http = require('http');
const WebSocket = require('ws');
const url = require('url');
const fs = require('fs');

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
            fs.readFile('./index.html', (err, data) => {
                if (err) {
                    res.writeHead(404, { 'Content-Type': 'text/html' });
                    return res.end("404 Not Found");
                }
                res.writeHead(200, { 'Content-Type': 'text/html' });
                data = data.toString();
                res.write(data.replace('<IP_HERE>', '192.168.1.19'));
                return res.end();
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
