Imports SPASM
Imports System.Windows.Media.Animation

Namespace ValueConverters

    Public Class DoubleToZoomLevelConverter
        Implements IValueConverter

        Public Function Convert(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
            If value Is Nothing OrElse CStr(value) = "" Then
                Return 1.0
            End If
            Return New String((CDbl("0" & CStr(value)) * 100.0).ToString("0") & "%")
        End Function

        Public Function ConvertBack(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
            Dim NumStr As String = CStr(value).Replace("%", "")
            Return Double.Parse(NumStr) / 100
        End Function
    End Class

    Public Class YConverter
        Implements IMultiValueConverter

        Public Function Convert(values() As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IMultiValueConverter.Convert
            On Error Resume Next

            Dim Scenario As Scenario
            If TypeOf values(0) Is MapData Then
                Scenario = values(0).Scenario
            ElseIf TypeOf values(0) Is Scenario Then
                Scenario = values(0)
            Else
                Return Nothing
            End If

            Dim Index As Object = values(1)
            Dim ObjY As Double = values(2)
            Dim ObjHeight As Double = values(3)
            Dim ObjZRaw As Object = values(4)

            Dim Images = Scenario.Images

            Dim ImgSource As ImageSource = Nothing
            Dim ZImg As ZeldaImage = Nothing
            If Not Index.Equals(DependencyProperty.UnsetValue) AndAlso Index < Images.Count Then
                ZImg = Images(Index)
            End If
            If ZImg IsNot Nothing Then
                ImgSource = ZImg.Image
            End If

            Dim ImgHeight As Double
            If ImgSource Is Nothing Then
                ImgHeight = ObjHeight
            Else
                ImgHeight = ImgSource.Height
            End If

            Dim ObjZ As Double = 0.0
            If Not ObjZRaw.Equals(DependencyProperty.UnsetValue) Then
                ObjZ = ObjZRaw
            End If

            Dim Result = ObjY + ObjHeight - ImgHeight - ObjZ
            Return Math.Min(255, Math.Max(0, Math.Floor(Result)))
        End Function

        Public Function ConvertBack(value As Object, targetTypes() As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object() Implements System.Windows.Data.IMultiValueConverter.ConvertBack
            Return Nothing
        End Function
    End Class

    Public Class XConverter
        Implements IMultiValueConverter

        Public Function Convert(values() As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IMultiValueConverter.Convert
            On Error Resume Next

            Dim Scenario As Scenario
            If TypeOf values(0) Is MapData Then
                Scenario = values(0).Scenario
            ElseIf TypeOf values(0) Is Scenario Then
                Scenario = values(0)
            Else
                Return Nothing
            End If


            Dim Index As Object = values(1)
            Dim ObjX As Double = values(2)
            Dim ObjWidth As Double = values(3)
            Dim Images = Scenario.Images

            Dim ImgSource As ImageSource = Nothing
            Dim ZImg As ZeldaImage = Nothing
            If Not Index.Equals(DependencyProperty.UnsetValue) AndAlso Index < Images.Count Then
                ZImg = Images(Index)
            End If
            If ZImg IsNot Nothing Then
                ImgSource = ZImg.Image
            End If

            Dim ImgWidth As Double
            If ImgSource Is Nothing Then
                ImgWidth = ObjWidth
            Else
                ImgWidth = ImgSource.Width
            End If
            Dim Result = ObjX + (ObjWidth - ImgWidth) / 2
            Return Math.Min(255, Math.Max(0, Math.Floor(Result)))
        End Function

        Public Function ConvertBack(value As Object, targetTypes() As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object() Implements System.Windows.Data.IMultiValueConverter.ConvertBack
            Return Nothing
        End Function
    End Class

    Public Class ZConverter
        Implements IMultiValueConverter

        Public Function Convert(values() As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IMultiValueConverter.Convert
            On Error Resume Next

            Dim ObjY As Double = values(0)
            Dim ObjHeight As Double = values(1)
            Dim ObjZ As Byte = values(2)
            Dim ObjDepth As Byte = values(3)

            Dim Result = ObjY + (ObjHeight / 2)
            If ObjZ = 0 And ObjDepth = 0 Then
                Result = 0
            End If
            Return CInt(Math.Min(255, Math.Max(0, Math.Floor(Result))))
        End Function

        Public Function ConvertBack(value As Object, targetTypes() As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object() Implements System.Windows.Data.IMultiValueConverter.ConvertBack
            Return Nothing
        End Function
    End Class

    Public Class BitValueConverter
        Implements IValueConverter

        Private target As Byte = 0

        Public Function Convert(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
            Dim mask As Byte = parameter
            target = SPASMHelper.Eval(value)
            Return (target And mask)
        End Function

        Public Function ConvertBack(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
            Dim newVal As Integer = 0
            If TypeOf value Is Boolean AndAlso value = True Then
                newVal = parameter
            Else
                newVal = SPASMHelper.Eval(value)
            End If
            target = target And (Not CByte(parameter)) Or newVal
            Return target
        End Function
    End Class

    Public Class DirectionConverter
        Implements IValueConverter

        Public Function Convert(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
            If value Is Nothing OrElse value = "" Then Return Nothing

            Dim dir As String = CStr(value)
            Return SPASMHelper.Eval(dir)
        End Function

        Public Function ConvertBack(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
            Dim dir As Integer = value
            Select Case dir
                Case 0 : Return "d_down"
                Case 1 : Return "d_left"
                Case 2 : Return "d_right"
                Case 3 : Return "d_up"
                Case Else : Return "d_none"
            End Select
        End Function
    End Class

    Public Class VolumeConverter
        Implements IMultiValueConverter

        Public Function Convert(values() As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IMultiValueConverter.Convert
            Return CInt((256) - ((values(0) * values(1)) / 256))
        End Function

        Public Function ConvertBack(value As Object, targetTypes() As Type, parameter As Object, culture As Globalization.CultureInfo) As Object() Implements IMultiValueConverter.ConvertBack
            Return Nothing
        End Function
    End Class

    Public Class TileConverter
        Implements IMultiValueConverter

        Public Function Convert(values() As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IMultiValueConverter.Convert
            If Not TypeOf values(0) Is Tileset Then
                Return Nothing
            End If
            Dim Tileset As Tileset = values(0)
            Dim Index As Byte = values(1)
            Index = Index Mod 128
            If Index < Tileset.Tiles.Count Then
                Return Tileset.Tiles(Index).Image
            Else
                Return Nothing
            End If
        End Function

        Public Function ConvertBack(value As Object, targetTypes() As Type, parameter As Object, culture As Globalization.CultureInfo) As Object() Implements IMultiValueConverter.ConvertBack
            Dim Tile As TileImageSource = value
            Return {Nothing, Tile.Index}
        End Function
    End Class

    Public Class CollisionConverter
        Implements IValueConverter

        Public Function Convert(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.Convert
            Dim Index As Integer = value
            Return If(Index >= 128, Visibility.Visible, Visibility.Hidden)
        End Function

        Public Function ConvertBack(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.ConvertBack
            Return 0
        End Function
    End Class

    Public Class XImageConverter
        Implements IMultiValueConverter

        Public Function Convert(values() As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IMultiValueConverter.Convert
            Dim Scenario As Scenario = TryCast(values(0), Scenario)
            If Scenario Is Nothing Then Return Nothing

            Dim Index As Integer = values(1)
            If Index > Scenario.Images.Count Then
                Return Nothing
            End If

            Return Scenario.Images(Index).Image
        End Function

        Public Function ConvertBack(value As Object, targetTypes() As Type, parameter As Object, culture As Globalization.CultureInfo) As Object() Implements IMultiValueConverter.ConvertBack
            Return Nothing
        End Function
    End Class

    Public Class GapConverter
        Implements IValueConverter

        Public Function Convert(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.Convert
            Return If(value, 8, 0)
        End Function

        Public Function ConvertBack(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.ConvertBack
            Return Nothing
        End Function
    End Class

    Public Class GraphicsConverter
        Implements IMultiValueConverter

        Dim Images As ICollection(Of ZeldaImage)

        Public Function Convert1(values() As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IMultiValueConverter.Convert
            Images = values(0)
            Dim ImageIndex = SPASMHelper.Eval(values(1))
            Dim ActualLabels = SPASMHelper.Defines.OfType(Of String).Where(Function(z) SPASMHelper.Defines(z) = ImageIndex).Select(Function(s) s.ToUpper)

            Dim Result = From i As ZeldaImage In Images Where ActualLabels.Contains(i.Label) Select i
            Return Result(0)
        End Function

        Public Function ConvertBack1(value As Object, targetTypes() As Type, parameter As Object, culture As Globalization.CultureInfo) As Object() Implements IMultiValueConverter.ConvertBack
            Return {Images, value.Label}
        End Function
    End Class

    Public Enum TileIndexType
        X
        Y
    End Enum

    Public Class TileIndexConverter
        Inherits TileIndexComponentConverter

        Public Overrides Function Convert(Value As Integer, Parameter As Object) As Double
            Dim Result = MyBase.Convert(Value, Parameter)
            If Result = -1 Then Return -1

            Return Result * 16
        End Function
    End Class

    Public Class TileIndexComponentConverter
        Inherits OneWayConverter(Of Integer, Double)

        Public Overrides Function Convert(Value As Integer, Parameter As Object) As Double
            If Parameter = TileIndexType.X Then
                Return (Value Mod 16)
            ElseIf Parameter = TileIndexType.Y Then
                Return (Math.Floor(Value / 16))
            End If
            Return -1
        End Function
    End Class


    Public Class DashedStrokeConverter
        Inherits OneWayConverter(Of Boolean, DoubleCollection)
        Public Overrides Function Convert(Value As Boolean, Parameter As Object) As DoubleCollection
            If Value Then
                Return New DoubleCollection({4.0, 1.5})
            Else
                Return New DoubleCollection({4.0, 0.0})
            End If
        End Function
    End Class
End Namespace