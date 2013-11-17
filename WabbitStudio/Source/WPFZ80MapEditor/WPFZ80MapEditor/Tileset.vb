Imports System.IO
Imports System.Globalization
Imports System.Collections.ObjectModel

Public Class Tileset
    Inherits List(Of ImageSource)

    Public Name As String

    Sub New(FileName As String)
        Name = Path.GetFileNameWithoutExtension(FileName)

        Dim Image As New BitmapImage(New Uri(FileName, UriKind.Absolute))
        For i = 0 To (Image.Height / Image.Width) - 1

            'Dim Brush As New ImageBrush
            'Brush.ImageSource = Image
            'Brush.Viewbox = New Rect(0, 0, Image.Width, Image.Width)
            'Brush.ViewboxUnits = BrushMappingMode.RelativeToBoundingBox
            'Brush.TileMode = TileMode.None

            'Dim Visual As New DrawingVisual

            'Dim Context = Visual.RenderOpen()
            'Context.DrawRectangle(New SolidColorBrush(Colors.Red), Nothing, New Rect(0, 0, Image.Width, Image.Width))
            'Context.Close()

            'Dim Target As New RenderTargetBitmap(Image.Width, Image.Width, Image.DpiX, Image.DpiY, PixelFormats.Pbgra32)
            'Target.Render(Visual)

            'Me.Add(Target)

            Dim CroppedImage As New CroppedBitmap(Image, New Int32Rect(0, i * Image.PixelWidth, Image.PixelWidth, Image.PixelWidth))
            Me.Add(CroppedImage)
        Next

    End Sub
End Class
