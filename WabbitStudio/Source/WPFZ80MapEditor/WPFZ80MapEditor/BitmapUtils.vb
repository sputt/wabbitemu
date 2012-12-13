Imports System.Windows.Media

Public Class BitmapUtils

    Public Shared Function Mask(Source As BitmapSource, MaskColor As Color) As ImageSource



        Dim Dest As New WriteableBitmap(Source.Width, Source.Height, 96, 96, PixelFormats.Bgra32, Nothing)
        Dest.Lock()

        Dim Width As Integer = Source.Width
        Dim Height As Integer = Source.Height
        Dim PixelData(Width * Height * 4) As Byte

        Dim SourceData(Width * Height * 4) As Byte
        Source.CopyPixels(SourceData, Width * 4, 0)

        For y = 0 To Height - 1
            For x = 0 To Width - 1


                Dim i = y * Width * 4 + x * 4

                Dim PixelColor = Color.FromArgb(255, SourceData(i + 2), SourceData(i + 1), SourceData(i))

                PixelData(i + 2) = PixelColor.R
                PixelData(i + 1) = PixelColor.G
                PixelData(i + 0) = PixelColor.B
                PixelData(i + 3) = IIf(PixelColor.Equals(MaskColor), 0, 255)
            Next
        Next

        Dest.WritePixels(New Int32Rect(0, 0, Width, Height), PixelData, Width * 4, 0)

        Dest.Unlock()

        Return Dest
    End Function
End Class
