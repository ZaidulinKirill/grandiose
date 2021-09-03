{
  "targets": [
    {
      "target_name": "ndi",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "src/ndi.cpp" ],
      "include_dirs": [
        "include",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
      'conditions': [
        ['OS=="win" and target_arch == "ia32"', {
          "link_settings": {
            "libraries": [ "Processing.NDI.Lib.x86.lib" ],
            "library_dirs": [ "lib/win_x86" ]
          },
          "copies": [
            {
              "destination": "build/Release",
              "files": [
                "lib/win_x86/Processing.NDI.Lib.x86.dll"
              ]
            }
          ]},
        ],
        ['OS=="win" and target_arch == "x64"', {
          "link_settings": {
            "libraries": [ "Processing.NDI.Lib.x64.lib" ],
            "library_dirs": [ "lib/win_x64" ]
          },
          "copies": [
            {
              "destination": "build/Release",
              "files": [
                "lib/win_x64/Processing.NDI.Lib.x64.dll"
              ]
            }
          ]},
        ]
      ]
    }
  ]
}