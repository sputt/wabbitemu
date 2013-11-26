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

    Public Flags As Byte

    Protected Overrides Sub FromStruct(Obj As AZDrawEntry)
        With Obj.Position
            X = .X : W = .W
            Y = .Y : H = .H
            Z = .Z : D = .D
        End With
        Image = Obj.Image
    End Sub

End Class
