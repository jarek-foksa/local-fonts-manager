
let SystemFontsManager = {
  getFontsMetadata: () => { return []; },
  getFontData: () => { return null; }
};

if (process.platform === "darwin") {
  SystemFontsManager = require("./build/Release/MacFontsManager.node");
}
else if (process.platform === "linux") {
  SystemFontsManager = require("./build/Release/LinuxFontsManager.node");
}

module.exports = {
  getFontsMetadata() {
    return SystemFontsManager.getFontsMetadata();
  },

  getFontData(arg) {
    if (typeof arg === "string") {
      return SystemFontsManager.getFontData(arg).buffer;
    }
    else {
      return new ArrayBuffer();
    }
  }
};
