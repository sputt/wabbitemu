Set asm = CreateObject("Wabbit.SPASM")

asm.Defines.Add "test", 34

asm.Assemble " .echo ""hello "",test"
WScript.Echo asm.StdOut.ReadAll
