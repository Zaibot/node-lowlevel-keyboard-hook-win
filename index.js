var hid = require('node-hid');


hid.devices().forEach((device) => {
  console.log('Open');
  var stream = new hid.HID(device.path);
  stream.on('data', (data) => console.log(data));
});
