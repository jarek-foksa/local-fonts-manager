#include <napi.h>
#include <fstream>
#include <iterator>
#include <vector>
#include <fontconfig/fontconfig.h>

static const FcChar8 *toFcChar8(const char *charPtr) {
  return reinterpret_cast<const FcChar8*>(charPtr);
}

Napi::Array getFontsMetadata(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Array fonts = Napi::Array::New(env);

  int fontsCounter = 0;

  FcConfig *config = FcInitLoadConfigAndFonts();
  FcPattern *pattern = FcPatternCreate();

  FcObjectSet *objectSet = FcObjectSetBuild (
    FC_FAMILY,
    FC_FULLNAME,
    FC_POSTSCRIPT_NAME,
    FC_FILE,
    FC_STYLE,
    (char *) 0
  );

  FcFontSet *fontSet = FcFontList(config, pattern, objectSet);

  for (int i = 0; fontSet && i < fontSet->nfont; ++i) {
    FcPattern* font = fontSet->fonts[i];
    FcChar8 *family, *fullName, *postscriptName, *file, *style;
    int weight = 400;
    bool italic = false;

    // https://www.xfree86.org/current/fontconfig.3.html
    // https://www.freedesktop.org/software/fontconfig/fontconfig-devel/fcpatternget-type.html
    if (
      FcPatternGetString(font,  FC_FAMILY,           0, &family)          == FcResultMatch &&
      FcPatternGetString(font,  FC_FULLNAME,         0, &fullName)        == FcResultMatch &&
      FcPatternGetString(font,  FC_POSTSCRIPT_NAME,  0, &postscriptName)  == FcResultMatch &&
      FcPatternGetString(font,  FC_FILE,             0, &file)            == FcResultMatch &&
      FcPatternGetString(font,  FC_STYLE,            0, &style)           == FcResultMatch
    ) {
      if (FcStrStr(style, toFcChar8("Italic")) || FcStrStr(style, toFcChar8("Oblique"))) {
        italic = true;
      }

      if (FcStrStr(style, toFcChar8("Thin")) || FcStrStr(style, toFcChar8("Hairline"))) {
        weight = 100;
      }
      else if (FcStrStr(style, toFcChar8("Extra Light")) || FcStrStr(style, toFcChar8("Ultra Light"))) {
        weight = 200;
      }
      else if (FcStrStr(style, toFcChar8("Light"))) {
        weight = 300;
      }
      else if (FcStrStr(style, toFcChar8("Regular")) || FcStrStr(style, toFcChar8("Normal"))) {
        weight = 400;
      }
      else if (FcStrStr(style, toFcChar8("Medium"))) {
        weight = 500;
      }
      else if (FcStrStr(style, toFcChar8("Semi Bold")) || FcStrStr(style, toFcChar8("Demi Bold"))) {
        weight = 600;
      }
      else if (FcStrStr(style, toFcChar8("Bold"))) {
        weight = 700;
      }
      else if (FcStrStr(style, toFcChar8("Extra Bold")) || FcStrStr(style, toFcChar8("Ultra Bold"))) {
        weight = 800;
      }
      else if (FcStrStr(style, toFcChar8("Black")) || FcStrStr(style, toFcChar8("Heavy"))) {
        weight = 900;
      }

      Napi::Object data = Napi::Object::New(env);
      data.Set("family", (char*)family);
      data.Set("postscriptName", (char*)postscriptName);
      data.Set("style", (char*)style);
      data.Set("fullName", (char*)fullName);
      data.Set("weight", weight);
      data.Set("italic", italic);
      data.Set("stretch", 1);

      fonts[fontsCounter] = data;
      fontsCounter += 1;
    }
  }

  if (fontSet) {
    FcFontSetDestroy(fontSet);
  }

  return fonts;
}

Napi::Buffer<uint8_t> getFontData(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  std::string queryPostscriptName = info[0].As<Napi::String>().Utf8Value();
  std::string filePath;

  FcConfig *config = FcInitLoadConfigAndFonts();
  FcPattern *pattern = FcPatternCreate();
  FcObjectSet *objectSet = FcObjectSetBuild (FC_POSTSCRIPT_NAME, FC_FILE, (char *) 0);
  FcFontSet *fontSet = FcFontList(config, pattern, objectSet);

  for (int i = 0; fontSet && i < fontSet->nfont; ++i) {
    FcPattern* font = fontSet->fonts[i];
    FcChar8 *postscriptName, *file;

    if (
      FcPatternGetString(font,  FC_POSTSCRIPT_NAME,  0, &postscriptName)  == FcResultMatch &&
      FcPatternGetString(font,  FC_FILE,             0, &file)            == FcResultMatch
    ) {
      std::string postscriptNameAsString((char*)postscriptName);

      if (postscriptNameAsString == queryPostscriptName) {
        std::string fileAsString((char*)file);
        filePath = fileAsString;
      }
    }
  }

  if (fontSet) {
    FcFontSetDestroy(fontSet);
  }

  std::ifstream input(filePath, std::ios::binary);
  std::vector<unsigned char> data(std::istreambuf_iterator<char>(input), {});

  return Napi::Buffer<unsigned char>::Copy(env, &data[0], data.size());
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "getFontsMetadata"), Napi::Function::New(env, getFontsMetadata)
  );
  exports.Set(
    Napi::String::New(env, "getFontData"), Napi::Function::New(env, getFontData)
  );

  return exports;
}

NODE_API_MODULE(LocalFontsManager, Init);
