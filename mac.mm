#include <napi.h>
#import <AppKit/AppKit.h>

Napi::Array getFontsMetadata(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  NSArray *availableFontFamilies = [[NSFontManager sharedFontManager] availableFontFamilies];

  // Maps NSFont weights to CSS font weights (https://sketchplugins.com/d/193-how-to-get-font-weight/4)
  int weightsMap[16] = {100,100,100,200,300,400,500,500,600,700,800,900,900,900,900,900};

  int fontFamiliesCount = [availableFontFamilies count];
  int fontsCounter = 0;

  Napi::Array fonts = Napi::Array::New(env);

  for (int i = 0; i < fontFamiliesCount; i++) {
    NSString *fontFamily = [availableFontFamilies objectAtIndex:i];
    NSArray *familyMembers = [[NSFontManager sharedFontManager] availableMembersOfFontFamily:fontFamily];
    int membersCount = [familyMembers count];

    for (int j = 0; j < membersCount; j++) {
      NSArray *familyMember = [familyMembers objectAtIndex:j];

      NSString *postscriptName = [familyMember objectAtIndex:0]; // e.g. "Arial", "Arial Black"
      NSString *style = [familyMember objectAtIndex:1];          // e.g. "Bold", "Italic", "Roman"
      NSString *fullName;                                        // e.g. "Arial Black Italic"

      if ([style isEqualToString:@"Regular"]){
        fullName = fontFamily;
      }
      else {
        fullName = [[fontFamily stringByAppendingString:@" "] stringByAppendingString:style];
      }

      CTFontDescriptorRef fontRef = CTFontDescriptorCreateWithNameAndSize ((CFStringRef)postscriptName, 0.0);
      CFURLRef url = (CFURLRef)CTFontDescriptorCopyAttribute(fontRef, kCTFontURLAttribute);
      NSString *fontPath = [NSString stringWithString:[(NSURL *)CFBridgingRelease(url) path]];

      if ([@[ @"ttf", @"otf", @"ttc", @"dfont"] containsObject:[[fontPath pathExtension] lowercaseString]]) {
        int nsFontWeight = [[familyMember objectAtIndex:2] intValue];
        int weight = weightsMap[nsFontWeight];
        bool italic = [style containsString:@"Italic"];

        Napi::Object data = Napi::Object::New(env);
        data.Set("family", std::string([fontFamily UTF8String]));
        data.Set("postscriptName", std::string([postscriptName UTF8String]));
        data.Set("style", std::string([style UTF8String]));
        data.Set("fullName", std::string([fullName UTF8String]));
        data.Set("weight", weight);
        data.Set("italic", italic);
        data.Set("stretch", 1);

        fonts[fontsCounter] = data;
        fontsCounter += 1;
      }
    }
  }

  return fonts;
}

Napi::Buffer<uint8_t> getFontData(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  std::string postscriptNameUTF8 = info[0].As<Napi::String>().Utf8Value();
  NSString* postscriptName = [NSString stringWithUTF8String:postscriptNameUTF8.c_str()];

  CTFontDescriptorRef fontRef = CTFontDescriptorCreateWithNameAndSize ((CFStringRef)postscriptName, 0.0);
  CFURLRef url = (CFURLRef)CTFontDescriptorCopyAttribute(fontRef, kCTFontURLAttribute);
  NSString *fontPath = [NSString stringWithString:[(NSURL *)CFBridgingRelease(url) path]];

  NSData *fontData = [NSData dataWithContentsOfFile:fontPath];
  const uint8 *bytes = (uint8 *)[fontData bytes];
  std::vector<uint8_t> data;
  data.assign(bytes, bytes + [fontData length]);

  return Napi::Buffer<uint8_t>::Copy(env, &data[0], data.size());
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
