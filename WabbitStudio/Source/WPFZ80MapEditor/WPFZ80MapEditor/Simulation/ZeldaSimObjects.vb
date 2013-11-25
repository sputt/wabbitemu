Imports System.Runtime.InteropServices
Imports System.Collections.ObjectModel

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Structure AZDrawEntry
    Public Position As AZPosition
    Public Image As UShort
    Public Flags As Byte
End Structure

Public Class ZDrawEntry
    Inherits ZBaseObject

    'Public Shared ReadOnly ImageSourceProperty As DependencyProperty =
    '    DependencyProperty.Register("ImageSource", GetType(ImageSource), GetType(ZDrawEntry))

    Public Flags As Byte

    Public Shared Function FromData(Data() As Byte) As ZDrawEntry
        Dim h = GCHandle.Alloc(Data, GCHandleType.Pinned)
        Dim Obj As AZDrawEntry = Marshal.PtrToStructure(h.AddrOfPinnedObject, GetType(AZDrawEntry))
        h.Free()

        Dim DrawEntry As New ZDrawEntry
        With Obj.Position
            DrawEntry.X = .X : DrawEntry.W = .W
            DrawEntry.Y = .Y : DrawEntry.H = .H
            DrawEntry.Z = .Z : DrawEntry.D = .D
        End With
        DrawEntry.Image = Obj.Image
        DrawEntry.Flags = Obj.Flags
        Return DrawEntry
    End Function
End Class
