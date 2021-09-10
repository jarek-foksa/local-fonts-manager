
# Local Fonts Manager

Native module for Node.js and Electron to access the metadata and binary data of local system fonts.

## Usage

```js
let LocalFontsManager = require("local-fonts-manager");

let fontsMetadata = LocalFontsManager.getFontsMetadata();
let verdanaFontData = LocalFontsManager.getFontData("Verdana-Bold")

```
