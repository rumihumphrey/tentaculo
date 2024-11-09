// Node.js code to run inside node.script

const http = require('http');
const Max = require('max-api');  // Import the max-api module

// Define the port
const PORT = 5002;

// Create the server
const server = http.createServer((req, res) => {
    if (req.method === 'POST' && req.url === '/chase') {
        let body = '';
        // Collect the data
        req.on('data', chunk => {
            body += chunk.toString();
        });

        // When all data is received
        req.on('end', () => {
            // Parse the JSON data
            try {
                const data = JSON.parse(body);

                // Send the data to Max outlet
                Max.outlet(data);  // Sends data.message to Max/MSP
                // Send response back to client
                res.writeHead(200, {'Content-Type': 'application/json'});
                res.end(JSON.stringify({status: 'Bang back'}));
            } catch (e) {
                res.writeHead(400, {'Content-Type': 'application/json'});
                res.end(JSON.stringify({status: 'Error', error: 'Invalid JSON'}));
            }
        });
    } else {
        res.writeHead(404, {'Content-Type': 'application/json'});
        res.end(JSON.stringify({status: 'Not Found'}));
    }
});

// Start the server
server.listen(PORT, () => {
    // Print to Max console
    Max.post(`Server is listening on port ${PORT}\n`);
});
