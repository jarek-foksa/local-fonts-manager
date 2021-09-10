{
  "targets": [
    {
      "target_name": "",
      "sources": [],
      "conditions": [
        [
          'OS=="mac"',
        {
          "sources": ["mac.mm"],
          "target_name": "MacFontsManager"
        }
      ],
      [
        'OS=="linux"',
        {
          "sources": ["linux.cc"],
          "target_name": "LinuxFontsManager"
        }
      ],
    ],
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include\")"
    ],
    "libraries": [],
    "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"],
    "xcode_settings": {
      "OTHER_CPLUSPLUSFLAGS": ["-std=c++14", "-stdlib=libc++", "-mmacosx-version-min=10.8"],
      "OTHER_LDFLAGS": ["-framework CoreFoundation -framework AppKit -framework Contacts"]
    }
  }]
}