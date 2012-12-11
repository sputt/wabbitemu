Imports SPASM

Namespace ValueConverters

    Public Class ZoomLevelToDoubleConverter
        Implements IValueConverter

        Public Function Convert(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
            'Dim NumStr As String = CStr(value).Replace("%", "")
            'Return Double.Parse(NumStr) / 100
            Return 0.0
        End Function

        Public Function ConvertBack(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
            'If value Is Nothing OrElse CStr(value) = "" Then
            '    Return 1.0
            'End If
            'Return New String((CDbl("0" & CStr(value)) * 100.0).ToString() & "%")
            Return "100%"
        End Function
    End Class

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

    Public Class ImageIndexScenarioConverter
        Implements IMultiValueConverter

        Public Function Convert(values() As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IMultiValueConverter.Convert
            On Error Resume Next

            Dim Index = values(0)
            Dim Panel As Object = values(1)
            Dim Images = CType(Panel.DataContext, Scenario).Images

            Return CType(Images(Index), ImageSource)
        End Function

        Public Function ConvertBack(value As Object, targetTypes() As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object() Implements System.Windows.Data.IMultiValueConverter.ConvertBack
            Return Nothing
        End Function
    End Class

    Public Class ImageIndexConverter
        Implements IMultiValueConverter

        Public Function Convert(values() As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IMultiValueConverter.Convert
            On Error Resume Next

            Dim Index = values(0)
            Dim Layer As ObjectLayer = values(1)
            Dim Images = CType(Layer.Parent, MapContainer).DataContext.Scenario.Images

            Return CType(Images(Index), ImageSource)
        End Function

        Public Function ConvertBack(value As Object, targetTypes() As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object() Implements System.Windows.Data.IMultiValueConverter.ConvertBack
            Return Nothing
        End Function
    End Class


    Public Class YConverter
        Implements IMultiValueConverter

        Public Function Convert(values() As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IMultiValueConverter.Convert
            On Error Resume Next

            Dim Index = values(0)
            Dim ObjY As Double = values(1)
            Dim ObjHeight As Double = values(2)
            Dim ObjZ As Double = values(3)
            Dim Layer As ObjectLayer = values(4)
            Dim Images = CType(Layer.Parent, MapContainer).DataContext.Scenario.Images

            Dim ImgSource As ImageSource = Images(Index)
            Dim ImgHeight As Double
            If ImgSource Is Nothing Then
                ImgHeight = ObjHeight
            Else
                ImgHeight = ImgSource.Height
            End If
            Dim Result = ObjY + ObjHeight - ImgHeight - ObjZ - 2
            Return Math.Min(253, Math.Max(-2, Math.Floor(Result)))
        End Function

        Public Function ConvertBack(value As Object, targetTypes() As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object() Implements System.Windows.Data.IMultiValueConverter.ConvertBack
            Return Nothing
        End Function
    End Class

    Public Class XConverter
        Implements IMultiValueConverter

        Public Function Convert(values() As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IMultiValueConverter.Convert
            On Error Resume Next

            Dim Index = values(0)
            Dim ObjX As Double = values(1)
            Dim ObjWidth As Double = values(2)
            Dim Layer As ObjectLayer = values(3)
            Dim Images = CType(Layer.Parent, MapContainer).DataContext.Scenario.Images

            Dim ImgSource As ImageSource = Images(Index)
            Dim ImgWidth As Double
            If ImgSource Is Nothing Then
                ImgWidth = ObjWidth
            Else
                ImgWidth = ImgSource.Width
            End If
            Dim Result = ObjX + (ObjWidth - ImgWidth) / 2 - 2
            Return Math.Min(253, Math.Max(-2, Math.Floor(Result)))
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

            Dim Result = ObjY + (ObjHeight / 2)
            Return CInt(Math.Min(255, Math.Max(0, Math.Floor(Result))))
        End Function

        Public Function ConvertBack(value As Object, targetTypes() As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object() Implements System.Windows.Data.IMultiValueConverter.ConvertBack
            Return Nothing
        End Function
    End Class

    Public Class BitValueConverter
        Implements IValueConverter

        Private target As Byte

        Public Function Convert(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
            Dim mask As Byte = parameter
            target = SPASMHelper.Assemble(".db " & value)(0)
            Return ((mask And target) <> 0)
        End Function

        Public Function ConvertBack(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
            target = target Xor CByte(parameter)
            Return target
        End Function
    End Class


    Public Class GraphicsConverter
        Implements IValueConverter

        Public Function Convert(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
            Dim Result = From i As ZeldaImage In ZeldaImages.Images Where i.GetValue(ZeldaImage.LabelProperty) = CStr(value) Select i
            Return Result(0)
        End Function

        Public Function ConvertBack(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
            Return CType(value, DependencyObject).GetValue(ZeldaImage.LabelProperty)
        End Function
    End Class

    Public Class DirectionConverter
        Implements IValueConverter

        Public Function Convert(value As Object, targetType As System.Type, parameter As Object, culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
            Dim dir As String = CStr(value)
            Debug.WriteLine("dir: " & dir)
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

End Namespace