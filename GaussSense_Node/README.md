#GaussSense Node.js Socket.io Example

傳送 GaussSense 的 Arduino mode 感測值與運算值至瀏覽器，讓開發者可以直接開發瀏覽器上的互動網頁。

## 使用方法
1. 安裝 Node.js (> v4.1.1)
2. 在此資料夾中執行 ```npm install```
4. 修改 index.js 中 serial port path
5. 在此資料夾中執行 ```node index``` 開啟 server
6. Arduino Leonardo 上傳 ```arduino_firmware/Mini_GaussSense_V2BasicBipolarPoint```
7. 連結 Mini GaussSense
8. 打開瀏覽器，連結 http://localhost:5000/

## Libraries
- [node-serialport](https://github.com/voodootikigod/node-serialport): Node.js package to access serial ports for reading and writing
- [express.js](http://expressjs.com): Minimalist web framework for Node.js
- [socket.io](http://socket.io): JavaScript library for realtime web applications
- [three.js](http://threejs.org): JavaScript 3D library





