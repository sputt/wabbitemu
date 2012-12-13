Imports System.IO
Imports System.Text.RegularExpressions

Partial Public Class Scenario
    Private Sub LoadImages(FileName As String)
        Dim Rx As New Regex(
    "^(?<Name>[a-z_]+_gfx)(\s*|\s+with\s+bm_map\s*=\s*(?<X>\d+)x(?<Y>\d+)\s*)" & _
    "^#include\s+""(?<FileName>.+)""\s*" & _
    "(^\s*|(?<ExtraDefines>(^[a-z0-9_]+\s*=\s*[a-z0-9_]+\s*)+))$", RegexOptions.Multiline Or RegexOptions.Compiled)

        Dim Stream = New StreamReader(FileName)
        Dim Matches = Rx.Matches(Stream.ReadToEnd())
        Stream.Close()

        ' Empty first image
        Dim Uri As New Uri(Directory.GetCurrentDirectory() & "\Scenario\question.bmp", UriKind.Absolute)
        Dim QuestionBitmap As New BitmapImage(Uri)
        Images.Add(New ZeldaImage("", QuestionBitmap))

        Dim Index As Integer = 1
        For Each Match As Match In Matches
            Dim Groups = Match.Groups
            Dim LabelName As String = Groups("Name").Value
            SPASMHelper.Assembler.AddDefine(LabelName, Index)
            SPASMHelper.Assembler.AddDefine(Replace(LabelName, "_gfx", "_anim"), Index)

            Dim Image As ImageSource = Nothing
            Uri = New Uri(Directory.GetCurrentDirectory() & "\Scenario\" & Groups("FileName").Value, UriKind.Absolute)
            If File.Exists(Uri.LocalPath) Then
                Image = New BitmapImage(Uri)
                Image = BitmapUtils.Mask(Image, Color.FromArgb(255, 168, 230, 29))

                If Groups("X").Success And Groups("Y").Success Then
                    Dim TotalX = CInt(Groups("X").Value)
                    Dim TotalY = CInt(Groups("Y").Value)
                    Dim EachWidth As Integer = (CInt(Image.Width) - (2 * TotalX)) / TotalX
                    Dim EachHeight As Integer = (CInt(Image.Height) - (2 * TotalY)) / TotalY
                    SPASMHelper.Assembler.AddDefine(LabelName & "_width", EachWidth)
                    SPASMHelper.Assembler.AddDefine(LabelName & "_height", EachHeight)

                    Dim ImagePreview As ImageSource = Nothing
                    For X = 0 To TotalX - 1
                        For Y = 0 To TotalY - 1
                            Dim CroppedImage As New CroppedBitmap(Image, New Int32Rect((EachWidth + 2) * X + 1, (EachHeight + 2) * Y + 1, EachWidth, EachHeight))
                            Dim ItemLabel = LabelName & (X * TotalY) + Y + 1
                            Images.Add(New ZeldaImage(ItemLabel, CroppedImage))
                            SPASMHelper.Assembler.AddDefine(ItemLabel, Index)
                            Index += 1
                        Next
                    Next

                Else
                    SPASMHelper.Assembler.AddDefine(LabelName & "_width", CInt(Image.Width))
                    SPASMHelper.Assembler.AddDefine(LabelName & "_height", CInt(Image.Height))

                    Images.Add(New ZeldaImage(LabelName, Image))
                    Index += 1
                End If
            Else
                Images.Add(New ZeldaImage(LabelName, QuestionBitmap))
                Index += 1
            End If

            If Groups("ExtraDefines").Success Then
                SPASMHelper.Assemble(Groups("ExtraDefines").Value)
            End If
        Next

        'Images.Sort()
    End Sub
End Class
