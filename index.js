'use strict';

let hooks = {};

if (process.platform === 'win32') {
  const child = require('child_process').execFile('Release/LLKeyboardHook.exe', [], {
      detached: true,
      stdio: [ 'ignore', 1, 2 ],
  });
  child.unref();
  child.stdout.on('data', function(data) {
      const keyCode = data.toString().trim().replace(/\n/g, '');
      if (hooks[keyCode]) {
        hooks[keyCode].forEach((fn) => fn());
      }
  });
} else {
  console.warn('LL Keyboard Hook Win only works on Windows');
}

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

module.exports = {
  on: (keyCode, fn) => {
    if (KEY_NAMES[keyCode]) {
      let codes = KEY_NAMES[keyCode];
      codes.forEach(() => module.exports._hook(code, fn));
    } else {
      this._hook(keyCode, fn);
    }
  },
  _hook: (code, fn) => {
    hooks[code] = hooks[code] || [];
    hooks[code].push(fn);
  },
  unbind: () => {
    hooks = {};
    return true;
  }
};
