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