Imports System.IO
Imports System.Text.RegularExpressions

Public Class ZeldaImage
    Inherits DependencyObject
    Implements IComparable

    Public Shared ReadOnly LabelProperty = DependencyProperty.Register("Label", GetType(String), GetType(ZeldaImage))
    Public Shared ReadOnly ImageProperty = DependencyProperty.Register("Image", GetType(ImageSource), GetType(ZeldaImage))

    Public Sub New(Label As String, Image As ImageSource)
        SetValue(LabelProperty, Label.ToUpper())
        SetValue(ImageProperty, Image)
    End Sub

    Public Function CompareTo1(obj As Object) As Integer Implements System.IComparable.CompareTo
        Dim this = GetValue(LabelProperty)
        Dim other = CType(obj, DependencyObject).GetValue(LabelProperty)
        Return this < other
    End Function
End Class

Public Class ZeldaImages
    ' This is somewhat related to scenario so perhaps should be reconsidered 
    Public Shared Images As List(Of ZeldaImage) = New List(Of ZeldaImage)

    Shared Sub Load(FileName As String, Scenario As Scenario)
        Dim Rx As New Regex(
            "^(?<Name>[a-z_]+_gfx)(\s*|\s+with\s+bm_map\s*=\s*(?<X>\d+)x(?<Y>\d+)\s*)" & _
            "^#include\s+""(?<FileName>.+)""\s*" & _
            "(^\s*|(?<ExtraDefines>(^[a-z0-9_]+\s*=\s*[a-z0-9_]+\s*)+))$", RegexOptions.Multiline Or RegexOptions.Compiled)

        Dim Stream = New StreamReader(FileName)
        Dim Matches = Rx.Matches(Stream.ReadToEnd())
        Stream.Close()

        ' Empty first image
        Scenario.Images.Add(Nothing)

        Dim Index As Integer = 1
        For Each Match As Match In Matches
            Dim Groups = Match.Groups
            Dim LabelName As String = Groups("Name").Value
            SPASMHelper.Assembler.AddDefine(LabelName, Index)
            SPASMHelper.Assembler.AddDefine(Replace(LabelName, "_gfx", "_anim"), Index)

            Dim Image As BitmapImage = Nothing
            Dim Uri As New Uri(Directory.GetCurrentDirectory() & "\Scenario\" & Groups("FileName").Value, UriKind.Absolute)
            If File.Exists(Uri.LocalPath) Then
                Image = New BitmapImage(Uri)

                If Groups("X").Success And Groups("Y").Success Then
                    Dim TotalX = CInt(Groups("X").Value)
                    Dim TotalY = CInt(Groups("Y").Value)
                    Dim EachWidth = (Image.PixelWidth - (2 * TotalX)) / TotalX
                    Dim EachHeight = (Image.PixelHeight - (2 * TotalY)) / TotalY
                    SPASMHelper.Assembler.AddDefine(LabelName & "_width", EachWidth)
                    SPASMHelper.Assembler.AddDefine(LabelName & "_height", EachHeight)

                    Dim ImagePreview As ImageSource = Nothing
                    For X = 0 To TotalX - 1
                        For Y = 0 To TotalY - 1
                            Dim CroppedImage As New CroppedBitmap(Image, New Int32Rect((EachWidth + 2) * X + 1, (EachHeight + 2) * Y + 1, EachWidth, EachHeight))
                            If ImagePreview Is Nothing Then
                                ImagePreview = CroppedImage
                            End If
                            Scenario.Images.Add(CroppedImage)
                            SPASMHelper.Assembler.AddDefine(LabelName & (X * TotalY) + Y + 1, Index)
                            Index += 1
                        Next
                    Next
                    Images.Add(New ZeldaImage(LabelName, ImagePreview))
                Else
                    SPASMHelper.Assembler.AddDefine(LabelName & "_width", Image.PixelWidth)
                    SPASMHelper.Assembler.AddDefine(LabelName & "_height", Image.PixelHeight)

                    'Debug.WriteLine("Adding " & LabelName & "_width = " & Image.PixelWidth)
                    'Debug.WriteLine("Adding " & LabelName & "_height = " & Image.PixelHeight)

                    Images.Add(New ZeldaImage(LabelName, Image))
                    Scenario.Images.Add(Image)
                    Index += 1
                End If
            Else
                Scenario.Images.Add(Nothing)
                Index += 1
            End If

            If Groups("ExtraDefines").Success Then
                SPASMHelper.Assemble(Groups("ExtraDefines").Value)
            End If
        Next

        ZeldaImages.Images.Sort()
    End Sub

End Class
