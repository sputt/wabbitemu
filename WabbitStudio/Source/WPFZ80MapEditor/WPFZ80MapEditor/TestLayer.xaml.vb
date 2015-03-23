Public Class TestLayer
    Implements IMapLayer

    Public Property GameModel As GameModel
        Get
            Return GetValue(GameModelProperty)
        End Get

        Set(ByVal value As GameModel)
            SetValue(GameModelProperty, value)
        End Set
    End Property

    Public Shared ReadOnly GameModelProperty As DependencyProperty = _
                           DependencyProperty.Register("GameModel", _
                           GetType(GameModel), GetType(TestLayer), _
                           New PropertyMetadata(Nothing))

    Private Sub MapLayer_MouseLeftButtonDown(sender As Object, e As MouseButtonEventArgs)
        Debug.WriteLine("CLICKED ON TEST LAYER!")
        Dim X = CByte(e.GetPosition(sender).X)
        Dim Y = CByte(e.GetPosition(sender).Y)
        If Not GameModel.IsInitialized Then
            GameModel.Initialize(Map.Index, X, Y)
            GameModel.Start()
            e.Handled = True
        ElseIf GameModel.IsRetargetModeActive Then
            GameModel.SetLocation(X, Y, Map.Index)
            GameModel.EnableRetargetMode(False)
            e.Handled = True
        End If
    End Sub
End Class
