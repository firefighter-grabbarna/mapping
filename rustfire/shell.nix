with (import <nixpkgs> {});
mkShell {
    buildInputs = [ esbuild cargo-watch ];
}
