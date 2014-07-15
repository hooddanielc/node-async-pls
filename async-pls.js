var addon = require('./build/Release/asyncpls');

console.log(addon.lol(function () { console.log(arguments); }));