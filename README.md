Low Level Keyboard Hooks
=========================

**NOTE:** This is for windows only

We use this module to get better media key bindings on windows in [GPMDP](https://github.com/MarshallOfSound/Google-Play-Music-Desktop-Player-UNOFFICIAL-)

## Usage

Requiring the module returns an object with two methods

### on(keyEvent, keyCode, fn)
This will call `fn()` every time the key with key code `keyCode` performed whatever action `keyEvent` is.

#### Possible Values for keyEvent
| `"up"` | `"down"` | `"*"` |
|--------|----------|-------|
| Fires whenever that key is released | Fires whenever that key is pressed down, this event fires multiple times for held keys | Fires for any key event (up or down) |

**NOTE:** `keyCode` has to be a [vkCode](http://www.kbdedit.com/manual/low_level_vk_list.html) or `"*"`

### unbind()
This removes all keybindings

The MIT License (MIT)
Copyright (c) 2016 Samuel Attard

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
