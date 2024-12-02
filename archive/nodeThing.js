// Node.js code to run inside node.script using UDP

const dgram = require('dgram');
const Max = require('max-api');  // Import the max-api module

// Define the port to listen for UDP messages
const PORT = 4210;

// Create a UDP socket
const server = dgram.createSocket('udp4');

// Handle incoming messages
server.on('message', (message, rinfo) => {
    try {
        // Parse the JSON data
        const data = JSON.parse(message);
        
        // Send the data to Max outlet
        Max.outlet(data);  // Sends data.message to Max/MSP
    } catch (e) {
        Max.post(`Error parsing UDP message: ${e.message}`);
    }
});

// Handle server listening event
server.on('listening', () => {
    const address = server.address();
    Max.post(`UDP server is listening on ${address.address}:${address.port}`);
});

// Handle server error event
server.on('error', (err) => {
    Max.post(`UDP server error: ${err.message}`);
    server.close();
});

// Start listening for UDP messages on the defined port
server.bind(PORT);
