#define _MAPEDITOR
#include "graphics.asm"
#include "objects.asm"

#include "Mapdef.inc"
#include "objectdef.inc"

.echo > output.txt "Start: ", $
start:
.db $CC
 eye(1, 2, d_down)
 .echo >> output.txt "End: ", $, " = ", $ - start