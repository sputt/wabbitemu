Imports System.ComponentModel

Public Class Tile
    Inherits Grid

    Private Sub SetImageSource(NewIndex As Integer)
        If NewIndex <> -1 Then
            _Image.Source = Scenario.Instance.Tilesets.Values(Tileset)(NewIndex Mod 128)
        End If
    End Sub

    Protected Overrides Sub OnPropertyChanged(e As System.Windows.DependencyPropertyChangedEventArgs)
        MyBase.OnPropertyChanged(e)

        If e.Property Is Tile.IndexProperty Then
            SetImageSource(e.NewValue)
            If e.NewValue >= 128 Then
                If ShowCollidable Then
                    OverlayColor = Color.FromArgb(64, 255, 0, 0)
                End If
            Else
                OverlayColor = Color.FromArgb(0, 255, 255, 255)
            End If
        ElseIf e.Property Is Tile.TilesetProperty Then
            SetImageSource(Index)
        End If
    End Sub

    Public Event TileSelected(Tile As Tile, e As MouseButtonEventArgs)

    Public Shared ReadOnly IndexProperty As DependencyProperty =
        DependencyProperty.Register("Index", GetType(Integer), GetType(Tile),
                                    New FrameworkPropertyMetadata(-1, FrameworkPropertyMetadataOptions.AffectsRender))

    Public Shared ReadOnly TilesetProperty As DependencyProperty =
        DependencyProperty.Register("Tileset", GetType(Integer), GetType(Tile))

    Public IsCollidable As Boolean

    Public ShowCollidable As Boolean = True

    Public Property Tileset As Integer
        Get
            Return GetValue(TilesetProperty)
        End Get
        Set(value As Integer)
            SetValue(TilesetProperty, value)
        End Set
    End Property

    Public Property OverlayColor As Color
        Get
            If _Overlay Is Nothing OrElse _Overlay.Visibility = Windows.Visibility.Hidden Then
                Return Color.FromArgb(0, 0, 0, 0)
            Else
                Return CType(_Overlay.Fill, SolidColorBrush).Color
            End If
        End Get
        Set(value As Color)
            If _Overlay Is Nothing Then
                _Overlay = New Rectangle
                _Overlay.Stretch = Stretch.Fill
                Canvas.SetZIndex(_Overlay, 1)
                Children.Add(_Overlay)
            End If
            If value.A = 0 Then
                _Overlay.Visibility = Windows.Visibility.Hidden
            Else
                _Overlay.Visibility = Windows.Visibility.Visible
                _Overlay.Fill = New SolidColorBrush(value)
            End If
        End Set

    End Property

    Private _Image As Image
    Private _Overlay As Rectangle

    Private Shared _BoolToVis As New BooleanToVisibilityConverter()

    Public Sub New()
        MyBase.New()

        _Image = New Image
        '_Image.Width = 16
        '_Image.Height = 16
        _Image.Visibility = Windows.Visibility.Visible
        _Image.StretchDirection = StretchDirection.Both
        _Image.Stretch = Stretch.Fill
        Children.Add(_Image)
        Canvas.SetZIndex(_Image, 0)

        Dim HotOverlay As New Rectangle
        HotOverlay.Stretch = Stretch.Fill
        HotOverlay.Fill = New SolidColorBrush(Color.FromArgb(40, 255, 255, 255))
        Canvas.SetZIndex(HotOverlay, 99)

        HotOverlay.DataContext = Me
        Dim b As New Binding()
        b.Path = New PropertyPath(Tile.IsMouseOverProperty)
        b.Converter = _BoolToVis

        HotOverlay.SetBinding(Rectangle.VisibilityProperty, b)

        Children.Add(HotOverlay)
        'Background = New SolidColorBrush(Colors.Black)

    End Sub

    Public Property Index As Integer
        Get
            Return GetValue(IndexProperty)
        End Get
        Set(value As Integer)
            SetValue(IndexProperty, value)
        End Set
    End Property

End Class
