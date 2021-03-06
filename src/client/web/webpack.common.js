const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const { CleanWebpackPlugin } = require('clean-webpack-plugin');

module.exports = {
  entry: path.resolve(__dirname, './src/index.js'),
  output: {
    path: path.resolve(__dirname, './dist'),
    filename: 'bundle.js'
  },
  plugins: [
	new CleanWebpackPlugin(),
  	new HtmlWebpackPlugin( { template: "./src/index.html",
      filename: "./index.html" }),
  ],
  module: {
    rules: [
      {
        test: /\.(js)$/,
        exclude: /node_modules/,
        use: ['babel-loader']
      },
	 {
        test: /\.css$/i,
        use: ['style-loader', 'css-loader'],
      }
	]
  }
};
