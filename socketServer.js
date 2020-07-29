var http = require('http');  
const WebSocket = require('ws');
var url = require('url');  
var fs = require('fs');  
var path = require('path');
var beverageToVend;
var wss;



var server = http.createServer(function(request, response) {  
try {
    var pathValue = url.parse(request.url).pathname;  
    switch (pathValue) {  


		case '/Vend.do':
		console.log('received call for vend');
		var bevParam = url.parse(request.url,true).query;
		console.log(bevParam.b);
		beverageToVend = bevParam.b;

		response.writeHead(200, {  
                        'Content-Type': 'text/html'  
                    });  
        response.write("Vending in Progress :" + beverageToVend);  
        response.end(); 
		console.log('end of received call for vend');
		break;

       default:  
var filePath = '.' + pathValue;
console.log("path:"+pathValue);
    if (filePath == './')
        filePath = './index.html';

    var extname = path.extname(filePath);
    var contentType = 'text/html';
    switch (extname) {
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
            contentType = 'image/jpg';
            break;
        case '.wav':
            contentType = 'audio/wav';
            break;
    }
    console.log("received file call with ext:"+contentType+": filename:"+filePath);
    fs.readFile(filePath, function(error, content) {
        if (error) {
            if(error.code == 'ENOENT'){
                fs.readFile('./404.html', function(error, content) {
                    response.writeHead(200, { 'Content-Type': contentType });
                    response.end(content, 'utf-8');
                });
            }
            else {
                response.writeHead(500);
                response.end('Sorry, check with the site admin for error: '+error.code+' ..\n');
                response.end(); 
            }
        }
        else {
            response.writeHead(200, { 'Content-Type': contentType });
            response.end(content, 'utf-8');
        }
    });
	break;  

    } 
}catch(e){
	//ignore for now
}	
});  
wss = new WebSocket.Server({ server: server });

wss.on('connection', function connection(ws) {
  ws.on('message', function incoming(message) {
    console.log('received: %s', message);
    ws.send(beverageToVend);
    beverageToVend = "";
  });
  //ws.close();
});

server.listen(8082);  