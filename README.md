
# System Fonts Manager

Native module for Node.js and Electron to access the metadata and binary data of system fonts.

## Usage

```js
let SystemFontsManager = require("system-fonts-manager");

let fontsMetadata = SystemFontsManager.getFontsMetadata();
let verdanaFontData = SystemFontsManager.getFontData("Verdana-Bold")

```
