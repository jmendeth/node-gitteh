/*
 * The MIT License
 *
 * Copyright (c) 2010 Sam Day
 * Copyright (c) 2012 Xavier Mendez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "message.h"

#include "git2.h"

#include "error.h"


namespace gitteh {

// another approach would be to call prettify() with no buffer,
// allocate the size and call again, but this is faster
V8_SCB(Prettify) {
  
  // Allocate
  v8::String::Utf8Value msg (args[0]);
  int len = msg.length();
  char* out = new char [len += 2]; // trailing \n and \0

  // Call & return
  error_info info;
  int out_len = git_message_prettify(out, len, *msg, v8u::Bool(args[1]));
  if (out_len == -1) {
    collectErr(-1, info);
    delete [] out;
    V8_STHROW(composeErr(info));
  }
  v8::Local<v8::String> ret = v8u::Str(out, out_len-1);
  delete [] out;
  return ret;

}

};

