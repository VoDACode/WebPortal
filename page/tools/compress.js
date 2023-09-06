const fs = require('fs');
const path = require('path');
const uglifyJS = require('uglify-js');
const CleanCSS = require('clean-css');
const htmlMinifier = require('html-minifier');

let src = path.join(__dirname, '..', 'src');
if (!fs.existsSync(src)) throw new Error('Source directory not found!');

let dist = path.join(__dirname, '..', 'dist');
if (fs.existsSync(dist)) fs.rmSync(dist, { recursive: true, force: true });
fs.mkdirSync(dist);

let html = fs.readFileSync(path.join(src, 'index.html'), 'utf-8');
let js = fs.readFileSync(path.join(src, 'index.js'), 'utf-8');
let css = fs.readFileSync(path.join(src, 'index.css'), 'utf-8');

css = new CleanCSS().minify(css).styles;

js = uglifyJS.minify(js).code;
js = js.replace(/fetch/g, '$f');
js = js.replace(/document.getElementById/g, '$dgid');
js = js.replace(/document.querySelectorAll/g, '$dgqa');
js = js.replace(/document.getElementsByClassName/g, '$dgcns');
js = js.replace(/document/g, '$d');
js = '$d=document;$dgid=(e)=>$d.getElementById(e);$dgqa=(e)=>$d.querySelectorAll(e);$dgcns=(e)=>$d.getElementsByClassName(e);function $f(){return fetch(...arguments)}' + js;

html = html.replace(/<script src="index.js"><\/script>/, `<script>${js}</script>`);
html = html.replace(/<link rel="stylesheet" href="index.css">/, `<style>${css}</style>`);

let ids = html.match(/id="[^"]+"/g);
let iter = 0;
ids.forEach(element => {
    let id = element.replace('id="', '').replace('"', '');

    let regex = new RegExp(`\\$dgid\\(["|']${id}["|']\\)`, 'g');
    html = html.replace(regex, `$dgid("_id${iter}")`);
    regex = new RegExp(`["|']${id}["|']`, 'g');
    html = html.replace(regex, `"_id${iter}"`);
    regex = new RegExp(`id="${id}"`, 'g');
    html = html.replace(regex, `id="_id${iter}"`);
    regex = new RegExp(`for="${id}"`, 'g');
    html = html.replace(regex, `for="_id${iter}"`);
    regex = new RegExp(`#${id}`, 'g');
    html = html.replace(regex, `#_id${iter}`);
    iter++;
});

// TO DO: replace all classes with _cl0, _cl1, _cl2, ...

html = html.replace(/>\s+</g, '><');
html = htmlMinifier.minify(html, {
    collapseWhitespace: true,
    removeComments: true,
    removeAttributeQuotes: true
});

// get all ids in html

if(process.argv.includes('cpp')){
    html = html.replace(/\\/g, '\\\\');
    // replace all " with \"
    html = html.replace(/"/g, '\\"');
    // replace all ' with \'
    html = html.replace(/'/g, "\\'");
}

fs.writeFileSync(path.join(dist, 'index.html'), html);


function replaceClasses(html) {
    let classMap = new Map();
    let classIndex = 0;
  
    // знаходимо всі класи в html
    let classRegex = /\bclass\s*=\s*"([^"]*)"/g;
    let match;
    while ((match = classRegex.exec(html)) !== null) {
      let classes = match[1].split(/\s+/);
      for (let className of classes) {
        if (!classMap.has(className)) {
          classMap.set(className, `_cl${classIndex++}`);
        }
      }
    }
  
    // замінюємо класи в html
    for (let [oldClass, newClass] of classMap) {
      let classRegex = new RegExp(`([^-\\w])${oldClass}([^-\\w])`, 'g');
      html = html.replace(classRegex, `$1${newClass}$2`);
    }
  
    // знаходимо всі класи в CSS
    let cssClassRegex = /\.([\w-]+)/g;
    let cssMatches;
    let cssClasses = new Set();
    while ((cssMatches = cssClassRegex.exec(html)) !== null) {
      cssClasses.add(cssMatches[1]);
    }
  
    // замінюємо класи в CSS
    for (let [oldClass, newClass] of classMap) {
      if (cssClasses.has(oldClass)) {
        let cssRegex = new RegExp(`\\.${oldClass}(?!-)`, 'g');
        html = html.replace(cssRegex, `.${newClass}`);
      }
    }
  
    return html;
  }
  