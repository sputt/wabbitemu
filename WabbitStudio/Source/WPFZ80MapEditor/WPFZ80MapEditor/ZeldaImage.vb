Imports System.IO
Imports System.Text.RegularExpressions

Public Class ZeldaImage
    Implements IComparable

    Public Property Label As String
    Public Property Image As ImageSource
    Public Sub New(Label As String, Image As ImageSource)
        Me.Label = Label.ToUpper()
        Me.Image = Image
    End Sub

    Public Function CompareTo(Obj As Object) As Integer Implements System.IComparable.CompareTo
        Dim This = Label
        Dim Other = CType(Obj, ZeldaImage).Label
        Return This < Other
    End Function
End Class
