Imports System.IO
Imports System.Globalization
Imports System.Collections.ObjectModel

Public Class TileImageSource
    Public Property Image As ImageSource
    Public Property Index As Integer
    Public Property Tileset As Tileset

    Public Sub New()
    End Sub

    Public Sub New(Image, Index, Tileset)
        Me.Image = Image
        Me.Index = Index
        Me.Tileset = Tileset
    End Sub
End Class

Public Class Tileset

    'Public Shared ReadOnly NameProperty = DependencyProperty.Register("Name", GetType(String), GetType(Tileset))
    'Public Shared ReadOnly TilesProperty = DependencyProperty.Register("Tiles",
    '                                                                  GetType(ObservableCollection(Of TileImageSource)), GetType(Tileset))

    Public Property Name As String

    Public Property Tiles As List(Of TileImageSource)

    Sub New()
    End Sub

    Sub New(Name As String, FileName As String)
        Me.Name = Name

        Tiles = New List(Of TileImageSource)()

        MainWindow.Instance.Dispatcher.Invoke(
            Sub()
                Dim Image As New BitmapImage(New Uri(FileName, UriKind.Absolute))
                For i = 0 To (Image.Height / Image.Width) - 1
                    Dim CroppedImage As New CroppedBitmap(Image, New Int32Rect(0, i * Image.PixelWidth, Image.PixelWidth, Image.PixelWidth))
                    Tiles.Add(New TileImageSource(CroppedImage, i, Me))
                Next
            End Sub)
    End Sub

    Default Public ReadOnly Property Item(index As Integer) As ImageSource
        Get
            Return Tiles(index).Image
        End Get
    End Property

    Public ReadOnly Property Count() As Integer
        Get
            Return Tiles.Count
        End Get
    End Property
End Class

Public Class TilesCollection
    Inherits List(Of TileImageSource)
End Class