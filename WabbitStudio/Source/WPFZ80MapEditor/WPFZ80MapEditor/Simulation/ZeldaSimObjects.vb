Imports System.Runtime.InteropServices
Imports System.Collections.ObjectModel

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Public Structure AZDrawEntry
    Public Position As AZPosition
    Public Image As UShort
    Public Flags As Byte
End Structure

Public Class ZDrawEntry
    Inherits ZBaseObject(Of AZDrawEntry, ZDrawEntry)

    Public Const D_INVERT As Byte = 1 << 6
    Public Const D_NOSHADOW As Byte = 1
    Public Const D_SPARKLE As Byte = 1 << 3
    Public Const D_PLAYER As Byte = 1 << 4
    Public Const D_HORIZONTAL As Byte = 1 << 2

    Public Flags As Byte

    Protected Overrides Sub FromStruct(Obj As AZDrawEntry)
        With Obj.Position
            X = .X : W = .W
            Y = .Y : H = .H
            Z = .Z : D = .D
        End With
        Flags = Obj.Flags
        Image = Obj.Image
    End Sub

End Class
