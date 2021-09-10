
let LocalFontsManager = {
  getFontsMetadata: () => { return []; },
  getFontData: () => { return null; }
};

if (process.platform === "darwin") {
  LocalFontsManager = require("./build/Release/MacFontsManager.node");
}
else if (process.platform === "linux") {
  LocalFontsManager = require("./build/Release/LinuxFontsManager.node");
}

module.exports = {
  getFontsMetadata() {
    return LocalFontsManager.getFontsMetadata();
  },

  getFontData(arg) {
    if (typeof arg === "string") {
      return LocalFontsManager.getFontData(arg).buffer;
    }
    else {
      return new ArrayBuffer();
    }
  }
};
