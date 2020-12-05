const path = require('path');

//TODO: implement proper builing for HTML and JS code

module.exports = {
  //...
  entry:
  [
    path.join(__dirname, './data')
  ],
  devServer: {
    contentBase: path.join(__dirname, 'data')
  }
};