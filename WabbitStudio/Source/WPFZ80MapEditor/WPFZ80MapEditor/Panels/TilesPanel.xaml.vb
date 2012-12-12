Imports System.Windows.Media.Animation
Imports System.Windows.Media.Effects
Imports System.Linq

Public Class TilesPanel
    Implements ComponentModel.INotifyPropertyChanged


    Protected Class PanelTile
        Inherits Tile

        Public Shadows Event TileSelected(Tile As PanelTile, e As System.Windows.Input.MouseButtonEventArgs)

        Const SelectedZoom As Double = 1.4
        Const HoveredZoom As Double = 1.2

        Private _TileZoom As ScaleTransform

        Private _IsSelected As Boolean
        Public Property IsSelected As Boolean
            Get
                Return _IsSelected
            End Get
            Set(value As Boolean)
                _IsSelected = value
                If value Then
                    _TileZoom.BeginAnimation(ScaleTransform.ScaleXProperty, CreateZoomAnimation(SelectedZoom))
                    _TileZoom.BeginAnimation(ScaleTransform.ScaleYProperty, CreateZoomAnimation(SelectedZoom))

                    Dim Glow As New DropShadowEffect
                    Glow.Opacity = 1.0
                    Glow.Direction = 0
                    Glow.BlurRadius = 10
                    Glow.ShadowDepth = 0
                    Glow.Color = Colors.White
                    Me.Effect = Glow
                    Panel.SetZIndex(Me, 3)
                    Me.OverlayColor = Colors.Transparent
                Else
                    Dim NewZoom As Double = 1.0
                    If IsMouseOver Then
                        NewZoom = HoveredZoom
                    End If

                    If _TileZoom.ScaleX > 1.0 Then
                        _TileZoom.BeginAnimation(ScaleTransform.ScaleXProperty, CreateZoomAnimation(NewZoom))
                        _TileZoom.BeginAnimation(ScaleTransform.ScaleYProperty, CreateZoomAnimation(NewZoom))
                        Panel.SetZIndex(Me, 2)
                    Else
                        Panel.SetZIndex(Me, 0)
                    End If

                    Me.Effect = Nothing

                    Me.OverlayColor = Color.FromArgb(80, 255, 255, 255)
                End If

            End Set
        End Property

        Private Sub ZoomAnimationEnd(sender As Object, e As EventArgs)
            Dim clock As Clock = sender

            If clock.CurrentState <> ClockState.Active Then
                If Panel.GetZIndex(Me) = 2 Then
                    Panel.SetZIndex(Me, 0)
                End If
            End If
        End Sub

        Private Function CreateZoomAnimation(toValue As Double) As DoubleAnimation
            Dim da = New DoubleAnimation(toValue, New Duration(TimeSpan.FromMilliseconds(60)))
            AddHandler da.CurrentStateInvalidated, AddressOf ZoomAnimationEnd
            da.Freeze()
            Return da
        End Function

        Public Sub New()
            MyBase.New()

            Me.RenderTransformOrigin = New Point(0.5, 0.5)

            _TileZoom = New ScaleTransform
            _TileZoom.ScaleX = 1.0
            _TileZoom.ScaleY = 1.0
            Me.RenderTransform = _TileZoom
        End Sub

        Protected Overrides Sub OnMouseEnter(e As System.Windows.Input.MouseEventArgs)
            MyBase.OnMouseEnter(e)
            Dim TileZoom As ScaleTransform = Me.RenderTransform

            If IsSelected = False Then
                Panel.SetZIndex(Me, 1)
                TileZoom.BeginAnimation(ScaleTransform.ScaleXProperty, CreateZoomAnimation(HoveredZoom))
                TileZoom.BeginAnimation(ScaleTransform.ScaleYProperty, CreateZoomAnimation(HoveredZoom))
            End If
        End Sub

        Protected Overrides Sub OnMouseLeave(e As System.Windows.Input.MouseEventArgs)
            MyBase.OnMouseLeave(e)

            If IsSelected = False Then
                Panel.SetZIndex(Me, 0)
                Dim TileZoom As ScaleTransform = Me.RenderTransform
                TileZoom.BeginAnimation(ScaleTransform.ScaleXProperty, CreateZoomAnimation(1.0))
                TileZoom.BeginAnimation(ScaleTransform.ScaleYProperty, CreateZoomAnimation(1.0))
            End If
        End Sub

        Protected Overrides Sub OnMouseUp(e As System.Windows.Input.MouseButtonEventArgs)
            MyBase.OnMouseUp(e)
            OnMouseEnter(e)
            RaiseEvent TileSelected(Me, e)
        End Sub

    End Class

    Protected Sub OnTileSelect(Tile As PanelTile, e As System.Windows.Input.MouseButtonEventArgs)
        Select Case e.ChangedButton
            Case MouseButton.Left
                If Index = Tile.Index Then
                    Index = -1
                Else
                    Index = Tile.Index
                End If
            Case MouseButton.Middle
                If Tile.Index = Index Then
                    Index = Index Xor &H80
                Else
                    Index = Tile.Index Xor &H80
                End If
        End Select

    End Sub

    Private _Index As Integer = -1
    Public Property Index As Integer
        Get
            Return _Index
        End Get
        Set(value As Integer)
            Dim IsOneSelected As Boolean = False
            _Index = value

            Dim SelectedTile As Tile = Nothing
            Dim Tiles As List(Of PanelTile) = (From t In TilesGrid.Children Where TypeOf t Is PanelTile Select CType(t, PanelTile)).ToList()
            Tiles.ForEach(Sub(t)
                              t.Index = (t.Index Mod 128)
                              t.IsSelected = t.Index = (_Index Mod 128)
                              If t.IsSelected Then
                                  SelectedTile = t
                              End If
                          End Sub)

            IsOneSelected = (Not SelectedTile Is Nothing)
            If IsOneSelected Then
                SelectedTile.Index = value
                SelectedTile.BringIntoView()
            Else
                Tiles.ForEach(Sub(t) t.OverlayColor = Color.FromArgb(0, 255, 255, 255))
            End If

            RaiseEvent PropertyChanged(Me, New ComponentModel.PropertyChangedEventArgs("Index"))

        End Set
    End Property


    Public Sub Initialize(TilesetIndex As Integer)

        Dim Tileset = Scenario.Instance.Tilesets.Values(TilesetIndex)
        For Index As Integer = 0 To Tileset.Count - 1
            Dim tile = New PanelTile()
            tile.Tileset = TilesetIndex
            tile.Index = Index

            AddHandler tile.TileSelected, AddressOf OnTileSelect
            tile.Margin = New Thickness(2)
            Panel.SetZIndex(tile, 0)
            TilesGrid.Children.Add(tile)
        Next

    End Sub

    Public Event PropertyChanged(sender As Object, e As System.ComponentModel.PropertyChangedEventArgs) Implements System.ComponentModel.INotifyPropertyChanged.PropertyChanged
End Class
