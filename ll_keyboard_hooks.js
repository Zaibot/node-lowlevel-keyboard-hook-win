'use strict';

if (process.platform !== 'win32') return;

const hookKeyboard = require('bindings')('ll_keyboard_hooks');

let hooks = {};

hookKeyboard((data) => {
  const keyCode = data.toString().trim().replace(/\n/g, '');
  if (hooks[keyCode]) {
    hooks[keyCode].forEach((fn) => fn());
  }
});

const KEY_NAMES = {
  'Control': [162, 163],
  'Shift': [160, 161],
  'MediaPlayPause': [179],
  'MediaStop': [178],
  'MediaPreviousTrack': [177],
  'MediaNextTrack': [176]
};

for (let F = 1; F <= 24; F++) {
  KEY_NAMES[`F${F}`] = 112 + F - 1;
}

const _hook = (code, fn) => {
  hooks[code] = hooks[code] || [];
  hooks[code].push(fn);
}

module.exports = {
  on: (keyCode, fn) => {
    if (KEY_NAMES[keyCode]) {
      let codes = KEY_NAMES[keyCode];
      codes.forEach((code) => _hook(code, fn));
    } else {
      _hook(keyCode, fn);
    }
  },
  unbind: () => {
    hooks = {};
    return true;
  }
};
