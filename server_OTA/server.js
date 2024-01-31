const express = require('express');
const multer = require('multer');
const path = require('path');
const fs = require('fs');

const app = express();
const port = 8000;

// Tạo thư mục "uploads" nếu chưa tồn tại
const uploadDir = path.join(__dirname, 'uploads');
if (!fs.existsSync(uploadDir)) {
  fs.mkdirSync(uploadDir);
}

// Set up storage for uploaded files
const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, 'uploads/')
  },
  filename: function (req, file, cb) {
    cb(null, 'firmware.bin')
  }
})

const upload = multer({ storage: storage });

// Serve the "index.html" page
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'views', 'index.html'));
});

app.get('/assets/styleIndex.css',(req, res) => {
  res.sendFile(path.join(__dirname, 'assets', 'styleIndex.css'));
});

app.get('/assets/img/background_web.png',(req, res) => {
  res.sendFile(path.join(__dirname, 'assets', 'img', 'background_web.png'));
});

app.get('/assets/img/logo.png',(req, res) => {
  res.sendFile(path.join(__dirname, 'assets', 'img', 'logo.png'));
});

// Serve the "signIn.html" page with query parameter
app.get('/signIn.html', (req, res) => {
  const code = req.query.code;
  res.sendFile(path.join(__dirname, 'views', 'signIn.html'), { code });
});

app.get('/signOut.html', (req, res) => {
  const code = req.query.code;
  res.sendFile(path.join(__dirname, 'views', 'signOut.html'), { code });
});

// Xử lý việc tải lên file
app.post('/upload', upload.single('file'), (req, res) => {
  const fileUrl = `http://localhost:8000/firmware.bin`;
  res.send(`File is Upload success.`);
});

// Đích đến file thông qua URL
app.get('/firmware.bin', (req, res) => {
  res.sendFile(__dirname + '/uploads/firmware.bin');
});

app.listen(8000, () => {
  console.log('Server is runninng at http://localhost:8000/');
});