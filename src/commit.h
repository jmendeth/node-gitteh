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

#ifndef GITTEH_COMMIT_H
#define	GITTEH_COMMIT_H

#include "git2.h"

#include "v8u.hpp"

#include "object.h"

namespace gitteh {

class Commit : public GitObject {
public:
  Commit(git_commit* ptr);
  ~Commit();
  V8_SCTOR();

  static V8_SCB(Lookup); //static V8_SCB(LookupSync);

  NODE_STYPE(Commit);
protected:
  git_commit* const commit;
};

};

#endif	/* GITTEH_COMMIT_H */

