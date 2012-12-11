Imports System.Windows.Media.Animation

Public Class LayerContainer
    Inherits Grid

    Public Shared TileSize As New Size
    Public Shared MapSize As New Size

    Public Const DefaultPadding = 12
    Private Shared _Padding As Integer = DefaultPadding

    Private _Grid As Grid


    Public Shared ReadOnly ShowGridPaddingProperty As DependencyProperty = DependencyProperty.Register("ShowGridPadding", GetType(Visibility), GetType(LayerContainer),
                                                                                             New FrameworkPropertyMetadata(Visibility.Visible, FrameworkPropertyMetadataOptions.AffectsRender))

    Protected Overrides Sub OnPropertyChanged(e As System.Windows.DependencyPropertyChangedEventArgs)
        MyBase.OnPropertyChanged(e)

        If e.Property.Name = LayerContainer.ShowGridPaddingProperty.Name Then
            If e.NewValue = Windows.Visibility.Visible Then
                _Padding = DefaultPadding
            Else
                _Padding = 0
            End If

            SetGridPadding()
        End If
    End Sub

    Public Property ShowGridPadding As Visibility
        Get
            Return GetValue(ShowGridPaddingProperty)
        End Get
        Set(value As Visibility)
            SetValue(ShowGridPaddingProperty, value)
        End Set
    End Property

    Public Sub New()
        _Grid = Me

        TileSize.Width = 16
        TileSize.Height = 16

        MapSize.Width = 16
        MapSize.Height = 16

    End Sub

    Private Sub SetGridPadding()

        Dim Diff = _Grid.ColumnDefinitions(0).Width.Value - (TileSize.Width * MapSize.Width + _Padding)

        For i = 0 To _Grid.ColumnDefinitions.Count - 1
            _Grid.ColumnDefinitions(i).Width = New GridLength(TileSize.Width * MapSize.Width + _Padding)
        Next

        For i = 0 To _Grid.RowDefinitions.Count - 1
            _Grid.RowDefinitions(i).Height = New GridLength(TileSize.Height * MapSize.Height + _Padding)
        Next

        Dim tt As TranslateTransform = CType(Me.RenderTransform, TransformGroup).Children.First(Function(t) TypeOf t Is TranslateTransform)
        tt.X += (Diff * (_Grid.ColumnDefinitions.Count)) / 2
        tt.Y += (Diff * (_Grid.RowDefinitions.Count)) / 2

    End Sub

    Public Function AddMap(x As Integer, y As Integer, Map As MapData) As MapContainer
        For i = _Grid.ColumnDefinitions.Count To x
            Dim ColDef = New ColumnDefinition With {.Width = New GridLength(TileSize.Width * MapSize.Width + _Padding)}
            _Grid.ColumnDefinitions.Add(ColDef)
        Next

        For i = _Grid.RowDefinitions.Count To y
            Dim RowDef = New RowDefinition With {.Height = New GridLength(TileSize.Height * MapSize.Height + _Padding)}
            _Grid.RowDefinitions.Add(RowDef)
        Next

        Dim Container As New MapContainer(Map)
        Me.Children.Add(Container)

        Grid.SetColumn(Container, x)
        Grid.SetRow(Container, y)
        Grid.SetZIndex(Container, y)
        Return Container
    End Function

    Public Sub AddLeftColumn()
        ColumnDefinitions.Insert(0, New ColumnDefinition With {.Width = New GridLength(TileSize.Width * MapSize.Width + _Padding)})
    End Sub

    Public Sub AddTopRow()
        RowDefinitions.Insert(0, New RowDefinition With {.Height = New GridLength(TileSize.Height * MapSize.Height + _Padding)})
    End Sub

    Public Sub AddRightColumn()
        ColumnDefinitions.Add(New ColumnDefinition With {.Width = New GridLength(TileSize.Width * MapSize.Width + _Padding)})
    End Sub

    Public Sub AddBottomRow()
        RowDefinitions.Add(New RowDefinition With {.Height = New GridLength(TileSize.Height * MapSize.Height + _Padding)})
    End Sub

End Class
