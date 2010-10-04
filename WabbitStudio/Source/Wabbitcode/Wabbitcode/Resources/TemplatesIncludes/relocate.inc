#macro relocate(new_location)
 #ifdef old_location
   .echo "Zelda: ",__file,":",__line,": error: You cannot nest relocate blocks."
 #else
   #define old_location eval($)
   .org new_location
   #define g_location eval(new_location)
 #endif
#endmacro

#macro endrelocate()
 #ifdef g_location
   .org $-g_location + old_location
   #undefine g_location
   #undefine old_location
 #else
   .echo "Error line ",__line,": No relocate statements corresponds to this endrelocate."
 #endif
#endmacro