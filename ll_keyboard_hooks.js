var express = require('express');

console.log('1');
module.exports = require('bindings')('ll_keyboard_hooks');

console.log('1');
module.exports((a) => {
  console.log(a);
});
console.log('1');

console.log('this works');
console.log('It keeps running');

setInterval(() => {
  console.log('ding');
}, 2000);

express().listen(3000);
