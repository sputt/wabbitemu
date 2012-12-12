Imports System.IO
Imports System.Text.RegularExpressions

Public Class ZeldaImage
    Inherits DependencyObject
    Implements IComparable

    Public Shared ReadOnly LabelProperty = DependencyProperty.Register("Label", GetType(String), GetType(ZeldaImage))
    Public Shared ReadOnly ImageProperty = DependencyProperty.Register("Image", GetType(ImageSource), GetType(ZeldaImage))

    Public Property Image As ImageSource
        Get
            Return GetValue(ImageProperty)
        End Get
        Set(value As ImageSource)
            SetValue(ImageProperty, value)
        End Set
    End Property


    Public Sub New(Label As String, Image As ImageSource)
        SetValue(LabelProperty, Label.ToUpper())
        SetValue(ImageProperty, Image)
    End Sub

    Public Function CompareTo(obj As Object) As Integer Implements System.IComparable.CompareTo
        Dim this = GetValue(LabelProperty)
        Dim other = CType(obj, DependencyObject).GetValue(LabelProperty)
        Return this < other
    End Function
End Class
