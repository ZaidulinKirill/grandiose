const bindings = require('bindings')('ndi');

const ndi = bindings('ndi')
module.exports = {
  find: find,
};
