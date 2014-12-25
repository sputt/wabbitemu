Imports System.ComponentModel

Public Enum LayerType
    MapsetLayer
    MapLayer
    ObjectLayer
    EnemyLayer
    MiscLayer
End Enum

Public Class AppModel
    Implements INotifyPropertyChanged

    Private _CurrentLayer As LayerType

    Public Property CurrentLayer As LayerType
        Get
            Return _CurrentLayer
        End Get
        Set(value As LayerType)
            If _CurrentLayer <> value Then
                _CurrentLayer = value
                RaisePropertyChanged("CurrentLayer")
            End If
        End Set
    End Property

    Private _Scenario As Scenario
    Public Property Scenario As Scenario
        Get
            Return _Scenario
        End Get
        Set(value As Scenario)
            If _Scenario IsNot value Then
                _Scenario = value
                RaisePropertyChanged("Scenario")
            End If
        End Set
    End Property

    Private _SelectedMap As MapData
    Public Property SelectedMap As MapData
        Get
            Return _SelectedMap
        End Get
        Set(value As MapData)
            If _SelectedMap IsNot value Then
                _SelectedMap = value
                RaisePropertyChanged("SelectedMap")
            End If
        End Set
    End Property


    Private _Status As String
    Public Property Status As String
        Get
            Return _Status
        End Get
        Set(value As String)
            If _Status <> value Then
                _Status = value
                RaisePropertyChanged("Status")
            End If
        End Set
    End Property

    Private Sub RaisePropertyChanged(PropName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(PropName))
    End Sub

    Public Event PropertyChanged(sender As Object, e As System.ComponentModel.PropertyChangedEventArgs) Implements System.ComponentModel.INotifyPropertyChanged.PropertyChanged

End Class

Public Class LayerHitTestConverter
    Implements IValueConverter

    Public Function Convert(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.Convert
        Return value = parameter
    End Function

    Public Function ConvertBack(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.ConvertBack
        Return Nothing
    End Function
End Class

Public Class ActiveLayerConverter
    Implements IMultiValueConverter

    Public Function Convert(values() As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IMultiValueConverter.Convert
        Return values(0) = values(1)
    End Function

    Public Function ConvertBack(value As Object, targetTypes() As Type, parameter As Object, culture As Globalization.CultureInfo) As Object() Implements IMultiValueConverter.ConvertBack
        Return Nothing
    End Function
End Class