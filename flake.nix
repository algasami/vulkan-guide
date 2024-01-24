{
  description = "Vulkan-Guide Flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-23.11";
    flake-utils.url = "github:numtide/flake-utils/4022d587cbbfd70fe950c1e2083a02621806a725";
  };

  outputs = inputs@{ self, nixpkgs, flake-utils, ... }:
  let

    lastModifiedDate = self.lastModifiedDate or self.lastModified or "19700101";
    version = builtins.substring 0 0 lastModifiedDate;

  in flake-utils.lib.eachDefaultSystem (system:
    let
      name = "vk_guide_flake";
      src = ./.;
      pkgs = import nixpkgs { inherit system; };
      pkg_buildInputs = with pkgs; [
        glslang
        SDL2
        vulkan-headers
        vulkan-loader
        vulkan-validation-layers
      ];
      pkg_nativeBuildInputs = with pkgs; [
        clang-tools_9
        gcc12
        gdb
        cmake
        gnumake
        coreutils
      ];
    in
    {
      devShells.default = with pkgs; pkgs.mkShell {
        buildInputs = pkg_buildInputs ++ pkg_nativeBuildInputs;
        # include all of them so that it can be easier to debug
        VULKAN_SDK = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
      };
    }
  );
}
