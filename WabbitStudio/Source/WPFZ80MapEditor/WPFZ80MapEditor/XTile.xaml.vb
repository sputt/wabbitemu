Public Class XTile
    'Inherits UserControl

    Public Shared ReadOnly IndexProperty As DependencyProperty =
        DependencyProperty.Register("Index", GetType(Double), GetType(XTile),
                                    New FrameworkPropertyMetadata(-1.0, FrameworkPropertyMetadataOptions.AffectsRender,
                                                                  AddressOf OnIndexPropertyChanged))
    Public Shared ReadOnly IsAnimatedProperty As DependencyProperty =
        DependencyProperty.Register("IsAnimated", GetType(Boolean), GetType(XTile),
                                    New FrameworkPropertyMetadata(False))
    Public Shared ReadOnly AnimDefProperty As DependencyProperty =
        DependencyProperty.Register("AnimDef", GetType(ZDef), GetType(XTile))
    Public Shared ReadOnly ScenarioProperty As DependencyProperty =
        DependencyProperty.Register("Scenario", GetType(Scenario), GetType(XTile))
    Public Shared ReadOnly TileImageProperty As DependencyProperty =
        DependencyProperty.Register("TileImage", GetType(ImageSource), GetType(XTile),
                                    New PropertyMetadata(Nothing, Nothing, AddressOf OnTileImagePropertyCoerce))

    Private Shared Sub OnIndexPropertyChanged(d As DependencyObject, basevalue As Object)
        Dim tile As XTile = d
        tile.CoerceValue(TileImageProperty)
    End Sub

    Public Property Scenario As Scenario
        Get
            Return GetValue(ScenarioProperty)
        End Get
        Set(value As Scenario)
            SetValue(ScenarioProperty, value)
        End Set
    End Property

    Private Shared Function OnTileImagePropertyCoerce(d As DependencyObject, basevalue As Object) As Object
        Dim tile As XTile = d
        Dim tileIndex = tile.Index
        If tileIndex < 0 Then tileIndex = 0
        Try
            Return Scenario.Instance.Tilesets(0)(tileIndex Mod 128)
        Catch e As Exception
            Debug.WriteLine("TileIndex: " & tileIndex)
            Return Scenario.Instance.Tilesets(0)(0)
        End Try
    End Function

    Private ReadOnly Property Index() As Integer
        Get
            Return GetValue(IndexProperty)
        End Get
    End Property

    Public Property TileImage As ImageSource
        Get
            GetValue(TileImageProperty)
        End Get
        Set(value As ImageSource)
            SetValue(TileImageProperty, value)
        End Set
    End Property

    Public Property IsAnimated
        Get
            Return GetValue(IsAnimatedProperty)
        End Get
        Set(value)
            SetValue(IsAnimatedProperty, value)
        End Set
    End Property
End Class
