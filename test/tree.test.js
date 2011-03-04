var vows = require("vows"),
	assert = require("assert"),
	gitteh = require("../build/default/gitteh"),
	path = require("path"),
	fixtureValues = require("./fixtures/values"),
	helpers = require("./fixtures/helpers.js");

var DIRECTORY_ATTRIBUTE = helpers.fromOctal(40000);

var repo = new gitteh.Repository(fixtureValues.REPO_PATH);

var createTreeTestContext = function(treeFixture) {
	var context = {
		topic: function() {
			return repo.getTree(treeFixture.id);
		},
		
		"gives us a Tree": function(tree) {
			assert.isTrue(!!tree);
		},
		
		"with correct id": function(tree) {
			assert.equal(tree.id, treeFixture.id);
		}
	};
	
	// Run assertions on the contents of tree.
	var createEntriesChecks = function(entriesFixture, path) {
		var entriesContext = {
			topic: function(tree) {
				return tree.entries;
			},
			
			"has correct number of entries": function(entries) {
				assert.length(entries, entriesFixture.length);
			}
		};
		
		context[(path == "") ? "- root entries" : ("- tree " + path)] = entriesContext;

		for(var i = 0; i < entriesFixture.length; i++) {
			entriesContext["- entry " + entriesFixture[i].filename] = (function(i) {
				var theContext = {
					topic: function(entries) {
						return entries[i];
					},
					
					"has correct name": function(entry) {
						assert.equal(entry.filename, entriesFixture[i].filename);
					},
					
					"has correct attributes": function(entry) {
						assert.equal(helpers.toOctal(entry.attributes), entriesFixture[i].attributes);
					},
					
					"has correct id": function(entry) {
						assert.equal(entry.id, entriesFixture[i].id);
					}
				};
				
				// We're dealing with a folder here. Let's recurse into it and check it out.
				if(entriesFixture[i].attributes == DIRECTORY_ATTRIBUTE) {
					createEntriesChecks(entriesFixture[i].entries, path + "/" + entriesFixture[i].filename);
				}
				
				return theContext;				
			})(i);
		}
		
		
	};
	
	createEntriesChecks(treeFixture.entries, "");
	
	return context;
}

vows.describe("Tree").addBatch({
	"First tree": createTreeTestContext(fixtureValues.FIRST_TREE),
	"Second tree": createTreeTestContext(fixtureValues.SECOND_TREE),
	"Third tree": createTreeTestContext(fixtureValues.THIRD_TREE),
	"Fourth tree": createTreeTestContext(fixtureValues.FOURTH_TREE),
	"Fifth tree": createTreeTestContext(fixtureValues.FIFTH_TREE),
	
	"Retrieving tree entries by name": {
		topic: function() {
			var tree = repo.getTree(fixtureValues.FIRST_TREE.id);
			this.context.tree = tree;
			return tree.getByName(fixtureValues.FIRST_TREE.entries[0].filename);
		},
		
		"gives us the correct entry": function(entry) {
			assert.equal(entry.filename, fixtureValues.FIRST_TREE.entries[0].filename);
			assert.equal(entry.id, fixtureValues.FIRST_TREE.entries[0].id);	
		},
		
		"identical to getting it via index": function(entry) {
			assert.isTrue(entry === this.context.tree.entries[0]);
		}
	},
	
	"Loading a non-existent tree": {
		topic: function() {
			return repo.getTree(helpers.getSHA1("foo"));
		},
		
		"gives us null": function(tree) {
			assert.isNull(tree);
		}
	},
	
	"Loading a tree that is actually a commit": {
		topic: function() {
			return repo.getTree(fixtureValues.FIRST_COMMIT.id);
		},
		
		"gives us null": function(tree) {
			assert.isNull(tree);
		}
	},
	
	"Looking up a non-existant tree entry": {
		topic: function() {
			var tree = repo.getTree(fixtureValues.FIRST_TREE.id);
			return tree.getByName("foo.bar.i.dont.exist");
		},
		
		"gives us null": function(entry) {
			assert.isNull(entry);
		}
	},
	
	"Looking up an entry out of bounds": {
		topic: function() {
			var tree = repo.getTree(fixtureValues.FIRST_TREE.id);
			
			return function() {
				return tree.entries[fixtureValues.FIRST_TREE.entries.length];
			};
		},
		
		"gives us undefined": function(fn) {
			assert.doesNotThrow(fn, Error);
			assert.isUndefined(fn());
		}
	},
	
	"Creating a new Tree": {
		topic: function() {
			return repo.createTree();
		},
		
		"gives us a new Tree": function(tree) {
			assert.isTrue(!!tree);
		},
		
		"with correct identity": function(tree) {
			assert.isNull(tree.id);
			assert.length(tree.entries, 0);
		},
		
		"- adding an invalid entry": function(tree) {
			assert.throws(function() {
				tree.addEntry();
			}, Error);
		},
		
		"- adding an entry": {
			topic: function(tree) {
				tree.addEntry(fixtureValues.EMPTY_BLOB, "test", helpers.fromOctal(100644));
				return tree;
			},
			
			"adds to tree *entries* correctly": function(tree) {
				assert.length(tree.entries, 1);
			},
			
			"entry has correct values": function(tree) {
				assert.equal(tree.entries[0].id, fixtureValues.EMPTY_BLOB);
				assert.equal(tree.entries[0].attributes, helpers.fromOctal(100644));
				assert.equal(tree.entries[0].filename, "test");
			},
			
			"- saving": {
				topic: function(tree) {
					tree.save();
					return tree;
				},
				
				"updates id correctly": function(tree) {
					assert.equal(tree.id, "f05af273ba36fe5176e5eaab349661a56b3d27a0");
				},
				
				"this tree is now available from Repository": function(tree) {
					assert.isTrue(tree === repo.getTree("f05af273ba36fe5176e5eaab349661a56b3d27a0"));
				},
				
				"- then deleting entry again": {
					topic: function(tree) {
						this.context.tree = tree;
						return function() {
							tree.removeEntry(0);
						};
					},
					
					"works correctly": function(fn) {
						assert.doesNotThrow(fn, Error);
					},
					
					"results in an empty tree": function(fn) {
						assert.length(this.context.tree.entries, 0);
					},

					"which cannot be saved": function() {
						var t = this;

						assert.throws(function() {
							t.context.tree.save();
						}, Error);
					}
				}
			}
		}
	},
	
	"Creating a new Tree and trying to save it with no entries": {
		topic: function() {
			var tree = repo.createTree();
			
			return function() {
				tree.save();
			};
		},

		"throws an Error": function(fn) {
			assert.throws(fn, Error);
		}
	}
}).export(module);