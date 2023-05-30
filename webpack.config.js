const path = require('path');
const { VueLoaderPlugin } = require('vue-loader');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');

module.exports = (env, argv) => {
  const isOnProduction = argv.mode === 'production';

  return {
    entry: path.resolve(__dirname, 'src', 'main.ts'),
    output: {
      filename: 'index.js',
      path: path.resolve(__dirname, 'dist'),
      clean: true,
    },
    plugins: [
      new VueLoaderPlugin(),
      new HtmlWebpackPlugin({
        template: path.resolve(__dirname, 'public', 'index.html'),
      }),
      ...(isOnProduction ? [new MiniCssExtractPlugin()] : []),
    ],
    module: {
      rules: [
        {
          test: /\.jsx?$/,
          exclude: /node_modules/,
          use: [
            {
              loader: 'babel-loader',
              options: {
                presets: [['@babel/preset-env', { targets: 'defaults' }]],
              },
            },
          ],
        },
        {
          test: /\.ts$/,
          exclude: /node_modules/,
          use: [
            {
              loader: 'ts-loader',
              options: {
                appendTsSuffixTo: [/\.vue$/],
              },
            },
          ],
        },
        {
          test: /\.vue$/,
          exclude: /node_modules/,
          use: ['vue-loader'],
        },
        {
          test: /\.s?(c|a)ss$/,
          use: [
            isOnProduction ? MiniCssExtractPlugin.loader : 'vue-style-loader',
            'css-loader',
            {
              loader: 'sass-loader',
              options: {
                additionalData: `@import 'bulma/sass/utilities/mixins.sass';`,
              },
            },
          ],
        },
        {
          test: /\.(woff|woff2|eot|ttf|otf|wasm)$/i,
          type: 'asset',
        },
        {
          test: /\.asm$/i,
          type: 'asset/source',
        },
      ],
    },
    resolve: {
      extensions: ['.ts', '.tsx', '.js', '.jsx'],
      alias: {
        '@': path.resolve(__dirname, 'src'),
      },
      symlinks: false,
    },
  };
};
