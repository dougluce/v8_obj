{
  "variables": {
    "cflags_cc": [
      "-Wall",
      "-Werror",
      "-O3"
    ],
    "include_dirs": [
      "<!(node -e \"require('nan')\")"
    ]
  },
  "targets": [
    {
      "target_name": "<(module_name)",
      "sources": [ "v8_obj.cc" ],
      "cflags_cc": [ "<@(cflags_cc)" ],
      "include_dirs": [ "<@(include_dirs)" ]
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "<(module_name)" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
          "destination": "<(module_path)"
        }
      ]
    }
  ]
}
