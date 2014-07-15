var addon = require('./build/Release/async-pls.node');

console.log(addon.lol(function () { console.log(arguments); }));