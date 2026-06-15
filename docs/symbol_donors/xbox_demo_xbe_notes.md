# Xbox Demo XBE Analysis

File: `orig/xbox/demo/lego.xbe`
Build: `c:\projects\lego\Release\lego.pdb` (x86, signed NA region, 2.1MB)

## Source file paths embedded in binary

### Shared Nu engine files (not in platform subdir — same source as GC)
- `nu3d\nuanim.c` — animation curves, keys, buffers (actively being decomped)
- `nu3d\nutexanm.c` — texture animation

### Xbox-specific Nu implementations
- `nu3d\xbox\nudgmngr.c` — debug manager
- `nu3d\xbox\nutex.c` — texture
- `nu3d\xbox\nugobj.c` — geometry object
- `nu3d\xbox\nucamera.c` — camera
- `nu3d\xbox\nufontx.cpp` — font (extended)
- `nu3d\xbox\nuhgobj.c` — hierarchical geometry object
- `nu3d\xbox\numtl.c` — material
- `nu3d\xbox\nurndr.c` — renderer
- `nu3d\xbox\nuscene.c` — scene
- `nu3d\xbox\nuxscn1.cpp` — scene 1
- `xbox\glutils.c` — DirectX/GL utilities
- `xbox\listman.c` — list manager

### Game/engine shared paths
- `\projects\gameapi\gamelib\terrain.c`
- `\projects\gameapi\edtools\edui.c`
- `\projects\gameapi\gui\gamemenu\apisave.c`
- `\projects\nu2api\nusound3\xbox\nusound.cpp` — NuSound 3 Xbox

## Notes
- No Nu debug strings (stripped from demo build)
- No inline PDB/CodeView debug info — PDB was external at `c:\projects\lego\Release\lego.pdb`
- Game script/object names are present as strings (character names, animation IDs, etc.)
- The shared `nu3d\nuanim.c` and `nu3d\nutexanm.c` paths confirm these are cross-platform source files
