Imports System.Windows.Media.Animation
Imports System.Windows.Media.Effects
Imports System.Linq

Public Class TilesPanel
    Implements ComponentModel.INotifyPropertyChanged

    Private LastTab As Object = Nothing

    Protected Sub OnTileSelect(Tile As PanelTile, e As System.Windows.Input.MouseButtonEventArgs)
        LastTab = TileTabs.SelectedValue

        Select Case e.ChangedButton
            Case MouseButton.Left
                If Index = Tile.Index Then
                    Index = -1
                    SelectedTile = Nothing
                Else
                    Index = Tile.Index
                    SelectedTile = Tile
                End If
            Case MouseButton.Middle
                If (Tile.Index Mod &H80) = (Index Mod &H80) Then
                    Index = Index Xor &H80
                Else
                    Index = Tile.Index Or &H80
                End If
                SelectedTile = Tile
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

            Me.SelectedTile = Nothing

            Dim Tiles As List(Of PanelTile) = Utils.FindChildren(Of PanelTile)(Me).ToList()
            Tiles.ForEach(Sub(t)
                              t.Index = (t.Index Mod 128)
                              t.IsSelected = t.Index = (_Index Mod 128)
                              ' It used to be selected
                              If t.IsSelected Then
                                  t.Index = value
                                  IsOneSelected = True
                                  t.BringIntoView()
                              End If
                          End Sub)

            If Not IsOneSelected Then
                Tiles.ForEach(Sub(t)
                                  t.OverlayColor = Color.FromArgb(0, 255, 255, 255)
                              End Sub)
            End If

            RaiseEvent PropertyChanged(Me, New ComponentModel.PropertyChangedEventArgs("Index"))

        End Set
    End Property

    Private _SelectedTile As Tile
    Public Property SelectedTile As Tile
        Get
            Return _SelectedTile
        End Get
        Set(value As Tile)
            _SelectedTile = value
        End Set
    End Property

    Public Event PropertyChanged(sender As Object, e As System.ComponentModel.PropertyChangedEventArgs) Implements System.ComponentModel.INotifyPropertyChanged.PropertyChanged
End Class
