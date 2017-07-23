'use strict';

if (process.platform !== 'win32') return;

const hookKeyboard = require('bindings')('ll_keyboard_hooks');

const getMode = (modes) => {
  return modes.split(',').reduce((state, cur) => {
    if (cur === 'up') {
      return state | 0x01;
    } else if (cur === 'down') {
      return state | 0x02;
    } else {
      throw Error(`expects 'up' and/or 'down' splitted by a comma`);
    }
  }, 0)
}

module.exports = {
  on: (modes, fn) => {
    hookKeyboard.run(getMode(modes), (eventData) => {
      const parts = eventData.split('::');
      fn(parts[0], parts[1]);
    });
  },
  stop: () => {
    hookKeyboard.stop();
  }
};
