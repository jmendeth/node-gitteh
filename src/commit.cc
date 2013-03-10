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

#include "commit.h"

#include "repository.h"
#include "common.h"
#include "error.h"
#include "oid.h"


using v8u::Int;
using v8u::Symbol;
using v8u::Bool;
using v8u::Func;
using v8u::Persist;
using v8::Object;
using v8::Local;
using v8::Persistent;
using v8::Function;

namespace gitteh {

Commit::Commit(git_commit* ptr): commit(ptr) {}
Commit::~Commit() {
  if (invalid) return;
  git_commit_free(commit);
}

V8_ESCTOR(Commit) { V8_CTOR_NO_JS }

// TODO: methods go here

// STATIC / FACTORY METHODS

//// Commit.lookup(...)

GITTEH_WORK_PRE(commit_lookup) {
  git_oid oid;
  Persistent<Object> repo;
  git_commit* out;
  error_info err;

  Persistent<Function> cb;
  uv_work_t req;
};

V8_SCB(Commit::Lookup) {
  Local<Object> repo_obj, oid_obj;
  if (!(args[0]->IsObject() && Repository::HasInstance(repo_obj = v8u::Obj(args[0]))))
    V8_STHROW(v8u::TypeErr("Repository needed as first argument."));
  if (!(args[1]->IsObject() && Oid::HasInstance(oid_obj = v8u::Obj(args[1]))))
    V8_STHROW(v8u::TypeErr("OID needed as second argument."));

  commit_lookup_req* r = new commit_lookup_req;
  r->repo = Persist(repo_obj);
  memcpy(r->oid.id, node::ObjectWrap::Unwrap<Oid>(oid_obj)->oid.id, GIT_OID_RAWSZ);

  r->cb = Persist(v8u::Cast<Function>(args[2]));
  GITTEH_WORK_QUEUE(commit_lookup);
} GITTEH_WORK(commit_lookup) {
  int status = git_commit_lookup(&r->out, node::ObjectWrap::Unwrap<Repository>(r->repo)->repo, &r->oid);
  r->repo.Dispose();
  if (status == GIT_OK) return;
  collectErr(status, r->err);
  r->out = NULL;
} GITTEH_WORK_AFTER(commit_lookup) {
  v8::Handle<v8::Value> argv [2];
  if (r->out) {
    argv[0] = v8::Null();
    argv[1] = (new Commit(r->out))->Wrapped();
  } else {
    argv[0] = composeErr(r->err);
    argv[1] = v8::Null();
  }
  GITTEH_WORK_CALL(2);
} GITTEH_END



NODE_ETYPE(Commit, "Commit") {
  //TODO
  
  Local<Function> func = templ->GetFunction();
  
  func->Set(Symbol("lookup"), Func(Lookup)->GetFunction());
//  func->Set(Symbol("lookupSync"), Func(LookupSync)->GetFunction());
  
} NODE_TYPE_END()

V8_POST_TYPE(Commit)

};

